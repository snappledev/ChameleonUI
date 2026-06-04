#pragma once
#include <string>
#include <algorithm>
#include <cstdint>
#include <cmath>

namespace InputSanitizer
{
    #pragma region string sanitization
    inline std::wstring SanitizeString(const std::wstring& input,
        bool allowNewlines = false,
        bool allowTabs = false)
    {
        std::wstring result;
        result.reserve(input.size());
        for (wchar_t c : input) {
            if (c == L'\0')                          continue;
            if (c == 0x7F)                           continue;
            if (c >= 0xFDD0 && c <= 0xFDEF)          continue;
            if ((c & 0xFFFE) == 0xFFFE)              continue;
            if (c < 0x20) {
                if (c == L'\n' && allowNewlines) { result += c; continue; }
                if (c == L'\r' && allowNewlines)     continue;
                if (c == L'\t' && allowTabs) { result += c; continue; }
                continue;
            }
            result += c;
        }
        return result;
    }

    inline std::wstring ClampLength(const std::wstring& input, size_t maxLength)
    {
        if (maxLength == 0 || input.size() <= maxLength) return input;
        return input.substr(0, maxLength);
    }
    #pragma endregion

    #pragma region numeric validation

    inline float SanitizeFloat(float value, float minVal = -1e9f, float maxVal = 1e9f)
    {
        if (std::isnan(value) || std::isinf(value)) return 0.f;
        return std::clamp(value, minVal, maxVal);
    }

    inline int SanitizeInt(int value, int minVal = -100000, int maxVal = 100000)
    {
        return std::clamp(value, minVal, maxVal);
    }

    #pragma endregion

    #pragma region path sanitization
    inline std::wstring SanitizePath(const std::wstring& path, size_t maxLength = 260)
    {
        if (path.empty() || path.size() > maxLength) return {};
        if (path.find(L'\0') != std::wstring::npos) return {};
        if (path.find(L"..") != std::wstring::npos) return {};
        if (path.size() >= 2 && path[0] == L'\\' && path[1] == L'\\') return {};
        for (wchar_t c : path) {
            if (c < 0x20) return {};
        }
        return path;
    }
    #pragma endregion

    #pragma region color validation
    inline uint8_t SanitizeColorComponent(int value)
    {
        return static_cast<uint8_t>(std::clamp(value, 0, 255));
    }
    #pragma endregion

    #pragma region buffer size guard
    inline size_t SafeAllocSize(size_t count, size_t elemSize, size_t maxBytes = 256 * 1024 * 1024)
    {
        if (count == 0 || elemSize == 0) return 0;
        size_t total = count * elemSize;
        if (total / elemSize != count) return 0;
        if (total > maxBytes) return 0;
        return total;
    }
    #pragma endregion
}