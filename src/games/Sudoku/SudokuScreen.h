#pragma once
#include "BaseGameScreen.h"
#include "SudokuLogic.h"
#include "SudokuSolver.h"
#include <vector>

class SudokuScreen : public BaseGameScreen {
private:
  SudokuLogic logic;
  int selectedRow;
  int selectedCol;
  bool isNoteMode;

  std::vector<SudokuStep> animSteps;
  std::vector<std::vector<char>> baseBoard;
  std::vector<std::vector<std::bitset<10>>> baseNotes;
  std::vector<std::pair<int, int>> errorHighlightCells;

  int hintCellR = -1;
  int hintCellC = -1;

  const int CELL_SIZE = 50;
  const int OFFSET_X = 150;
  const int OFFSET_Y = 75;

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