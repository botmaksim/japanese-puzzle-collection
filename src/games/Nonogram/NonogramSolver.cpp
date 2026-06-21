#include "NonogramSolver.h"

std::vector<std::pair<int, int>>
NonogramSolver::GetErrors(const NonogramLogic &logic) {
  std::vector<std::pair<int, int>> errs;
  auto playerGrid = logic.GetPlayerGrid();
  auto solution = logic.GetSolutionRaw();
  int gridSize = logic.GetSize();
  for (int r = 0; r < gridSize; ++r) {
    for (int c = 0; c < gridSize; ++c) {
      if (playerGrid[r][c] == CellState::FILLED && !solution[r][c]) {
        errs.push_back({r, c});
      } else if (playerGrid[r][c] == CellState::CROSSED && solution[r][c]) {
        errs.push_back({r, c});
      }
    }
  }
  return errs;
}

std::vector<NonogramStep>
NonogramSolver::GenerateSteps(const NonogramLogic &logic) {
  std::vector<NonogramStep> steps;
  auto grid = logic.GetPlayerGrid();
  auto solution = logic.GetSolutionRaw();
  int size = logic.GetSize();

  bool changed = true;
  while (changed) {
    changed = false;
    for (int r = 0; r < size; ++r) {
      std::vector<CellState> currentLine = grid[r];
      std::vector<CellState> resultLine;
      if (SolveLineDP(logic.GetRowClue(r), currentLine, resultLine)) {
        for (int c = 0; c < size; ++c) {
          if (grid[r][c] == CellState::EMPTY &&
              resultLine[c] != CellState::EMPTY) {
            grid[r][c] = resultLine[c];
            steps.push_back(
                {resultLine[c] == CellState::FILLED ? 1 : 0, r, c, r, -1});
            changed = true;
          }
        }
      }
    }

    for (int c = 0; c < size; ++c) {
      std::vector<CellState> currentLine(size);
      for (int r = 0; r < size; ++r)
        currentLine[r] = grid[r][c];

      std::vector<CellState> resultLine;
      if (SolveLineDP(logic.GetColClue(c), currentLine, resultLine)) {
        for (int r = 0; r < size; ++r) {
          if (grid[r][c] == CellState::EMPTY &&
              resultLine[r] != CellState::EMPTY) {
            grid[r][c] = resultLine[r];
            steps.push_back(
                {resultLine[r] == CellState::FILLED ? 1 : 0, r, c, -1, c});
            changed = true;
          }
        }
      }
    }
    bool full = true;
    for (int r = 0; r < size; ++r) {
      for (int c = 0; c < size; ++c) {
        if (grid[r][c] == CellState::EMPTY)
          full = false;
      }
    }

    if (!full && !changed) {
      int bestR = -1;
      int bestC = -1;
      int bestContraLen = 10000;

      for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
          if (grid[r][c] == CellState::EMPTY) {
            auto testGrid = grid;
            // Guess the WRONG value
            testGrid[r][c] =
                solution[r][c] ? CellState::CROSSED : CellState::FILLED;
            bool conflict = false;
            bool tChanged = true;
            int stepsTaken = 0;

            while (tChanged && !conflict) {
              tChanged = false;
              for (int tr = 0; tr < size; ++tr) {
                std::vector<CellState> currentLine = testGrid[tr];
                std::vector<CellState> resultLine;
                if (!SolveLineDP(logic.GetRowClue(tr), currentLine,
                                 resultLine)) {
                  conflict = true;
                  break;
                }

                for (int tc = 0; tc < size; ++tc) {
                  if (testGrid[tr][tc] == CellState::EMPTY &&
                      resultLine[tc] != CellState::EMPTY) {
                    testGrid[tr][tc] = resultLine[tc];
                    tChanged = true;
                    stepsTaken++;
                  }
                }
              }
              if (conflict)
                break;

              for (int tc = 0; tc < size; ++tc) {
                std::vector<CellState> currentLine(size);
                for (int tr = 0; tr < size; ++tr)
                  currentLine[tr] = testGrid[tr][tc];
                std::vector<CellState> resultLine;
                if (!SolveLineDP(logic.GetColClue(tc), currentLine,
                                 resultLine)) {
                  conflict = true;
                  break;
                }

                for (int tr = 0; tr < size; ++tr) {
                  if (testGrid[tr][tc] == CellState::EMPTY &&
                      resultLine[tr] != CellState::EMPTY) {
                    testGrid[tr][tc] = resultLine[tr];
                    tChanged = true;
                    stepsTaken++;
                  }
                }
              }
            }

            if (conflict) {
              if (stepsTaken < bestContraLen) {
                bestContraLen = stepsTaken;
                bestR = r;
                bestC = c;
              }
            }
          }
        }
      }

      if (bestR == -1) {
        for (int r = 0; r < size; ++r) {
          for (int c = 0; c < size; ++c) {
            if (grid[r][c] == CellState::EMPTY) {
              bestR = r;
              bestC = c;
              break;
            }
          }
          if (bestR != -1)
            break;
        }
      }

      if (solution[bestR][bestC]) {
        steps.push_back({1, bestR, bestC, -1, -1});
        grid[bestR][bestC] = CellState::FILLED;
      } else {
        steps.push_back({0, bestR, bestC, -1, -1});
        grid[bestR][bestC] = CellState::CROSSED;
      }
      changed = true;
    }
  }
  return steps;
}
