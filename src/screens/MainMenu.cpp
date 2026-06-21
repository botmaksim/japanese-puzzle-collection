#include "MainMenu.h"

MainMenu::MainMenu()
    : BaseMenuScreen("JAPANESE PUZZLE COLLECTION", "- CHOOSE GAME -") {}

void MainMenu::Init() {
  BaseMenuScreen::Init();
  nextScreen = ScreenType::MENU;
  buttons.clear();
  AddButton("Play Sudoku", [this]() { nextScreen = ScreenType::SUDOKU_MENU; });
  AddButton("Play Nonogram",
            [this]() { nextScreen = ScreenType::NONOGRAM_MENU; });
  AddButton("Exit", [this]() { nextScreen = ScreenType::EXIT; });
}