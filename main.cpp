#include "raylib.h"
#include "thememanager.hpp"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


#include <string>
#include <vector>

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
        currentPath = GetWorkingDirectory();
        Refresh();
    }

    void Refresh()
    {
        entries.clear();
        entries.push_back("<- Back");

        FilePathList files = LoadDirectoryFiles(currentPath.c_str());

        if (files.count == 0 && !DirectoryExists(currentPath.c_str()))
        {
            showError = true;
            errorMessage = "Permission Denied: Cannot access directory.";
            UnloadDirectoryFiles(files);
            return;
        }

        for (unsigned int i = 0; i < files.count; i++)
        {
            std::string path = files.paths[i];
            std::string name = GetFileName(path.c_str());

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
    InitWindow(760, 520, "FSBrowser - Themed File & Folder Picker");
    SetTargetFPS(60);

    // Initialize ThemeManager with White & Blue base colors
    // Base 1: Canvas/Background color
    // Base 2: Brand/Accent color
    ThemeManager theme(Color{245, 246, 250, 255}, Color{41, 128, 185, 255}, 15);
    theme.ApplyTheme();

    FSBrowser browser;
    std::string selectedDisplayName = "None";
    std::string selectedFullPath = "";

    while (!WindowShouldClose())
    {
        // ----------------- DRAG AND DROP -----------------
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

        // ----------------- DRAWING -----------------
        BeginDrawing();
        ClearBackground(theme.GetBackgroundColor());

        // Header
        GuiLabel((Rectangle){30, 20, 260, 26}, "File & Folder Browser");

        // Quick Preset Theme Switchers
        if (GuiButton((Rectangle){420, 20, 95, 26}, "White-Blue"))
        {
            theme.SetBaseColors(Color{245, 246, 250, 255}, Color{41, 128, 185, 255});
        }
        if (GuiButton((Rectangle){525, 20, 105, 26}, "Black-Orange"))
        {
            theme.SetBaseColors(Color{25, 25, 28, 255}, Color{230, 126, 34, 255});
        }
        if (GuiButton((Rectangle){640, 20, 90, 26}, "Dark-Teal"))
        {
            theme.SetBaseColors(Color{20, 30, 35, 255}, Color{26, 188, 156, 255});
        }

        // Action Button
        if (GuiButton((Rectangle){30, 65, 160, 36}, "#03# Browse..."))
        {
            browser.Open();
        }

        // Selected Label
        std::string labelText = "Selected: " + selectedDisplayName;
        GuiLabel((Rectangle){30, 120, 700, 26}, labelText.c_str());

        // Subtitle / Hint using derived muted text color
        Color muted = theme.GetMutedTextColor();
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(muted));
        GuiLabel((Rectangle){30, 155, 700, 24}, "Desktop tip: Drag & drop files or directories into this window anytime.");
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, ColorToInt(theme.GetTextColor()));

        // ----------------- MODAL BROWSER DIALOG -----------------
        if (browser.active)
        {
            // Dim background overlay
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.55f));

            // Dialog Window
            Rectangle dialogRect = {40, 30, 680, 450};
            GuiPanel(dialogRect, "Select File or Folder");

            // Current Directory
            GuiLabel((Rectangle){55, 65, 650, 24}, browser.currentPath.c_str());

            // Build item list string
            std::string listText = "";
            for (size_t i = 0; i < browser.entries.size(); i++)
            {
                listText += browser.entries[i];
                if (i + 1 < browser.entries.size())
                    listText += ";";
            }

            int prevActive = browser.activeItem;
            GuiListView((Rectangle){55, 95, 650, 285}, listText.c_str(), &browser.scrollIndex, &browser.activeItem);

            // Directory traversal
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

            // Bottom Buttons
            if (GuiButton((Rectangle){240, 395, 180, 34}, "Select Current Folder"))
            {
                selectedFullPath = browser.currentPath;
                selectedDisplayName = GetFileName(selectedFullPath.c_str());
                if (selectedDisplayName.empty())
                    selectedDisplayName = selectedFullPath;
                browser.active = false;
            }

            if (GuiButton((Rectangle){435, 395, 120, 34}, "Select File"))
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

            if (GuiButton((Rectangle){570, 395, 135, 34}, "Cancel"))
            {
                browser.active = false;
            }

            // Permission Error Popup
            if (browser.showError)
            {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.4f));

                int msgActive = -1;
                GuiMessageBox((Rectangle){180, 175, 400, 150},
                              "#187#Access Error",
                              browser.errorMessage.c_str(),
                              "OK",
                              &msgActive);

                if (msgActive >= 0)
                {
                    browser.showError = false;
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}