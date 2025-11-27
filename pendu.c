/*gcc pendu.c -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -o pendu.exe*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>          
#include "raylib.h"

#define MAX_TRIES 6
#define WORD_COUNT 22
#define MAX_SNOW 220
// Taille maximale d'un mot géré (inclut le '\0')
#define MAX_WORD_LEN 64

typedef struct { float x, y, vy, size; } Snowflake;
static Snowflake snow[MAX_SNOW];
static bool snow_initialized = false;

// Structure regroupant l'état du jeu
typedef struct {
    const char *word;
    int wordLength;
    char guessed[MAX_WORD_LEN];
    int tries;
    char guessedLetters[26];
    int guessedCount;
    bool won;
    float winTimer;
    int winCountdownActive;
    int shouldExit;
} GameState;


// Liste de mots de Noël
const char *words[WORD_COUNT] = {
    "sapin", "renne", "cadeau", "neige", "guirlande", "boule", "etoile", "cloche", "flocon", "bougie",
    "chaussette", "elfe", "lutin","reveillon", "chocolat", "chant", "buche", "biscuit", 
    "traineau", "decoration", "bougies", "jouets"
};

// AZERTY -> lettres a-z (Raylib)
const int azertyKeys[26] = {
    KEY_Q, // a
    KEY_B, // b
    KEY_C, // c
    KEY_D, // d
    KEY_E, // e
    KEY_F, // f
    KEY_G, // g
    KEY_H, // h
    KEY_I, // i
    KEY_J, // j
    KEY_K, // k
    KEY_L, // l
    KEY_SEMICOLON, // m
    KEY_N, // n
    KEY_O, // o
    KEY_P, // p
    KEY_A, // q
    KEY_R, // r
    KEY_S, // s
    KEY_T, // t
    KEY_U, // u
    KEY_V, // v
    KEY_W, // w
    KEY_X, // x
    KEY_Y, // y
    KEY_Z  // z
};

// AZERTY chars correspondant aux touches ci-dessus
const char azertyChars[26] = {
    'a', // KEY_Q
    'b', // KEY_B
    'c', // KEY_C
    'd', // KEY_D
    'e', // KEY_E
    'f', // KEY_F
    'g', // KEY_G
    'h', // KEY_H
    'i', // KEY_I
    'j', // KEY_J
    'k', // KEY_K
    'l', // KEY_L
    'm', // KEY_SEMICOLON
    'n', // KEY_N
    'o', // KEY_O
    'p', // KEY_P
    'q', // KEY_A
    'r', // KEY_R
    's', // KEY_S
    't', // KEY_T
    'u', // KEY_U
    'v', // KEY_V
    'z', // KEY_Z
    'x', // KEY_X
    'y', // KEY_Y
    'w'  // KEY_W
};


void DrawSimplePendu(int tries) {
    // Couleurs
    Color skin = (Color){255, 224, 189, 255};
    Color red = RED;
    Color white = WHITE;
    Color black = BLACK;
    Color gold = (Color){255, 215, 0, 255};

    // --- POTENCE ---
    Color woodMain = (Color){139, 90, 43, 255};
    Color woodShadow = (Color){100, 60, 20, 255};
    int baseX = 700, baseY = 350;

    DrawRectangle(baseX, baseY, 40, 550, woodMain);
    DrawRectangle(baseX + 35, baseY, 5, 550, woodShadow);
    DrawRectangle(baseX, baseY, 250, 30, woodMain);
    DrawRectangle(baseX, baseY + 25, 250, 5, woodShadow);
    DrawRectangle(baseX + 240, baseY, 6, 80, (Color){180, 150, 90, 255});
    DrawCircle(baseX + 243, baseY + 80, 5, gold);

    // --- Père Noël ---
    int x = baseX + 240; 
    int y = baseY + 130; 

    if (tries >= 1) {
        // --- Tête ---
        DrawCircle(x, y, 50, skin); 

        // Bonnet
        DrawCircle(x, y - 35, 55, red);
        DrawRectangle(x - 55, y - 15, 110, 16, white);
        DrawCircle(x + 50, y - 50, 12, white);

        // Barbe 
        DrawCircle(x, y + 30, 40, white);
        DrawRectangle(x - 40, y + 30, 80, 40, white);

        // Yeux et nez
        DrawCircle(x - 14, y - 8, 5, black);
        DrawCircle(x + 14, y - 8, 5, black);
        DrawCircle(x, y + 8, 5, RED);
    }

    if (tries >= 2) {
        // --- Corps ---
        DrawRectangle(x - 35, y + 60, 70, 100, red); 
        DrawRectangle(x - 35, y + 100, 70, 12, white); 
        DrawRectangle(x - 35, y + 115, 70, 12, black); 
        DrawRectangle(x - 5, y + 115, 10, 12, gold); 
    }

    if (tries >= 3) {
        // --- Bras gauche ---
        DrawLineEx((Vector2){x - 10, y + 70}, (Vector2){x - 70, y + 120}, 18, red);
        DrawCircle(x - 70, y + 120, 10, white);
    }

    if (tries >= 4) {
        // --- Bras droit ---
        DrawLineEx((Vector2){x + 10, y + 70}, (Vector2){x + 70, y + 120}, 18, red);
        DrawCircle(x + 70, y + 120, 10, white);
    }

    if (tries >= 5) {
        // --- Jambe gauche ---
        DrawLineEx((Vector2){x - 15, y + 160}, (Vector2){x - 35, y + 230}, 18, red);
        DrawCircle(x - 35, y + 230, 12, black);
    }

    if (tries >= 6) {
        // --- Jambe droite ---
        DrawLineEx((Vector2){x + 15, y + 160}, (Vector2){x + 35, y + 230}, 18, red);
        DrawCircle(x + 35, y + 230, 12, black);
    }
}

void initSnowflakes(int screenW, int screenH) {
    for (int i = 0; i < MAX_SNOW; i++) {
        snow[i].x = (float)(rand() % screenW);
        snow[i].y = (float)(rand() % screenH);
        snow[i].vy = 0.4f + (rand() % 100) / 250.0f;
        snow[i].size = 1.0f + (rand() % 7) / 3.0f;
    }
    snow_initialized = true;
}

void updateSnowflakes(int screenW, int screenH) {
    for (int i = 0; i < MAX_SNOW; i++) {
        snow[i].y += snow[i].vy;
        snow[i].x += sinf((float)GetTime() + i) * 0.4f;
        if (snow[i].y > screenH + 10) {
            snow[i].y = -10 - (rand() % 50);
            snow[i].x = (float)(rand() % screenW);
        }
    }
}

// Prototypes pour la décomposition fonctionnelle
void InitGame(GameState *gs);
void HandleInput(GameState *gs);
void UpdateGame(GameState *gs);

// Initialise l'état du jeu
void InitGame(GameState *gs) {
    gs->word = words[rand() % WORD_COUNT];
    gs->wordLength = (int)strlen(gs->word);
    if (gs->wordLength >= MAX_WORD_LEN) gs->wordLength = MAX_WORD_LEN - 1;
    for (int i = 0; i < gs->wordLength; i++) gs->guessed[i] = '_';
    gs->guessed[gs->wordLength] = '\0';

    gs->tries = 0;
    for (int i = 0; i < 26; i++) gs->guessedLetters[i] = 0;
    gs->guessedCount = 0;
    gs->won = false;
    gs->winTimer = 0.0f;
    gs->winCountdownActive = 0;
        gs->shouldExit = 0; // Initialize shouldExit to false
}

// Gère les entrées du joueur (lettres et reset)
void HandleInput(GameState *gs) {
    if (!gs->won && gs->tries < MAX_TRIES) {
        for (int i = 0; i < 26; i++) {
            int key = azertyKeys[i];
            if (IsKeyPressed(key)) {
                char letter = azertyChars[i];

                // Vérifie si la lettre a déjà été proposée
                bool alreadyGuessed = false;
                for (int j = 0; j < gs->guessedCount; j++)
                    if (gs->guessedLetters[j] == letter) alreadyGuessed = true;

                if (!alreadyGuessed) {
                    gs->guessedLetters[gs->guessedCount++] = letter;
                    bool correct = false;
                    for (int j = 0; j < gs->wordLength; j++)
                        if (gs->word[j] == letter) {
                            gs->guessed[j] = letter;
                            correct = true;
                        }
                    if (!correct) gs->tries++;
                }
            }
        }
    }

    // Reset (ENTRER) si perdu
    if (!gs->won && gs->tries >= MAX_TRIES) {
        if (IsKeyPressed(KEY_ENTER)) {
            const char *oldWord = gs->word; // mémorise le mot actuel
            do {
                gs->word = words[rand() % WORD_COUNT]; // tire un nouveau mot
            } while (gs->word == oldWord); // recommence si c'est le même mot
            gs->wordLength = (int)strlen(gs->word);
            if (gs->wordLength >= MAX_WORD_LEN) gs->wordLength = MAX_WORD_LEN - 1;
            for (int i = 0; i < gs->wordLength; i++) gs->guessed[i] = '_';
            gs->guessed[gs->wordLength] = '\0';
            gs->tries = 0;
            gs->guessedCount = 0;
            gs->won = false;
            gs->winTimer = 0.0f;
            gs->winCountdownActive = 0;
        }
    }
}

// Met à jour l'état (victoire/décompte)
void UpdateGame(GameState *gs) {
    gs->won = strcmp(gs->word, gs->guessed) == 0;

    if (gs->won && !gs->winCountdownActive) {
        gs->winCountdownActive = 1;
        gs->winTimer = 0.0f;
    }

    if (gs->winCountdownActive && gs->won) {
        gs->winTimer += GetFrameTime();
        if (gs->winTimer >= 3.0f) {
            gs->shouldExit = 1;
        }
    }
}

// Programme principal
int main() {
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "Mini-Jeu : Pendu");

    //-----------------------------------------------------
    // 🎵 MUSIQUE PENDU : PIANO.mp3
    //-----------------------------------------------------
    if (!IsAudioDeviceReady()) InitAudioDevice();

    Music pianoMusic = LoadMusicStream("PIANO.mp3");
    SetMusicVolume(pianoMusic, 1.0f);
    PlayMusicStream(pianoMusic);

    initSnowflakes(screenWidth, screenHeight);

    
    SetTargetFPS(60);

    // Polices et fond
    Font fontNoel   = LoadFontEx("MerryChristmasFlake.ttf", 80, 0, 0);      // Pour le titre
    Font fontSleigh = LoadFontEx("SantasSleighFull.ttf", 50, 0, 0);          // Pour texte courant
    Font fontWin    = LoadFontEx("SantasSleighFull Bold.ttf", 80, 0, 0);     // Pour messages fin
    Texture2D background = LoadTexture("background.png");
    Texture2D neige = LoadTexture("neige.png");


    srand(time(NULL));

    GameState gs = {0};
    InitGame(&gs);


    while (true) {    

        UpdateMusicStream(pianoMusic);

        if (IsKeyDown(KEY_LEFT_ALT) && IsKeyDown(KEY_F4)) break;   // Alt+F4 pour quitter
        // Input AZERTY
            // Tableau des lettres correspondant aux touches AZERTY

    HandleInput(&gs);
    UpdateGame(&gs);

    if (gs.shouldExit) break;

    updateSnowflakes(screenWidth, screenHeight);


////////////////////////////// 
// Dessin
////////////////////////////////
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // Fond centré
    float scale = fmax((float)GetScreenWidth()/background.width,
                        (float)GetScreenHeight()/background.height);
    int newWidth = background.width * scale;
    int newHeight = background.height * scale;
    int posX = (GetScreenWidth() - newWidth)/2;
    int posY = (GetScreenHeight() - newHeight)/2;
    DrawTexturePro(background,
                    (Rectangle){0,0,(float)background.width,(float)background.height},
                    (Rectangle){(float)posX,(float)posY,(float)newWidth,(float)newHeight},
                    (Vector2){0,0},0.0f,WHITE);


    // Neige animée
    for (int i = 0; i < MAX_SNOW; i++) {
        DrawCircle((int)snow[i].x, (int)snow[i].y, snow[i].size, Fade(RAYWHITE, 0.95f));
    }               

    // Titre centré
    float titleSize = 80.0f;
    Vector2 titlePos = {
        GetScreenWidth()/2 - MeasureTextEx(fontNoel, "Pendu de Noel", titleSize, 2).x/2,
        100
    };
    DrawTextEx(fontNoel, "Pendu de Noel", titlePos, titleSize, 2, RED);

    // Texte mot et lettres 
    int offsetXRight = 1300;

    // "Mot à deviner:" 
    DrawTextEx(fontWin, "Mot a deviner:", (Vector2){offsetXRight, 150}, 50, 2, DARKGREEN);

    // Mot deviné
    DrawTextEx(fontSleigh, gs.guessed, (Vector2){offsetXRight, 220}, 60, 2, BLACK);

    // "Lettres essayees:" 
    DrawTextEx(fontWin, "Lettres essayees:", (Vector2){offsetXRight, 350}, 50, 2, MAROON);

    // Lettres déjà proposées
    char lettersStr[60] = {0};
    int pos = 0;
    for (int i = 0; i < gs.guessedCount; i++) {
        lettersStr[pos++] = gs.guessedLetters[i];
        lettersStr[pos++] = ' ';
    }
    lettersStr[pos] = '\0';
    DrawTextEx(fontSleigh, lettersStr, (Vector2){offsetXRight, 400}, 50, 2, DARKBLUE);

    // Positionner la neige devant le bas du poteau
    int baseX = 700;    
    int baseY = 350;
    int poteauHeight = 550;
    int poteauWidth = 40;

    Rectangle sourceRec = { 0, 0, (float)neige.width, (float)neige.height }; // taille originale
    Rectangle destRec = {
        baseX + poteauWidth / 2 - (neige.width / 2) / 2.0f,   // X centré sur poteau
        baseY + poteauHeight - (neige.height / 2.0f) + 20,   // Y aligné sur le bas du poteau
        neige.width / 2.0f,                                 // largeur réduite de moitié
        neige.height / 2.0f                                // hauteur réduite de moitié
    };
    Vector2 origin = { 0, 0 };

    // Pendu
    DrawSimplePendu(gs.tries);
    
    // Neige
    DrawTexturePro(neige, sourceRec, destRec, origin, 0.0f, WHITE);

    // Messages fin
    float offsetX = -200; 

    if (gs.won)
    DrawTextEx(fontWin, "Vous avez gagne !",
        (Vector2){GetScreenWidth()/2 - MeasureTextEx(fontWin,"Vous avez gagne !",50,2).x/2 + offsetX, 920},
        50, 2, DARKGREEN);
    else if (gs.tries >= MAX_TRIES) {
        DrawTextEx(fontWin, "Vous avez perdu ! Le mot etait:",
                (Vector2){GetScreenWidth()/2 - MeasureTextEx(fontWin,"Vous avez perdu ! Le mot etait:",50,2).x/2 + offsetX, 920},
                50, 2, MAROON);
    DrawTextEx(fontWin, gs.word,
        (Vector2){GetScreenWidth()/2 - MeasureTextEx(fontWin,gs.word,60,2).x/2 + offsetX, 960},
        60, 2, DARKBLUE);
    }

    // Reset 
    if (!gs.won && gs.tries >= MAX_TRIES) {
    DrawTextEx(fontWin, "Appuyez sur ENTRER pour rejouer",
               (Vector2){GetScreenWidth()/2 - MeasureTextEx(fontWin,"Appuyez sur ENTRER pour rejouer",40,1).x/2, 250},
               40, 1, DARKBROWN);
    }


    EndDrawing();

}

    UnloadTexture(background);
    UnloadTexture(neige);
    UnloadFont(fontNoel);
    UnloadFont(fontWin);
    UnloadFont(fontSleigh);
    UnloadMusicStream(pianoMusic);
    CloseWindow();
    return 0;
}
