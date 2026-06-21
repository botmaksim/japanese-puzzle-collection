#include "NonogramScreen.h"
#include <raylib.h>
#include <string>

void NonogramScreen::ApplyAnimState() {
  logic.SetPlayerGrid(baseBoard);
  for (int i = 0; i < animStepIndex && i < (int)animSteps.size(); i++) {
    int r = animSteps[i].r;
    int c = animSteps[i].c;
    logic.ClearCell(r, c);
    if (animSteps[i].type == 0)
      logic.ToggleCrossCell(r, c);
    else
      logic.ToggleCell(r, c);
  }
}

void NonogramScreen::CalculateAnimSteps() {
  logic.ResetToInitial();
  animSteps = NonogramSolver::GenerateSteps(logic);
  errorHighlightCells.clear();
  baseBoard = logic.GetPlayerGrid();
}

void NonogramScreen::OnCheckPressed() {
  errorHighlightCells.clear();
  checkPassed = false;

  if (logic.IsGameWon()) {
    checkPassed = true;
  } else {
    errorHighlightCells = NonogramSolver::GetErrors(logic);
    if (errorHighlightCells.empty())
      checkPassed = true;
  }
}

void NonogramScreen::OnRestartPressed() {
  logic.ResetToInitial();
  errorHighlightCells.clear();
  checkPassed = false;
}

void NonogramScreen::Init() {
  BaseGameScreen::Init();
  nextScreen = ScreenType::NONOGRAM;

  int currentSize = GetConfig();
  if (currentSize < 5)
    currentSize = 5;

  titleText = "NONOGRAM";
  if (currentSize == 5)
    titleText += " - BEGINNER";
  else if (currentSize == 10)
    titleText += " - CLASSIC";
  else if (currentSize == 15)
    titleText += " - EXPERT";
  else
    titleText += " - CUSTOM (" + std::to_string(currentSize) + "x" +
                 std::to_string(currentSize) + ")";

  logic.GenerateNewGame(currentSize);

  int maxRowClues = 0;
  for (int r = 0; r < currentSize; ++r) {
    if ((int)logic.GetRowClue(r).size() > maxRowClues)
      maxRowClues = logic.GetRowClue(r).size();
  }
  int maxColClues = 0;
  for (int c = 0; c < currentSize; ++c) {
    if ((int)logic.GetColClue(c).size() > maxColClues)
      maxColClues = logic.GetColClue(c).size();
  }

  cellSize = 50;
  int fontSize = cellSize;
  int approxRowClueWidth = 0;
  int approxColClueHeight = 0;

  while (cellSize > 4) {
    fontSize = cellSize;
    if (fontSize > 20)
      fontSize = 20;

    approxRowClueWidth =
        maxRowClues *
        (fontSize * 1.5 + 6); // Add some buffer for multi-digit clues
    approxColClueHeight = maxColClues * (fontSize + 2);

    int totalW = approxRowClueWidth + currentSize * cellSize;
    int totalH = approxColClueHeight + currentSize * cellSize;

    // Leave space for UI on the right (620 limit) and title on top (80 limit)
    if (totalW <= 620 && totalH <= 480) {
      break;
    }
    cellSize--;
  }

  if (cellSize < 4)
    cellSize = 4;

  int actualGridSize = currentSize * cellSize;

  offsetX = approxRowClueWidth +
            (620 - (approxRowClueWidth + actualGridSize)) / 2 + 10;
  offsetY = 80 + approxColClueHeight +
            (480 - (approxColClueHeight + actualGridSize)) / 2;

  errorHighlightCells.clear();
}

void NonogramScreen::UpdateInput() {
  Vector2 mouse = GetMousePosition();
  int size = logic.GetSize();
  int c = (mouse.x - offsetX) / cellSize;
  int r = (mouse.y - offsetY) / cellSize;
  bool inBounds = (r >= 0 && r < size && c >= 0 && c < size);

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
      IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    if (inBounds) {
      checkPassed = false;
      errorHighlightCells.clear();
      isDragging = true;
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        dragButton = MOUSE_LEFT_BUTTON;
        CellState current = logic.GetCellState(r, c);
        dragTargetState = (current == CellState::FILLED) ? CellState::EMPTY
                                                         : CellState::FILLED;
        logic.ClearCell(r, c);
        if (dragTargetState == CellState::FILLED)
          logic.ToggleCell(r, c);
      } else {
        dragButton = MOUSE_RIGHT_BUTTON;
        CellState current = logic.GetCellState(r, c);
        dragTargetState = (current == CellState::CROSSED) ? CellState::EMPTY
                                                          : CellState::CROSSED;
        logic.ClearCell(r, c);
        if (dragTargetState == CellState::CROSSED)
          logic.ToggleCrossCell(r, c);
      }
      lastDragR = r;
      lastDragC = c;
    }
  } else if (isDragging && (IsMouseButtonDown(MOUSE_LEFT_BUTTON) ||
                            IsMouseButtonDown(MOUSE_RIGHT_BUTTON))) {
    if (inBounds && (r != lastDragR || c != lastDragC)) {
      logic.ClearCell(r, c);
      if (dragTargetState == CellState::FILLED)
        logic.ToggleCell(r, c);
      else if (dragTargetState == CellState::CROSSED)
        logic.ToggleCrossCell(r, c);
      lastDragR = r;
      lastDragC = c;
    }
  } else {
    isDragging = false;
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && gameWon) {
    nextScreen = ScreenType::MENU;
  }

  gameWon = logic.IsGameWon();
}

std::vector<int>
NonogramScreen::GetClueStatus(const std::vector<int> &clues,
                              const std::vector<CellState> &line) {
  std::vector<int> status(clues.size(), 0);
  if (clues.empty())
    return status;

  std::vector<CellState> resLine;
  bool isValid = SolveLineDP(clues, line, resLine);

  if (!isValid) {
    for (size_t i = 0; i < status.size(); ++i)
      status[i] = 2; // RED
    return status;
  }

  int totalClueCells = 0;
  for (int c : clues)
    totalClueCells += c;
  int userFilled = 0;
  for (auto c : line)
    if (c == CellState::FILLED)
      userFilled++;

  if (userFilled == totalClueCells) {
    for (size_t i = 0; i < status.size(); ++i)
      status[i] = 1; // GRAY
    return status;
  }

  int l_clue = 0;
  int i = 0;
  while (i < (int)line.size()) {
    if (line[i] == CellState::CROSSED) {
      i++;
      continue;
    }
    if (line[i] == CellState::EMPTY)
      break;
    if (line[i] == CellState::FILLED) {
      int len = 0;
      while (i < (int)line.size() && line[i] == CellState::FILLED) {
        len++;
        i++;
      }
      if (i == (int)line.size() || line[i] == CellState::CROSSED) {
        if (l_clue < (int)clues.size() && clues[l_clue] == len) {
          status[l_clue] = 1;
          l_clue++;
        } else
          break;
      } else
        break;
    }
  }

  int r_clue = (int)clues.size() - 1;
  i = (int)line.size() - 1;
  while (i >= 0 && r_clue >= l_clue) {
    if (line[i] == CellState::CROSSED) {
      i--;
      continue;
    }
    if (line[i] == CellState::EMPTY)
      break;
    if (line[i] == CellState::FILLED) {
      int len = 0;
      while (i >= 0 && line[i] == CellState::FILLED) {
        len++;
        i--;
      }
      if (i < 0 || line[i] == CellState::CROSSED) {
        if (r_clue >= 0 && clues[r_clue] == len) {
          status[r_clue] = 1;
          r_clue--;
        } else
          break;
      } else
        break;
    }
  }

  return status;
}

void NonogramScreen::DrawGame() {
  int size = logic.GetSize();

  if (!isAnimating) {
    int instrWidth =
        MeasureText("LMB: Fill | RMB: Cross | BACKSPACE: Menu", 20);
    DrawText("LMB: Fill | RMB: Cross | BACKSPACE: Menu", (800 - instrWidth) / 2,
             570, 20, GRAY);
  } else {
    int instrWidth = MeasureText("ANIMATING SOLVER...", 20);
    DrawText("ANIMATING SOLVER...", (800 - instrWidth) / 2, 570, 20,
             animPaused ? ORANGE : RED);
  }

  int animTargetR = -1;
  int animTargetC = -1;
  int animReasonR = -1;
  int animReasonC = -1;
  if (isAnimating && animStepIndex < (int)animSteps.size()) {
    animTargetR = animSteps[animStepIndex].r;
    animTargetC = animSteps[animStepIndex].c;
    animReasonR = animSteps[animStepIndex].reasonR;
    animReasonC = animSteps[animStepIndex].reasonC;
  }

  int fontSize = cellSize;
  if (fontSize > 20)
    fontSize = 20;

  for (int r = 0; r < size; ++r) {
    bool highlightRed = isAnimating && (r == animReasonR);
    const auto &clues = logic.GetRowClue(r);
    std::vector<CellState> lineRow = logic.GetPlayerGrid()[r];
    auto status = GetClueStatus(clues, lineRow);

    int textX = offsetX - 4;
    for (int i = clues.size() - 1; i >= 0; --i) {
      int textW = MeasureText(TextFormat("%d", clues[i]), fontSize);
      Color color = BLACK;
      if (highlightRed || status[i] == 2)
        color = RED;
      else if (status[i] == 1)
        color = LIGHTGRAY;

      int yPos = offsetY + r * cellSize + (cellSize - fontSize) / 2;
      DrawText(TextFormat("%d", clues[i]), textX - textW, yPos, fontSize,
               color);

      if (status[i] == 1) {
        DrawLine(textX - textW - 2, yPos + fontSize / 2, textX + 2,
                 yPos + fontSize / 2, LIGHTGRAY);
      }
      textX -= (textW + 6);
    }
  }

  for (int c = 0; c < size; ++c) {
    bool highlightRed = isAnimating && (c == animReasonC);
    const auto &clues = logic.GetColClue(c);
    std::vector<CellState> lineCol(size);
    for (int r = 0; r < size; ++r)
      lineCol[r] = logic.GetPlayerGrid()[r][c];
    auto status = GetClueStatus(clues, lineCol);

    int textY = offsetY - fontSize - 2;
    for (int i = clues.size() - 1; i >= 0; --i) {
      int textW = MeasureText(TextFormat("%d", clues[i]), fontSize);
      Color color = BLACK;
      if (highlightRed || status[i] == 2)
        color = RED;
      else if (status[i] == 1)
        color = LIGHTGRAY;

      int xPos = offsetX + c * cellSize + (cellSize - textW) / 2;
      DrawText(TextFormat("%d", clues[i]), xPos, textY, fontSize, color);

      if (status[i] == 1) {
        DrawLine(xPos - 2, textY + fontSize / 2, xPos + textW + 2,
                 textY + fontSize / 2, LIGHTGRAY);
      }
      textY -= (fontSize + 2);
    }
  }

  for (int r = 0; r < size; ++r) {
    for (int c = 0; c < size; ++c) {
      int x = offsetX + c * cellSize;
      int y = offsetY + r * cellSize;

      CellState state = logic.GetCellState(r, c);
      if (state == CellState::FILLED) {
        int rectPad = (cellSize > 10) ? 2 : 1;
        DrawRectangle(x + rectPad, y + rectPad, cellSize - rectPad * 2,
                      cellSize - rectPad * 2, DARKGRAY);
      } else if (state == CellState::CROSSED) {
        int pad = cellSize / 4;
        if (pad < 1)
          pad = 1;
        DrawLineEx({(float)x + pad, (float)y + pad},
                   {(float)(x + cellSize - pad), (float)(y + cellSize - pad)},
                   (cellSize > 10) ? 2.0f : 1.0f, RED);
        DrawLineEx({(float)x + pad, (float)(y + cellSize - pad)},
                   {(float)(x + cellSize - pad), (float)y + pad},
                   (cellSize > 10) ? 2.0f : 1.0f, RED);
      }

      DrawRectangleLines(x, y, cellSize, cellSize, LIGHTGRAY);

      bool isError = false;
      for (auto &err : errorHighlightCells) {
        if (err.first == r && err.second == c) {
          isError = true;
          break;
        }
      }
      if (isError) {
        DrawRectangle(x, y, cellSize, cellSize, ColorAlpha(RED, 0.4f));
      }

      if (isAnimating && r == animTargetR && c == animTargetC) {
        DrawRectangle(x, y, cellSize, cellSize, ColorAlpha(GREEN, 0.4f));
      } else if (isAnimating && (r == animReasonR || c == animReasonC)) {
        DrawRectangle(x, y, cellSize, cellSize, ColorAlpha(RED, 0.2f));
      }
    }
  }

  DrawRectangleLinesEx({(float)offsetX, (float)offsetY,
                        (float)(size * cellSize), (float)(size * cellSize)},
                       2, BLACK);

  for (int i = 0; i <= size; i++) {
    if (i % 5 == 0) {
      DrawLineEx(
          {(float)offsetX, (float)(offsetY + i * cellSize)},
          {(float)(offsetX + size * cellSize), (float)(offsetY + i * cellSize)},
          2, BLACK);
      DrawLineEx(
          {(float)(offsetX + i * cellSize), (float)offsetY},
          {(float)(offsetX + i * cellSize), (float)(offsetY + size * cellSize)},
          2, BLACK);
    }
  }

  if (checkPassed) {
    DrawRectangle(630, 280, 160, 60, LIGHTGRAY);
    DrawText("All correct!", 650, 300, 20, DARKGREEN);
  }

  if (logic.IsGameWon()) {
    int winWidth = MeasureText("YOU WIN!", 60);
    int bgWidth = winWidth + 40;
    int bgHeight = 100;
    int cY = offsetY + (size * cellSize) / 2;
    int cX = offsetX + (size * cellSize) / 2;
    DrawRectangle(cX - bgWidth / 2, cY - bgHeight / 2, bgWidth, bgHeight,
                  ColorAlpha(BLACK, 0.7f));
    DrawText("YOU WIN!", cX - winWidth / 2, cY - 30, 60, GREEN);
  }
}