#include "../../include/util/localization.h"

#pragma region lifecycle
void Localization::RegisterLanguage(const std::wstring& langCode,
    const LocalizedString* strings, size_t count)
{
    auto& table = m_languages[langCode];
    table.reserve(count);
    for (size_t i = 0; i < count; ++i)
        table[strings[i].key] = strings[i].value;
}

bool Localization::SetLanguage(const std::wstring& langCode)
{
    if (m_languages.find(langCode) == m_languages.end())
        return false;

    if (m_activeLang != langCode)
    {
        m_activeLang = langCode;
        if (m_onChanged) m_onChanged();
    }
    return true;
}

std::vector<std::wstring> Localization::GetAvailableLanguages() const
{
    std::vector<std::wstring> result;
    result.reserve(m_languages.size());
    for (auto& pair : m_languages)
        result.push_back(pair.first);
    return result;
}
#pragma endregion lifecycle

#pragma region query
const std::wstring& Localization::Tr(const std::wstring& key) const
{
    auto langIt = m_languages.find(m_activeLang);
    if (langIt != m_languages.end())
    {
        auto it = langIt->second.find(key);
        if (it != langIt->second.end())
            return it->second;
    }

    if (m_activeLang != m_defaultLang)
    {
        auto defIt = m_languages.find(m_defaultLang);
        if (defIt != m_languages.end())
        {
            auto it = defIt->second.find(key);
            if (it != defIt->second.end())
                return it->second;
        }
    }

    auto& cached = m_fallbackCache[key];
    cached = key;
    return cached;
}
#pragma endregion query