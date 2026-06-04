#!/usr/bin/env python3
"""
Universal icon atlas generator with embedded binary.
- If SVGs found → uses Spreet (packed atlas) → parses JSON → generates .h
- If only PNGs found → uses ImageMagick montage (grid atlas) → generates .h
In both cases, the resulting .h file contains the full PNG atlas as a byte array,
and the intermediate PNG file is deleted. The .h file is placed inside the input
directory (or optionally at a custom location).
"""

import json
import sys
import os
import subprocess
import argparse
import shutil

def parse_args():
    parser = argparse.ArgumentParser(description='Generate an icon atlas + C++ header with embedded PNG binary.')
    parser.add_argument('input_dir', help='Directory containing SVG or PNG icons')
    parser.add_argument('-o', '--output', default=None,
                        help='Base name for output files (default: directory name). '
                             'If a path is given, it overrides the default output location. '
                             'Otherwise the files are placed inside input_dir.')
    parser.add_argument('--name', default=None, help='C++ array name (default: derived from output)')
    # PNG options
    parser.add_argument('--cell', type=int, default=48, help='Cell size for PNG grid (width=height) [default: 48]')
    parser.add_argument('--columns', type=int, default=40, help='Columns in PNG grid [default: 40]')
    parser.add_argument('--order-file', default=None, help='File with ordered PNG filenames (one per line)')
    # SVG options
    parser.add_argument('--no-clean', action='store_true', help='Keep intermediate JSON (SVG mode)')
    parser.add_argument('--spreet-args', default='', help='Extra arguments passed to spreet (e.g. "--minify false")')
    return parser.parse_args()

def detect_mode(directory):
    svgs = [f for f in os.listdir(directory) if f.lower().endswith('.svg')]
    pngs = [f for f in os.listdir(directory) if f.lower().endswith('.png')]
    if svgs:
        return 'svg'
    elif pngs:
        return 'png'
    else:
        return None

def check_tool(command, name, install_url):
    """Check if a command is available in PATH. Exit with error message if not."""
    if shutil.which(command) is None:
        print(f"Error: '{command}' is not installed or not in your PATH.")
        print(f"  {name} is required for this operation.")
        print(f"  Please install it from: {install_url}")
        sys.exit(1)

def bytes_to_c_array(data, var_name, max_per_line=16):
    """Convert bytes to a C++ constexpr byte array literal."""
    lines = []
    lines.append(f"inline constexpr unsigned char {var_name}[] = {{")
    for i in range(0, len(data), max_per_line):
        chunk = data[i:i+max_per_line]
        hex_bytes = ', '.join(f'0x{b:02x}' for b in chunk)
        lines.append(f"    {hex_bytes},")
    lines.append("};")
    lines.append(f"inline constexpr unsigned int {var_name}Size = sizeof({var_name});")
    return '\n'.join(lines)

def run_svg_mode(args, base, array_name, input_dir, output_prefix):
    png_path = output_prefix + '.png'
    json_path = output_prefix + '.json'
    header_path = output_prefix + '.h'

    # 1. Check that spreet is installed
    check_tool('spreet', 'Spreet', 'https://github.com/priteau/spreet')

    # 2. Run Spreet to generate PNG atlas + JSON metadata
    # Note: spreet expects the output base name without extension, and creates files in the current directory.
    # We need to run it from the input_dir or adjust paths. Let's change working directory to input_dir.
    original_cwd = os.getcwd()
    os.chdir(input_dir)
    try:
        # base is just the filename part (no directory) because we are inside input_dir
        cmd = ['spreet', '.', base] + args.spreet_args.split()
        print(f"[SVG] Running Spreet: {' '.join(cmd)}")
        res = subprocess.run(cmd, capture_output=True, text=True)
        if res.returncode != 0:
            print("Spreet failed:")
            print(res.stderr)
            sys.exit(1)
    finally:
        os.chdir(original_cwd)

    # Spreet creates output files inside input_dir with the given base name.
    # Now check if they exist.
    if not os.path.exists(json_path):
        print(f"Error: Expected JSON file '{json_path}' not created.")
        sys.exit(1)

    # 3. Parse JSON to get icon positions
    with open(json_path, 'r', encoding='utf-8') as f:
        data = json.load(f)

    entries = []
    for name, info in data.items():
        x, y, w, h = int(info['x']), int(info['y']), int(info['width']), int(info['height'])
        entries.append((name, x, y, w, h))
    entries.sort(key=lambda e: e[0])

    # 4. Read the generated PNG atlas as binary
    if not os.path.exists(png_path):
        print(f"Error: Atlas PNG '{png_path}' not found.")
        sys.exit(1)
    with open(png_path, 'rb') as f:
        png_data = f.read()

    # 5. Build header content
    lines = []
    lines.append("#pragma once")
    lines.append('#include "../../../include/util/icondata.h"')
    lines.append("")
    lines.append(f"// Auto-generated from {os.path.basename(json_path)}")
    lines.append("// Embedded atlas binary – PNG bytes compiled directly into the executable.")
    lines.append("")
    
    # Embedded data array
    atlas_var_name = f"k{array_name}_AtlasData"
    lines.append(bytes_to_c_array(png_data, atlas_var_name))
    lines.append("")
    
    # Icon lookup table
    lines.append(f"constexpr int k{array_name}Count = {len(entries)};")
    lines.append(f"inline constexpr IconData k{array_name}[] = {{")
    for name, x, y, w, h in entries:
        lines.append(f'    {{ L"{name}", {x}, {y}, {w}, {h} }},')
    lines.append("};")

    with open(header_path, 'w', encoding='utf-8') as fh:
        fh.write('\n'.join(lines) + '\n')

    # 6. Clean up intermediate files
    if not args.no_clean:
        os.remove(json_path)
        print(f"[SVG] Removed intermediate '{json_path}'")
    os.remove(png_path)
    print(f"[SVG] Removed atlas PNG '{png_path}'")

    print(f"[SVG] Done → {header_path} (embedded PNG binary)")

def run_png_mode(args, base, array_name, input_dir, output_prefix):
    png_path = output_prefix + '.png'
    header_path = output_prefix + '.h'
    cell = args.cell
    columns = args.columns

    # 1. Check that ImageMagick is installed
    check_tool('magick', 'ImageMagick', 'https://imagemagick.org/script/download.php')

    # Gather PNG filenames
    if args.order_file:
        with open(args.order_file, 'r', encoding='utf-8') as f:
            png_names = [line.strip() for line in f if line.strip()]
    else:
        png_names = sorted([f for f in os.listdir(input_dir) if f.lower().endswith('.png')])

    if not png_names:
        print("No PNG files found.")
        sys.exit(1)

    # Create temporary file list for montage (relative paths from input_dir)
    list_path = os.path.join(input_dir, '_montage_list.txt')
    with open(list_path, 'w', encoding='utf-8') as f:
        for name in png_names:
            if ' ' in name:
                f.write(f'"{name}"\n')
            else:
                f.write(name + '\n')

    # Run montage – output goes directly to png_path (which is inside input_dir)
    montage_cmd = [
        'magick', 'montage', f'@{list_path}',
        '-geometry', f'{cell}x{cell}+0+0',
        '-background', 'none',
        '-tile', f'{columns}x',
        png_path
    ]

    print(f"[PNG] Running ImageMagick: {' '.join(montage_cmd)}")
    # montage must be run from input_dir because the list contains relative filenames
    res = subprocess.run(montage_cmd, cwd=input_dir, capture_output=True, text=True)
    os.remove(list_path)

    if res.returncode != 0:
        print("ImageMagick montage failed:")
        print(res.stderr)
        sys.exit(1)

    total = len(png_names)
    rows = (total + columns - 1) // columns
    print(f"[PNG] Grid {columns}x{rows}, {total} icons")

    # Read the generated PNG atlas as binary
    if not os.path.exists(png_path):
        print(f"Error: Atlas PNG '{png_path}' not found.")
        sys.exit(1)
    with open(png_path, 'rb') as f:
        png_data = f.read()

    # Build header content
    lines = []
    lines.append("#pragma once")
    lines.append('#include "../../../include/util/icondata.h"')
    lines.append("")
    lines.append(f"// Auto-generated grid atlas from '{input_dir}'")
    lines.append(f"// Cell size: {cell}x{cell}, Grid: {columns}x{rows}")
    lines.append("// Embedded atlas binary – PNG bytes compiled directly into the executable.")
    lines.append("")
    
    # Embedded data array
    atlas_var_name = f"k{array_name}_AtlasData"
    lines.append(bytes_to_c_array(png_data, atlas_var_name))
    lines.append("")
    
    # Icon lookup table
    lines.append(f"constexpr int k{array_name}Count = {total};")
    lines.append(f"inline constexpr IconData k{array_name}[] = {{")
    for i, name in enumerate(png_names):
        icon_name = os.path.splitext(name)[0]
        col = i % columns
        row = i // columns
        x = col * cell
        y = row * cell
        lines.append(f'    {{ L"{icon_name}", {x}, {y}, {cell}, {cell} }},')
    lines.append("};")

    with open(header_path, 'w', encoding='utf-8') as fh:
        fh.write('\n'.join(lines) + '\n')

    # Delete the atlas PNG (no longer needed)
    os.remove(png_path)
    print(f"[PNG] Removed atlas PNG '{png_path}'")

    print(f"[PNG] Done → {header_path} (embedded PNG binary)")

def main():
    args = parse_args()
    input_dir = os.path.abspath(args.input_dir)
    if not os.path.isdir(input_dir):
        print(f"Error: '{input_dir}' is not a directory.")
        sys.exit(1)

    # Determine output base name and path
    base_name = args.output or os.path.basename(input_dir.rstrip('/\\'))
    if args.output and os.path.dirname(args.output):
        # If user provided a path, use that as the full prefix (respects custom location)
        output_prefix = args.output
    else:
        # Otherwise place the output files inside the input directory
        output_prefix = os.path.join(input_dir, base_name)

    array_name = args.name or (base_name.replace('-', '_').replace(' ', '_') + 'Icons')

    mode = detect_mode(input_dir)
    if mode == 'svg':
        run_svg_mode(args, base_name, array_name, input_dir, output_prefix)
    elif mode == 'png':
        run_png_mode(args, base_name, array_name, input_dir, output_prefix)
    else:
        print(f"Error: No .svg or .png files found in '{input_dir}'.")
        sys.exit(1)

if __name__ == '__main__':
    main()