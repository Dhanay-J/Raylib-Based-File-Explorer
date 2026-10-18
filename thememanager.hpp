#ifndef THEME_MANAGER_HPP
#define THEME_MANAGER_HPP

#include "raylib.h"
#include "raygui.h"
#include <string>
#include <algorithm>

struct FontProperties
{
    std::string fontPath = "";
    int size = 18;
    int spacing = 0;
    Font fontHandle = {0};
    bool isLoaded = false;
};

class ThemeManager
{
public:
    Color basePrimary;
    Color baseSecondary;
    FontProperties fontProps;

    ThemeManager(Color primary, Color secondary, int fontSize = 18)
            : basePrimary(primary), baseSecondary(secondary)
    {
        fontProps.size = fontSize;
    }

    ~ThemeManager()
    {
        UnloadCustomFont();
    }

    // --- NEW: Load font from memory (for Android assets / embedded fonts) ---
    void LoadCustomFontFromMemory(const std::string &fileType, const unsigned char *data, int dataSize, int size, int spacing = 0)
    {
        UnloadCustomFont();
        fontProps.fontPath = "memory:" + fileType; // Marker for debugging
        fontProps.size = size;
        fontProps.spacing = spacing;

        if (data != nullptr && dataSize > 0)
        {
            // Load font from memory buffer
            fontProps.fontHandle = LoadFontFromMemory(fileType.c_str(), data, dataSize, size, nullptr, 0);

            if (fontProps.fontHandle.texture.id != 0)
            {
                // Set bilinear filtering to keep edges smooth
                SetTextureFilter(fontProps.fontHandle.texture, TEXTURE_FILTER_BILINEAR);

                fontProps.isLoaded = true;
                GuiSetFont(fontProps.fontHandle);
            }
            else
            {
                // Fallback if memory load fails
                GuiSetFont(GetFontDefault());
                fontProps.isLoaded = false;
            }
        }
        else
        {
            GuiSetFont(GetFontDefault());
            fontProps.isLoaded = false;
        }

        GuiSetStyle(DEFAULT, TEXT_SIZE, fontProps.size);
        GuiSetStyle(DEFAULT, TEXT_SPACING, fontProps.spacing);
    }

    // --- Existing: Load font from file path ---
    void LoadCustomFont(const std::string &path, int size, int spacing = 0)
    {
        UnloadCustomFont();
        fontProps.fontPath = path;
        fontProps.size = size;
        fontProps.spacing = spacing;

        if (!path.empty() && FileExists(path.c_str()))
        {
            // Load font at the target size for sharp rendering (no pixelation)
            fontProps.fontHandle = LoadFontEx(path.c_str(), size, nullptr, 0);

            // Set bilinear filtering to keep edges smooth
            SetTextureFilter(fontProps.fontHandle.texture, TEXTURE_FILTER_BILINEAR);

            fontProps.isLoaded = true;
            GuiSetFont(fontProps.fontHandle);
        }
        else
        {
            GuiSetFont(GetFontDefault());
            fontProps.isLoaded = false;
        }

        GuiSetStyle(DEFAULT, TEXT_SIZE, fontProps.size);
        GuiSetStyle(DEFAULT, TEXT_SPACING, fontProps.spacing);
    }

    void SetBaseColors(Color primary, Color secondary)
    {
        basePrimary = primary;
        baseSecondary = secondary;
        ApplyTheme();
    }

    Color GetBackgroundColor() const { return basePrimary; }

    Color GetSurfaceColor() const
    {
        return IsDark(basePrimary) ? Lighten(basePrimary, 0.08f) : Darken(basePrimary, 0.06f);
    }

    Color GetBorderColor() const
    {
        return IsDark(basePrimary) ? Lighten(basePrimary, 0.22f) : Darken(basePrimary, 0.22f);
    }

    Color GetTextColor() const
    {
        return IsDark(basePrimary) ? Color{240, 240, 240, 255} : Color{30, 30, 30, 255};
    }

    Color GetMutedTextColor() const
    {
        return IsDark(basePrimary) ? Color{150, 150, 150, 255} : Color{120, 120, 120, 255};
    }

    Color GetAccentColor() const { return baseSecondary; }

    Color GetAccentHover() const { return Lighten(baseSecondary, 0.12f); }

    Color GetAccentPressed() const { return Darken(baseSecondary, 0.15f); }

    void ApplyTheme()
    {
        Color bg = GetBackgroundColor();
        Color surface = GetSurfaceColor();
        Color border = GetBorderColor();
        Color text = GetTextColor();
        Color accent = GetAccentColor();
        Color accentHover = GetAccentHover();
        Color accentActive = GetAccentPressed();

        GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(bg));
        GuiSetStyle(DEFAULT, LINE_COLOR, ColorToInt(border));
        GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(text));

        GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(surface));
        GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(border));

        GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt(accentHover));
        GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, ColorToInt(accent));
        GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(IsDark(accentHover) ? WHITE : BLACK));

        GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(accentActive));
        GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, ColorToInt(accent));
        GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(IsDark(accentActive) ? WHITE : BLACK));

        GuiSetStyle(DEFAULT, TEXT_SIZE, fontProps.size);
        GuiSetStyle(DEFAULT, TEXT_SPACING, fontProps.spacing);
    }

private:
    void UnloadCustomFont()
    {
        if (fontProps.isLoaded)
        {
            UnloadFont(fontProps.fontHandle);
            fontProps.isLoaded = false;
        }
    }

    static bool IsDark(Color c)
    {
        float lum = (0.299f * c.r + 0.587f * c.g + 0.114f * c.b) / 255.0f;
        return lum < 0.5f;
    }

    static Color Lighten(Color c, float factor)
    {
        return Color{
                (unsigned char)std::min(255.0f, c.r + (255 - c.r) * factor),
                (unsigned char)std::min(255.0f, c.g + (255 - c.g) * factor),
                (unsigned char)std::min(255.0f, c.b + (255 - c.b) * factor),
                c.a};
    }

    static Color Darken(Color c, float factor)
    {
        return Color{
                (unsigned char)std::max(0.0f, c.r * (1.0f - factor)),
                (unsigned char)std::max(0.0f, c.g * (1.0f - factor)),
                (unsigned char)std::max(0.0f, c.b * (1.0f - factor)),
                c.a};
    }
};

#endif // THEME_MANAGER_HPP