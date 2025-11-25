#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define SIZE 9
#define CELL 80
#define INFO_BAR 100
#define WIDTH (CELL * SIZE)
#define HEIGHT (CELL * SIZE + INFO_BAR)

// ============================================================
// FONCTIONS LOGIQUES
// ============================================================
bool isValid(int grid[SIZE][SIZE], int row, int col, int val) {
    for (int i = 0; i < SIZE; i++)
        if (grid[row][i] == val || grid[i][col] == val)
            return false;

    int sr = (row / 3) * 3;
    int sc = (col / 3) * 3;
    for (int i = sr; i < sr + 3; i++)
        for (int j = sc; j < sc + 3; j++)
            if (grid[i][j] == val)
                return false;
    return true;
}

bool fillGrid(int grid[SIZE][SIZE]) {
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

void makePuzzle(int grid[SIZE][SIZE], int emptyCount) {
    int removed = 0;
    while (removed < emptyCount) {
        int r = rand() % SIZE;
        int c = rand() % SIZE;
        if (grid[r][c] != 0) {
            grid[r][c] = 0;
            removed++;
        }
    }
}

void generateSudoku(int solution[SIZE][SIZE], int baseGrid[SIZE][SIZE], int grid[SIZE][SIZE], int emptyCount) {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            solution[i][j] = 0;

    fillGrid(solution);

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            baseGrid[i][j] = solution[i][j];

    makePuzzle(baseGrid, emptyCount);

    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            grid[i][j] = baseGrid[i][j];
}

// ============================================================
// PROGRAMME PRINCIPAL
// ============================================================
int main(void) {
    srand(time(NULL));
    InitWindow(WIDTH, HEIGHT, "Sudoku Raylib - Boutons interactifs");
    SetTargetFPS(60);

    Font font = LoadFont("RobotoMono-Regular.ttf");

    int solution[SIZE][SIZE] = {0};
    int baseGrid[SIZE][SIZE] = {0};
    int grid[SIZE][SIZE] = {0};
    int difficulty = 45;
    const char *difficultyName = "Moyen";

    generateSudoku(solution, baseGrid, grid, difficulty);

    int selectedRow = -1, selectedCol = -1;
    Rectangle btnNew = { 50, CELL * SIZE + 20, 200, 60 };
    Rectangle btnDiff = { WIDTH/2 - 100, CELL * SIZE + 20, 200, 60 };
    Rectangle btnQuit = { WIDTH - 250, CELL * SIZE + 20, 200, 60 };

    bool sudokuCompleted = false;  // 🟢 on retient le succès ici

    while (!WindowShouldClose()) {

        Vector2 mouse = GetMousePosition();

        // --- Boutons ---
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            if (CheckCollisionPointRec(mouse, btnNew)) {
                generateSudoku(solution, baseGrid, grid, difficulty);
                selectedRow = selectedCol = -1;
            }
            else if (CheckCollisionPointRec(mouse, btnDiff)) {
                if (difficulty == 35) { difficulty = 45; difficultyName = "Moyen"; }
                else if (difficulty == 45) { difficulty = 55; difficultyName = "Difficile"; }
                else { difficulty = 35; difficultyName = "Facile"; }
            }
            else if (CheckCollisionPointRec(mouse, btnQuit)) {
                break;
            }
            else if (mouse.y < CELL * SIZE) {
                selectedCol = mouse.x / CELL;
                selectedRow = mouse.y / CELL;
            }
        }

        // --- Clavier ---
        if (selectedRow >= 0 && selectedCol >= 0 && baseGrid[selectedRow][selectedCol] == 0) {
            for (int i = KEY_ONE; i <= KEY_NINE; i++) {
                if (IsKeyPressed(i))
                    grid[selectedRow][selectedCol] = i - KEY_ZERO;
            }
            if (IsKeyPressed(KEY_ZERO) || IsKeyPressed(KEY_BACKSPACE))
                grid[selectedRow][selectedCol] = 0;
        }

        BeginDrawing();
        ClearBackground((Color){240,240,240,255});

        bool full = true, allCorrect = true;

        // --- Grille Sudoku ---
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                int val = grid[i][j];
                Color bg = ((i + j) % 2 == 0) ? (Color){245,245,245,255} : WHITE;
                DrawRectangle(j * CELL, i * CELL, CELL, CELL, bg);

                if (val > 0) {
                    Color col;
                    if (baseGrid[i][j] != 0)
                        col = BLACK;
                    else if (val == solution[i][j])
                        col = DARKBLUE;
                    else
                        col = ORANGE;

                    char txt[2];
                    sprintf(txt, "%d", val);
                    Vector2 size = MeasureTextEx(font, txt, CELL * 0.6f, 0);
                    DrawTextEx(font, txt,
                               (Vector2){j * CELL + (CELL - size.x) / 2, i * CELL + (CELL - size.y) / 2},
                               CELL * 0.6f, 0, col);

                    if (val != solution[i][j]) allCorrect = false;
                } else {
                    full = false;
                    allCorrect = false;
                }
            }
        }

        // --- Lignes ---
        for (int i = 0; i <= SIZE; i++) {
            int thick = (i % 3 == 0) ? 3 : 1;
            DrawLineEx((Vector2){0, i * CELL}, (Vector2){WIDTH, i * CELL}, thick, BLACK);
            DrawLineEx((Vector2){i * CELL, 0}, (Vector2){i * CELL, CELL * SIZE}, thick, BLACK);
        }

        // --- Case sélectionnée ---
        if (selectedRow >= 0 && selectedCol >= 0)
            DrawRectangleLinesEx((Rectangle){selectedCol * CELL, selectedRow * CELL, CELL, CELL}, 3, BLUE);

        // --- Victoire ---
        if (full && allCorrect) {
            sudokuCompleted = true;  // ✅ On le marque comme réussi
            DrawRectangle(0, CELL * SIZE / 2 - 40, WIDTH, 80, Fade(GREEN, 0.8f));
            DrawTextEx(font, "✅ Sudoku complet et correct !",
                       (Vector2){WIDTH/2 - MeasureTextEx(font, "✅ Sudoku complet et correct !", 40, 0).x/2,
                                 CELL * SIZE / 2 - 20},
                       40, 0, WHITE);
        }

        // --- Bandeau inférieur ---
        DrawRectangle(0, CELL * SIZE, WIDTH, INFO_BAR, (Color){230,230,230,255});

        // Boutons
        Color cNew = CheckCollisionPointRec(mouse, btnNew) ? (Color){150,255,150,255} : (Color){180,255,180,255};
        Color cDiff = CheckCollisionPointRec(mouse, btnDiff) ? (Color){180,210,255,255} : (Color){200,230,255,255};
        Color cQuit = CheckCollisionPointRec(mouse, btnQuit) ? (Color){255,180,180,255} : (Color){255,200,200,255};

        DrawRectangleRec(btnNew, cNew);
        DrawRectangleRec(btnDiff, cDiff);
        DrawRectangleRec(btnQuit, cQuit);

        DrawRectangleLinesEx(btnNew, 2, DARKGREEN);
        DrawRectangleLinesEx(btnDiff, 2, DARKBLUE);
        DrawRectangleLinesEx(btnQuit, 2, DARKGRAY);

        float txtSize = 25;

        const char *txtNew = "Nouvelle grille";
        Vector2 sNew = MeasureTextEx(font, txtNew, txtSize, 0);
        DrawTextEx(font, txtNew, (Vector2){btnNew.x + (btnNew.width - sNew.x)/2, btnNew.y + (btnNew.height - sNew.y)/2}, txtSize, 0, BLACK);

        const char *txtDiff = TextFormat("Difficulte: %s", difficultyName);
        Vector2 sDiff = MeasureTextEx(font, txtDiff, txtSize, 0);
        DrawTextEx(font, txtDiff, (Vector2){btnDiff.x + (btnDiff.width - sDiff.x)/2, btnDiff.y + (btnDiff.height - sDiff.y)/2}, txtSize, 0, BLACK);

        const char *txtQuit = "Quitter";
        Vector2 sQuit = MeasureTextEx(font, txtQuit, txtSize, 0);
        DrawTextEx(font, txtQuit, (Vector2){btnQuit.x + (btnQuit.width - sQuit.x)/2, btnQuit.y + (btnQuit.height - sQuit.y)/2}, txtSize, 0, BLACK);

        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();

    // --- Sauvegarde du résultat ---
    FILE *f = fopen("sudoku_result.txt", "w");
    if (f) {
        if (sudokuCompleted)
            fprintf(f, "1");   // Sudoku terminé
        else
            fprintf(f, "0");   // Abandonné
        fclose(f);
    }

    return 0;
}