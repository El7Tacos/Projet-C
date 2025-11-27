/* Compiler avec: gcc morpion.c -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -o morpion.exe */

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SIZE 3

#define MAX_SNOW 220

typedef struct { float x, y, vy, size; } Snowflake;
static Snowflake snow[MAX_SNOW];
static bool snow_initialized = false;

typedef enum { EMPTY, PLAYER, AI } Cell;
Cell board[SIZE][SIZE];
int gameOver = 0;
int winner = EMPTY;
Texture2D croixTex;
Texture2D rondTex;
Texture2D background;
Texture2D guirlande;
Font  fontNoel;



float aiDelay = 1.0f;   // délai avant que l’IA joue (1 seconde)
float aiTimer = 0.0f;   // compteur de temps depuis le coup du joueur
int aiPending = 0;      // indique si l'IA doit bientôt jouer
float endTimer = 0.0f;  // compteur pour le décompte de fin
int endCountdownActive = 0;



// Taille et position de la grille 
int gridSize = 600;
int offsetX, offsetY;

// Prototypes 
void ResetBoard(void);
int CheckWin(Cell c);
int IsBoardFull(void);
void AI_Move(void);
void DrawX(int x, int y, int cellSize, Texture2D croixTex);
void DrawO(int x, int y, int cellSize, Texture2D rondTex);
void initSnowflakes(int screenW, int screenH);
void updateSnowflakes(int screenW, int screenH);

void ResetBoard() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            board[i][j] = EMPTY;
    gameOver = 0;
    winner = EMPTY;
}

// Vérifie si un joueur a gagné
int CheckWin(Cell c) {
    for (int i = 0; i < SIZE; i++) {
        if (board[i][0] == c && board[i][1] == c && board[i][2] == c) return 1;
        if (board[0][i] == c && board[1][i] == c && board[2][i] == c) return 1;
    }
    if (board[0][0] == c && board[1][1] == c && board[2][2] == c) return 1;
    if (board[0][2] == c && board[1][1] == c && board[2][0] == c) return 1;
    return 0;
}

// Vérifie si le plateau est plein
int IsBoardFull() {
    for (int i = 0; i < SIZE; i++)
        for (int j = 0; j < SIZE; j++)
            if (board[i][j] == EMPTY) return 0;
    return 1;
}

// L’IA joue son coup
void AI_Move() {
    int smartMove = (rand() % 100) < 30; // 30% de chance de réfléchir

    if (smartMove) {
        // 1️⃣ Essaie de gagner
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = AI;
                    if (CheckWin(AI)) return;
                    board[i][j] = EMPTY;
                }
            }
        }
        // 2️⃣ Essaie de bloquer le joueur
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = PLAYER;
                    if (CheckWin(PLAYER)) {
                        board[i][j] = AI;
                        return;
                    }
                    board[i][j] = EMPTY;
                }
            }
        }
    }

    // 3️⃣ Sinon, joue au hasard
    int emptyCells[SIZE*SIZE][2];
    int count = 0;
    for (int i=0;i<SIZE;i++)
        for (int j=0;j<SIZE;j++)
            if (board[i][j]==EMPTY) {
                emptyCells[count][0]=i;
                emptyCells[count][1]=j;
                count++;
            }
    if (count>0) {
        int choice = rand() % count;
        int r = emptyCells[choice][0];
        int c = emptyCells[choice][1];
        board[r][c] = AI;
    }
}

void DrawX(int x, int y, int cellSize, Texture2D croixTex) {
    // centre de la case
    Vector2 center = { x + cellSize/2.0f, y + cellSize/2.0f };

    float scale = (float)cellSize / croixTex.width * 0.8f;
    float destW = croixTex.width * scale;
    float destH = croixTex.height * scale;

    Rectangle srcRec = { 0.0f, 0.0f, (float)croixTex.width, (float)croixTex.height };
    Rectangle destRec = { center.x - destW/2.0f, center.y - destH/2.0f, destW, destH };
    Vector2 origin = { 0.0f, 0.0f }; 

    DrawTexturePro(croixTex, srcRec, destRec, origin, 0.0f, WHITE);
}

void DrawO(int x, int y, int cellSize, Texture2D rondTex) {
    Vector2 center = { x + cellSize/2.0f, y + cellSize/2.0f };

    float scale = (float)cellSize / rondTex.width * 0.8f;

    float destW = rondTex.width * scale;
    float destH = rondTex.height * scale;

    Rectangle srcRec = { 0.0f, 0.0f, (float)rondTex.width, (float)rondTex.height };
    Rectangle destRec = { center.x - destW/2.0f, center.y - destH/2.0f, destW, destH };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(rondTex, srcRec, destRec, origin, 0.0f, WHITE);
}

// Initialise les flocons de neige
void initSnowflakes(int screenW, int screenH) {
    for (int i = 0; i < MAX_SNOW; i++) {
        snow[i].x = (float)(rand() % screenW);
        snow[i].y = (float)(rand() % screenH);
        snow[i].vy = 0.4f + (rand() % 100) / 250.0f;
        snow[i].size = 1.0f + (rand() % 7) / 3.0f;
    }
    snow_initialized = true;
}

// Met à jour la position des flocons
void updateSnowflakes(int screenW, int screenH) {
    if (!snow_initialized) initSnowflakes(screenW, screenH);
    for (int i = 0; i < MAX_SNOW; i++) {
        snow[i].y += snow[i].vy;
        snow[i].x += sinf((float)GetTime() + i) * 0.4f;
        if (snow[i].y > screenH + 10) {
            snow[i].y = -10 - (rand() % 50);
            snow[i].x = (float)(rand() % screenW);
        }
    }
}

int main(void) {
    int windowWidth = 1920;
    int windowHeight = 1080;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "Mini-Jeu : Morpion");
    //----------------------------------------
    // 🎵 MUSIQUE MORPION : PIANO.mp3
    //----------------------------------------
    if (!IsAudioDeviceReady()) InitAudioDevice();
    Music pianoMusic = LoadMusicStream("PIANO.mp3");
    SetMusicVolume(pianoMusic, 1.0f);
    PlayMusicStream(pianoMusic);

    fontNoel = LoadFontEx("MerryChristmasFlake.ttf", 80, 0, 0);
    GenTextureMipmaps(&fontNoel.texture);
    SetTextureFilter(fontNoel.texture, TEXTURE_FILTER_BILINEAR);
    
    Font fontWin = LoadFontEx("SantasSleighFull Bold.ttf", 80, 0, 0);
    GenTextureMipmaps(&fontWin.texture);
    SetTextureFilter(fontWin.texture, TEXTURE_FILTER_BILINEAR);

    Font fontReplay = LoadFontEx("SantasSleighFull.ttf", 50, 0, 0);
    GenTextureMipmaps(&fontReplay.texture);
    SetTextureFilter(fontReplay.texture, TEXTURE_FILTER_BILINEAR);

    croixTex = LoadTexture("croix.png");
    rondTex  = LoadTexture("rond.png");
    background = LoadTexture("background.png"); 
    guirlande = LoadTexture("guirlande.png");

    Color BLEUCLAIR = (Color){ 182, 195, 210, 255 };

    SetWindowPosition(0, 0); // Coin haut gauche
    SetTargetFPS(60);
    srand(time(NULL));
    ResetBoard();

    // Calcul offsets pour centrer la grille
    offsetX = (windowWidth - gridSize)/2;
    offsetY = (windowHeight - gridSize)/2 + 50;

    initSnowflakes(windowWidth, windowHeight);

    int shouldExit = 0; // permet sortie uniquement si le joueur a gagné

    while (1) {
        UpdateMusicStream(pianoMusic);
        // Alt+F4 = quitter immédiatement
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyDown(KEY_F4)) break;
        // Si la fenêtre demande à se fermer, n'accepter que si le joueur a gagné
        if (WindowShouldClose()) {
            if (winner == PLAYER || shouldExit) break;
        }

        // Gestion du joueur
        if (!gameOver && !aiPending && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mouse = GetMousePosition();
            int row = (mouse.y - offsetY) / (gridSize / SIZE);
            int col = (mouse.x - offsetX) / (gridSize / SIZE);

            if (row >=0 && row < SIZE && col >=0 && col < SIZE && board[row][col]==EMPTY) {
                board[row][col] = PLAYER;

                if (CheckWin(PLAYER)) {
                    winner = PLAYER;
                    gameOver = 1;
                    endTimer = 0.0f;
                    endCountdownActive = 1;   // démarre le compte à rebours
                } else if (IsBoardFull()) {
                    gameOver = 1;
                } else {
                    aiPending = 1;   // active le timer
                    aiTimer = 0.0f;  // remet le chrono à zéro
                }

            }
        }

        
        // Rejouer (seulement si le joueur n'a PAS gagné)
        if (gameOver && winner != PLAYER && IsKeyPressed(KEY_ENTER)) {
            ResetBoard();
        }

        
        // Gestion du délai IA
        if (aiPending && !gameOver) {
            aiTimer += GetFrameTime();  // ajoute le temps écoulé depuis la dernière frame
            if (aiTimer >= aiDelay) {
                AI_Move();
                aiPending = 0; // reset du flag

                if (CheckWin(AI)) {
                    winner = AI;
                    gameOver = 1;
                } else if (IsBoardFull()) {
                    gameOver = 1;
                }
            }
        }

    updateSnowflakes(windowWidth, windowHeight);

// Fermeture auto après 3 secondes si le joueur gagne
if (endCountdownActive && winner == PLAYER) {
    endTimer += GetFrameTime();
    if (endTimer >= 3.0f) {
        shouldExit = 1; // demander sortie propre de la boucle principale
        break;
    }
}

////////////////////////////// 
// Dessin
////////////////////////////////
BeginDrawing();

// Affichage du fond sans déformation
float scale = fmax((float)GetScreenWidth() / background.width,
                   (float)GetScreenHeight() / background.height);

int newWidth  = background.width * scale;
int newHeight = background.height * scale;

int posX = (GetScreenWidth()  - newWidth)  / 2;
int posY = (GetScreenHeight() - newHeight) / 2;

DrawTexturePro(
    background,
    (Rectangle){0, 0, (float)background.width, (float)background.height},
    (Rectangle){(float)posX, (float)posY, (float)newWidth, (float)newHeight},
    (Vector2){0, 0},
    0.0f,
    WHITE
);

// Titre dans la fenêtre
float titleSize = 80.0f;
Vector2 titlePos = {
    windowWidth/2 - MeasureTextEx(fontNoel, "Mini-Jeu : Morpion", titleSize, 2).x/2,
    80
};
DrawTextEx(fontNoel, "Mini-Jeu : Morpion", titlePos, titleSize, 2, DARKBLUE);

// Fond de la grille
DrawRectangle(offsetX, offsetY, gridSize, gridSize, BLEUCLAIR);

// Neige animée
for (int i = 0; i < MAX_SNOW; i++) {
    DrawCircle((int)snow[i].x, (int)snow[i].y, snow[i].size, Fade(RAYWHITE, 0.95f));
}

// Dessin des lignes
int cellSize = gridSize / SIZE;
for (int i=1;i<SIZE;i++) {
    DrawLine(offsetX, offsetY + i*cellSize, offsetX + gridSize, offsetY + i*cellSize, DARKGREEN);
    DrawLine(offsetX + i*cellSize, offsetY, offsetX + i*cellSize, offsetY + gridSize, DARKGREEN);
}
        
// Cadre autour de la grille
DrawRectangleLines(offsetX , offsetY , gridSize, gridSize, RED);

// Guirlande
int guirlandeWidth = guirlande.width;
int guirlandeHeight = guirlande.height;
int guirlandeX = offsetX + (gridSize - guirlandeWidth) / 2;
int guirlandeY = offsetY + gridSize - guirlandeHeight / 2 -575;

DrawTexture(guirlande, guirlandeX, guirlandeY, WHITE);


// Dessin des X et O
for (int i=0;i<SIZE;i++) {
    for (int j=0;j<SIZE;j++) {
        int x = offsetX + j*cellSize;
        int y = offsetY + i*cellSize;

        if (board[i][j]==PLAYER) {
            DrawX(x, y, cellSize, croixTex);
        } else if (board[i][j]==AI) {
            DrawO(x, y, cellSize, rondTex);
        }
    }
}

// Message de fin
if (gameOver) {
    if (winner == PLAYER)
DrawTextEx(fontWin, "Vous avez gagne !",
(Vector2){ windowWidth/2 - MeasureTextEx(fontWin, "Vous avez gagne !", 60, 2).x/2 -10,
            offsetY + gridSize + 20 },
60, 2, DARKGREEN);
else if (winner == AI)
DrawTextEx(fontWin, "Perdu !",
(Vector2){ windowWidth/2 - MeasureTextEx(fontWin, "Perdu !", 60, 2).x/2,
            offsetY + gridSize + 20 },
60, 2, RED);
else
DrawTextEx(fontWin, "Match nul !",
(Vector2){ windowWidth/2 - MeasureTextEx(fontWin, "Match nul !", 60, 2).x/2,
        offsetY + gridSize + 20 },
60, 2, DARKGRAY);


// Texte de redémarrage (seulement si le joueur n'a PAS gagné)
if (winner != PLAYER) {
    float replaySize = 40; 
    DrawTextEx(
        fontReplay,
        "Appuyez sur ENTRER pour rejouer",
        (Vector2){
            windowWidth/2 - MeasureTextEx(fontReplay, "Appuyez sur ENTRER pour rejouer", replaySize, 1).x/2 -10,
            offsetY + gridSize + 95
        },
        replaySize,
        1,
        DARKBLUE
    );
}


        }

        EndDrawing();
    }

    UnloadTexture(croixTex);
    UnloadTexture(rondTex);
    UnloadTexture(background);
    UnloadTexture(guirlande);
    UnloadFont(fontNoel);
    UnloadFont(fontWin);
    UnloadFont(fontReplay);
    UnloadMusicStream(pianoMusic);

    CloseWindow();
    return 0;
}
