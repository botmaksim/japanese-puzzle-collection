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

void NonogramScreen::OnHintPressed() {
  errorHighlightCells = NonogramSolver::GetErrors(logic);
  if (errorHighlightCells.empty()) {
    auto steps = NonogramSolver::GenerateSteps(logic);
    if (!steps.empty()) {
      auto &s = steps[0];
      logic.ClearCell(s.r, s.c);
      if (s.type == 0)
        logic.ToggleCrossCell(s.r, s.c);
      else
        logic.ToggleCell(s.r, s.c);
      hintCellR = s.r;
      hintCellC = s.c;
    }
  }
}

void NonogramScreen::OnRestartPressed() {
  logic.ResetToInitial();
  errorHighlightCells.clear();
  hintCellR = -1;
  hintCellC = -1;
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

  int maxGridSize = 400;
  cellSize = maxGridSize / currentSize;
  if (cellSize > 50)
    cellSize = 50;

  int actualGridSize = currentSize * cellSize;

  offsetX = (800 - actualGridSize) / 2 + 50;
  offsetY = (600 - actualGridSize) / 2 + 30;

  errorHighlightCells.clear();
}

void NonogramScreen::UpdateInput() {
  Vector2 mouse = GetMousePosition();
  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) ||
      IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    hintCellR = -1;
    hintCellC = -1;
    int c = (mouse.x - offsetX) / cellSize;
    int r = (mouse.y - offsetY) / cellSize;

    int size = logic.GetSize();
    if (r >= 0 && r < size && c >= 0 && c < size) {
      if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        logic.ToggleCell(r, c);
      } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        logic.ToggleCrossCell(r, c);
      }
      errorHighlightCells.clear();
    }
  }
}

void NonogramScreen::DrawGame() {
  int size = logic.GetSize();

  if (!isAnimating) {
    int instrWidth =
        MeasureText("LMB: Fill | RMB: Cross | BACKSPACE: Menu", 20);
    DrawText("LMB: Fill | RMB: Cross | BACKSPACE: Menu", (800 - instrWidth) / 2,
             offsetY + size * cellSize + 30, 20, GRAY);
  } else {
    int instrWidth = MeasureText("ANIMATING SOLVER...", 20);
    DrawText("ANIMATING SOLVER...", (800 - instrWidth) / 2,
             offsetY + size * cellSize + 30, 20, animPaused ? ORANGE : RED);
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

  int fontSize = cellSize / 2;
  if (fontSize < 12)
    fontSize = 12;
  if (fontSize > 20)
    fontSize = 20;

  for (int r = 0; r < size; ++r) {
    bool highlightRed = isAnimating && (r == animReasonR);
    const auto &clues = logic.GetRowClue(r);
    int textX = offsetX - 8;
    for (int i = clues.size() - 1; i >= 0; --i) {
      int textW = MeasureText(TextFormat("%d", clues[i]), fontSize);
      DrawText(TextFormat("%d", clues[i]), textX - textW,
               offsetY + r * cellSize + (cellSize - fontSize) / 2, fontSize,
               highlightRed ? RED : BLACK);
      textX -= (textW + 16);
    }
  }

  for (int c = 0; c < size; ++c) {
    bool highlightRed = isAnimating && (c == animReasonC);
    const auto &clues = logic.GetColClue(c);
    int textY = offsetY - fontSize - 5;
    for (int i = clues.size() - 1; i >= 0; --i) {
      int textW = MeasureText(TextFormat("%d", clues[i]), fontSize);
      DrawText(TextFormat("%d", clues[i]),
               offsetX + c * cellSize + (cellSize - textW) / 2, textY, fontSize,
               highlightRed ? RED : BLACK);
      textY -= (fontSize + 5);
    }
  }

  for (int r = 0; r < size; ++r) {
    for (int c = 0; c < size; ++c) {
      int x = offsetX + c * cellSize;
      int y = offsetY + r * cellSize;

      bool isError = false;
      for (auto &err : errorHighlightCells)
        if (err.first == r && err.second == c)
          isError = true;
      if (isError) {
        DrawRectangle(x, y, cellSize, cellSize, RED);
      } else if (isAnimating && r == animTargetR && c == animTargetC) {
        DrawRectangle(x, y, cellSize, cellSize, GREEN);
      } else if (isAnimating && (r == animReasonR || c == animReasonC)) {
        DrawRectangle(x, y, cellSize, cellSize, ColorAlpha(RED, 0.2f));
      } else if (!isAnimating && r == hintCellR && c == hintCellC) {
        DrawRectangle(x, y, cellSize, cellSize, GREEN);
      }

      DrawRectangleLines(x, y, cellSize, cellSize, LIGHTGRAY);

      CellState state = logic.GetCellState(r, c);
      if (state == CellState::FILLED) {
        DrawRectangle(x + 2, y + 2, cellSize - 4, cellSize - 4, DARKGRAY);
      } else if (state == CellState::CROSSED) {
        int pad = cellSize / 4;
        DrawLineEx({(float)x + pad, (float)y + pad},
                   {(float)(x + cellSize - pad), (float)(y + cellSize - pad)},
                   2.0f, RED);
        DrawLineEx({(float)x + pad, (float)(y + cellSize - pad)},
                   {(float)(x + cellSize - pad), (float)y + pad}, 2.0f, RED);
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