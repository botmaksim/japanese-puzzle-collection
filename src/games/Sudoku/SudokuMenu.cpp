#include "SudokuMenu.h"

SudokuMenu::SudokuMenu() : BaseMenuScreen("SUDOKU", "- SELECT DIFFICULTY -") {}

void SudokuMenu::Init() {
  BaseMenuScreen::Init();
  nextScreen = ScreenType::SUDOKU_MENU;
  buttons.clear();
  AddButton("Easy", [this]() {
    configValue = 0;
    nextScreen = ScreenType::SUDOKU;
  });

  AddButton("Medium", [this]() {
    configValue = 1;
    nextScreen = ScreenType::SUDOKU;
  });

  AddButton("Hard", [this]() {
    configValue = 2;
    nextScreen = ScreenType::SUDOKU;
  });

  AddButton("Impossible", [this]() {
    configValue = 3;
    nextScreen = ScreenType::SUDOKU;
  });

  AddButton("<- Back", [this]() { nextScreen = ScreenType::MENU; });
}