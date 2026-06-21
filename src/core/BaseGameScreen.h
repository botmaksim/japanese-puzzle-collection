#pragma once
#include "IScreen.h"
#include <raylib.h>
#include <string>

class BaseGameScreen : public IScreen {
protected:
  ScreenType nextScreen;
  std::string titleText;
  bool isAnimating;
  bool animPaused;
  float animTimer;
  float animSpeed;
  int animStepIndex;

  virtual void ApplyAnimState() = 0;
  virtual void CalculateAnimSteps() = 0;
  virtual int GetAnimStepsCount() const = 0;
  virtual void OnHintPressed() = 0;
  virtual void OnRestartPressed() = 0;
  virtual void UpdateInput() = 0;
  virtual void DrawGame() = 0;

  void DrawButton(Rectangle rect, const char *text);

public:
  BaseGameScreen();
  virtual void Init() override;
  virtual void Update() override;
  virtual void Draw() override;
  virtual ScreenType GetNextScreen() override;
};
