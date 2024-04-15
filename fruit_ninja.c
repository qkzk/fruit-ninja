/*******************************************************************************************
*
*   FRUIT NINJA
*
*   COMPILATION (Linux - GCC):
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -lraylib -lGL -lm -lpthread -ldl
-lrt -lX11
*
*
********************************************************************************************/
#include "raylib.h"

//----------------------------------------------------------------------------------
// Useful values definitions
//----------------------------------------------------------------------------------
#define PLAYER_LIFES 5
#define BRICKS_LINES 5
#define BRICKS_PER_LINE 20

#define BRICKS_POSITION_Y 50

#define MAX_FRUITS 5
#define FPS 60
#define MAX_DURATION (FPS * 30)

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// LESSON 01: Window initialization and screens management
typedef enum GameScreen { LOGO, TITLE, GAMEPLAY, ENDING } GameScreen;

Color randomColor(void);

// Player structure
typedef struct Player {
  Vector2 position;
  Vector2 speed;
  Vector2 size;
  Rectangle bounds;
  int lifes;
} Player;

// Ball structure
typedef struct Ball {
  Vector2 position;
  Vector2 speed;
  float radius;
  bool active;
} Ball;

// Bricks structure
typedef struct Brick {
  Vector2 position;
  Vector2 size;
  Rectangle bounds;
  int resistance;
  bool active;
} Brick;

// Fruit Structure
typedef struct Fruit {
  Vector2 position;
  Vector2 size;
  Vector2 direction;
  Vector2 acceleration;
  bool is_hit;
  Color color;
} Fruit;

// GameState
typedef struct GameState {
  int nbFruits;
} GameState;

/* Square of the distance between a & b */
float distance_sq(Vector2 a, Vector2 b) {
  return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y);
}

/* true iff the fruit is outside of bounds */
bool isFruitOutside(Fruit fruit, int screenWidth, int screenHeight) {
  return (fruit.position.x - fruit.size.x > screenWidth ||
          fruit.position.y - fruit.size.y > screenHeight ||
          fruit.position.x + fruit.size.x < 0 ||
          fruit.position.y + fruit.size.y < 0);
}

/* Remove hit fruits from array */
void removeShrinkedFruits(Fruit fruits[], int* nbFruits) {
  int write_index = 0;
  for (int read_index = 0; read_index < *nbFruits; read_index++) {
    if (fruits[read_index].size.x > 0) {
      fruits[write_index] = fruits[read_index];
      write_index++;
    }
  }
  *nbFruits = write_index;
}

/* Complete the fruits array */
void completeFruits(Fruit fruits[],
                    int* nbFruits,
                    int screenWidth,
                    int screenHeight) {
  int added = 0;
  for (int i = *nbFruits; i < MAX_FRUITS; i++) {
    int x = GetRandomValue(screenWidth / 20, 19 * screenWidth / 20);
    int y = GetRandomValue(screenWidth / 20, screenHeight - screenWidth / 20);

    int vx = GetRandomValue(-1, 1);
    int vy = GetRandomValue(-1, 1);

    /* int ax = GetRandomValue(-2, 2); */
    /* int ay = GetRandomValue(-2, 2); */

    fruits[i].size = (Vector2){screenWidth / 20, screenWidth / 20};
    fruits[i].position = (Vector2){x, y};
    fruits[i].direction = (Vector2){vx, vy};
    fruits[i].acceleration = (Vector2){0, 0};
    fruits[i].is_hit = false;
    fruits[i].color = randomColor();
    added++;
  }
  *nbFruits += added;
}

void shrinkFruits(Fruit fruits[],
                  int* nbFruits,
                  int screenWidth,
                  int screenHeight) {
  for (int i = 0; i < *nbFruits; i++) {
    if (fruits[i].is_hit && fruits[i].size.x > 0) {
      fruits[i].size.x--;
      fruits[i].size.y--;
    }
  }
}

void moveFruits(Fruit fruits[],
                int* nbFruits,
                int screenWidth,
                int screenHeight) {
  for (int i = 0; i < *nbFruits; i++) {
    for (int i = 0; i < *nbFruits; i++) {
      if (fruits[i].is_hit) {
        continue;
      }
      fruits[i].direction.x += fruits[i].acceleration.x;
      fruits[i].direction.y += fruits[i].acceleration.y;

      fruits[i].position.x += fruits[i].direction.x;
      fruits[i].position.y += fruits[i].direction.y;
    }
  }
}

void hitFruits(Fruit fruits[],
               int* nbFruits,
               int screenWidth,
               int screenHeight,
               Vector2 cursor) {
  for (int i = 0; i < *nbFruits; i++) {
    for (int i = 0; i < *nbFruits; i++) {
      if (fruits[i].is_hit) {
        continue;
      }
      if (distance_sq(fruits[i].position, cursor) <
          fruits[i].size.x * fruits[i].size.x) {
        fruits[i].is_hit = true;
      }

      if (isFruitOutside(fruits[i], screenWidth, screenHeight)) {
        fruits[i].is_hit = true;
      }
    }
  }
}

/* Generate a random Color */
Color randomColor(void) {
  int red = GetRandomValue(0, 256);
  int green = GetRandomValue(0, 256);
  int blue = GetRandomValue(0, 256);
  int alpha = 255;

  return (Color){(unsigned char)red, (unsigned char)green, (unsigned char)blue,
                 (unsigned char)alpha};
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main() {
  // Initialization
  //--------------------------------------------------------------------------------------
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  InitWindow(screenWidth, screenHeight, "FRUIT NINJA");

  // NOTE: Load resources (textures, fonts, audio) after Window
  // initialization

  // Game required variables
  GameScreen screen = LOGO;  // Current game screen state

  int framesCounter = 0;    // General pourpose frames counter
  int gameResult = -1;      // Game result: 0 - Loose, 1 - Win, -1 - Not defined
  bool gamePaused = false;  // Game paused state toggle

  GameState gameState;
  gameState.nbFruits = 2;

  Fruit fruits[MAX_FRUITS];
  completeFruits(fruits, &gameState.nbFruits, screenWidth, screenHeight);
  Vector2 cursor;

  SetTargetFPS(FPS);  // Set desired framerate (frames per second)
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose())  // Detect window close button or ESC key
  {
    // Update
    //----------------------------------------------------------------------------------
    switch (screen) {
      case LOGO: {
        // Update LOGO screen data here!

        framesCounter++;

        if (framesCounter > 120) {
          screen = TITLE;  // Change to TITLE screen after 3 seconds
          framesCounter = 0;
        }

      } break;
      case TITLE: {
        // Update TITLE screen data here!

        framesCounter++;

        // LESSON 03: Inputs management (keyboard, mouse)
        if (IsKeyPressed(KEY_ENTER)) {
          screen = GAMEPLAY;
          framesCounter = 0;
        }

      } break;
      case GAMEPLAY: {
        if (!gamePaused) {
          cursor = GetMousePosition();

          if (framesCounter % 3 == 0) {
            moveFruits(fruits, &gameState.nbFruits, screenWidth, screenHeight);
          }
          hitFruits(fruits, &gameState.nbFruits, screenWidth, screenHeight,
                    cursor);
          shrinkFruits(fruits, &gameState.nbFruits, screenWidth, screenHeight);
          removeShrinkedFruits(fruits, &gameState.nbFruits);
          completeFruits(fruits, &gameState.nbFruits, screenWidth,
                         screenHeight);
          framesCounter++;
          if (framesCounter >= MAX_DURATION) {
            framesCounter = 0;
            screen = ENDING;
            gameState.nbFruits = 0;
          }
        }
      } break;
      case ENDING: {
        // Update END screen data here!

        framesCounter++;

        // LESSON 03: Inputs management (keyboard, mouse)
        if (IsKeyPressed(KEY_ENTER)) {
          // Replay / Exit game logic
          screen = TITLE;
          framesCounter = 0;
        }

      } break;
      default:
        break;
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

    ClearBackground(GRAY);

    switch (screen) {
      case LOGO: {
        // Draw LOGO screen here!

        DrawText("Fruit Ninja by qkzk", 20, 20, 40, DARKPURPLE);

      } break;
      case TITLE: {
        // Draw TITLE screen here!

        DrawText("Fruit Ninja by qkzk", 20, 20, 40, DARKPURPLE);

        if ((framesCounter / 30) % 2 == 0)
          DrawText("PRESS [ENTER] to START",
                   GetScreenWidth() / 2 -
                       MeasureText("PRESS [ENTER] to START", 20) / 2,
                   GetScreenHeight() / 2 + 60, 20, RAYWHITE);

      } break;
      case GAMEPLAY: {
        // Draw GAMEPLAY screen here!
        for (int i = 0; i < gameState.nbFruits; i++) {
          DrawCircleV(fruits[i].position, fruits[i].size.x, fruits[i].color);
        }

        // Draw pause message when required
        if (gamePaused)
          DrawText("Game Paused",
                   screenWidth / 2 - MeasureText("GAME PAUSED", 40) / 2,
                   screenHeight / 2 + 60, 40, DARKPURPLE);

      } break;
      case ENDING: {
        // Draw END screen here!

        DrawText("Game Over", 20, 20, 40, DARKPURPLE);

        if ((framesCounter / 30) % 2 == 0)
          DrawText("PRESS [ENTER] TO PLAY AGAIN",
                   GetScreenWidth() / 2 -
                       MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
                   GetScreenHeight() / 2 + 80, 20, RAYWHITE);

      } break;
      default:
        break;
    }

    EndDrawing();
    //----------------------------------------------------------------------------------
  }

  // De-Initialization
  //--------------------------------------------------------------------------------------

  // NOTE: Unload any loaded resources (texture, fonts, audio)

  CloseWindow();  // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}
