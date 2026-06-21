#include "NonogramMenu.h"

NonogramMenu::NonogramMenu() : BaseMenuScreen("NONOGRAM", "- SELECT SIZE -") {}

void NonogramMenu::Init() {
  BaseMenuScreen::Init();
  nextScreen = ScreenType::NONOGRAM_MENU;
  customInputStr = "";
  ShowMainOptions();
}

void NonogramMenu::ShowMainOptions() {
  isCustomMode = false;
  subtitle = "- NONOGRAM SIZE -";
  buttons.clear();

  AddButton("Beginner (5x5)", [this]() {
    configValue = 5;
    nextScreen = ScreenType::NONOGRAM;
  });
  AddButton("Classic (10x10)", [this]() {
    configValue = 10;
    nextScreen = ScreenType::NONOGRAM;
  });
  AddButton("Expert (15x15)", [this]() {
    configValue = 15;
    nextScreen = ScreenType::NONOGRAM;
  });
  AddButton("Custom Size...", [this]() { ShowCustomOptions(); });
  AddButton("<- Back", [this]() { nextScreen = ScreenType::MENU; });
}

void NonogramMenu::ShowCustomOptions() {
  isCustomMode = true;
  subtitle = "- ENTER CUSTOM SIZE (5-40) -";
  buttons.clear();
  AddButton("", []() {});

  AddButton("PLAY", [this]() {
    int size = customInputStr.empty() ? 5 : std::stoi(customInputStr);
    if (size < 5)
      size = 5;
    if (size > 40)
      size = 40;
    configValue = size;
    nextScreen = ScreenType::NONOGRAM;
  });

  AddButton("<- Back", [this]() { ShowMainOptions(); });
}

void NonogramMenu::Update() {
  if (isCustomMode) {
    int key = GetCharPressed();
    while (key > 0) {
      if ((key >= '0') && (key <= '9') && (customInputStr.length() < 2)) {
        customInputStr += (char)key;
      }
      key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && !customInputStr.empty()) {
      customInputStr.pop_back();
    }
    if (IsKeyPressed(KEY_ENTER) && !customInputStr.empty()) {
      int size = std::stoi(customInputStr);
      if (size < 5)
        size = 5;
      if (size > 40)
        size = 40;
      configValue = size;
      nextScreen = ScreenType::NONOGRAM;
    }
  }
  BaseMenuScreen::Update();
}

void NonogramMenu::Draw() {
  BaseMenuScreen::Draw();
  if (isCustomMode) {
    Rectangle inputBox = GetButtonRect(0);
    DrawRectangleRec(inputBox, WHITE);
    DrawRectangleLinesEx(inputBox, 2, BLACK);
    int textWidth = MeasureText(customInputStr.c_str(), 24);
    DrawText(customInputStr.c_str(),
             inputBox.x + (inputBox.width - textWidth) / 2, inputBox.y + 15, 24,
             BLACK);
    if ((int)(GetTime() * 2) % 2 == 0) {
      DrawText("_", inputBox.x + (inputBox.width + textWidth) / 2 + 2,
               inputBox.y + 15, 24, BLACK);
    }
  }
}