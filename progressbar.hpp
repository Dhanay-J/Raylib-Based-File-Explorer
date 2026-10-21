#pragma once
#include "raylib.h"
#include <string>

enum class TextPosition
{
    None,
    Center,
    Left,
    Right,
    InsideLeft,
    InsideRight
};

class ProgressBar
{
public:
    ProgressBar(float width = 300.0f, float height = 24.0f,
                float min = 0.0f, float max = 100.0f, float step = 1.0f);
    ~ProgressBar();

    void Update(float deltaTime);
    void Draw(float x, float y) const;
    void Draw(Rectangle bounds) const;

    void SetValue(float v);
    void Increment();
    void Decrement();
    void Reset();

    // Getters
    float GetValue() const;
    float GetMin() const;
    float GetMax() const;
    float GetStep() const;
    float GetWidth() const;
    float GetHeight() const;
    float GetPercentage() const;
    const std::string &GetText() const;
    TextPosition GetTextPosition() const;
    Color GetFGColor() const;
    Color GetBGColor() const;
    Color GetFontColor() const;
    bool IsAnimating() const;

    // Setters
    void SetMin(float v);
    void SetMax(float v);
    void SetStep(float v);
    void SetWidth(float w);
    void SetHeight(float h);
    void SetText(const std::string &t);
    void SetTextPosition(TextPosition p);
    void SetFGColor(Color c);
    void SetBGColor(Color c);
    void SetFontColor(Color c);
    void SetFont(Font font);

    // Appearance
    void SetBorderColor(Color c);
    void SetShowBorder(bool show);
    void SetAnimationSpeed(float speed);
    void SetGlowSpeed(float speed);
    void SetGlowIntensity(float intensity);
    void SetCornerRoundness(float r);

private:
    float m_width, m_height;
    float m_min, m_max, m_step;
    float m_value;
    float m_displayValue;
    float m_animSpeed;
    float m_glowPhase;
    float m_glowSpeed;
    float m_glowIntensity;
    float m_roundness;
    bool m_showBorder;
    Color m_borderColor;
    Font m_font;
    bool m_hasCustomFont = false;
    std::string m_text;
    TextPosition m_textPos;
    Color m_fg, m_bg;
    Color m_fontColor;

    void ClampAndSnap();
};