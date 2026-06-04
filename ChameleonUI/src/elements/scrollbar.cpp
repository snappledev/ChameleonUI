#include "../../include/elements/scrollbar.h"
#include "../../include/skins/skin.h"
#include "../../include/core/uistyle.h"

#pragma region drawing
void Scrollbar::Draw(IRenderer* renderer, Skin* skin, const Rect& clientRect)
{
    if (!m_state.scrollbarShown || !skin || !skin->GetStyle())
        return;

    const UIStyle* style = skin->GetStyle();
    Rect track = ComputeTrackRect(clientRect);
    m_state.lastThumbRect = ComputeThumbRect(track, style->scrollbarMinThumbHeight);
    bool vertical = (m_direction == ScrollDirection::Vertical);
    skin->DrawScrollbar(renderer, track, m_state.lastThumbRect, vertical, m_state.thumbHovered);
}
#pragma endregion