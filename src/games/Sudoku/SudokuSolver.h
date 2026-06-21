#pragma once
#include "SudokuLogic.h"
#include <vector>

struct SudokuStep {
  int type;
  int r, c;
  int val;
  int reasonType;
  int reasonR, reasonC;
  std::vector<std::pair<int, int>> multiTargets;
};

class SudokuSolver {
public:
  static std::vector<SudokuStep> GenerateSteps(const SudokuLogic &logic);
  static std::vector<std::pair<int, int>> GetErrors(const SudokuLogic &logic);
};
