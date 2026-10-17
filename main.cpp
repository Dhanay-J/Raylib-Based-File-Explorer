#include "raylib.h"

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

    // Error handling state
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
        entries.push_back("<- Back"); // Navigation indicator

        FilePathList files = LoadDirectoryFiles(currentPath.c_str());

        // Permission check: failed load on an existing path indicates unreadable directory
        if (files.count == 0 && !DirectoryExists(currentPath.c_str()))
        {
            showError = true;
            errorMessage = "Permission Denied: Unable to read directory contents.";
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
    InitWindow(740, 500, "FSBrowser - File & Folder Picker");
    SetTargetFPS(60);

    // Increase default raygui font size by 4-5px (default is 10)
    GuiSetStyle(DEFAULT, TEXT_SIZE, 15);

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
                {
                    selectedDisplayName = selectedFullPath; // Fallback for root drives
                }
            }
            UnloadDroppedFiles(droppedFiles);
        }

        // ----------------- DRAWING -----------------
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        // Header
        GuiLabel((Rectangle){30, 24, 400, 26}, "File & Folder Browser");

        // Combined Open Button
        if (GuiButton((Rectangle){30, 60, 160, 36}, "#03# Browse..."))
        {
            browser.Open();
        }

        // Display Selected Name Only
        std::string labelText = "Selected: " + selectedDisplayName;
        GuiLabel((Rectangle){30, 115, 680, 26}, labelText.c_str());

        // Secondary hint
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0x888888FF);
        GuiLabel((Rectangle){30, 150, 680, 24}, "Desktop tip: You can also drag & drop any file or folder directly here.");
        GuiSetStyle(LABEL, TEXT_COLOR_NORMAL, 0x1B1B1BFF);

        // ----------------- MODAL BROWSER DIALOG -----------------
        if (browser.active)
        {
            // Dim background overlay
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));

            // Dialog Window
            Rectangle dialogRect = {40, 30, 660, 430};
            GuiPanel(dialogRect, "Select File or Folder");

            // Current Directory Path
            GuiLabel((Rectangle){55, 65, 630, 24}, browser.currentPath.c_str());

            // Build item list string for GuiListView
            std::string listText = "";
            for (size_t i = 0; i < browser.entries.size(); i++)
            {
                listText += browser.entries[i];
                if (i + 1 < browser.entries.size())
                    listText += ";";
            }

            int prevActive = browser.activeItem;
            GuiListView((Rectangle){55, 95, 630, 265}, listText.c_str(), &browser.scrollIndex, &browser.activeItem);

            // Item selection / directory navigation
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

                    // Verify read permission on target directory
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

            // Button: Select active folder
            if (GuiButton((Rectangle){230, 375, 180, 34}, "Select Current Folder"))
            {
                selectedFullPath = browser.currentPath;
                selectedDisplayName = GetFileName(selectedFullPath.c_str());
                if (selectedDisplayName.empty())
                    selectedDisplayName = selectedFullPath;
                browser.active = false;
            }

            // Button: Select file
            if (GuiButton((Rectangle){425, 375, 120, 34}, "Select File"))
            {
                if (browser.activeItem >= 0)
                {
                    std::string chosen = browser.entries[browser.activeItem];
                    if (chosen != "<- Back" && chosen.rfind("[DIR] ", 0) != 0)
                    {
                        selectedFullPath = browser.currentPath + "/" + chosen;
                        selectedDisplayName = chosen; // File name only
                        browser.active = false;
                    }
                }
            }

            // Button: Cancel
            if (GuiButton((Rectangle){560, 375, 125, 34}, "Cancel"))
            {
                browser.active = false;
            }

            // ----------------- PERMISSION ERROR POPUP -----------------
            if (browser.showError)
            {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.4f));

                int msgActive = -1;
                GuiMessageBox((Rectangle){170, 160, 400, 150},
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