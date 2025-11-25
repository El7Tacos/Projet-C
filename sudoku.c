#include "sudoku.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <math.h>


#define SIZE 9
#define CELL 80
#define ORIGIN_X 560
#define ORIGIN_Y 140

// ----- Décor Noël identique au Morpion -----
#define MAX_SNOW 220
typedef struct { float x, y, vy, size; } Snowflake;
static Snowflake snow[MAX_SNOW];
static bool snow_initialized = false;

Texture2D backgroundTex;
Font fontTitle;
Font fontSubtitle;

// ========================== VARIABLES INTERNES ==========================
static int solution[SIZE][SIZE];
static int baseGrid[SIZE][SIZE];
static int grid[SIZE][SIZE];
static int selectedRow = -1, selectedCol = -1;
static bool sudokuCompleted = false;
static bool initialized = false;

static int sudokuLevel = 2; // 1 = facile, 2 = moyen, 3 = difficile

// ========================== LOGIQUE SUDOKU ==========================
static bool isValid(int grid[SIZE][SIZE], int row, int col, int val) {
    for (int i = 0; i < SIZE; i++)
        if (grid[row][i] == val || grid[i][col] == val)
            return false;

    int sr = (row / 3) * 3, sc = (col / 3) * 3;
    for (int i = sr; i < sr + 3; i++)
        for (int j = sc; j < sc + 3; j++)
            if (grid[i][j] == val)
                return false;
    return true;
}

static bool fillGrid(int grid[SIZE][SIZE]) {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (grid[r][c] == 0) {
                int nums[9] = {1,2,3,4,5,6,7,8,9};
                for (int i = 0; i < 9; i++) {
                    int j = rand() % 9;
                    int tmp = nums[i]; nums[i] = nums[j]; nums[j] = tmp;
                }
                for (int n = 0; n < 9; n++) {
                    int val = nums[n];
                    if (isValid(grid, r, c, val)) {
                        grid[r][c] = val;
                        if (fillGrid(grid)) return true;
                        grid[r][c] = 0;
                    }
                }
                return false;
            }
        }
    }
    return true;
}

static void makePuzzle(int grid[SIZE][SIZE], int emptyCount) {
    int removed = 0;
    while (removed < emptyCount) {
        int r = rand() % SIZE, c = rand() % SIZE;
        if (grid[r][c] != 0) {
            grid[r][c] = 0;
            removed++;
        }
    }
}

static void generateSudoku(void) {
    int temp[SIZE][SIZE] = {0};
    fillGrid(temp);

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++) {
            solution[i][j] = temp[i][j];
            baseGrid[i][j] = temp[i][j];
            grid[i][j] = temp[i][j];
        }

    int emptyCount = 45;
    if (sudokuLevel == 1) emptyCount = 30;
    if (sudokuLevel == 2) emptyCount = 38;
    if (sudokuLevel == 3) emptyCount = 52;

    makePuzzle(baseGrid, emptyCount);

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            grid[i][j] = baseGrid[i][j];

    sudokuCompleted = false;
    initialized = true;
}

// ========================== RÉINITIALISATION ==========================
void SudokuReset(void) {
    initialized = false;
    sudokuCompleted = false;
}

// ========================== STATUT ==========================
bool SudokuIsCompleted(void) {
    return sudokuCompleted;
}
void SudokuSetLevel(int level) {
    if (level < 1) level = 1;
    if (level > 3) level = 3;
    sudokuLevel = level;
    initialized = false;
    sudokuCompleted = false;
}

// ========================== NEIGE ==========================
static void initSnowflakes(int W, int H) {
    for (int i = 0; i < MAX_SNOW; i++) {
        snow[i].x = rand() % W;
        snow[i].y = rand() % H;
        snow[i].vy = 0.4f + (rand() % 100) / 250.0f;
        snow[i].size = 1.0f + (rand() % 7) / 3.0f;
    }
    snow_initialized = true;
}
static void updateSnowflakes(int W, int H) {
    if (!snow_initialized) initSnowflakes(W, H);
    for (int i = 0; i < MAX_SNOW; i++) {
        snow[i].y += snow[i].vy;
        snow[i].x += sinf(GetTime() + i) * 0.4f;
        if (snow[i].y > H + 10) {
            snow[i].y = -10 - (rand() % 50);
            snow[i].x = rand() % W;
        }
    }
}

// ========================== LANCEMENT ==========================
bool StartSudoku(Font ignoredFontFromPlateau) {

    SetExitKey(KEY_NULL);
    srand(time(NULL));
    if (!initialized) generateSudoku();

    // ----- CHARGEMENT ASSETS NOËL -----
    backgroundTex = LoadTexture("background.png");

    fontTitle = LoadFontEx("MerryChristmasFlake.ttf", 80, 0, 0);
    GenTextureMipmaps(&fontTitle.texture);
    SetTextureFilter(fontTitle.texture, TEXTURE_FILTER_BILINEAR);

    fontSubtitle = LoadFontEx("SantasSleighFull.ttf", 50, 0, 0);
    GenTextureMipmaps(&fontSubtitle.texture);
    SetTextureFilter(fontSubtitle.texture, TEXTURE_FILTER_BILINEAR);

    int W = GetScreenWidth();
    int H = GetScreenHeight();
    int totalSize = CELL * SIZE;

    int originX = (W - totalSize) / 2;
    int originY = (H - totalSize) / 2;
    
    while (!WindowShouldClose()) {

        Vector2 mouse = GetMousePosition();

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        mouse.x > originX && mouse.x < originX + CELL*SIZE &&
        mouse.y > originY && mouse.y < originY + CELL*SIZE) {
    
        selectedCol = (mouse.x - originX) / CELL;
        selectedRow = (mouse.y - originY) / CELL;
    }
    
        // ----- ÉCRITURE -----
        if (selectedRow >= 0 && selectedCol >= 0 &&
            baseGrid[selectedRow][selectedCol] == 0) {

            for (int i = KEY_ONE; i <= KEY_NINE; i++)
                if (IsKeyPressed(i))
                    grid[selectedRow][selectedCol] = i - KEY_ZERO;

            if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_BACKSPACE))
                grid[selectedRow][selectedCol] = 0;
        }

        // ----- RESET -----
        if (IsKeyPressed(KEY_R)) generateSudoku();

        // ----- VÉRIFICATION -----
        bool full = true, allCorrect = true;
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++) {
                int v = grid[i][j];
                if (v == 0) { full = false; allCorrect = false; }
                else if (v != solution[i][j]) allCorrect = false;
            }

        if (full && allCorrect) {
            sudokuCompleted = true;
        }

        // ----- UPDATE NEIGE -----
        updateSnowflakes(W, H);

        // =====================================================================
        //                                DESSIN
        // =====================================================================
        BeginDrawing();

        // ---- 1) Fond Noël (exact Morpion) ----
        float scale = fmax((float)W / backgroundTex.width,
                           (float)H / backgroundTex.height);

        int newW = backgroundTex.width * scale;
        int newH = backgroundTex.height * scale;
        int posX = (W - newW) / 2;
        int posY = (H - newH) / 2;

        DrawTexturePro(backgroundTex,
            (Rectangle){0,0,(float)backgroundTex.width,(float)backgroundTex.height},
            (Rectangle){posX, posY, newW, newH},
            (Vector2){0,0}, 0, WHITE);

        // ---- 2) Titre ----
        DrawTextEx(fontTitle, "Mini-Jeu : Sudoku",
                   (Vector2){ W/2 - MeasureTextEx(fontTitle,
                   "Mini-Jeu : Sudoku", 80, 2).x/2, 40},
                   80, 2, DARKBLUE);

        // ---- 4) Neige ----
        for (int i = 0; i < MAX_SNOW; i++)
            DrawCircle(snow[i].x, snow[i].y, snow[i].size,
                       Fade(RAYWHITE, 0.95f));

        // ---- 5) Zone Sudoku ----
        for (int i = 0; i < SIZE; i++)
            for (int j = 0; j < SIZE; j++) {

                int v = grid[i][j];
                Color bg = ((i + j) % 2 == 0) ? (Color){245,245,245,255} : WHITE;

                DrawRectangle(originX + j*CELL, originY + i*CELL, CELL, CELL, bg);


                if (v > 0) {
                    Color c = baseGrid[i][j]
                        ? BLACK
                        : (v == solution[i][j] ? DARKBLUE : ORANGE);

                    char s[2]; sprintf(s, "%d", v);

                    Vector2 m = MeasureTextEx(fontSubtitle, s, CELL*0.6f, 0);
                    DrawTextEx(fontSubtitle, s,
                        (Vector2){ originX + j*CELL + (CELL - m.x)/2,
                                   originY + i*CELL + (CELL - m.y)/2 },
                        CELL*0.6f, 0, c);
             
                }
            }

        // ---- 6) Ligne du Sudoku ----
        for (int i = 0; i <= SIZE; i++) {
            int thick = (i % 3 == 0) ? 3 : 1;
            DrawLineEx((Vector2){originX, originY + i*CELL},
           (Vector2){originX + CELL*SIZE, originY + i*CELL},
           thick, DARKGREEN);

            DrawLineEx((Vector2){originX + i*CELL, originY},
           (Vector2){originX + i*CELL, originY + CELL*SIZE},
           thick, RED);

        }

        // ---- 7) Surlignage case sélectionnée ----
        if (selectedRow >= 0 && selectedCol >= 0)
        DrawRectangleLinesEx(
            (Rectangle){ originX + selectedCol*CELL,
                         originY + selectedRow*CELL,
                         CELL, CELL },
            3, BLUE);
        

        // ---- 9) Message si terminé ----
        if (sudokuCompleted) {
            DrawTextEx(fontTitle, "Sudoku terminé !",
                (Vector2){ W/2 - MeasureTextEx(fontTitle,
                    "Sudoku terminé !", 60, 2).x/2, originY + CELL*SIZE + 40 },
                60,
                2,
                DARKGREEN);

            EndDrawing();
            WaitTime(1.5f);

            UnloadTexture(backgroundTex);
            UnloadFont(fontTitle);
            UnloadFont(fontSubtitle);
            return true;
        }

        EndDrawing();
    }

    // Unload si ESC ou quit
    UnloadTexture(backgroundTex);
    UnloadFont(fontTitle);
    UnloadFont(fontSubtitle);

    return false;
}
