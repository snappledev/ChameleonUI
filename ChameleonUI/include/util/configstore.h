#pragma once
#include "../rendering/renderer.h"
#include <string>
#include <unordered_map>
#include <vector>

class ConfigStore
{
public:
    #pragma region app identity
    void SetAppName(const std::wstring& appName);
    const std::wstring& GetAppName() const { return m_appName; }

    void SetBaseDirectory(const std::wstring& path);
    const std::wstring& GetConfigDirectory() const { return m_configDir; }
    #pragma endregion

    #pragma region bind
    void Bind(const std::wstring& key, bool* ptr) { m_entries[key] = Entry{ ptr, Type::Bool }; }
    void Bind(const std::wstring& key, int* ptr) { m_entries[key] = Entry{ ptr, Type::Int }; }
    void Bind(const std::wstring& key, float* ptr) { m_entries[key] = Entry{ ptr, Type::Float }; }
    void Bind(const std::wstring& key, std::wstring* ptr) { m_entries[key] = Entry{ ptr, Type::String }; }
    void Bind(const std::wstring& key, Color* ptr) { m_entries[key] = Entry{ ptr, Type::Color }; }
    #pragma endregion

    #pragma region config save/load
    bool SaveConfig(const std::wstring& configName) const;
    bool LoadConfig(const std::wstring& configName);
    bool DeleteConfig(const std::wstring& configName);
    bool RenameConfig(const std::wstring& oldName, const std::wstring& newName);
    bool ConfigExists(const std::wstring& configName) const;

    std::vector<std::wstring> ListConfigs() const;
    #pragma endregion

    #pragma region raw path
    bool Save(const std::wstring& filePath) const;
    bool Load(const std::wstring& filePath);
    #pragma endregion

    #pragma region reset
    void SnapshotDefaults();
    void ResetAll();
    void ResetKey(const std::wstring& key);
    #pragma endregion

    #pragma region query
    bool HasKey(const std::wstring& key) const { return m_entries.count(key) > 0; }
    size_t Count() const { return m_entries.size(); }
    #pragma endregion

    #pragma region active config
    const std::wstring& GetActiveConfigName() const { return m_activeConfig; }
    void SetActiveConfigName(const std::wstring& name) { m_activeConfig = name; }
    #pragma endregion

private:
    enum class Type { Bool, Int, Float, String, Color };

    struct Entry
    {
        void* ptr = nullptr;
        Type type = Type::Bool;
    };

    struct DefaultValue
    {
        Type type;
        bool bVal = false;
        int iVal = 0;
        float fVal = 0.f;
        std::wstring sVal;
        Color cVal;
    };

    std::unordered_map<std::wstring, Entry>        m_entries;
    std::unordered_map<std::wstring, DefaultValue> m_defaults;

    std::wstring m_appName;
    std::wstring m_configDir;
    std::wstring m_activeConfig;

    std::wstring BuildConfigPath(const std::wstring& configName) const;
    void EnsureDirectoryExists() const;

    static std::wstring ColorToString(const Color& c);
    static Color StringToColor(const std::wstring& s);
};