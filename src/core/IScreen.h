#pragma once

enum class ScreenType {
  MENU,
  SUDOKU_MENU,
  SUDOKU,
  NONOGRAM_MENU,
  NONOGRAM,
  EXIT
};

class IScreen {
protected:
  int configValue = 0;

public:
  virtual ~IScreen() = default;
  virtual void Init() = 0;
  virtual void Update() = 0;
  virtual void Draw() = 0;
  virtual ScreenType GetNextScreen() = 0;
  virtual int GetConfig() { return configValue; }
  virtual void SetConfig(int config) { configValue = config; }
};