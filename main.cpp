#include <cstdio>
#include <sys/stat.h>
#include <vector>
#include <string>
#include <iostream>

// Include the generated header
#include "assets/roboto_font.h"

#include "raylib.h"
#include "thememanager.hpp"
#include "progressbar.hpp"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// --- Cross-Platform Home Directory Helper ---
std::string getHomeDir()
{
#ifdef __ANDROID__
    return "/storage/emulated/0";
#elif defined(_WIN32) || defined(_WIN64)
    const char *home = std::getenv("USERPROFILE");
    if (home)
        return std::string(home);
    return "C:/";
#else
    const char *home = std::getenv("HOME");
    if (home)
        return std::string(home);
    return "/";
#endif
}

// --- Permission Handling JNI ---
static bool g_storagePermissionGranted = false;

bool hasStorageAccess()
{
    return g_storagePermissionGranted;
}

// --- FSBrowser Structure ---
struct FSBrowser
{
    bool active = false;
    std::string currentPath = "";
    std::vector<std::string> entries;
    int scrollIndex = 0;
    int activeItem = -1;
    bool showError = false;
    std::string errorMessage = "";

    void Open()
    {
        active = true;
        std::string startPath = getHomeDir();
        if (!DirectoryExists(startPath.c_str()))
        {
            startPath = GetApplicationDirectory();
        }
        currentPath = startPath;
        Refresh();
    }

    void Refresh()
    {
        entries.clear();

#ifdef __ANDROID__
        if (currentPath != "/storage/emulated/0")
#else
        if (currentPath != "/" && currentPath != getHomeDir())
#endif
        {
            entries.push_back("<- Back");
        }

        FilePathList files = LoadDirectoryFiles(currentPath.c_str());

        if (files.count == 0 && !DirectoryExists(currentPath.c_str()))
        {
            showError = true;
            errorMessage = "Error: Cannot access directory '" + currentPath + "'";
            UnloadDirectoryFiles(files);
            return;
        }

        for (unsigned int i = 0; i < files.count; i++)
        {
            std::string path = files.paths[i];
            std::string name = GetFileName(path.c_str());
            if (!name.empty() && name[0] == '.')
                continue;

            if (DirectoryExists(path.c_str()))
            {
                entries.push_back("[DIR] " + name);
            }
            else
            {
                entries.push_back(name);
            }
        }
        UnloadDirectoryFiles(files);
        activeItem = -1;
    }
};

int main()
{
    InitWindow(760, 560, "FSBrowser & Progress Bar Demo");
    SetTargetFPS(60);

    ThemeManager theme(Color{245, 246, 250, 255}, Color{41, 128, 185, 255}, 15);

    // --- LOAD FONT FROM MEMORY ---
    theme.LoadCustomFontFromMemory(
        ".ttf",
        assets_Roboto_Regular_ttf,
        assets_Roboto_Regular_ttf_len,
        18,
        0);

    if (theme.fontProps.isLoaded)
    {
        printf("[INFO] Custom font loaded from memory successfully.\n");
    }
    else
    {
        printf("[ERROR] Failed to load custom font from memory.\n");
    }

    theme.ApplyTheme();

    FSBrowser browser;
    std::string selectedDisplayName = "None";
    std::string selectedFullPath = "";

    // --- Animated Progress Bar ---
    ProgressBar progressBar;
    progressBar.SetMin(0.0f);
    progressBar.SetMax(100.0f);
    progressBar.SetStep(1.0f);
    progressBar.SetWidth(540.0f);
    progressBar.SetHeight(2.0f);
    progressBar.SetFGColor(Color{41, 128, 185, 255});
    progressBar.SetBGColor(Color{225, 228, 235, 255});
    progressBar.SetText("0%");
    progressBar.SetTextPosition(TextPosition::Center);
    progressBar.SetAnimationSpeed(80.0f);
    progressBar.SetGlowSpeed(4.0f);
    progressBar.SetGlowIntensity(0.35f);
    progressBar.SetCornerRoundness(5.0f);
    progressBar.SetFont(theme.fontProps.fontHandle);

    // --- Simulated task state (non-blocking, driven by dt) ---
    bool simRunning = false;
    bool simLoop = true;
    float simValue = progressBar.GetMin();
    float simSpeed = 35.0f;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();
            if (droppedFiles.count > 0)
            {
                selectedFullPath = droppedFiles.paths[0];
                selectedDisplayName = GetFileName(selectedFullPath.c_str());
                if (selectedDisplayName.empty())
                    selectedDisplayName = selectedFullPath;
            }
            UnloadDroppedFiles(droppedFiles);
        }

        BeginDrawing();
        ClearBackground(theme.GetBackgroundColor());

        // Header
        GuiLabel((Rectangle){30, 15, 260, 26}, "File & Folder Browser");

        // Theme Pickers
        if (GuiButton((Rectangle){420, 15, 95, 24}, "White-Blue"))
        {
            theme.SetBaseColors(Color{245, 246, 250, 255}, Color{41, 128, 185, 255});
            progressBar.SetFGColor(Color{41, 128, 185, 255});
            progressBar.SetBGColor(Color{225, 228, 235, 255});
        }
        if (GuiButton((Rectangle){525, 15, 105, 24}, "Black-Orange"))
        {
            theme.SetBaseColors(Color{25, 25, 28, 255}, Color{230, 126, 34, 255});
            progressBar.SetFGColor(Color{230, 126, 34, 255});
            progressBar.SetBGColor(Color{45, 45, 50, 255});
        }
        if (GuiButton((Rectangle){640, 15, 90, 24}, "Dark-Teal"))
        {
            theme.SetBaseColors(Color{20, 30, 35, 255}, Color{26, 188, 156, 255});
            progressBar.SetFGColor(Color{26, 188, 156, 255});
            progressBar.SetBGColor(Color{35, 48, 55, 255});
        }

        // ====================================================================
        // --- ANIMATED PROGRESS BAR SECTION ---
        // ====================================================================
        GuiGroupBox((Rectangle){30, 50, 700, 110}, "Operation Status & Progress");

        if (simRunning)
        {
            simValue += simSpeed * dt;
            if (simValue >= progressBar.GetMax())
            {
                if (simLoop)
                    simValue = progressBar.GetMin();
                else
                {
                    simValue = progressBar.GetMax();
                    simRunning = false;
                }
            }
            progressBar.SetValue(simValue);
        }

        // Push percentage text (updated every frame so it stays in sync)
        char pctBuf[32];
        snprintf(pctBuf, sizeof(pctBuf), "%.2f%%", progressBar.GetPercentage() * 100.0f);
        progressBar.SetText(pctBuf);

        // Drives smooth chase + glow phase
        progressBar.Update(dt);

        // Render the animated rounded progress bar with inside glow
        progressBar.Draw((Rectangle){110, 70, 540, 26});

        // Controls: Run/Pause, Reset
        const char *playText = simRunning ? "Pause" : "Simulate";
        if (GuiButton((Rectangle){110, 110, 100, 28}, playText))
            simRunning = !simRunning;

        if (GuiButton((Rectangle){220, 110, 80, 28}, "Reset"))
        {
            simRunning = false;
            simValue = progressBar.GetMin();
            progressBar.Reset();
            progressBar.Update(0.0f); // snap instantly, skip the chase animation
        }

        // Loop Toggle
        GuiCheckBox((Rectangle){320, 115, 18, 18}, "Loop animation", &simLoop);

        // Manual slider when paused (only write back when actually dragging)
        if (!simRunning)
        {
            float v = progressBar.GetValue();
            if (GuiSlider((Rectangle){510, 115, 140, 18}, "Manual: ", "",
                          &v, progressBar.GetMin(), progressBar.GetMax()))
            {
                progressBar.SetValue(v);
                simValue = progressBar.GetValue();
            }
        }
        // ====================================================================

        // File browser trigger button
        if (GuiButton((Rectangle){30, 175, 160, 34}, "#03# Browse..."))
        {
            browser.Open();
        }

        std::string labelText = "Selected: " + selectedDisplayName;
        GuiLabel((Rectangle){30, 220, 700, 26}, labelText.c_str());

        Color muted = theme.GetMutedTextColor();
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(muted));
        GuiLabel((Rectangle){30, 250, 700, 24}, "Tip: Drag & drop files or run simulated progress for copying/loading operations.");
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(theme.GetTextColor()));

        // Modal Browser
        if (browser.active)
        {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));
            Rectangle dialogRect = {40, 40, 680, 460};
            GuiPanel(dialogRect, "Select File or Folder");
            GuiLabel((Rectangle){55, 75, 650, 24}, browser.currentPath.c_str());

            std::string listText = "";
            for (size_t i = 0; i < browser.entries.size(); i++)
            {
                listText += browser.entries[i];
                if (i + 1 < browser.entries.size())
                    listText += ";";
            }

            int prevActive = browser.activeItem;
            GuiListView((Rectangle){55, 105, 650, 290}, listText.c_str(), &browser.scrollIndex, &browser.activeItem);

            if (browser.activeItem != -1 && browser.activeItem != prevActive)
            {
                std::string chosen = browser.entries[browser.activeItem];
                if (chosen == "<- Back")
                {
                    browser.currentPath = GetPrevDirectoryPath(browser.currentPath.c_str());
                    browser.Refresh();
                }
                else if (chosen.rfind("[DIR] ", 0) == 0)
                {
                    std::string folderName = chosen.substr(6);
                    std::string nextPath = browser.currentPath + "/" + folderName;
                    if (!DirectoryExists(nextPath.c_str()))
                    {
                        browser.showError = true;
                        browser.errorMessage = "Permission Denied: Cannot access folder '" + folderName + "'";
                    }
                    else
                    {
                        browser.currentPath = nextPath;
                        browser.Refresh();
                    }
                }
            }

            if (GuiButton((Rectangle){240, 410, 180, 34}, "Select Current Folder"))
            {
                selectedFullPath = browser.currentPath;
                selectedDisplayName = GetFileName(selectedFullPath.c_str());
                if (selectedDisplayName.empty())
                    selectedDisplayName = selectedFullPath;
                browser.active = false;
            }

            if (GuiButton((Rectangle){435, 410, 120, 34}, "Select File"))
            {
                if (browser.activeItem >= 0)
                {
                    std::string chosen = browser.entries[browser.activeItem];
                    if (chosen != "<- Back" && chosen.rfind("[DIR] ", 0) != 0)
                    {
                        selectedFullPath = browser.currentPath + "/" + chosen;
                        selectedDisplayName = chosen;
                        browser.active = false;
                    }
                }
            }

            if (GuiButton((Rectangle){570, 410, 135, 34}, "Cancel"))
            {
                browser.active = false;
            }

            if (browser.showError)
            {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.4f));
                int msgActive = -1;
                GuiMessageBox((Rectangle){180, 185, 400, 150},
                              "#187#Access Error",
                              browser.errorMessage.c_str(),
                              "OK",
                              &msgActive);
                if (msgActive >= 0)
                    browser.showError = false;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}