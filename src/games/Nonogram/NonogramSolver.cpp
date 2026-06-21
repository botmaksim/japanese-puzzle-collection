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

void GetPerms(size_t clueIdx, size_t pos, const std::vector<int> &clues,
              const std::vector<CellState> &currentLine,
              std::vector<CellState> &currentPerm,
              std::vector<std::vector<CellState>> &validPerms) {
  if (clueIdx == clues.size()) {
    for (size_t i = pos; i < currentLine.size(); ++i) {
      if (currentLine[i] == CellState::FILLED)
        return;
      currentPerm[i] = CellState::CROSSED;
    }
    validPerms.push_back(currentPerm);
    return;
  }

  int remainingBlocks = clues.size() - clueIdx - 1;
  int minRequired = 0;
  for (size_t i = clueIdx; i < clues.size(); ++i)
    minRequired += clues[i];
  minRequired += remainingBlocks;

  int maxStart = currentLine.size() - minRequired;
  for (int start = pos; start <= maxStart; ++start) {
    bool canPlace = true;
    for (int i = pos; i < start; ++i) {
      if (currentLine[i] == CellState::FILLED) {
        canPlace = false;
        break;
      }
    }
    if (!canPlace)
      break;

    for (int i = 0; i < clues[clueIdx]; ++i) {
      if (currentLine[start + i] == CellState::CROSSED) {
        canPlace = false;
        break;
      }
    }
    if (start + clues[clueIdx] < currentLine.size()) {
      if (currentLine[start + clues[clueIdx]] == CellState::FILLED)
        canPlace = false;
    }

    if (canPlace) {
      for (int i = pos; i < start; ++i)
        currentPerm[i] = CellState::CROSSED;
      for (int i = 0; i < clues[clueIdx]; ++i)
        currentPerm[start + i] = CellState::FILLED;
      if (start + clues[clueIdx] < currentLine.size())
        currentPerm[start + clues[clueIdx]] = CellState::CROSSED;

      GetPerms(clueIdx + 1, start + clues[clueIdx] + 1, clues, currentLine,
               currentPerm, validPerms);
    }
  }
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
      std::vector<CellState> currentPerm(size, CellState::EMPTY);
      std::vector<std::vector<CellState>> validPerms;
      GetPerms(0, 0, logic.GetRowClue(r), currentLine, currentPerm, validPerms);

      if (validPerms.empty())
        continue;

      for (int c = 0; c < size; ++c) {
        if (grid[r][c] == CellState::EMPTY) {
          bool allFilled = true, allCrossed = true;
          for (const auto &perm : validPerms) {
            if (perm[c] != CellState::FILLED)
              allFilled = false;
            if (perm[c] != CellState::CROSSED)
              allCrossed = false;
          }
          if (allFilled) {
            grid[r][c] = CellState::FILLED;
            steps.push_back({1, r, c, r, -1});
            changed = true;
          } else if (allCrossed) {
            grid[r][c] = CellState::CROSSED;
            steps.push_back({0, r, c, r, -1});
            changed = true;
          }
        }
      }
    }

    for (int c = 0; c < size; ++c) {
      std::vector<CellState> currentLine(size);
      for (int r = 0; r < size; ++r)
        currentLine[r] = grid[r][c];

      std::vector<CellState> currentPerm(size, CellState::EMPTY);
      std::vector<std::vector<CellState>> validPerms;
      GetPerms(0, 0, logic.GetColClue(c), currentLine, currentPerm, validPerms);

      if (validPerms.empty())
        continue;

      for (int r = 0; r < size; ++r) {
        if (grid[r][c] == CellState::EMPTY) {
          bool allFilled = true, allCrossed = true;
          for (const auto &perm : validPerms) {
            if (perm[r] != CellState::FILLED)
              allFilled = false;
            if (perm[r] != CellState::CROSSED)
              allCrossed = false;
          }
          if (allFilled) {
            grid[r][c] = CellState::FILLED;
            steps.push_back({1, r, c, -1, c});
            changed = true;
          } else if (allCrossed) {
            grid[r][c] = CellState::CROSSED;
            steps.push_back({0, r, c, -1, c});
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
                std::vector<CellState> currentPerm(size, CellState::EMPTY);
                std::vector<std::vector<CellState>> validPerms;
                GetPerms(0, 0, logic.GetRowClue(tr), currentLine, currentPerm,
                         validPerms);
                if (validPerms.empty()) {
                  conflict = true;
                  break;
                }

                for (int tc = 0; tc < size; ++tc) {
                  if (testGrid[tr][tc] == CellState::EMPTY) {
                    bool allFilled = true, allCrossed = true;
                    for (const auto &perm : validPerms) {
                      if (perm[tc] != CellState::FILLED)
                        allFilled = false;
                      if (perm[tc] != CellState::CROSSED)
                        allCrossed = false;
                    }
                    if (allFilled) {
                      testGrid[tr][tc] = CellState::FILLED;
                      tChanged = true;
                      stepsTaken++;
                    } else if (allCrossed) {
                      testGrid[tr][tc] = CellState::CROSSED;
                      tChanged = true;
                      stepsTaken++;
                    }
                  }
                }
              }
              if (conflict)
                break;

              for (int tc = 0; tc < size; ++tc) {
                std::vector<CellState> currentLine(size);
                for (int tr = 0; tr < size; ++tr)
                  currentLine[tr] = testGrid[tr][tc];
                std::vector<CellState> currentPerm(size, CellState::EMPTY);
                std::vector<std::vector<CellState>> validPerms;
                GetPerms(0, 0, logic.GetColClue(tc), currentLine, currentPerm,
                         validPerms);
                if (validPerms.empty()) {
                  conflict = true;
                  break;
                }

                for (int tr = 0; tr < size; ++tr) {
                  if (testGrid[tr][tc] == CellState::EMPTY) {
                    bool allFilled = true, allCrossed = true;
                    for (const auto &perm : validPerms) {
                      if (perm[tr] != CellState::FILLED)
                        allFilled = false;
                      if (perm[tr] != CellState::CROSSED)
                        allCrossed = false;
                    }
                    if (allFilled) {
                      testGrid[tr][tc] = CellState::FILLED;
                      tChanged = true;
                      stepsTaken++;
                    } else if (allCrossed) {
                      testGrid[tr][tc] = CellState::CROSSED;
                      tChanged = true;
                      stepsTaken++;
                    }
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
        int minPerms = 1000000;
        for (int r = 0; r < size; ++r) {
          for (int c = 0; c < size; ++c) {
            if (grid[r][c] == CellState::EMPTY) {
              std::vector<CellState> cLine = grid[r];
              std::vector<CellState> cPerm(size, CellState::EMPTY);
              std::vector<std::vector<CellState>> vPerms;
              GetPermsInline(0, 0, logic.GetRowClue(r), cLine, cPerm, vPerms);
              int p = vPerms.size();

              std::vector<CellState> cCol(size);
              for (int tr = 0; tr < size; ++tr)
                cCol[tr] = grid[tr][c];
              std::vector<CellState> cColPerm(size, CellState::EMPTY);
              std::vector<std::vector<CellState>> vColPerms;
              GetPermsInline(0, 0, logic.GetColClue(c), cCol, cColPerm,
                             vColPerms);
              if (vColPerms.size() < p)
                p = vColPerms.size();

              if (p < minPerms) {
                minPerms = p;
                bestR = r;
                bestC = c;
              }
            }
          }
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
