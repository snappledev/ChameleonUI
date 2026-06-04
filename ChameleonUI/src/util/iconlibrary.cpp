#include "../../include/util/iconlibrary.h"
#include "../../include/rendering/renderer.h"
#include <cassert>

#pragma region registration
bool IconLibrary::RegisterIcons(IRenderer* renderer, const unsigned char* atlasData,
    size_t atlasDataSize, const IconData* data, size_t count)
{
    assert(renderer && "IconLibrary::RegisterIcons called with null renderer");
    assert(atlasData && atlasDataSize > 0 && "IconLibrary::RegisterIcons called with empty atlas data");
    assert(data && count > 0 && "IconLibrary::RegisterIcons called with empty icon data");

    void* image = renderer->LoadImageFromMemory(atlasData, atlasDataSize);
    if (!image)
        return false;
    m_atlasHandles.push_back(image);
    AddIconArray(data, count, image);
    return true;
}

bool IconLibrary::RegisterIcons(IRenderer* renderer, int resourceId,
    const IconData* data, size_t count)
{
    assert(renderer && "IconLibrary::RegisterIcons called with null renderer");
    assert(data && count > 0 && "IconLibrary::RegisterIcons called with empty icon data");

    void* image = renderer->LoadImageFromResource(resourceId);
    if (!image)
        return false;
    m_atlasHandles.push_back(image);
    AddIconArray(data, count, image);
    return true;
}

bool IconLibrary::RegisterIcons(IRenderer* renderer, const wchar_t* filePath,
    const IconData* data, size_t count)
{
    assert(renderer && "IconLibrary::RegisterIcons called with null renderer");
    assert(data && count > 0 && "IconLibrary::RegisterIcons called with empty icon data");

    void* image = renderer->LoadImage(filePath);
    if (!image)
        return false;
    m_atlasHandles.push_back(image);
    AddIconArray(data, count, image);
    return true;
}

void IconLibrary::AddIconArray(const IconData* data, size_t count, void* atlasImage)
{
    m_icons.reserve(m_icons.size() + count);

    for (size_t i = 0; i < count; ++i) {
        IconEntry entry;
        entry.image = atlasImage;
        entry.srcRect = Rect(
            static_cast<float>(data[i].x),
            static_cast<float>(data[i].y),
            static_cast<float>(data[i].width),
            static_cast<float>(data[i].height));
        m_icons[data[i].name] = entry;
    }
}
#pragma endregion registration

#pragma region retrieval
IconRef IconLibrary::RetrieveIcon(const std::wstring& name,
    float displayW, float displayH) const
{
    auto it = m_icons.find(name);
    if (it == m_icons.end())
        return {};
    const IconEntry& e = it->second;
    return IconRef(e.image, e.srcRect, displayW, displayH);
}
#pragma endregion retrieval

#pragma region cleanup
void IconLibrary::FreeAll(IRenderer* renderer)
{
    if (renderer)
    {
        for (void* h : m_atlasHandles)
            renderer->FreeImage(h);
    }
    m_atlasHandles.clear();
    m_icons.clear();
}
#pragma endregion cleanup