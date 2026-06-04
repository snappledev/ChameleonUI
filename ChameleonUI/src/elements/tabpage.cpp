#include "../../include/elements/tabpage.h"
#include "../../include/layouts/panel.h"
#include "../../include/layouts/layout_helpers.h"
#include "../../include/skins/skin.h"
#include "../../include/util/scoped_timer.h"
#include <algorithm>

#pragma region lifecycle
TabPage::TabPage(const std::wstring& title)
    : m_title(title)
{
    SetAutoLayoutVertical(false);

    auto& panel = AddChild<Panel>();
    panel.SetScrollMode(ScrollMode::Vertical);
    panel.SetAutoLayoutVertical(true);
    panel.SetAutoSpacing(8);
    panel.SetPadding(0, 0, 0, 0);
    panel.SetMargin(0, 0, 0, 0);
    m_contentPanel = &panel;
    m_isTabPage = true;
}
#pragma endregion

#pragma region drawing
void TabPage::Draw(IRenderer* renderer)
{
    Skin* skin = GetEffectiveSkin();
    if (skin)
        skin->DrawTabPageBody(renderer, *this, GetAbsoluteRect());
}
#pragma endregion

#pragma region measure & arrange
void TabPage::Measure(const Size& availableSize)
{
    if (m_width > 0 && m_height > 0)
    {
        m_desiredSize.width = (float)m_width;
        m_desiredSize.height = (float)m_height;
        if (m_contentPanel)
        {
            float clientWidth = (float)m_width - (float)(m_padding.x + m_padding.w);
            if (clientWidth < 0) clientWidth = 0;
            m_contentPanel->Measure(Size{ clientWidth, INFINITY });
        }
        m_needMeasure = false;
        return;
    }

    if (!m_needMeasure) return;

    float clientWidth = availableSize.width - (float)(m_padding.x + m_padding.w);
    if (clientWidth < 0) clientWidth = 0;

    if (m_contentPanel)
        m_contentPanel->Measure(Size{ clientWidth, INFINITY });

    float contentW = m_contentPanel ? m_contentPanel->GetDesiredSize().width : 0.f;
    float contentH = m_contentPanel ? m_contentPanel->GetDesiredSize().height : 0.f;

    m_desiredSize.width = contentW + (float)(m_padding.x + m_padding.w);
    m_desiredSize.height = contentH + (float)(m_padding.y + m_padding.h);
    m_needMeasure = false;
}

void TabPage::Arrange(const Rect& finalRect)
{
    if (CanSkipArrange(finalRect)) return;
    UIElement::Arrange(finalRect);
    if (!m_contentPanel) return;

    Rect client = GetClientRect();
    Rect panelRect((float)client.x, (float)client.y, (float)client.w, (float)client.h);
    m_contentPanel->Arrange(panelRect);
}
#pragma endregion

#pragma region geometry
TabPage::TabButtonLayout TabPage::ComputeTabButtonLayout(
    const Rect& buttonRect, IRenderer* renderer,
    int fontSize, const wchar_t* fontFamily) const
{
    TabButtonLayout layout;

    Rect tp = m_tabPadding;
    Rect content(buttonRect.x + tp.x, buttonRect.y + tp.y,
        buttonRect.w - tp.x - tp.w, buttonRect.h - tp.y - tp.h);

    layout.hasIcon = m_showIcon && m_iconRef.IsValid();
    layout.hasText = m_showText && !m_title.empty();
    float iconW = m_iconRef.naturalWidth;
    float iconH = m_iconRef.naturalHeight;
    float spacing = m_iconSpacing;

    if (layout.hasIcon && !layout.hasText)
    {
        float ix = content.x + (content.w - iconW) * 0.5f;
        float iy = content.y + (content.h - iconH) * 0.5f;
        layout.iconRect = Rect(ix, iy, iconW, iconH);
        layout.textAnchor = TextAnchor::Center;
        return layout;
    }

    if (!layout.hasIcon && layout.hasText)
    {
        layout.textRect = content;
        layout.textAnchor = TextAnchor::Center;
        return layout;
    }

    if (layout.hasIcon && layout.hasText && renderer)
    {
        Rect textMeasure = renderer->MeasureText(m_title, fontSize, fontFamily);

        switch (m_iconPosition)
        {
        case IconPosition::Left:
        {
            float totalW = iconW + spacing + textMeasure.w;
            float startX = content.x + (content.w - totalW) * 0.5f;
            if (startX < content.x) startX = content.x;

            float iy = content.y + (content.h - iconH) * 0.5f;
            layout.iconRect = Rect(startX, iy, iconW, iconH);
            layout.textRect = Rect(startX + iconW + spacing, content.y,
                content.x + content.w - (startX + iconW + spacing), content.h);
            layout.textAnchor = TextAnchor::CenterLeft;
            break;
        }
        case IconPosition::Right:
        {
            float totalW = textMeasure.w + spacing + iconW;
            float startX = content.x + (content.w - totalW) * 0.5f;
            if (startX < content.x) startX = content.x;

            layout.textRect = Rect(startX, content.y, textMeasure.w, content.h);
            float iy = content.y + (content.h - iconH) * 0.5f;
            layout.iconRect = Rect(startX + textMeasure.w + spacing, iy, iconW, iconH);
            layout.textAnchor = TextAnchor::CenterLeft;
            break;
        }
        case IconPosition::Above:
        {
            float totalH = iconH + spacing + textMeasure.h;
            float startY = content.y + (content.h - totalH) * 0.5f;
            if (startY < content.y) startY = content.y;

            float ix = content.x + (content.w - iconW) * 0.5f;
            layout.iconRect = Rect(ix, startY, iconW, iconH);
            layout.textRect = Rect(content.x, startY + iconH + spacing, content.w, textMeasure.h);
            layout.textAnchor = TextAnchor::TopCenter;
            break;
        }
        case IconPosition::Below:
        {
            float totalH = textMeasure.h + spacing + iconH;
            float startY = content.y + (content.h - totalH) * 0.5f;
            if (startY < content.y) startY = content.y;

            layout.textRect = Rect(content.x, startY, content.w, textMeasure.h);
            float ix = content.x + (content.w - iconW) * 0.5f;
            layout.iconRect = Rect(ix, startY + textMeasure.h + spacing, iconW, iconH);
            layout.textAnchor = TextAnchor::TopCenter;
            break;
        }
        }
    }

    return layout;
}
#pragma endregion