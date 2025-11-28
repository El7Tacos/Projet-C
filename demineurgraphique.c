// gcc demineurgraphique.c -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -o demineur.exe

#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include <math.h>

static Sound bombSound;
Texture2D gFlagTex;

#define CELL_SIZE 64
#define MAX_SNOW 200

// —————————————————————————————————————————
// STRUCTURES
// —————————————————————————————————————————

typedef struct
{
    bool hasMine;
    bool revealed;
    bool flagged;
    int  neighborMines;
} Cell;

typedef struct
{
    float x, y;
    float vy;
    float size;
} Snowflake;

typedef struct
{
    int size;
    int mineCount;
    int flagsPlaced;
    bool gameOver;
    bool win;
    bool firstClick;
    Cell **board;

    int   revealedCount;
    int   safeCellsTotal;
    float progress;
    float requiredProgress;
} Game;

// —————————————————————————————————————————
// PROTOTYPES
// —————————————————————————————————————————

static void InitGame(Game *g, int size, int mines);
static void FreeGame(Game *g);
static void ResetBoard(Game *g);
static void PlaceMines(Game *g, int safeR, int safeC);
static void ComputeNeighborCounts(Game *g);

static bool InBounds(Game *g, int r, int c);
static void RevealCell(Game *g, int r, int c);
static void RevealEmpty(Game *g, int r, int c);
static void ToggleFlag(Game *g, int r, int c);
static void CheckWin(Game *g);

static void InitSnow(Snowflake *s, int count, int w, int h);
static void UpdateSnow(Snowflake *s, int count, int w, int h);
static void DrawSnow(const Snowflake *s, int count);

static void DrawGame(const Game *g, int ox, int oy,
                     Font titleFont, Font fontConsignes, Font fontNormal,
                     int cellSize);

static void DrawCellVisual(const Cell *c, int x, int y, int size,
                           Font cellFont, bool hovered);

static int RunDemineurLevel(int level);

// —————————————————————————————————————————
// ALLOCATION / INIT
// —————————————————————————————————————————

static void InitGame(Game *g, int size, int mines)
{
    g->size            = size;
    g->mineCount       = mines;
    g->flagsPlaced     = 0;
    g->gameOver        = false;
    g->win             = false;
    g->firstClick      = true;
    g->revealedCount   = 0;
    g->safeCellsTotal  = size * size - g->mineCount;
    g->progress        = 0.0f;

    if (g->mineCount == 10)
    {
        g->requiredProgress = 0.85f;
    }
    else if (g->mineCount == 22)
    {
        g->requiredProgress = 0.90f;
    }
    else
    {
        g->requiredProgress = 0.95f;
    }

    g->board = malloc(size * sizeof(Cell *));
    for (int r = 0; r < size; r++)
    {
        g->board[r] = malloc(size * sizeof(Cell));
    }

    ResetBoard(g);
}

static void FreeGame(Game *g)
{
    if (!g->board)
    {
        return;
    }

    for (int r = 0; r < g->size; r++)
    {
        free(g->board[r]);
    }

    free(g->board);
    g->board = NULL;
}

static void ResetBoard(Game *g)
{
    for (int r = 0; r < g->size; r++)
    {
        for (int c = 0; c < g->size; c++)
        {
            g->board[r][c].hasMine       = false;
            g->board[r][c].revealed      = false;
            g->board[r][c].flagged       = false;
            g->board[r][c].neighborMines = 0;
        }
    }

    g->flagsPlaced    = 0;
    g->gameOver       = false;
    g->win            = false;
    g->firstClick     = true;
    g->revealedCount  = 0;
    g->progress       = 0.0f;
}

static bool InBounds(Game *g, int r, int c)
{
    return (r >= 0 && r < g->size && c >= 0 && c < g->size);
}

static void PlaceMines(Game *g, int safeR, int safeC)
{
    int totalCells = g->size * g->size;
    int forbidden  = 0;

    // Comptage des cases interdites (zone 3x3 autour du premier clic)
    for (int dr = -1; dr <= 1; dr++)
    {
        for (int dc = -1; dc <= 1; dc++)
        {
            int nr = safeR + dr;
            int nc = safeC + dc;

            if (InBounds(g, nr, nc))
            {
                forbidden++;
            }
        }
    }

    int available = totalCells - forbidden;

    // Sécurité : éviter de demander plus de mines que de cases possibles
    if (g->mineCount > available)
    {
        g->mineCount = available;
    }

    int placed   = 0;
    int tries    = 0;
    int maxTries = totalCells * 10; // évite une boucle infinie

    while (placed < g->mineCount && tries < maxTries)
    {
        tries++;

        int r = rand() % g->size;
        int c = rand() % g->size;

        // Pas dans la zone du premier clic
        if (abs(r - safeR) <= 1 && abs(c - safeC) <= 1)
        {
            continue;
        }

        if (!g->board[r][c].hasMine)
        {
            g->board[r][c].hasMine = true;
            placed++;
        }
    }
}

static void ComputeNeighborCounts(Game *g)
{
    for (int r = 0; r < g->size; r++)
    {
        for (int c = 0; c < g->size; c++)
        {
            if (g->board[r][c].hasMine)
            {
                g->board[r][c].neighborMines = -1;
                continue;
            }

            int count = 0;

            for (int dr = -1; dr <= 1; dr++)
            {
                for (int dc = -1; dc <= 1; dc++)
                {
                    if (dr == 0 && dc == 0)
                    {
                        continue;
                    }

                    int nr = r + dr;
                    int nc = c + dc;

                    if (InBounds(g, nr, nc) && g->board[nr][nc].hasMine)
                    {
                        count++;
                    }
                }
            }

            g->board[r][c].neighborMines = count;
        }
    }
}

// —————————————————————————————————————————
// RÉVÉLATION / DRAPEAUX
// —————————————————————————————————————————

static void RevealEmpty(Game *g, int row, int col)
{
    typedef struct
    {
        int r;
        int c;
    } Node;

    int   max   = g->size * g->size;
    Node *queue = malloc(max * sizeof(Node));
    int   front = 0;
    int   back  = 0;

    queue[back++] = (Node){ row, col };

    while (front < back)
    {
        Node n = queue[front++];
        int  r = n.r;
        int  c = n.c;

        Cell *cell = &g->board[r][c];
        if (cell->revealed)
        {
            continue;
        }

        cell->revealed = true;
        g->revealedCount++;
        g->progress = (float)g->revealedCount / g->safeCellsTotal;

        if (cell->neighborMines != 0)
        {
            continue;
        }

        for (int dr = -1; dr <= 1; dr++)
        {
            for (int dc = -1; dc <= 1; dc++)
            {
                if (dr == 0 && dc == 0)
                {
                    continue;
                }

                int nr = r + dr;
                int nc = c + dc;

                if (!InBounds(g, nr, nc))
                {
                    continue;
                }

                Cell *next = &g->board[nr][nc];

                if (!next->revealed && !next->hasMine)
                {
                    queue[back++] = (Node){ nr, nc };
                }
            }
        }
    }

    free(queue);
}

static void RevealCell(Game *g, int row, int col)
{
    if (!InBounds(g, row, col))
    {
        return;
    }

    Cell *cell = &g->board[row][col];
    if (cell->revealed || cell->flagged)
    {
        return;
    }

    if (g->firstClick)
    {
        // Placement des mines uniquement après le tout premier clic
        PlaceMines(g, row, col);
        ComputeNeighborCounts(g);
        g->firstClick = false;
        return;
    }

    if (!cell->hasMine)
    {
        g->revealedCount++;
        g->progress = (float)g->revealedCount / (float)g->safeCellsTotal;
    }

    if (cell->hasMine)
    {
        // Déclenchement du son d'explosion
        PlaySound(bombSound);

        g->gameOver = true;
        g->win      = false;

        for (int r = 0; r < g->size; r++)
        {
            for (int c = 0; c < g->size; c++)
            {
                if (g->board[r][c].hasMine)
                {
                    g->board[r][c].revealed = true;
                }
            }
        }

        return;
    }

    if (cell->neighborMines == 0)
    {
        // Propagation complète à partir d'une case vide
        RevealEmpty(g, row, col);
    }
    else
    {
        // Révélation standard d'une case numérotée
        cell->revealed = true;
        g->revealedCount++;
        g->progress = (float)g->revealedCount / g->safeCellsTotal;
    }

    CheckWin(g);
}

static void ToggleFlag(Game *g, int row, int col)
{
    Cell *cell = &g->board[row][col];
    if (cell->revealed)
    {
        return;
    }

    if (cell->flagged)
    {
        cell->flagged = false;
        g->flagsPlaced--;
    }
    else
    {
        // Évite de placer plus de drapeaux que de mines
        if (g->flagsPlaced < g->mineCount)
        {
            cell->flagged = true;
            g->flagsPlaced++;
        }
    }
}

static void CheckWin(Game *g)
{
    int hidden = 0;

    for (int r = 0; r < g->size; r++)
    {
        for (int c = 0; c < g->size; c++)
        {
            if (!g->board[r][c].hasMine && !g->board[r][c].revealed)
            {
                hidden++;
            }
        }
    }

    if (hidden == 0)
    {
        g->gameOver = true;
        g->win      = true;

        for (int r = 0; r < g->size; r++)
        {
            for (int c = 0; c < g->size; c++)
            {
                if (g->board[r][c].hasMine)
                {
                    g->board[r][c].revealed = true;
                }
            }
        }
    }

    // Condition alternative de victoire basée sur le pourcentage découvert
    if (!g->gameOver && g->progress >= g->requiredProgress)
    {
        g->gameOver = true;
        g->win      = true;

        for (int r = 0; r < g->size; r++)
        {
            for (int c = 0; c < g->size; c++)
            {
                if (g->board[r][c].hasMine)
                {
                    g->board[r][c].revealed = true;
                }
            }
        }
    }
}

// —————————————————————————————————————————
// NEIGE
// —————————————————————————————————————————

static void InitSnow(Snowflake *snow, int count, int w, int h)
{
    for (int i = 0; i < count; i++)
    {
        snow[i].x   = rand() % w;
        snow[i].y   = rand() % h;
        snow[i].vy  = 40 + rand() % 60;
        snow[i].size = 1 + rand() % 4;
    }
}

static void UpdateSnow(Snowflake *snow, int count, int w, int h)
{
    float dt = GetFrameTime();

    for (int i = 0; i < count; i++)
    {
        snow[i].y += snow[i].vy * dt;
        snow[i].x += sinf(GetTime() + i * 0.5f) * 15.0f * dt;

        if (snow[i].y > h + 10)
        {
            snow[i].y = -10;
            snow[i].x = rand() % w;
        }
    }
}

static void DrawSnow(const Snowflake *s, int count)
{
    for (int i = 0; i < count; i++)
    {
        DrawCircle(s[i].x, s[i].y, s[i].size, Fade(WHITE, 0.8f));
    }
}

// —————————————————————————————————————————
// AFFICHAGE DES CASES
// —————————————————————————————————————————

static void DrawCellVisual(const Cell *c, int x, int y, int size,
                           Font cellFont, bool hovered)
{
    Rectangle rect  = (Rectangle){ x, y, size, size };
    Color     border = (Color){ 0, 100, 0, 255 };

    // Case fermée
    if (!c->revealed)
    {
        Color bg = ((x / size + y / size) % 2 == 0)
                 ? (Color){ 255, 255, 220, 255 }
                 : (Color){ 255, 240, 200, 255 };

        DrawRectangleRec(rect, bg);

        if (hovered)
        {
            DrawRectangleLinesEx(rect, 3, (Color){ 80, 140, 255, 200 });
        }
        else
        {
            DrawRectangleLinesEx(rect, 2, border);
        }

        // Affichage du drapeau
        if (c->flagged)
        {
            DrawTexturePro(
                gFlagTex,
                (Rectangle){ 0, 0, gFlagTex.width, gFlagTex.height },
                (Rectangle){ x + 5, y + 5, size - 10, size - 10 },
                (Vector2){ 0, 0 },
                0.0f,
                WHITE
            );

            return;
        }

        return;
    }

    // Case ouverte
    Color bg = (!c->hasMine)
             ? (Color){ 230, 230, 230, 255 }
             : (Color){ 200, 40, 40, 255 };

    DrawRectangleRec(rect, bg);
    DrawRectangleLinesEx(rect, 2, border);

    if (c->hasMine)
    {
        DrawCircle(x + size / 2,       y + size / 2,       size * 0.25f, BLACK);
        DrawCircle(x + size / 2 - 5.0f, y + size / 2 - 5.0f, size * 0.06f, WHITE);
    }
    else if (c->neighborMines > 0)
    {
        char txt[2] = { (char)('0' + c->neighborMines), '\0' };

        float   fontSize = size * 0.6f;
        Vector2 t        = MeasureTextEx(cellFont, txt, fontSize, 0);
        Vector2 pos      = { x + (size - t.x) / 2, y + (size - t.y) / 2 };

        Color col = BLUE;
        switch (c->neighborMines)
        {
            case 2: col = DARKGREEN;               break;
            case 3: col = RED;                     break;
            case 4: col = (Color){ 0,   0, 120, 255 }; break;
            case 5: col = (Color){ 120, 0,   0, 255 }; break;
            case 6: col = (Color){ 0, 120, 120, 255 }; break;
            case 7: col = BLACK;                   break;
            case 8: col = DARKGRAY;                break;
            default: break;
        }

        DrawTextEx(cellFont, txt, pos, fontSize, 0, col);
    }
}

// —————————————————————————————————————————
// DRAW GAME
// —————————————————————————————————————————

static void DrawGame(const Game *g, int ox, int oy,
                     Font titleFont, Font fontConsignes, Font fontNormal,
                     int cellSize)
{
    int gridW = g->size * cellSize;
    int gridH = g->size * cellSize;

    // Titre façon Morpion
    const char *title     = "Mini-Jeu : Demineur";
    float       titleSize = 80.0f;
    Vector2     t         = MeasureTextEx(titleFont, title, titleSize, 0);

    DrawTextEx(
        titleFont,
        title,
        (Vector2){ ox + gridW / 2 - t.x / 2, oy - t.y - 20 },
        titleSize,
        0,
        DARKGREEN
    );

    // Consignes
    const char *rules =
        "Consignes :\n"
        "- Explorer les cases sans tomber sur les mines\n"
        "- Atteignez l'objectif de decouverte pour gagner\n"
        "- Clic gauche : decouvrir la case \n"
        "- Clic droit : marquer la case avec un drapeau\n";

    float   fsRules = 40.0f;
    Vector2 mRules  = MeasureTextEx(fontConsignes, rules, fsRules, 0);

    // Position : à droite de la grille, centrée verticalement
    float rulesX = ox + gridW + 40.0f;
    float rulesY = oy + (gridH / 2.0f) - (mRules.y / 2.0f) - 40.0f;

    DrawTextEx(
        fontConsignes,
        rules,
        (Vector2){ rulesX, rulesY },
        fsRules,
        0,
        DARKBLUE
    );

    // HUD à gauche
    int hudX = ox - 260;
    int hudY = oy + 20;

    // Drapeaux restants
    char bufFlags[64];
    snprintf(
        bufFlags,
        sizeof(bufFlags),
        "Drapeaux :\n%d / %d",
        g->mineCount - g->flagsPlaced,
        g->mineCount
    );

    DrawTextEx(fontNormal, bufFlags, (Vector2){ hudX, hudY }, 36.0f, 0, RAYWHITE);

    // Découvertes
    char bufDisc[64];
    snprintf(
        bufDisc,
        sizeof(bufDisc),
        "Decouvertes :\n%d / %d\n(%.0f%%)",
        g->revealedCount,
        g->safeCellsTotal,
        g->progress * 100.0f
    );

    DrawTextEx(fontNormal, bufDisc, (Vector2){ hudX, hudY + 90 }, 32.0f, 0, RAYWHITE);

    // Barre de progression
    float barX = (float)hudX;
    float barY = (float)hudY + 200.0f;
    float barW = 220.0f;
    float barH = 20.0f;

    DrawRectangle(barX, barY, barW, barH, (Color){ 80,  80,  80, 200 });
    DrawRectangle(barX, barY, barW * g->progress, barH, (Color){ 0, 180, 60, 255 });
    DrawRectangleLines((int)barX, (int)barY, (int)barW, (int)barH, WHITE);

    // Objectif
    char bufObj[32];
    snprintf(
        bufObj,
        sizeof(bufObj),
        "Objectif :\n%.0f %%",
        g->requiredProgress * 100.0f
    );

    DrawTextEx(fontNormal, bufObj, (Vector2){ hudX, barY + 55 }, 32.0f, 0, GOLD);

    // Grille principale
    Vector2 mouse = GetMousePosition();

    for (int r = 0; r < g->size; r++)
    {
        for (int c = 0; c < g->size; c++)
        {
            int x = ox + c * cellSize;
            int y = oy + r * cellSize;

            bool hovered = CheckCollisionPointRec(
                mouse,
                (Rectangle){ x, y, cellSize, cellSize }
            );

            DrawCellVisual(&g->board[r][c], x, y, cellSize, fontNormal, hovered);
        }
    }

    DrawRectangleLines(ox, oy, gridW, gridH, RED);

    if (g->gameOver)
{
    float fsBoom  = 64.0f;
    float fsLose  = 40.0f;
    float fsHint  = 32.0f;

    float baseY = oy + gridH + 10;

    if (g->win)
    {
        // Victoire
        const char *msgWin = "Bravo, vous avez gagne. Vous pouvez relancer le de.";
        Vector2 sWin = MeasureTextEx(fontConsignes, msgWin, fsLose, 0);

        DrawTextEx(
            fontConsignes,
            msgWin,
            (Vector2){ ox + gridW/2 - sWin.x/2, baseY },
            fsLose,
            0,
            GREEN
        );
    }
    else
    {
        // Défaite

        // Ligne 1 : BOOM !
        const char *boom = "BOOM !";
        Vector2 sBoom = MeasureTextEx(fontConsignes, boom, fsBoom, 0);

        DrawTextEx(
            fontConsignes,
            boom,
            (Vector2){ ox + gridW/2 - sBoom.x/2, baseY },
            fsBoom,
            0,
            RED
        );

        // Ligne 2 : Vous avez perdu...
        const char *lose = "Vous avez perdu...";
        Vector2 sLose = MeasureTextEx(fontConsignes, lose, fsLose, 0);

        DrawTextEx(
            fontConsignes,
            lose,
            (Vector2){ ox + gridW/2 - sLose.x/2, baseY + sBoom.y + 10 },
            fsLose,
            0,
            RED
        );

        // Ligne 3 : Rejouer
        const char *hint = "Appuyez sur ENTRER pour rejouer";
        Vector2 sHint = MeasureTextEx(fontConsignes, hint, fsHint, 0);

        DrawTextEx(
            fontConsignes,
            hint,
            (Vector2){ ox + gridW/2 - sHint.x/2, baseY + sBoom.y + sLose.y + 25 },
            fsHint,
            0,
            DARKBROWN
        );
    }
}
}

// —————————————————————————————————————————
// BOUCLE PRINCIPALE DU MINI-JEU
// —————————————————————————————————————————

static int RunDemineurLevel(int level)
{
    // Fenêtre plein écran 1920x1080 (gérée par la fenêtre principale)
    int screenW = 1920;
    int screenH = 1080;

    //InitWindow(screenW, screenH, "Mini-Jeu : Demineur");
    //SetWindowPosition(0, 0);
    //SetTargetFPS(60);

    // Polices pour le jeu
    Font fontConsignes = LoadFontEx("SantasSleighFull Bold.ttf", 64, 0, 0);

    // Musique du mini-jeu
    if (!IsAudioDeviceReady())
    {
        InitAudioDevice();
    }

    Music pianoMusic = LoadMusicStream("PIANO.mp3");

    // Son de la bombe
    bombSound = LoadSound("bombe.mp3");
    SetSoundVolume(bombSound, 4.0f);

    SetMusicVolume(pianoMusic, 1.0f);
    PlayMusicStream(pianoMusic);

    GenTextureMipmaps(&fontConsignes.texture);
    SetTextureFilter(fontConsignes.texture, TEXTURE_FILTER_TRILINEAR);

    Font fontNormal = LoadFontEx("SantasSleighFull.ttf", 64, 0, 0);
    GenTextureMipmaps(&fontNormal.texture);
    SetTextureFilter(fontNormal.texture, TEXTURE_FILTER_TRILINEAR);

    // Police pour les titres
    Font titleFont = LoadFontEx("MerryChristmasFlake.ttf", 256, 0, 0);
    GenTextureMipmaps(&titleFont.texture);
    SetTextureFilter(titleFont.texture, TEXTURE_FILTER_TRILINEAR);

    // Texture du drapeau
    gFlagTex = LoadTexture("drapeau.png");

    // Fond de Noël
    Texture2D background = { 0 };
    Image     bg         = LoadImage("background.png");

    if (bg.data != NULL)
    {
        background = LoadTextureFromImage(bg);
        UnloadImage(bg);
    }

    // Paramètres selon la difficulté
    int size  = 9;
    int mines = 10;

    if (level == 2)
    {
        size  = 12;
        mines = 22;
    }
    else if (level == 3)
    {
        size  = 16;
        mines = 40;
    }

    // Taille des cases par niveau
    int cellSize = 64;  // niveau 1
    if (level == 2)
    {
        cellSize = 56;
    }
    else if (level == 3)
    {
        cellSize = 48;  // grille plus compacte
    }

    Game g;
    InitGame(&g, size, mines);

    int gridW = size * cellSize;
    int gridH = size * cellSize;

    // Centrage de la grille
    int originX = screenW / 2 - gridW / 2;
    int originY = screenH / 2 - gridH / 2;

    // Neige d'arrière-plan
    Snowflake snow[MAX_SNOW];
    InitSnow(snow, MAX_SNOW, screenW, screenH);

    int  result   = 0;
    bool quit     = false;
    float winTime = 0.0f; // temps écoulé après une victoire

    // Boucle principale du mini-jeu
    while (!WindowShouldClose() && !quit)
    {
        UpdateMusicStream(pianoMusic);

        Vector2 mouse = GetMousePosition();

        bool leftClick  = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        bool rightClick = IsMouseButtonPressed(MOUSE_RIGHT_BUTTON);

        int clickedRow = -1;
        int clickedCol = -1;

        if (mouse.x >= originX && mouse.x < originX + gridW &&
            mouse.y >= originY && mouse.y < originY + gridH)
        {
            clickedCol = (int)((mouse.x - originX) / cellSize);
            clickedRow = (int)((mouse.y - originY) / cellSize);
        }

        // Gestion des clics tant que la partie n'est pas terminée
        if (!g.gameOver)
        {
            if (leftClick && clickedRow >= 0)
            {
                RevealCell(&g, clickedRow, clickedCol);
            }

            if (rightClick && clickedRow >= 0)
            {
                ToggleFlag(&g, clickedRow, clickedCol);
            }
        }

        // Réinitialisation possible uniquement après une défaite
        if (IsKeyPressed(KEY_ENTER) && g.gameOver && !g.win)
        {
            ResetBoard(&g);
            winTime = 0.0f;
        }

        // Gestion de la fin de partie
        if (g.gameOver && g.win)
        {
            // Laisse le message de victoire affiché quelques secondes
            winTime += GetFrameTime();

            if (winTime >= 3.0f)
            {
                quit   = true;
                result = 1; // victoire renvoyée au plateau
            }
        }

        // Mise à jour de la neige
        UpdateSnow(snow, MAX_SNOW, screenW, screenH);

        // Rendu
        BeginDrawing();

        if (background.id != 0)
        {
            float scale = fmaxf(
                (float)screenW / background.width,
                (float)screenH / background.height
            );

            (void)scale; // évite un éventuel warning si non utilisé plus loin

            DrawTexturePro(
                background,
                (Rectangle){ 0, 0, (float)background.width, (float)background.height },
                (Rectangle){ 0, 0, (float)screenW, (float)screenH },
                (Vector2){ 0, 0 },
                0.0f,
                WHITE
            );
        }
        else
        {
            ClearBackground((Color){ 240, 240, 255, 255 });
        }

        DrawSnow(snow, MAX_SNOW);
        DrawGame(&g, originX, originY, titleFont, fontConsignes, fontNormal, cellSize);

        EndDrawing();
    }

    // Libération des ressources
    FreeGame(&g);

    if (background.id)
    {
        UnloadTexture(background);
    }

    UnloadFont(titleFont);
    UnloadFont(fontConsignes);
    UnloadTexture(gFlagTex);
    UnloadSound(bombSound);
    UnloadMusicStream(pianoMusic);

    //CloseWindow(); // géré par le programme principal
    return result;
}

// —————————————————————————————————————————
// MAIN EXTERNE
// —————————————————————————————————————————

int StartDemineur(int level)
{
    return RunDemineurLevel(level);
}
