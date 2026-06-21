#include "BaseMenuScreen.h"

BaseMenuScreen::BaseMenuScreen(std::string titleText, std::string subText)
    : title(titleText), subtitle(subText) {
  configValue = 0;
}

void BaseMenuScreen::Init() { configValue = 0; }

void BaseMenuScreen::AddButton(std::string text, std::function<void()> action) {
  buttons.push_back({text, action});
}

Rectangle BaseMenuScreen::GetButtonRect(int index) {
  return {250.0f, 180.0f + index * 70.0f, 300.0f, 50.0f};
}

void BaseMenuScreen::Update() {
  Vector2 mouse = GetMousePosition();
  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    for (size_t i = 0; i < buttons.size(); ++i) {
      if (CheckCollisionPointRec(mouse, GetButtonRect(i))) {
        buttons[i].action();
      }
    }
  }
}

void BaseMenuScreen::Draw() {
  int tWidth = MeasureText(title.c_str(), 30);
  int stWidth = MeasureText(subtitle.c_str(), 20);

  DrawText(title.c_str(), (800 - tWidth) / 2, 80, 30, DARKGRAY);
  DrawText(subtitle.c_str(), (800 - stWidth) / 2, 140, 20, GRAY);

  Vector2 mouse = GetMousePosition();
  for (size_t i = 0; i < buttons.size(); ++i) {
    Rectangle rect = GetButtonRect(i);
    bool isHovered = CheckCollisionPointRec(mouse, rect);

    DrawRectangleRec(rect, isHovered ? LIGHTGRAY : GRAY);
    DrawRectangleLinesEx(rect, 2, DARKGRAY);

    int tWidth = MeasureText(buttons[i].text.c_str(), 20);
    DrawText(buttons[i].text.c_str(), rect.x + (rect.width - tWidth) / 2,
             rect.y + 15, 20, BLACK);
  }
}

ScreenType BaseMenuScreen::GetNextScreen() { return nextScreen; }