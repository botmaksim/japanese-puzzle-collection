#include "SudokuSolver.h"

std::vector<std::pair<int, int>>
SudokuSolver::GetErrors(const SudokuLogic &logic) {
  std::vector<std::pair<int, int>> errs;
  auto board = logic.GetBoardRaw();
  auto trueSolution = logic.GetTrueSolution();
  for (int r = 0; r < 9; ++r) {
    for (int c = 0; c < 9; ++c) {
      if (board[r][c] != '.' && board[r][c] != trueSolution[r][c]) {
        errs.push_back({r, c});
      }
    }
  }
  return errs;
}

std::vector<SudokuStep> SudokuSolver::GenerateSteps(const SudokuLogic &logic) {
  std::vector<SudokuStep> steps;
  std::vector<std::vector<char>> simBoard = logic.GetBoardRaw();
  std::vector<std::vector<std::bitset<10>>> simNotes = logic.GetNotesRaw();

  auto placeValue = [&](int r, int c, int v, int reasonType, int reasonR,
                        int reasonC) {
    simBoard[r][c] = '0' + v;
    simNotes[r][c].reset();
    steps.push_back({1, r, c, v, reasonType, reasonR, reasonC, {}});
    std::vector<std::pair<int, int>> elims;
    for (int i = 0; i < 9; ++i) {
      if (simBoard[r][i] == '.' && simNotes[r][i][v]) {
        simNotes[r][i].reset(v);
        elims.push_back({r, i});
      }
    }
    for (int i = 0; i < 9; ++i) {
      if (simBoard[i][c] == '.' && simNotes[i][c][v]) {
        simNotes[i][c].reset(v);
        elims.push_back({i, c});
      }
    }
    int br = (r / 3) * 3, bc = (c / 3) * 3;
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        if (simBoard[br + i][bc + j] == '.' && simNotes[br + i][bc + j][v]) {
          simNotes[br + i][bc + j].reset(v);
          elims.push_back({br + i, bc + j});
        }
      }
    }
    if (!elims.empty()) {
      steps.push_back({2, r, c, v, 1, r, c, elims});
    }
  };
  for (int r = 0; r < 9; ++r) {
    for (int c = 0; c < 9; ++c) {
      if (simBoard[r][c] != '.') {
        int v = simBoard[r][c] - '0';
        std::vector<std::pair<int, int>> elims;
        for (int i = 0; i < 9; ++i) {
          if (simBoard[r][i] == '.' && simNotes[r][i][v]) {
            simNotes[r][i].reset(v);
            elims.push_back({r, i});
          }
          if (simBoard[i][c] == '.' && simNotes[i][c][v]) {
            simNotes[i][c].reset(v);
            elims.push_back({i, c});
          }
        }
        int br = (r / 3) * 3, bc = (c / 3) * 3;
        for (int i = 0; i < 3; ++i) {
          for (int j = 0; j < 3; ++j) {
            if (simBoard[br + i][bc + j] == '.' &&
                simNotes[br + i][bc + j][v]) {
              simNotes[br + i][bc + j].reset(v);
              elims.push_back({br + i, bc + j});
            }
          }
        }
        if (!elims.empty()) {
          steps.push_back({2, r, c, v, 1, r, c, elims});
        }
      }
    }
  }

  bool full = false;
  while (!full) {
    bool changed = true;
    while (changed) {
      changed = false;
      for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
          if (simBoard[r][c] == '.') {
            if (simNotes[r][c].count() == 1) {
              int v = 0;
              for (int i = 1; i <= 9; ++i)
                if (simNotes[r][c][i])
                  v = i;
              placeValue(r, c, v, 1, r, c);
              changed = true;
            }
          }
        }
      }
      for (int v = 1; v <= 9; ++v) {
        for (int r = 0; r < 9; ++r) {
          int count = 0, lastC = -1;
          for (int c = 0; c < 9; ++c) {
            if (simBoard[r][c] == '0' + v) {
              count = 0;
              break;
            }
            if (simBoard[r][c] == '.' && simNotes[r][c][v]) {
              count++;
              lastC = c;
            }
          }
          if (count == 1) {
            placeValue(r, lastC, v, 2, r, -1);
            changed = true;
          }
        }
        for (int c = 0; c < 9; ++c) {
          int count = 0, lastR = -1;
          for (int r = 0; r < 9; ++r) {
            if (simBoard[r][c] == '0' + v) {
              count = 0;
              break;
            }
            if (simBoard[r][c] == '.' && simNotes[r][c][v]) {
              count++;
              lastR = r;
            }
          }
          if (count == 1) {
            placeValue(lastR, c, v, 3, -1, c);
            changed = true;
          }
        }
        for (int b = 0; b < 9; ++b) {
          int count = 0, lastR = -1, lastC = -1;
          int br = (b / 3) * 3, bc = (b % 3) * 3;
          for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
              int r = br + i, c = bc + j;
              if (simBoard[r][c] == '0' + v) {
                count = 0;
                i = 3;
                break;
              }
              if (simBoard[r][c] == '.' && simNotes[r][c][v]) {
                count++;
                lastR = r;
                lastC = c;
              }
            }
          }
          if (count == 1) {
            placeValue(lastR, lastC, v, 4, br, bc);
            changed = true;
          }
        }
      }
    }

    full = true;
    for (int r = 0; r < 9 && full; ++r) {
      for (int c = 0; c < 9 && full; ++c) {
        if (simBoard[r][c] == '.') {
          full = false;
        }
      }
    }

    if (!full) {
      auto trueSolution = logic.GetTrueSolution();
      int bestR = -1;
      int bestC = -1;
      int bestContraLen = 10000;
      std::vector<std::pair<int, int>> bestContraCells;

      for (int r = 0; r < 9; ++r) {
        for (int c = 0; c < 9; ++c) {
          if (simBoard[r][c] == '.') {
            int trueV = trueSolution[r][c] - '0';

            for (int v = 1; v <= 9; ++v) {
              if (simNotes[r][c][v] && v != trueV) {
                std::vector<std::pair<int, int>> contraCells;
                auto b = simBoard;
                auto n = simNotes;
                auto pV = [&](int rr, int cc, int vv) {
                  b[rr][cc] = '0' + vv;
                  n[rr][cc].reset();
                  for (int i = 0; i < 9; ++i) {
                    n[rr][i].reset(vv);
                    n[i][cc].reset(vv);
                  }
                  int br = (rr / 3) * 3, bc = (cc / 3) * 3;
                  for (int i = 0; i < 3; ++i)
                    for (int j = 0; j < 3; ++j)
                      n[br + i][bc + j].reset(vv);
                  contraCells.push_back({rr, cc});
                };
                pV(r, c, v);
                bool cg = true;
                bool conflict = false;
                while (cg) {
                  cg = false;
                  for (int rr = 0; rr < 9; ++rr)
                    for (int cc = 0; cc < 9; ++cc)
                      if (b[rr][cc] == '.' && n[rr][cc].none()) {
                        conflict = true;
                        break;
                      }
                  if (conflict)
                    break;

                  for (int rr = 0; rr < 9; ++rr) {
                    for (int cc = 0; cc < 9; ++cc) {
                      if (b[rr][cc] == '.' && n[rr][cc].count() == 1) {
                        int vv = 0;
                        for (int i = 1; i <= 9; ++i)
                          if (n[rr][cc][i])
                            vv = i;
                        pV(rr, cc, vv);
                        cg = true;
                      }
                    }
                  }
                }

                if (conflict) {
                  if ((int)contraCells.size() < bestContraLen) {
                    bestContraLen = contraCells.size();
                    bestContraCells = contraCells;
                    bestR = r;
                    bestC = c;
                  }
                }
              }
            }
          }
        }
      }

      if (bestR == -1) {
        int minNotes = 10;
        for (int r = 0; r < 9; ++r) {
          for (int c = 0; c < 9; ++c) {
            if (simBoard[r][c] == '.') {
              int nCnt = simNotes[r][c].count();
              if (nCnt < minNotes) {
                minNotes = nCnt;
                bestR = r;
                bestC = c;
              }
            }
          }
        }
        bestContraCells.clear();
        for (int i = 0; i < 9; ++i) {
          if (i != bestC)
            bestContraCells.push_back({bestR, i});
          if (i != bestR)
            bestContraCells.push_back({i, bestC});
        }
        int br = (bestR / 3) * 3, bc = (bestC / 3) * 3;
        for (int i = 0; i < 3; ++i) {
          for (int j = 0; j < 3; ++j) {
            if (br + i != bestR && bc + j != bestC) {
              bestContraCells.push_back({br + i, bc + j});
            }
          }
        }
      }

      int trueV = trueSolution[bestR][bestC] - '0';
      placeValue(bestR, bestC, trueV, 5, bestR, bestC);
      if (!steps.empty()) {
        for (auto it = steps.rbegin(); it != steps.rend(); ++it) {
          if (it->type == 1) { // 1 = place value step
            it->multiTargets = bestContraCells;
            if (it->multiTargets.empty())
              it->multiTargets = {{bestR, bestC}};
            break;
          }
        }
      }
    }
  }

  return steps;
}
