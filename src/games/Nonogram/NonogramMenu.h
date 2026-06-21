#pragma once
#include "BaseMenuScreen.h"
#include <string>

class NonogramMenu : public BaseMenuScreen {
private:
  bool isCustomMode;
  std::string customInputStr;
  void ShowMainOptions();
  void ShowCustomOptions();

public:
  NonogramMenu();
  void Init() override;
  void Update() override;
  void Draw() override;
};