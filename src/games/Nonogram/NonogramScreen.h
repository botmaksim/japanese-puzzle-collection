#pragma once
#include "BaseGameScreen.h"
#include "NonogramLogic.h"
#include "NonogramSolver.h"
#include <vector>

class NonogramScreen : public BaseGameScreen {
private:
  NonogramLogic logic;

  int cellSize;
  int offsetX;
  int offsetY;

  std::vector<NonogramStep> animSteps;
  std::vector<std::vector<CellState>> baseBoard;
  std::vector<std::pair<int, int>> errorHighlightCells;

  int hintCellR = -1;
  int hintCellC = -1;

protected:
  void ApplyAnimState() override;
  void CalculateAnimSteps() override;
  int GetAnimStepsCount() const override { return animSteps.size(); }
  void OnHintPressed() override;
  void OnRestartPressed() override;
  void UpdateInput() override;
  void DrawGame() override;

public:
  void Init() override;
};