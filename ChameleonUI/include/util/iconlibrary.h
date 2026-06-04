#pragma once
#include <unordered_map>
#include <string>
#include "IconData.h"
#include "IconRef.h"

class IRenderer;

class IconLibrary
{
public:
    #pragma region register
    bool RegisterIcons(IRenderer* renderer, const unsigned char* atlasData,
        size_t atlasDataSize, const IconData* data, size_t count);
    bool RegisterIcons(IRenderer* renderer, int resourceId,
        const IconData* data, size_t count);
    bool RegisterIcons(IRenderer* renderer, const wchar_t* filePath,
        const IconData* data, size_t count);
    #pragma endregion

    #pragma region query
    IconRef RetrieveIcon(const std::wstring& name,
        float displayW = 16.f, float displayH = 16.f) const;

    bool Contains(const std::wstring& name) const {
        return m_icons.find(name) != m_icons.end();
    }

    size_t Count() const { return m_icons.size(); }
    #pragma endregion

    void FreeAll(IRenderer* renderer);

private:
    void AddIconArray(const IconData* data, size_t count, void* atlasImage);

    struct IconEntry {
        void* image = nullptr;
        Rect  srcRect;
    };

    std::unordered_map<std::wstring, IconEntry> m_icons;
    std::vector<void*>                           m_atlasHandles;
};