/* Compiler avec :
   gcc morpion.c -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -o morpion.exe
*/

#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define SIZE 3
#define MAX_SNOW 220

typedef struct
{
    float x;
    float y;
    float vy;
    float size;
} Snowflake;

static Snowflake snow[MAX_SNOW];
static bool       snow_initialized = false;

typedef enum
{
    EMPTY,
    PLAYER,
    AI
} Cell;

static Cell board[SIZE][SIZE];
static int  gameOver            = 0;
static int  winner              = EMPTY;

static Texture2D croixTex;
static Texture2D rondTex;
static Texture2D background;
static Texture2D guirlande;
static Font      fontNoel;

static float aiDelay           = 1.0f;  // délai avant que l'IA joue (en secondes)
static float aiTimer           = 0.0f;  // temps écoulé depuis le coup du joueur
static int   aiPending         = 0;     // l'IA doit-elle bientôt jouer ?
static float endTimer          = 0.0f;  // compteur pour le décompte de fin
static int   endCountdownActive = 0;

// Taille et position de la grille
static int gridSize = 600;
static int offsetX  = 0;
static int offsetY  = 0;

// Prototypes
static void ResetBoard(void);
static int  CheckWin(Cell c);
static int  IsBoardFull(void);
static void AI_Move(void);
static void DrawX(int x, int y, int cellSize, Texture2D tex);
static void DrawO(int x, int y, int cellSize, Texture2D tex);
static void initSnowflakes(int screenW, int screenH);
static void updateSnowflakes(int screenW, int screenH);

static void ResetBoard(void)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            board[i][j] = EMPTY;
        }
    }

    gameOver           = 0;
    winner             = EMPTY;
    aiPending          = 0;
    aiTimer            = 0.0f;
    endTimer           = 0.0f;
    endCountdownActive = 0;
}

// Vérifie si un joueur a gagné
static int CheckWin(Cell c)
{
    for (int i = 0; i < SIZE; i++)
    {
        if (board[i][0] == c && board[i][1] == c && board[i][2] == c)
        {
            return 1;
        }

        if (board[0][i] == c && board[1][i] == c && board[2][i] == c)
        {
            return 1;
        }
    }

    if (board[0][0] == c && board[1][1] == c && board[2][2] == c)
    {
        return 1;
    }

    if (board[0][2] == c && board[1][1] == c && board[2][0] == c)
    {
        return 1;
    }

    return 0;
}

// Vérifie si le plateau est plein
static int IsBoardFull(void)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == EMPTY)
            {
                return 0;
            }
        }
    }

    return 1;
}

// Coup de l'IA
static void AI_Move(void)
{
    int smartMove = (rand() % 100) < 30; // 30 % de chances de jouer "intelligemment"

    if (smartMove)
    {
        // 1. Tente de gagner
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                if (board[i][j] == EMPTY)
                {
                    board[i][j] = AI;

                    if (CheckWin(AI))
                    {
                        return;
                    }

                    board[i][j] = EMPTY;
                }
            }
        }

        // 2. Tente de bloquer le joueur
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                if (board[i][j] == EMPTY)
                {
                    board[i][j] = PLAYER;

                    if (CheckWin(PLAYER))
                    {
                        board[i][j] = AI;
                        return;
                    }

                    board[i][j] = EMPTY;
                }
            }
        }
    }

    // 3. Sinon, joue au hasard
    int emptyCells[SIZE * SIZE][2];
    int count = 0;

    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            if (board[i][j] == EMPTY)
            {
                emptyCells[count][0] = i;
                emptyCells[count][1] = j;
                count++;
            }
        }
    }

    if (count > 0)
    {
        int choice = rand() % count;
        int r      = emptyCells[choice][0];
        int c      = emptyCells[choice][1];

        board[r][c] = AI;
    }
}

static void DrawX(int x, int y, int cellSize, Texture2D tex)
{
    Vector2 center = { x + cellSize / 2.0f, y + cellSize / 2.0f };

    float    scale = (float)cellSize / tex.width * 0.8f;
    float    destW = tex.width * scale;
    float    destH = tex.height * scale;
    Rectangle src  = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Rectangle dst  = {
        center.x - destW / 2.0f,
        center.y - destH / 2.0f,
        destW,
        destH
    };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(tex, src, dst, origin, 0.0f, WHITE);
}

static void DrawO(int x, int y, int cellSize, Texture2D tex)
{
    Vector2 center = { x + cellSize / 2.0f, y + cellSize / 2.0f };

    float    scale = (float)cellSize / tex.width * 0.8f;
    float    destW = tex.width * scale;
    float    destH = tex.height * scale;
    Rectangle src  = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
    Rectangle dst  = {
        center.x - destW / 2.0f,
        center.y - destH / 2.0f,
        destW,
        destH
    };
    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(tex, src, dst, origin, 0.0f, WHITE);
}

// Initialise les flocons de neige
static void initSnowflakes(int screenW, int screenH)
{
    for (int i = 0; i < MAX_SNOW; i++)
    {
        snow[i].x    = (float)(rand() % screenW);
        snow[i].y    = (float)(rand() % screenH);
        snow[i].vy   = 0.4f + (rand() % 100) / 250.0f;
        snow[i].size = 1.0f + (rand() % 7) / 3.0f;
    }

    snow_initialized = true;
}

// Met à jour la position des flocons
static void updateSnowflakes(int screenW, int screenH)
{
    if (!snow_initialized)
    {
        initSnowflakes(screenW, screenH);
    }

    for (int i = 0; i < MAX_SNOW; i++)
    {
        snow[i].y += snow[i].vy;
        snow[i].x += sinf((float)GetTime() + (float)i) * 0.4f;

        if (snow[i].y > screenH + 10)
        {
            snow[i].y = -10.0f - (float)(rand() % 50);
            snow[i].x = (float)(rand() % screenW);
        }
    }
}

int main(void)
{
    int windowWidth  = 1920;
    int windowHeight = 1080;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "Mini-Jeu : Morpion");

    // Musique du morpion
    if (!IsAudioDeviceReady())
    {
        InitAudioDevice();
    }

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

    croixTex   = LoadTexture("croix.png");
    rondTex    = LoadTexture("rond.png");
    background = LoadTexture("background.png");
    guirlande  = LoadTexture("guirlande.png");

    Color BLEUCLAIR = (Color){ 182, 195, 210, 255 };

    SetWindowPosition(0, 0);
    SetTargetFPS(60);

    srand((unsigned int)time(NULL));
    ResetBoard();

    // Calcul des offsets pour centrer la grille
    offsetX = (windowWidth - gridSize) / 2;
    offsetY = (windowHeight - gridSize) / 2 + 50;

    initSnowflakes(windowWidth, windowHeight);

    int shouldExit = 0; // sortie autorisée uniquement si le joueur a gagné

    while (1)
    {
        UpdateMusicStream(pianoMusic);

        // Alt+F4 : quitter immédiatement
        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyDown(KEY_F4))
        {
            break;
        }

        // Si la fenêtre demande à se fermer, on n'accepte que si le joueur a gagné
        if (WindowShouldClose())
        {
            if (winner == PLAYER || shouldExit)
            {
                break;
            }
        }

        // Gestion du joueur
        if (!gameOver && !aiPending && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            Vector2 mouse = GetMousePosition();
            int     cell  = gridSize / SIZE;
            int     row   = (int)((mouse.y - offsetY) / cell);
            int     col   = (int)((mouse.x - offsetX) / cell);

            if (row >= 0 && row < SIZE && col >= 0 && col < SIZE && board[row][col] == EMPTY)
            {
                board[row][col] = PLAYER;

                if (CheckWin(PLAYER))
                {
                    winner             = PLAYER;
                    gameOver           = 1;
                    endTimer           = 0.0f;
                    endCountdownActive = 1; // déclenche le compte à rebours
                }
                else if (IsBoardFull())
                {
                    gameOver = 1;
                }
                else
                {
                    aiPending = 1;  // l'IA jouera après un délai
                    aiTimer   = 0.0f;
                }
            }
        }

        // Rejouer (seulement si le joueur n'a pas gagné)
        if (gameOver && winner != PLAYER && IsKeyPressed(KEY_ENTER))
        {
            ResetBoard();
        }

        // Gestion du délai de l'IA
        if (aiPending && !gameOver)
        {
            aiTimer += GetFrameTime();

            if (aiTimer >= aiDelay)
            {
                AI_Move();
                aiPending = 0;

                if (CheckWin(AI))
                {
                    winner  = AI;
                    gameOver = 1;
                }
                else if (IsBoardFull())
                {
                    gameOver = 1;
                }
            }
        }

        updateSnowflakes(windowWidth, windowHeight);

        // Fermeture automatique quelques secondes après une victoire du joueur
        if (endCountdownActive && winner == PLAYER)
        {
            endTimer += GetFrameTime();

            if (endTimer >= 3.0f)
            {
                shouldExit = 1;
                break;
            }
        }

        // --------------------------------
        // Dessin
        // --------------------------------
        BeginDrawing();

        // Affichage du fond
        float scale = fmaxf(
            (float)GetScreenWidth() / (float)background.width,
            (float)GetScreenHeight() / (float)background.height
        );

        int newWidth  = (int)(background.width * scale);
        int newHeight = (int)(background.height * scale);

        int posX = (GetScreenWidth()  - newWidth)  / 2;
        int posY = (GetScreenHeight() - newHeight) / 2;

        DrawTexturePro(
            background,
            (Rectangle){ 0.0f, 0.0f, (float)background.width, (float)background.height },
            (Rectangle){ (float)posX, (float)posY, (float)newWidth, (float)newHeight },
            (Vector2){ 0.0f, 0.0f },
            0.0f,
            WHITE
        );

        // Titre
        float   titleSize = 80.0f;
        const char *title = "Mini-Jeu : Morpion";
        Vector2 titleMeasure = MeasureTextEx(fontNoel, title, titleSize, 2.0f);
        Vector2 titlePos     = {
            windowWidth / 2.0f - titleMeasure.x / 2.0f,
            80.0f
        };

        DrawTextEx(fontNoel, title, titlePos, titleSize, 2.0f, DARKBLUE);

        // Fond de la grille
        DrawRectangle(offsetX, offsetY, gridSize, gridSize, BLEUCLAIR);

        // Neige
        for (int i = 0; i < MAX_SNOW; i++)
        {
            DrawCircle((int)snow[i].x, (int)snow[i].y, snow[i].size, Fade(RAYWHITE, 0.95f));
        }

        // Lignes de la grille
        int cellSize = gridSize / SIZE;

        for (int i = 1; i < SIZE; i++)
        {
            DrawLine(
                offsetX,
                offsetY + i * cellSize,
                offsetX + gridSize,
                offsetY + i * cellSize,
                DARKGREEN
            );

            DrawLine(
                offsetX + i * cellSize,
                offsetY,
                offsetX + i * cellSize,
                offsetY + gridSize,
                DARKGREEN
            );
        }

        // Cadre autour de la grille
        DrawRectangleLines(offsetX, offsetY, gridSize, gridSize, RED);

        // Guirlande
        int guirlandeWidth  = guirlande.width;
        int guirlandeHeight = guirlande.height;
        int guirlandeX      = offsetX + (gridSize - guirlandeWidth) / 2;
        int guirlandeY      = offsetY + gridSize - guirlandeHeight / 2 - 575;

        DrawTexture(guirlande, guirlandeX, guirlandeY, WHITE);

        // Pions
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                int x = offsetX + j * cellSize;
                int y = offsetY + i * cellSize;

                if (board[i][j] == PLAYER)
                {
                    DrawX(x, y, cellSize, croixTex);
                }
                else if (board[i][j] == AI)
                {
                    DrawO(x, y, cellSize, rondTex);
                }
            }
        }

        // Message de fin
        if (gameOver)
        {
            float msgSize = 60.0f;

            if (winner == PLAYER)
            {
                const char *msg = "Vous avez gagne !";
                Vector2      m   = MeasureTextEx(fontWin, msg, msgSize, 2.0f);

                DrawTextEx(
                    fontWin,
                    msg,
                    (Vector2){ windowWidth / 2.0f - m.x / 2.0f - 10.0f,
                               offsetY + gridSize + 20.0f },
                    msgSize,
                    2.0f,
                    DARKGREEN
                );
            }
            else if (winner == AI)
            {
                const char *msg = "Perdu !";
                Vector2      m   = MeasureTextEx(fontWin, msg, msgSize, 2.0f);

                DrawTextEx(
                    fontWin,
                    msg,
                    (Vector2){ windowWidth / 2.0f - m.x / 2.0f,
                               offsetY + gridSize + 20.0f },
                    msgSize,
                    2.0f,
                    RED
                );
            }
            else
            {
                const char *msg = "Match nul !";
                Vector2      m   = MeasureTextEx(fontWin, msg, msgSize, 2.0f);

                DrawTextEx(
                    fontWin,
                    msg,
                    (Vector2){ windowWidth / 2.0f - m.x / 2.0f,
                               offsetY + gridSize + 20.0f },
                    msgSize,
                    2.0f,
                    DARKGRAY
                );
            }

            // Texte de redémarrage (si le joueur n'a pas gagné)
            if (winner != PLAYER)
            {
                float       replaySize = 40.0f;
                const char *replayMsg  = "Appuyez sur ENTRER pour rejouer";
                Vector2     rm         = MeasureTextEx(fontReplay, replayMsg, replaySize, 1.0f);

                DrawTextEx(
                    fontReplay,
                    replayMsg,
                    (Vector2){ windowWidth / 2.0f - rm.x / 2.0f - 10.0f,
                               offsetY + gridSize + 95.0f },
                    replaySize,
                    1.0f,
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
