#include "raylib.h"
#include "utils.hpp"
#include <fstream>
#include <string>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 450

#define BAR_LEFT 10
#define BAR_RIGHT SCREEN_WIDTH - 10
#define BAR_TOP SCREEN_HEIGHT / 2 - 40
#define BAR_BOTTOM SCREEN_HEIGHT / 2 + 40
#define BAR_HEIGHT 80
#define BAR_WIDTH SCREEN_WIDTH - 20

#define GREENZONE_WIDTH_MIN 20
#define GREENZONE_WIDTH_MAX 60
#define GREENZONE_DIST_MIN 45

int main() {
  InitWindow(600, 450, "GAME NAME");
  SetTargetFPS(60);
  SetWindowState(FLAG_WINDOW_RESIZABLE);
  InitAudioDevice();

  RenderTexture2D target = LoadRenderTexture(600, 450);

  bool should_exit = false;

  int gz_w = GetRandomValue(GREENZONE_WIDTH_MIN, GREENZONE_WIDTH_MAX);
  int gz_x = GetRandomValue(BAR_LEFT, BAR_RIGHT - gz_w);

  int s_x = BAR_LEFT;
  int s_s = 0;
  bool s_d = 0;

  bool dead = false;
  bool started = false;

  int score = 0;
  int highscore = 0;

  std::ifstream hsfs("data/highscore");
  if (hsfs.is_open()) {
    std::string hstxt;
    getline(hsfs, hstxt);
    if (!hstxt.empty()) {
      highscore = std::stoi(hstxt);
    }
    hsfs.close();
  }

  while (!WindowShouldClose() && !should_exit) {
    float dt = GetFrameTime();

    if (!dead) {
      s_x += s_s * (s_d ? -1 : 1);
      if (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W)) {
        if (s_x > gz_x && s_x < gz_x + gz_w) {
          score++;
          if (score > highscore)
            highscore = score;
          gz_w = GetRandomValue(GREENZONE_WIDTH_MIN, GREENZONE_WIDTH_MAX);
          s_d = !s_d;
          s_s = (int)(s_s * 1.05);
          gz_x =
              s_d ? GetRandomValue(BAR_LEFT, s_x - gz_w - GREENZONE_DIST_MIN)
                  : GetRandomValue(s_x + GREENZONE_DIST_MIN, BAR_RIGHT - gz_w);
        } else if (started) {
          dead = true;
          started = false;
          s_s = 0;
          s_d = 0;
        }
      }

      if ((s_x > gz_x + gz_w && !s_d) || (s_x < gz_x && s_d)) {
        dead = true;
        started = false;
        s_s = 0;
        s_d = 0;
      }
    }

    if (!dead && s_s == 0 && (IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W))) {
      s_s = 3;
      started = true;
      dead = false;
      s_x = BAR_LEFT;
      gz_w = GetRandomValue(GREENZONE_WIDTH_MIN, GREENZONE_WIDTH_MAX);
      gz_x = GetRandomValue(BAR_LEFT, BAR_RIGHT - gz_w);
    }

    BeginTextureMode(target);
    ClearBackground(BLACK);

    DrawRectangle(BAR_LEFT, BAR_TOP, BAR_WIDTH, BAR_HEIGHT, WHITE);
    DrawRectangle(gz_x, BAR_TOP, gz_w, BAR_HEIGHT, GREEN);
    DrawLine(s_x, BAR_TOP, s_x, BAR_BOTTOM, RED);

    if (!started && !dead)
      DrawText("Press Space or W to Start",
               SCREEN_WIDTH / 2 -
                   MeasureText("Press Space or W to Start", 24) / 2,
               5, 24, WHITE);
    if (dead) {
      DrawText("You lost!", SCREEN_WIDTH / 2 - MeasureText("You lost!", 24) / 2,
               5, 24, WHITE);
      DrawText("Press R to Restart",
               SCREEN_WIDTH / 2 - MeasureText("Press R to Restart", 24) / 2, 50,
               24, WHITE);

      if (IsKeyPressed(KEY_R)) {
        s_x = BAR_LEFT;
        gz_w = GetRandomValue(GREENZONE_WIDTH_MIN, GREENZONE_WIDTH_MAX);
        gz_x = GetRandomValue(BAR_LEFT, BAR_RIGHT - gz_w);
        dead = false;
        started = false;
        score = 0;
      }
    }

    DrawText(("Score: " + std::to_string(score)).c_str(), 10, BAR_BOTTOM + 10,
             24, WHITE);
    DrawText(("Highscore: " + std::to_string(highscore)).c_str(), 10,
             BAR_BOTTOM + 35, 24, WHITE);

    EndTextureMode();

    BeginDrawing();
    ClearBackground(WHITE);
    DrawTextureRec(target.texture,
                   (Rectangle){0, 0, (float)target.texture.width,
                               -(float)target.texture.height},
                   (Vector2){0, 0}, WHITE);
    DrawFPS(0, 0);
    EndDrawing();
  }

  UnloadRenderTexture(target);
  CloseWindow();

  std::ofstream hsfs_out("data/highscore");
  if (hsfs_out.is_open()) {
    hsfs_out << std::to_string(highscore);
    hsfs_out.close();
  }

  return 0;
}
