#include "SudokuScreen.h"
#include <raylib.h>

void SudokuScreen::ApplyAnimState() {
  logic.SetBoardRaw(baseBoard);
  logic.SetNotesRaw(baseNotes);
  for (int i = 0; i < animStepIndex && i < (int)animSteps.size(); i++) {
    auto &s = animSteps[i];
    if (s.type == 0) {
      logic.SetNote(s.r, s.c, s.val, false);
    } else if (s.type == 1) {
      logic.SetPlayerCell(s.r, s.c, '0' + s.val);
      for (int v = 1; v <= 9; ++v)
        logic.SetNote(s.r, s.c, v, false);
    } else if (s.type == 2) {
      for (auto &tg : s.multiTargets) {
        logic.SetNote(tg.first, tg.second, s.val, false);
      }
    }
  }
}

void SudokuScreen::CalculateAnimSteps() {
  logic.ResetToInitial();
  logic.FillAllNotes();
  animSteps = SudokuSolver::GenerateSteps(logic);
  errorHighlightCells.clear();
  baseBoard = logic.GetBoardRaw();
  baseNotes = logic.GetNotesRaw();
}

void SudokuScreen::OnCheckPressed() {
  errorHighlightCells.clear();
  checkPassed = false;

  auto board = logic.GetBoardRaw();
  auto trueSol = logic.GetTrueSolution();
  for (int r = 0; r < 9; ++r) {
    for (int c = 0; c < 9; ++c) {
      if (board[r][c] != '.' && board[r][c] != trueSol[r][c]) {
        errorHighlightCells.push_back({r, c});
      }
    }
  }

  if (errorHighlightCells.empty()) {
    checkPassed = true;
  }
}

void SudokuScreen::OnRestartPressed() {
  logic.ResetToInitial();
  errorHighlightCells.clear();
  checkPassed = false;
  selectedRow = -1;
  selectedCol = -1;
}

void SudokuScreen::Init() {
  BaseGameScreen::Init();
  nextScreen = ScreenType::SUDOKU;

  int diff = GetConfig();
  int cellsToHide = 0;
  titleText = "SUDOKU";

  switch (diff) {
  case 0:
    cellsToHide = 35;
    titleText += " - EASY";
    break;
  case 1:
    cellsToHide = 45;
    titleText += " - MEDIUM";
    break;
  case 2:
    cellsToHide = 52;
    titleText += " - HARD";
    break;
  case 3:
    cellsToHide = 56;
    titleText += " - IMPOSSIBLE";
    break;
  default:
    cellsToHide = 35;
    titleText += " - EASY";
    break;
  }

  logic.GenerateNewGame(cellsToHide);

  selectedRow = -1;
  selectedCol = -1;
  isNoteMode = false;
  errorHighlightCells.clear();
}

void SudokuScreen::UpdateInput() {
  Vector2 mouse = GetMousePosition();
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
      IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    int c = (mouse.x - OFFSET_X) / CELL_SIZE;
    int r = (mouse.y - OFFSET_Y) / CELL_SIZE;

    if (r >= 0 && r < 9 && c >= 0 && c < 9) {
      checkPassed = false;
      errorHighlightCells.clear();
      selectedRow = r;
      selectedCol = c;
      isNoteMode = IsMouseButtonPressed(MOUSE_RIGHT_BUTTON);

      if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        char val = logic.GetBoardRaw()[r][c];
        if (val >= '1' && val <= '9') {
          int v = val - '0';
          for (int i = 0; i < 9; ++i) {
            logic.SetNote(r, i, v, false);
            logic.SetNote(i, c, v, false);
          }
          int br = (r / 3) * 3, bc = (c / 3) * 3;
          for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
              logic.SetNote(br + i, bc + j, v, false);
            }
          }
        }
      }
    } else {
      selectedRow = -1;
      selectedCol = -1;
      isNoteMode = false;
    }
  }

  if (selectedRow != -1 && selectedCol != -1) {
    int key = GetKeyPressed();
    if (key >= KEY_ZERO && key <= KEY_NINE) {
      checkPassed = false;
      int val = key - KEY_ZERO;
      if (key == KEY_ZERO) {
        logic.ToggleAllNotes(selectedRow, selectedCol);
      } else if (isNoteMode) {
        logic.ToggleNote(selectedRow, selectedCol, val);
      } else {
        logic.SetPlayerCell(selectedRow, selectedCol, '0' + val);
        errorHighlightCells.clear();
      }
    } else if (key == KEY_DELETE || key == KEY_BACKSPACE) {
      checkPassed = false;
      logic.ClearCell(selectedRow, selectedCol);
      errorHighlightCells.clear();
    }
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && gameWon) {
    nextScreen = ScreenType::MENU;
  }

  bool isFull = true;
  bool hasErrors = false;
  auto board = logic.GetBoardRaw();
  auto trueSol = logic.GetTrueSolution();
  for (int r = 0; r < 9; ++r) {
    for (int c = 0; c < 9; ++c) {
      if (board[r][c] == '.')
        isFull = false;
      else if (board[r][c] != trueSol[r][c])
        hasErrors = true;
    }
  }
  if (isFull && !hasErrors) {
    gameWon = true;
  } else {
    gameWon = false;
  }
}

void SudokuScreen::DrawGame() {
  if (!isAnimating) {
    int iWidth = MeasureText("LMB: Normal | RMB: Note | 0: Toggle All Notes | "
                             "DEL: Clear | BACKSPACE: Menu",
                             20);
    DrawText("LMB: Normal | RMB: Note | 0: Toggle All Notes | DEL: Clear | "
             "BACKSPACE: Menu",
             (800 - iWidth) / 2, 570, 20, GRAY);
  } else {
    int iWidth = MeasureText("ANIMATING SOLVER...", 20);
    DrawText("ANIMATING SOLVER...", (800 - iWidth) / 2, 570, 20,
             animPaused ? ORANGE : RED);
  }

  int animTargetR = -1;
  int animTargetC = -1;
  int animReasonR = -1;
  int animReasonC = -1;
  int animReasonType = 0;
  int animRVal = -1;
  bool isAnimSet = false;
  int animType = -1;
  std::vector<std::pair<int, int>> animMultiTargets;
  if (isAnimating && animStepIndex < (int)animSteps.size()) {
    auto &s = animSteps[animStepIndex];
    animTargetR = s.r;
    animTargetC = s.c;
    animRVal = s.val;
    animReasonType = s.reasonType;
    animReasonR = s.reasonR;
    animReasonC = s.reasonC;
    animType = s.type;
    if (s.type == 1) {
      isAnimSet = true;
      if (s.reasonType == 5)
        animMultiTargets = s.multiTargets;
    } else if (s.type == 2) {
      animMultiTargets = s.multiTargets;
    }
  }

  for (int r = 0; r < 9; r++) {
    for (int c = 0; c < 9; c++) {
      int x = OFFSET_X + c * CELL_SIZE;
      int y = OFFSET_Y + r * CELL_SIZE;

      Color bgColor = LIGHTGRAY;
      if (!isAnimating && r == selectedRow && c == selectedCol) {
        bgColor = isNoteMode ? ORANGE : SKYBLUE;
      }

      bool isError = false;
      for (auto &err : errorHighlightCells)
        if (err.first == r && err.second == c)
          isError = true;
      if (isError)
        bgColor = RED;

      if (isAnimating) {
        if (animReasonType == 1 && r == animReasonR && c == animReasonC) {
          bgColor = RED;
        } else if (animReasonType == 2 && r == animReasonR) {
          bgColor = ColorAlpha(RED, 0.4f);
        } else if (animReasonType == 3 && c == animReasonC) {
          bgColor = ColorAlpha(RED, 0.4f);
        } else if (animReasonType == 4 && (r / 3) * 3 == animReasonR &&
                   (c / 3) * 3 == animReasonC) {
          bgColor = ColorAlpha(RED, 0.4f);
        } else if (animReasonType == 5 && animType == 1) {
          for (auto &tg : animMultiTargets) {
            if (tg.first == r && tg.second == c)
              bgColor = ColorAlpha(RED, 0.5f);
          }
        }

        if (isAnimSet && r == animTargetR && c == animTargetC) {
          if (animReasonType == 5) {
            bgColor = RED;
          } else {
            bgColor = GREEN;
          }
        }
      }

      if (bgColor.a != 0)
        DrawRectangle(x, y, CELL_SIZE, CELL_SIZE, bgColor);

      char val = logic.GetCell(r, c);
      if (isAnimating && isAnimSet && r == animTargetR && c == animTargetC) {
        val = '0' + animRVal;
      }

      if (val != '.') {
        Color textColor = logic.IsCellLocked(r, c) ? BLACK : BLUE;
        if (isAnimating && isAnimSet && r == animTargetR && c == animTargetC) {
          textColor = (animReasonType == 5) ? GREEN : WHITE;
        }
        int textW = MeasureText(TextFormat("%c", val), 24);
        DrawText(TextFormat("%c", val), x + (CELL_SIZE - textW) / 2, y + 12, 24,
                 textColor);
      } else {
        for (int i = 1; i <= 9; i++) {
          if (logic.HasNote(r, c, i)) {
            int noffX = (i - 1) % 3;
            int noffY = (i - 1) / 3;
            Color noteColor = GRAY;

            if (isAnimating && animType == 0 && r == animTargetR &&
                c == animTargetC && i == animRVal) {
              noteColor = GREEN;
              DrawRectangle(x + 5 + noffX * 15 - 2, y + 3 + noffY * 15 - 2, 10,
                            14, ColorAlpha(GREEN, 0.4f));
            } else if (isAnimating && animType == 2 && i == animRVal) {
              for (auto &tg : animMultiTargets) {
                if (tg.first == r && tg.second == c) {
                  noteColor = GREEN;
                  DrawRectangle(x + 5 + noffX * 15 - 2, y + 3 + noffY * 15 - 2,
                                10, 14, ColorAlpha(GREEN, 0.4f));
                }
              }
            }

            DrawText(TextFormat("%d", i), x + 5 + noffX * 15,
                     y + 3 + noffY * 15, 10, noteColor);
          }
        }
      }
    }
  }

  for (int i = 0; i <= 9; i++) {
    int thick = (i % 3 == 0) ? 3 : 1;
    DrawLineEx(
        {(float)OFFSET_X, (float)(OFFSET_Y + i * CELL_SIZE)},
        {(float)(OFFSET_X + 9 * CELL_SIZE), (float)(OFFSET_Y + i * CELL_SIZE)},
        thick, BLACK);
    DrawLineEx(
        {(float)(OFFSET_X + i * CELL_SIZE), (float)OFFSET_Y},
        {(float)(OFFSET_X + i * CELL_SIZE), (float)(OFFSET_Y + 9 * CELL_SIZE)},
        thick, BLACK);
  }

  if (checkPassed) {
    DrawRectangle(630, 280, 160, 60, LIGHTGRAY);
    DrawText("All correct!", 650, 300, 20, DARKGREEN);
  }

  if (gameWon || logic.IsGameWon()) {
    int winWidth = MeasureText("YOU WIN!", 60);
    int bgWidth = winWidth + 40;
    int bgHeight = 100;
    int cY = OFFSET_Y + (9 * CELL_SIZE) / 2;
    int cX = OFFSET_X + (9 * CELL_SIZE) / 2;
    DrawRectangle(cX - bgWidth / 2, cY - bgHeight / 2, bgWidth, bgHeight,
                  ColorAlpha(BLACK, 0.7f));
    DrawText("YOU WIN!", cX - winWidth / 2, cY - 30, 60, GREEN);
  }
}
