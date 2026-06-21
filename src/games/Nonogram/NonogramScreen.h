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

  bool checkPassed = false;

  bool isDragging = false;
  int dragButton = -1;
  CellState dragTargetState = CellState::EMPTY;
  int lastDragR = -1;
  int lastDragC = -1;

  std::vector<int> GetClueStatus(const std::vector<int> &clues,
                                 const std::vector<CellState> &line);
  bool gameWon = false;

protected:
  void ApplyAnimState() override;
  void CalculateAnimSteps() override;
  int GetAnimStepsCount() const override { return animSteps.size(); }
  void OnCheckPressed() override;
  void OnRestartPressed() override;
  void UpdateInput() override;
  void DrawGame() override;

public:
  void Init() override;
};