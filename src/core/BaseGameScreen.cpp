#include "BaseGameScreen.h"

BaseGameScreen::BaseGameScreen() {
  isAnimating = false;
  animPaused = false;
  animTimer = 0.0f;
  animSpeed = 1.0f;
  animStepIndex = 0;
}

void BaseGameScreen::Init() {
  isAnimating = false;
  animPaused = false;
  animTimer = 0.0f;
  animSpeed = 1.0f;
  animStepIndex = 0;
}

void BaseGameScreen::DrawButton(Rectangle rect, const char *text) {
  Vector2 mouse = GetMousePosition();
  bool hovered = CheckCollisionPointRec(mouse, rect);
  DrawRectangleRec(rect, hovered ? LIGHTGRAY : GRAY);
  DrawRectangleLinesEx(rect, 2, DARKGRAY);
  int tw = MeasureText(text, 20);
  DrawText(text, rect.x + (rect.width - tw) / 2, rect.y + 10, 20, BLACK);
}

void BaseGameScreen::Update() {
  if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressed(KEY_ESCAPE)) {
    if (isAnimating) {
      isAnimating = false;
    } else {
      nextScreen = ScreenType::MENU;
    }
  }

  Rectangle btnHint = {650, 100, 120, 40};
  Rectangle btnVis = {650, 160, 120, 40};
  Rectangle btnRestart = {650, 220, 120, 40};

  Rectangle btnPrev = {650, 220, 55, 40};
  Rectangle btnNext = {715, 220, 55, 40};
  Rectangle btnPause = {650, 270, 120, 40};
  Rectangle btnSpeed = {650, 320, 120, 40};
  Rectangle btnExitAnim = {650, 370, 120, 40};

  Vector2 mouse = GetMousePosition();
  bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

  if (!isAnimating) {
    if ((clicked && CheckCollisionPointRec(mouse, btnHint)) ||
        IsKeyPressed(KEY_H)) {
      OnHintPressed();
    }
    if ((clicked && CheckCollisionPointRec(mouse, btnVis)) ||
        IsKeyPressed(KEY_A)) {
      CalculateAnimSteps();
      if (GetAnimStepsCount() > 0) {
        isAnimating = true;
        animStepIndex = 0;
        animTimer = 0.0f;
        animPaused = false;
        animSpeed = 1.0f;
        ApplyAnimState();
      }
    }
    if ((clicked && CheckCollisionPointRec(mouse, btnRestart)) ||
        IsKeyPressed(KEY_R)) {
      OnRestartPressed();
    }
    UpdateInput();
  } else {
    if ((clicked && CheckCollisionPointRec(mouse, btnPrev)) ||
        IsKeyPressed(KEY_LEFT)) {
      if (animStepIndex > 0)
        animStepIndex--;
      ApplyAnimState();
      animTimer = 0;
    }
    if ((clicked && CheckCollisionPointRec(mouse, btnNext)) ||
        IsKeyPressed(KEY_RIGHT)) {
      if (animStepIndex < GetAnimStepsCount())
        animStepIndex++;
      ApplyAnimState();
      animTimer = 0;
    }
    if ((clicked && CheckCollisionPointRec(mouse, btnPause)) ||
        IsKeyPressed(KEY_SPACE)) {
      animPaused = !animPaused;
    }
    if ((clicked && CheckCollisionPointRec(mouse, btnSpeed)) ||
        IsKeyPressed(KEY_S)) {
      if (animSpeed == 1.0f)
        animSpeed = 2.0f;
      else if (animSpeed == 2.0f)
        animSpeed = 4.0f;
      else if (animSpeed == 4.0f)
        animSpeed = 6.0f;
      else if (animSpeed == 6.0f)
        animSpeed = 8.0f;
      else if (animSpeed == 8.0f)
        animSpeed = 16.0f;
      else
        animSpeed = 1.0f;
    }
    if ((clicked && CheckCollisionPointRec(mouse, btnExitAnim)) ||
        IsKeyPressed(KEY_X)) {
      isAnimating = false;
    }

    if (!animPaused && animStepIndex < GetAnimStepsCount()) {
      animTimer += GetFrameTime() * animSpeed;
      if (animTimer >= 2.0f) {
        animStepIndex++;
        ApplyAnimState();
        animTimer = 0.0f;
      }
    }
  }
}

void BaseGameScreen::Draw() {
  ClearBackground(RAYWHITE);

  int tWidth = MeasureText(titleText.c_str(), 30);
  DrawText(titleText.c_str(), (800 - tWidth) / 2, 20, 30, BLACK);

  DrawGame();
  if (!isAnimating) {
    DrawButton({650, 100, 120, 40}, "Hint");
    DrawButton({650, 160, 120, 40}, "Auto-Solve");
    DrawButton({650, 220, 120, 40}, "Restart");
  } else {
    DrawButton({650, 220, 55, 40}, "Prev");
    DrawButton({715, 220, 55, 40}, "Next");
    DrawButton({650, 270, 120, 40}, animPaused ? "Play" : "Pause");
    std::string spdTxt = "Speed: " + std::to_string((int)animSpeed) + "x";
    DrawButton({650, 320, 120, 40}, spdTxt.c_str());
    DrawButton({650, 370, 120, 40}, "Stop Anim");
  }
}

ScreenType BaseGameScreen::GetNextScreen() { return nextScreen; }
