#include "progressbar.hpp"
#include <cmath>
#include <algorithm>

// ---------- Ctor / Dtor ----------
ProgressBar::ProgressBar(float width, float height,
                         float min, float max, float step)
    : m_width(width), m_height(height),
      m_min(min), m_max(max), m_step(step > 0 ? step : 1.0f),
      m_value(min), m_displayValue(min),
      m_animSpeed(150.0f),
      m_glowPhase(0.0f), m_glowSpeed(4.0f), m_glowIntensity(0.35f),
      m_roundness(1.0f),
      m_text(""), m_textPos(TextPosition::Center),
      m_fg(GREEN), m_bg(DARKGRAY), m_fontColor(WHITE),
      m_font(GetFontDefault()), m_hasCustomFont(false),
      m_showBorder(false), m_borderColor(BLACK)
{
    if (m_max <= m_min)
        m_max = m_min + 1.0f;
}

ProgressBar::~ProgressBar() = default;

// ---------- Update (non-blocking) ----------
void ProgressBar::Update(float deltaTime)
{
    // Smoothly chase target value
    float diff = m_value - m_displayValue;
    float stepAmt = m_animSpeed * deltaTime;
    if (std::fabs(diff) <= stepAmt)
        m_displayValue = m_value;
    else
        m_displayValue += (diff > 0 ? stepAmt : -stepAmt);

    // Advance glow phase (kept in memory for API consistency, though visually removed)
    m_glowPhase += m_glowSpeed * deltaTime;
    if (m_glowPhase > 6.2831853f)
        m_glowPhase -= 6.2831853f;
}

// ---------- Draw ----------
void ProgressBar::Draw(Rectangle bounds) const
{
    float ratio = GetPercentage();
    float roundness = m_roundness * 0.5f;

    // 1. Draw Background
    DrawRectangleRounded(bounds, roundness, 12, m_bg);

    // 2. Draw Fill (Dual color, clean modern look)
    if (ratio > 0.0001f)
    {
        Rectangle fill = bounds;
        fill.width = bounds.width * ratio;

        // Removed the forced minimum width constraint so small widths (3, 6, etc.) scale correctly
        if (fill.width > bounds.width)
            fill.width = bounds.width;

        if (m_showBorder)
        {
            BeginScissorMode(
                (int)(bounds.x + 1.5f),
                (int)(bounds.y + 1.5f),
                (int)(fill.width - 3.0f),
                (int)(bounds.height - 3.0f));
            Rectangle fullInner = {bounds.x + 1.5f, bounds.y + 1.5f, bounds.width - 3.0f, bounds.height - 3.0f};
            DrawRectangleRounded(fullInner, roundness, 12, m_fg);
            EndScissorMode();
        }
        else
        {
            BeginScissorMode((int)bounds.x, (int)bounds.y, (int)fill.width, (int)bounds.height);
            DrawRectangleRounded(bounds, roundness, 12, m_fg);
            EndScissorMode();
        }
    }

    // 3. Draw Border
    if (m_showBorder)
    {
        DrawRectangleRoundedLinesEx(bounds, roundness, 12, 2.0f, m_borderColor);
    }

    // 4. Draw Text
    if (!m_text.empty() && m_textPos != TextPosition::None)
    {
        float fontSize = bounds.height * 0.55f;
        if (fontSize < 10.0f)
            fontSize = 10.0f;

        Vector2 textSize = MeasureTextEx(m_font, m_text.c_str(), fontSize, 1.0f);
        float tx = bounds.x, ty = bounds.y + (bounds.height - textSize.y) * 0.5f;

        switch (m_textPos)
        {
        case TextPosition::Center:
            tx = bounds.x + (bounds.width - textSize.x) * 0.5f;
            break;
        case TextPosition::Left:
            tx = bounds.x - textSize.x - 8.0f;
            break;
        case TextPosition::Right:
            tx = bounds.x + bounds.width + 8.0f;
            break;
        case TextPosition::InsideLeft:
            tx = bounds.x + 8.0f;
            break;
        case TextPosition::InsideRight:
            tx = bounds.x + bounds.width - textSize.x - 8.0f;
            break;
        default:
            break;
        }

        // Drop shadow
        Vector2 shadowPos = {tx + 1.0f, ty + 1.0f};
        DrawTextEx(m_font, m_text.c_str(), shadowPos, fontSize, 1.0f, Fade(BLACK, 0.5f));

        // Main text (Using custom font color)
        Vector2 textPos = {tx, ty};
        DrawTextEx(m_font, m_text.c_str(), textPos, fontSize, 1.0f, m_fontColor);
    }
}

void ProgressBar::Draw(float x, float y) const
{
    Rectangle bounds = {x, y, m_width, m_height};
    Draw(bounds);
}

void ProgressBar::SetFont(Font font)
{
    m_font = font;
    m_hasCustomFont = true;
}

// ---------- Value ----------
void ProgressBar::SetValue(float v)
{
    m_value = v;
    ClampAndSnap();
}

void ProgressBar::Increment()
{
    m_value += m_step;
    ClampAndSnap();
}

void ProgressBar::Decrement()
{
    m_value -= m_step;
    ClampAndSnap();
}

void ProgressBar::Reset()
{
    m_value = m_min;
    ClampAndSnap();
}

void ProgressBar::ClampAndSnap()
{
    if (m_step > 0.0f)
    {
        float offset = m_value - m_min;
        offset = std::round(offset / m_step) * m_step;
        m_value = m_min + offset;
    }
    if (m_value < m_min)
        m_value = m_min;
    if (m_value > m_max)
        m_value = m_max;
}

// ---------- Getters ----------
float ProgressBar::GetValue() const { return m_value; }
float ProgressBar::GetMin() const { return m_min; }
float ProgressBar::GetMax() const { return m_max; }
float ProgressBar::GetStep() const { return m_step; }
float ProgressBar::GetWidth() const { return m_width; }
float ProgressBar::GetHeight() const { return m_height; }
bool ProgressBar::IsAnimating() const { return std::fabs(m_value - m_displayValue) > 0.001f; }

float ProgressBar::GetPercentage() const
{
    if (m_max == m_min)
        return 0.0f;
    float d = m_displayValue;
    if (d < m_min)
        d = m_min;
    if (d > m_max)
        d = m_max;
    return (d - m_min) / (m_max - m_min);
}

const std::string &ProgressBar::GetText() const { return m_text; }
TextPosition ProgressBar::GetTextPosition() const { return m_textPos; }
Color ProgressBar::GetFGColor() const { return m_fg; }
Color ProgressBar::GetBGColor() const { return m_bg; }
Color ProgressBar::GetFontColor() const { return m_fontColor; }

// ---------- Setters ----------
void ProgressBar::SetMin(float v)
{
    m_min = v;
    if (m_max <= m_min)
        m_max = m_min + 1.0f;
    ClampAndSnap();
}

void ProgressBar::SetMax(float v)
{
    m_max = v;
    if (m_max <= m_min)
        m_min = m_max - 1.0f;
    ClampAndSnap();
}

void ProgressBar::SetStep(float v)
{
    m_step = (v > 0 ? v : 1.0f);
    ClampAndSnap();
}

void ProgressBar::SetWidth(float w) { m_width = (w > 0 ? w : 1.0f); }
void ProgressBar::SetHeight(float h) { m_height = (h > 0 ? h : 1.0f); }
void ProgressBar::SetText(const std::string &t) { m_text = t; }
void ProgressBar::SetTextPosition(TextPosition p) { m_textPos = p; }
void ProgressBar::SetFGColor(Color c) { m_fg = c; }
void ProgressBar::SetBGColor(Color c) { m_bg = c; }
void ProgressBar::SetFontColor(Color c) { m_fontColor = c; }
void ProgressBar::SetAnimationSpeed(float s) { m_animSpeed = (s > 0 ? s : 1.0f); }
void ProgressBar::SetGlowSpeed(float s) { m_glowSpeed = s; }
void ProgressBar::SetGlowIntensity(float i) { m_glowIntensity = std::clamp(i, 0.0f, 1.0f); }
void ProgressBar::SetCornerRoundness(float r) { m_roundness = std::clamp(r, 0.0f, 1.0f); }
void ProgressBar::SetBorderColor(Color c) { m_borderColor = c; }
void ProgressBar::SetShowBorder(bool show) { m_showBorder = show; }