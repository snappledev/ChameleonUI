#include "../../include/util/configstore.h"
#include <ShlObj.h>
#include <algorithm>
#include <filesystem>
#include <fstream>

#pragma region helpers
std::wstring ConfigStore::ColorToString(const Color& c)
{
    return std::to_wstring(c.r) + L"," + std::to_wstring(c.g) + L"," +
        std::to_wstring(c.b) + L"," + std::to_wstring(c.a);
}

Color ConfigStore::StringToColor(const std::wstring& s)
{
    int r = 0, g = 0, b = 0, a = 255;
    swscanf_s(s.c_str(), L"%d,%d,%d,%d", &r, &g, &b, &a);
    return Color((uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a);
}

std::wstring ConfigStore::BuildConfigPath(const std::wstring& configName) const
{
    return m_configDir + L"\\" + configName + L".cfg";
}

void ConfigStore::EnsureDirectoryExists() const
{
    if (!m_configDir.empty())
        std::filesystem::create_directories(m_configDir);
}
#pragma endregion helpers

#pragma region directory
void ConfigStore::SetAppName(const std::wstring& appName)
{
    m_appName = appName;

    PWSTR appDataPath = nullptr;
    if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &appDataPath)))
    {
        m_configDir = std::wstring(appDataPath) + L"\\" + appName;
        CoTaskMemFree(appDataPath);
    }
    else
    {
        m_configDir = L".\\" + appName;
    }

    EnsureDirectoryExists();
}

void ConfigStore::SetBaseDirectory(const std::wstring& path)
{
    m_configDir = path;
    EnsureDirectoryExists();
}
#pragma endregion directory

#pragma region config operations
bool ConfigStore::SaveConfig(const std::wstring& configName) const
{
    EnsureDirectoryExists();
    return Save(BuildConfigPath(configName));
}

bool ConfigStore::LoadConfig(const std::wstring& configName)
{
    if (!Load(BuildConfigPath(configName)))
        return false;
    m_activeConfig = configName;
    return true;
}

bool ConfigStore::DeleteConfig(const std::wstring& configName)
{
    std::wstring path = BuildConfigPath(configName);
    if (!std::filesystem::exists(path)) return false;
    return std::filesystem::remove(path);
}

bool ConfigStore::RenameConfig(const std::wstring& oldName, const std::wstring& newName)
{
    std::wstring oldPath = BuildConfigPath(oldName);
    std::wstring newPath = BuildConfigPath(newName);
    if (!std::filesystem::exists(oldPath)) return false;
    if (std::filesystem::exists(newPath)) return false;
    std::filesystem::rename(oldPath, newPath);
    if (m_activeConfig == oldName)
        m_activeConfig = newName;
    return true;
}

bool ConfigStore::ConfigExists(const std::wstring& configName) const
{
    return std::filesystem::exists(BuildConfigPath(configName));
}

std::vector<std::wstring> ConfigStore::ListConfigs() const
{
    std::vector<std::wstring> result;
    if (m_configDir.empty() || !std::filesystem::exists(m_configDir))
        return result;

    for (auto& entry : std::filesystem::directory_iterator(m_configDir))
    {
        if (!entry.is_regular_file()) continue;
        if (entry.path().extension() == L".cfg")
            result.push_back(entry.path().stem().wstring());
    }

    std::sort(result.begin(), result.end());
    return result;
}
#pragma endregion config operations

#pragma region defaults
void ConfigStore::SnapshotDefaults()
{
    m_defaults.clear();
    for (auto& [key, entry] : m_entries)
    {
        DefaultValue dv;
        dv.type = entry.type;
        switch (entry.type)
        {
        case Type::Bool:   dv.bVal = *static_cast<bool*>(entry.ptr);           break;
        case Type::Int:    dv.iVal = *static_cast<int*>(entry.ptr);            break;
        case Type::Float:  dv.fVal = *static_cast<float*>(entry.ptr);          break;
        case Type::String: dv.sVal = *static_cast<std::wstring*>(entry.ptr);   break;
        case Type::Color:  dv.cVal = *static_cast<Color*>(entry.ptr);          break;
        }
        m_defaults[key] = dv;
    }
}

void ConfigStore::ResetAll()
{
    for (auto& [key, dv] : m_defaults)
    {
        auto it = m_entries.find(key);
        if (it == m_entries.end()) continue;
        Entry& entry = it->second;
        switch (dv.type)
        {
        case Type::Bool:   *static_cast<bool*>(entry.ptr) = dv.bVal; break;
        case Type::Int:    *static_cast<int*>(entry.ptr) = dv.iVal; break;
        case Type::Float:  *static_cast<float*>(entry.ptr) = dv.fVal; break;
        case Type::String: *static_cast<std::wstring*>(entry.ptr) = dv.sVal; break;
        case Type::Color:  *static_cast<Color*>(entry.ptr) = dv.cVal; break;
        }
    }
}

void ConfigStore::ResetKey(const std::wstring& key)
{
    auto dit = m_defaults.find(key);
    auto eit = m_entries.find(key);
    if (dit == m_defaults.end() || eit == m_entries.end()) return;
    DefaultValue& dv = dit->second;
    Entry& entry = eit->second;
    switch (dv.type)
    {
    case Type::Bool:   *static_cast<bool*>(entry.ptr) = dv.bVal; break;
    case Type::Int:    *static_cast<int*>(entry.ptr) = dv.iVal; break;
    case Type::Float:  *static_cast<float*>(entry.ptr) = dv.fVal; break;
    case Type::String: *static_cast<std::wstring*>(entry.ptr) = dv.sVal; break;
    case Type::Color:  *static_cast<Color*>(entry.ptr) = dv.cVal; break;
    }
}
#pragma endregion defaults

#pragma region file io
bool ConfigStore::Save(const std::wstring& filePath) const
{
    std::wofstream file(filePath);
    if (!file.is_open()) return false;

    for (auto& [key, entry] : m_entries)
    {
        switch (entry.type)
        {
        case Type::Bool:
            file << L"bool:" << key << L"="
                << (*static_cast<bool*>(entry.ptr) ? L"true" : L"false") << L"\n";
            break;
        case Type::Int:
            file << L"int:" << key << L"="
                << *static_cast<int*>(entry.ptr) << L"\n";
            break;
        case Type::Float:
            file << L"float:" << key << L"="
                << *static_cast<float*>(entry.ptr) << L"\n";
            break;
        case Type::String:
            file << L"string:" << key << L"="
                << *static_cast<std::wstring*>(entry.ptr) << L"\n";
            break;
        case Type::Color:
            file << L"color:" << key << L"="
                << ColorToString(*static_cast<Color*>(entry.ptr)) << L"\n";
            break;
        }
    }

    return true;
}

bool ConfigStore::Load(const std::wstring& filePath)
{
    std::wifstream file(filePath);
    if (!file.is_open()) return false;

    std::wstring line;
    while (std::getline(file, line))
    {
        if (line.empty()) continue;

        size_t colonPos = line.find(L':');
        if (colonPos == std::wstring::npos) continue;

        size_t equalsPos = line.find(L'=', colonPos);
        if (equalsPos == std::wstring::npos) continue;

        std::wstring type = line.substr(0, colonPos);
        std::wstring key = line.substr(colonPos + 1, equalsPos - colonPos - 1);
        std::wstring valStr = line.substr(equalsPos + 1);

        auto it = m_entries.find(key);
        if (it == m_entries.end()) continue;

        Entry& entry = it->second;

        try {
            if (type == L"bool" && entry.type == Type::Bool)
                *static_cast<bool*>(entry.ptr) = (valStr == L"true");
            else if (type == L"int" && entry.type == Type::Int)
                *static_cast<int*>(entry.ptr) = std::stoi(valStr);
            else if (type == L"float" && entry.type == Type::Float)
                *static_cast<float*>(entry.ptr) = std::stof(valStr);
            else if (type == L"string" && entry.type == Type::String)
                *static_cast<std::wstring*>(entry.ptr) = valStr;
            else if (type == L"color" && entry.type == Type::Color)
                *static_cast<Color*>(entry.ptr) = StringToColor(valStr);
        }
        catch (...) {
            continue;
        }
    }

    return true;
}
#pragma endregion file io