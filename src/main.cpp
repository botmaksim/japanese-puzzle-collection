#include "IScreen.h"
#include "MainMenu.h"
#include "NonogramMenu.h"
#include "NonogramScreen.h"
#include "SudokuMenu.h"
#include "SudokuScreen.h"
#include <algorithm>
#include <memory>
#include <raylib.h>

int main() {
  const float virtualWidth = 800.0f;
  const float virtualHeight = 600.0f;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow((int)virtualWidth, (int)virtualHeight,
             "Japanese Puzzle Collection");
  SetTargetFPS(60);
  auto menu = std::make_unique<MainMenu>();
  auto sudokuMenu = std::make_unique<SudokuMenu>();
  auto sudokuGame = std::make_unique<SudokuScreen>();
  auto nonogramMenu = std::make_unique<NonogramMenu>();
  auto nonogramGame = std::make_unique<NonogramScreen>();

  IScreen *currentScreen = menu.get();
  currentScreen->Init();
  ScreenType currentType = ScreenType::MENU;
  Camera2D camera = {0};
  camera.target = {0.0f, 0.0f};
  camera.rotation = 0.0f;

  while (!WindowShouldClose() && currentType != ScreenType::EXIT) {
    if (IsKeyPressed(KEY_F11) ||
        (IsKeyDown(KEY_LEFT_ALT) && IsKeyPressed(KEY_ENTER))) {
      ToggleFullscreen();
    }
    float scale = std::min((float)GetScreenWidth() / virtualWidth,
                           (float)GetScreenHeight() / virtualHeight);
    camera.zoom = scale;
    camera.offset.x = (GetScreenWidth() - (virtualWidth * scale)) * 0.5f;
    camera.offset.y = (GetScreenHeight() - (virtualHeight * scale)) * 0.5f;
    SetMouseOffset(-(int)camera.offset.x, -(int)camera.offset.y);
    SetMouseScale(1.0f / scale, 1.0f / scale);
    currentScreen->Update();
    ScreenType nextType = currentScreen->GetNextScreen();
    if (nextType != currentType) {
      int savedConfig = currentScreen->GetConfig();

      currentType = nextType;
      switch (currentType) {
      case ScreenType::MENU:
        currentScreen = menu.get();
        break;
      case ScreenType::SUDOKU_MENU:
        currentScreen = sudokuMenu.get();
        break;
      case ScreenType::SUDOKU:
        currentScreen = sudokuGame.get();
        break;
      case ScreenType::NONOGRAM_MENU:
        currentScreen = nonogramMenu.get();
        break;
      case ScreenType::NONOGRAM:
        currentScreen = nonogramGame.get();
        break;
      case ScreenType::EXIT:
        break;
      }

      if (currentType != ScreenType::EXIT) {
        if (currentType == ScreenType::SUDOKU ||
            currentType == ScreenType::NONOGRAM) {
          currentScreen->SetConfig(savedConfig);
        }
        currentScreen->Init();
      }
    }
    BeginDrawing();
    ClearBackground(RAYWHITE);

    BeginMode2D(camera);
    if (currentType != ScreenType::EXIT) {
      currentScreen->Draw();
    }
    EndMode2D();

    EndDrawing();
  }

  CloseWindow();
  return 0;
}