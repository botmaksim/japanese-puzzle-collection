#pragma once
#include "NonogramLogic.h"
#include <vector>

struct NonogramStep {
  int type;
  int r, c;
  int reasonR;
  int reasonC;
};

class NonogramSolver {
public:
  static std::vector<NonogramStep> GenerateSteps(const NonogramLogic &logic);
  static std::vector<std::pair<int, int>> GetErrors(const NonogramLogic &logic);
};
