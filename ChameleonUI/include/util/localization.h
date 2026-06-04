#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

struct LocalizedString {
    const wchar_t* key;
    const wchar_t* value;
};

class Localization
{
public:
    #pragma region registration
    void RegisterLanguage(const std::wstring& langCode,
        const LocalizedString* strings, size_t count);

    bool SetLanguage(const std::wstring& langCode);
    #pragma endregion

    #pragma region query
    void SetDefaultLanguage(const std::wstring& langCode) { m_defaultLang = langCode; }

    const std::wstring& GetLanguage() const { return m_activeLang; }

    std::vector<std::wstring> GetAvailableLanguages() const;

    const std::wstring& Tr(const std::wstring& key) const;

    void OnLanguageChanged(std::function<void()> callback) { m_onChanged = callback; }
    #pragma endregion

private:
    using StringTable = std::unordered_map<std::wstring, std::wstring>;

    std::unordered_map<std::wstring, StringTable> m_languages;
    std::wstring m_activeLang = L"en";
    std::wstring m_defaultLang = L"en";
    std::function<void()> m_onChanged;

    mutable std::unordered_map<std::wstring, std::wstring> m_fallbackCache;
};