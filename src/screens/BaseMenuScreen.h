#pragma once
#include "IScreen.h"
#include <functional>
#include <raylib.h>
#include <string>
#include <vector>
struct MenuButton {
  std::string text;
  std::function<void()> action;
};

class BaseMenuScreen : public IScreen {
protected:
  ScreenType nextScreen;
  std::string title;
  std::string subtitle;
  std::vector<MenuButton> buttons;

  Rectangle GetButtonRect(int index);

public:
  BaseMenuScreen(std::string titleText, std::string subText);
  virtual void Init() override;
  virtual void Update() override;
  virtual void Draw() override;
  virtual ScreenType GetNextScreen() override;
  void AddButton(std::string text, std::function<void()> action);
};