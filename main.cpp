
#include "raylib.h"
#include "raygui.h"

int main() {
	InitWindow(800, 450, "raygui - Hello World");
	SetTargetFPS(60);

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);

		GuiLabel((Rectangle){300, 170, 200, 30}, "Hello, raygui!");
		if (GuiButton((Rectangle){325, 220, 150, 40}, "OK")) {
			// The button is intentionally non-functional.
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
