// gcc palteau.c dice3d.c nine.c four.c five.c teleport7.c fifteen.c eighteen.c sudoku.c demineurgraphique.c -Iinclude -Llib -lraylib -lopengl32 -lgdi32 -lwinmm -o plateau.exe

#include <math.h>
#include "raylib.h"
#include "dice3d.h"
#include "nine.h"
#include "player.h"
#include "sudoku.h"
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <stdio.h>
#include "eighteen.h"
#include "raymath.h"
#include "fifteen.h"
#include "teleport7.h"
#include "four.h"
#include "demineur.h"
#include "five.h"

// Couleurs personnalisées
const Color ecriture_cases = {239,184,16, 255}; //Couleur écriture des cases
const Color numero_cases = {27,41,76,255}; //Couleur numéro des cases

bool DrawFancyButton(Font font, const char *text, Rectangle rec) {

    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, rec);

    Color redTop    = (Color){230, 50, 50, 255};
    Color redBottom = (Color){150, 10, 10, 255};
    Color gold      = (Color){255, 220, 120, 255};

    // Dégradé rouge
    DrawRectangleGradientV(rec.x, rec.y, rec.width, rec.height, redTop, redBottom);

    // Contour doré
    DrawRectangleRoundedLines(rec, 0.25f, 14, gold);

    // Glow au survol
    if (hover) {
        DrawRectangleRounded(rec, 0.25f, 14, (Color){255,255,255,30});
    }

    // Ombre légèrement en dessous
    DrawRectangleRounded(
        (Rectangle){rec.x, rec.y + 5, rec.width, rec.height},
        0.25f, 14, (Color){0,0,0,50}
    );

    // Texte centré
    int size = rec.height * 0.55f;   
    if (size < 40) size = 40;        
    Vector2 s = MeasureTextEx(font, text, size, 0);

    DrawTextEx(
        font,
        text,
        (Vector2){ rec.x + rec.width/2 - s.x/2, rec.y + rec.height/2 - s.y/2 },
        size,0,gold
    );

    return hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

#define CASES_PAR_COTE 6

typedef enum { STATE_MENU, STATE_OPTIONS, STATE_BOARD, STATE_WIN } GameState;
bool musicEnabled = true;

// ============================================================
// Utils
// ============================================================
Vector2 CaseToPos(int index, int offsetX, int offsetY, int tailleCase)
{
    // Coordonnées EXACTES selon ton schéma jaune

    int col[20] = {
        0, 1, 2, 3,      // 0-3
        3,               // 4
        3, 2, 1,          // 5-7
        1,                // 8
        1, 2, 3, 4, 5,   // 9-13
        5,               // 14
        5, 6, 7,         // 15-17
        7,               // 18
        7                // 19
    };
    

    int row[20] = {
        0, 0, 0, 0,      // 0-3
        1,               // 4
        2, 2, 2,         // 5-7
        3,               // 8
        4, 4, 4, 4, 4,   // 9-13
        3,               // 14
        2, 2, 2,         // 15-17
        3,               // 18
        4                // 19
    };
    
    float x = offsetX + col[index] * tailleCase;
    float y = offsetY + row[index] * tailleCase;

    return (Vector2){ x, y };
}

bool DrawButton(Font font, const char *text, Rectangle rec, Color normal, Color hover, int size) {
    Vector2 mouse = GetMousePosition();
    bool h = CheckCollisionPointRec(mouse, rec);
    Color c = h ? hover : normal;
    DrawRectangleRounded(rec, 0.25f, 10, c);
    DrawRectangleRoundedLines(rec, 0.25f, 10, Fade(BLACK, 0.25f));
    Vector2 s = MeasureTextEx(font, text, size, 0);
    DrawTextEx(font, text, (Vector2){rec.x + rec.width/2 - s.x/2, rec.y + rec.height/2 - s.y/2}, size, 0, WHITE);
    return h && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

// petit label aligné à gauche
void DrawLabelL(Font font, const char *text, Vector2 pos, int size, Color col) {
    DrawTextEx(font, text, pos, size, 0, col);
}
// ============================================================
// Dessin des points (pips) d'un dé dans un rectangle
// ============================================================
void DrawDiceDots(Rectangle rect, int value, Color color) {
    float cx = rect.x + rect.width  / 2.0f;
    float cy = rect.y + rect.height / 2.0f;
    float r  = 12.0f;   // rayon des points

    float dx = rect.width  / 4.0f;
    float dy = rect.height / 4.0f;

    // Positions standards des 9 points
    Vector2 p1 = { cx - dx, cy - dy };
    Vector2 p2 = { cx,      cy - dy };
    Vector2 p3 = { cx + dx, cy - dy };
    Vector2 p4 = { cx - dx, cy      };
    Vector2 p5 = { cx,      cy      };
    Vector2 p6 = { cx + dx, cy      };
    Vector2 p7 = { cx - dx, cy + dy };
    Vector2 p8 = { cx,      cy + dy };
    Vector2 p9 = { cx + dx, cy + dy };

    switch (value) {
        case 1:
            DrawCircleV(p5, r, color);
            break;
        case 2:
            DrawCircleV(p1, r, color);
            DrawCircleV(p9, r, color);
            break;
        case 3:
            DrawCircleV(p1, r, color);
            DrawCircleV(p5, r, color);
            DrawCircleV(p9, r, color);
            break;
        case 4:
            DrawCircleV(p1, r, color);
            DrawCircleV(p3, r, color);
            DrawCircleV(p7, r, color);
            DrawCircleV(p9, r, color);
            break;
        case 5:
            DrawCircleV(p1, r, color);
            DrawCircleV(p3, r, color);
            DrawCircleV(p5, r, color);
            DrawCircleV(p7, r, color);
            DrawCircleV(p9, r, color);
            break;
        case 6:
            DrawCircleV(p1, r, color);
            DrawCircleV(p3, r, color);
            DrawCircleV(p4, r, color);
            DrawCircleV(p6, r, color);
            DrawCircleV(p7, r, color);
            DrawCircleV(p9, r, color);
            break;
        default:
            break;
    }
}

// ============================================================
// Surbrillance bleue quand la souris survole un dé
// ============================================================
void DrawDiceHighlight(Rectangle rect) {
    // Halo bleu translucide
    DrawRectangleRounded(rect, 0.35f, 8, (Color){80,140,255,60});

    // Contour bleu vif
    DrawRectangleRoundedLines(rect, 0.35f, 8, (Color){80,140,255,220});
}

// ============================================================
// Plateau (affichage)
// ============================================================

void Draw3DBoard(Font font, int totalCases, int offsetX, int offsetY, int tailleCase,
                 Color caseA, Color caseB, Color contour, Color accent)
{
    for (int i = 0; i < totalCases; i++) {

        Vector2 pos = CaseToPos(i, offsetX, offsetY, tailleCase);
        Rectangle r = (Rectangle){pos.x, pos.y, tailleCase, tailleCase};

        // Couleur de fond de case (alterné)
        Color base = (i % 2 == 0) ? caseA : caseB;
        DrawRectangleRounded(r, 0.15f, 6, base);  
        DrawRectangleRoundedLines(r, 0.15f, 6, Fade(contour, 0.35f));

        // ------ CASE SPÉCIALE 9 : RECULE ------
        if (i == 9) {
            const char *l1 = "SURPRISE";
            const char *l2 = "MAGIQUE !";

            int size1 = 26;
            int size2 = 26;

            Vector2 s1 = MeasureTextEx(font, l1, size1, 0);
            Vector2 s2 = MeasureTextEx(font, l2, size2, 0);

            float x1 = pos.x + (tailleCase - s1.x) / 2;
            float x2 = pos.x + (tailleCase - s2.x) / 2;
            float baseY = pos.y + (tailleCase - (s1.y + s2.y)) / 2;

            DrawTextEx(font, l1, (Vector2){x1, baseY}, size1, 0, ecriture_cases);
            DrawTextEx(font, l2, (Vector2){x2, baseY + s1.y + 4}, size2, 0, ecriture_cases);

            DrawTextEx(font, TextFormat("%d", i), 
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
        else if (i == 1) {
            const char *txt = "SUDOKU";
            Color col = ecriture_cases; 
        
            Vector2 s = MeasureTextEx(font, txt, 26, 0);
            DrawTextEx(font, txt,
                (Vector2){pos.x + (tailleCase - s.x)/2, pos.y + (tailleCase - s.y)/2},24, 0, col);
        
            DrawTextEx(font, TextFormat("%d", i),
                (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
        else if (i == 3 || i == 11 || i == 16) {
            const char *txt = "PENDU";
            Color col = ecriture_cases;
        
            Vector2 s = MeasureTextEx(font, txt, 26, 0);
        
            DrawTextEx(font, txt,
                (Vector2){pos.x + (tailleCase - s.x)/2,pos.y + (tailleCase - s.y)/2},26, 0, col);
        
            // numéro de la case
            DrawTextEx(font, TextFormat("%d", i),
                (Vector2){pos.x + 10, pos.y + 8},26, 0, numero_cases);
        }
        else if (i == 5) {
            const char *l1 = "RETOUR";
            const char *l2 = "DEPART !";

            int size1 = 26;
            int size2 = 26;

            Vector2 s1 = MeasureTextEx(font, l1, size1, 0);
            Vector2 s2 = MeasureTextEx(font, l2, size2, 0);

            float x1 = pos.x + (tailleCase - s1.x) / 2;
            float x2 = pos.x + (tailleCase - s2.x) / 2;

            float baseY = pos.y + (tailleCase - (s1.y + s2.y + 4)) / 2;

            DrawTextEx(font, l1, (Vector2){x1, baseY}, size1, 0,(ecriture_cases));    

            DrawTextEx(font, l2, (Vector2){x2, baseY + s1.y + 4}, size2, 0,(ecriture_cases));

            // numéro de la case
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
        else if (i == 4) {
            const char *l1 = "CADEAU";
            const char *l2 = "DU LUTIN !";

            int size1 = 26;
            int size2 = 26;

            Vector2 s1 = MeasureTextEx(font, l1, size1, 0);
            Vector2 s2 = MeasureTextEx(font, l2, size2, 0);

            float x1 = pos.x + (tailleCase - s1.x) / 2;
            float x2 = pos.x + (tailleCase - s2.x) / 2;

            float baseY = pos.y + (tailleCase - (s1.y + s2.y + 4)) / 2;

            DrawTextEx(font, l1, (Vector2){x1, baseY}, size1, 0,(ecriture_cases));    
            DrawTextEx(font, l2, (Vector2){x2, baseY + s1.y + 4}, size2, 0,(ecriture_cases));

            // numéro de la case 
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);

            } else if (i == 7) {
            const char *txt1 = "TELEPORTATION";
            const char *txt2 = "MAGIQUE !";
        
            int size1 = 24;
            int size2 = 25;
        
            Vector2 s1 = MeasureTextEx(font, txt1, size1, 0);
            Vector2 s2 = MeasureTextEx(font, txt2, size2, 0);
        
            float x1 = pos.x + (tailleCase - s1.x)/2;
            float x2 = pos.x + (tailleCase - s2.x)/2;
            float baseY = pos.y + (tailleCase - (s1.y + s2.y + 4)) / 2;
        
            DrawTextEx(font, txt1, (Vector2){x1, baseY}, size1, 0, (ecriture_cases));
            DrawTextEx(font, txt2, (Vector2){x2, baseY + s1.y + 4}, size2, 0, (ecriture_cases));
        
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
        
                // ------ CASE SPÉCIALE 8 : MORPION ------
        else if (i == 8) {
            const char *txt = "MORPION";
            Color col = ecriture_cases; 

            Vector2 s = MeasureTextEx(font, txt, 26, 0);
            DrawTextEx(font, txt,
                (Vector2){pos.x + (tailleCase - s.x)/2, pos.y + (tailleCase - s.y)/2},24, 0, col);

            DrawTextEx(font, TextFormat("%d", i),
                (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
        else if (i == 14) {
            const char *txt = "MORPION";
            Color col = ecriture_cases;
        
            Vector2 s = MeasureTextEx(font, txt, 26, 0);
            DrawTextEx(font, txt,
                (Vector2){pos.x + (tailleCase - s.x)/2, pos.y + (tailleCase - s.y)/2},24, 0, col);
        
            DrawTextEx(font, TextFormat("%d", i),
                (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
        
        // ------ CASE SPÉCIALE 10 : SUDOKU ------
        else if (i == 10) {
            const char *txt = "SUDOKU";
            Color col = ecriture_cases; 
        
            Vector2 s = MeasureTextEx(font, txt, 26, 0);
            DrawTextEx(font, txt,
                (Vector2){pos.x + (tailleCase - s.x)/2, pos.y + (tailleCase - s.y)/2},26, 0, col);
        
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
        else if (i == 12) {
            const char *txt1 = "REPOS";
            const char *txt2 = "MERITE !";
        
            int size1 = 26;
            int size2 = 26;
        
            Vector2 s1 = MeasureTextEx(font, txt1, size1, 0);
            Vector2 s2 = MeasureTextEx(font, txt2, size2, 0);
        
            float x1 = pos.x + (tailleCase - s1.x)/2;
            float x2 = pos.x + (tailleCase - s2.x)/2;
            float baseY = pos.y + (tailleCase - (s1.y + s2.y + 4)) / 2;
        
            DrawTextEx(font, txt1, (Vector2){x1, baseY}, size1, 0, (ecriture_cases));
            DrawTextEx(font, txt2, (Vector2){x2, baseY + s1.y + 4}, size2, 0, (ecriture_cases));
        
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);

        } else if (i == 15) {
            const char *l1 = "CADEAU";
            const char *l2 = "SURPRISE !";
        
            int size1 = 26;
            int size2 = 26;
        
            Vector2 s1 = MeasureTextEx(font, l1, size1, 0);
            Vector2 s2 = MeasureTextEx(font, l2, size2, 0);
        
            float x1 = pos.x + (tailleCase - s1.x) / 2;
            float x2 = pos.x + (tailleCase - s2.x) / 2;
            float baseY = pos.y + (tailleCase - (s1.y + s2.y + 6)) / 2;
        
            DrawTextEx(font, l1, (Vector2){x1, baseY}, size1, 0, (ecriture_cases));
            DrawTextEx(font, l2, (Vector2){x2, baseY + s1.y + 6}, size2, 0, (ecriture_cases));
        
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
        
        else if (i == 17) {
            const char *txt = "SUDOKU";
            Color col = ecriture_cases; 
        
            Vector2 s = MeasureTextEx(font, txt, 26, 0);
            DrawTextEx(font, txt,
                       (Vector2){pos.x + (tailleCase - s.x)/2, pos.y + (tailleCase - s.y)/2},
                       24, 0, col);
        
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
        else if (i == 18) {
            const char *l1 = "SURPRISE";
            const char *l2 = "DU PERE NOEL !";
        
            int size1 = 25;
            int size2 = 25;
        
            Vector2 s1 = MeasureTextEx(font, l1, size1, 0);
            Vector2 s2 = MeasureTextEx(font, l2, size2, 0);
        
            float x1 = pos.x + (tailleCase - s1.x) / 2;
            float x2 = pos.x + (tailleCase - s2.x) / 2;
        
            float baseY = pos.y + (tailleCase - (s1.y + s2.y + 6)) / 2;
        
            DrawTextEx(font, l1, (Vector2){x1, baseY}, size1, 0, (ecriture_cases));
            DrawTextEx(font, l2, (Vector2){x2, baseY + s1.y + 6}, size2, 0, (ecriture_cases));
        
            // numéro de la case
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
        // ------ CASE DEMINEUR FACILE (Case 3) ------
        else if (i == 2) {
            const char *txt = "DEMINEUR";
            int size = 26;
            Vector2 s = MeasureTextEx(font, txt, size, 0);

            float x = pos.x + (tailleCase - s.x) / 2;
            float y = pos.y + (tailleCase - s.y) / 2;

            DrawTextEx(font, txt, (Vector2){x, y}, size, 0, (ecriture_cases));
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }

        // ------ CASE DEMINEUR MOYEN (Case 12) ------
        else if (i == 6) {
            const char *txt = "DEMINEUR";
            int size = 26;
            Vector2 s = MeasureTextEx(font, txt, size, 0);

            float x = pos.x + (tailleCase - s.x) / 2;
            float y = pos.y + (tailleCase - s.y) / 2;

            DrawTextEx(font, txt, (Vector2){x, y}, size, 0, (ecriture_cases));
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }

        // ------ CASE DEMINEUR DIFFICILE (Case 13) ------
        else if (i == 13) {
            const char *txt = "DEMINEUR";
            int size = 26;
            Vector2 s = MeasureTextEx(font, txt, size, 0);

            float x = pos.x + (tailleCase - s.x) / 2;
            float y = pos.y + (tailleCase - s.y) / 2;

            DrawTextEx(font, txt, (Vector2){x, y}, size, 0, (ecriture_cases));
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);

        }
        // ------ CASE SPÉCIALE 0 : DÉPART ------
        else if (i == 0) {
            const char *txt = "DEPART";
            int size = 28;
            Vector2 s = MeasureTextEx(font, txt, size, 0);

            float x = pos.x + (tailleCase - s.x) / 2;
            float y = pos.y + (tailleCase - s.y) / 2;

            DrawTextEx(font, txt, (Vector2){x, y}, size, 0, (ecriture_cases));
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
        // ------ CASE FINALE : ARRIVÉE (19) ------
        else if (i == totalCases - 1) {
            const char *txt = "ARRIVEE";
            int size = 28;
            Vector2 s = MeasureTextEx(font, txt, size, 0);

            float x = pos.x + (tailleCase - s.x) / 2;
            float y = pos.y + (tailleCase - s.y) / 2;

            // texte case
            DrawTextEx(font, txt, (Vector2){x, y}, size, 0, (ecriture_cases));

            // numéro case 19 
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
        // ------ CASE NORMALE ------
        else {
            DrawTextEx(font, TextFormat("%d", i),
                       (Vector2){pos.x + 10, pos.y + 8}, 28, 0, numero_cases);
        }
    }
}

// ============================================================
// MAIN
// ============================================================
int main(void) {
    bool shouldExit = false;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1920, 1080, "Jeu de plateau");
    InitAudioDevice();                 // Active le son
    Music music = LoadMusicStream("All-is-fine.mp3");  // Charge la musique
    PlayMusicStream(music);            // Lance la musique

    SetExitKey(KEY_NULL); 
    SetTargetFPS(60);
    srand((unsigned)time(NULL));

    Font font = LoadFontEx("Poppins-Medium.ttf", 256, 0, 0);
    GenTextureMipmaps(&font.texture);
    SetTextureFilter(font.texture, TEXTURE_FILTER_TRILINEAR);
    
    if (font.texture.id == 0) font = GetFontDefault();
    
    Font customFont = LoadFontEx("MerryChristmasFlake.ttf", 64, 0, 0);
    GenTextureMipmaps(&customFont.texture);
    SetTextureFilter(customFont.texture, TEXTURE_FILTER_TRILINEAR);

    Font fontCandy = LoadFontEx("Candcu_.ttf", 140, 0, 0);
    GenTextureMipmaps(&fontCandy.texture);
    SetTextureFilter(fontCandy.texture, TEXTURE_FILTER_TRILINEAR);

    Font fontSantabold = LoadFontEx("SantasSleighFull Bold.ttf", 80, 0, 0); 
    GenTextureMipmaps(&fontSantabold.texture);
    SetTextureFilter(fontSantabold.texture, TEXTURE_FILTER_BILINEAR);

    Font fontSanta = LoadFontEx("SantasSleighFull.ttf", 50, 0, 0); 
    GenTextureMipmaps(&fontSanta.texture);
    SetTextureFilter(fontSanta.texture, TEXTURE_FILTER_BILINEAR);

    GameState state = STATE_MENU;

    Player player = (Player){0, 0, false, 0.0f};
    int diceValue = 1;
    int tricheValue = 1;   
    int fakeRoll = 1;  
    bool trapActive = false;
    float trapTimer = 0.0f;
    bool trapActive4 = false;
    float trapTimer4 = 0.0f;
    bool tpActive7 = false;
    float tpTimer7 = 0.0f;
    bool tpInvisible7 = false;
    bool trapActive15 = false;
    float trapTimer15 = 0.0f;
    bool trapActive18 = false;
    float trapTimer18 = 0.0f;
    bool trapActive5 = false;
    float trapTimer5 = 0.0f;

    // Palette de Noël
    Color bg1 = (Color){240, 240, 255, 255}; // Blanc neige
    Color bg2 = (Color){200, 0, 0, 255};     // Rouge Noël
    Color accent = (Color){0, 100, 0, 255};  // Vert sapin
    Color accentLight = (Color){0, 150, 0, 255};
    Color caseA = (Color){180, 40, 40, 255}; // Rouge pâle pour les cases
    Color caseB = (Color){40, 120, 60, 255}; // Vert pour les cases
    const Color ecriture_cases = {255, 215, 0, 255}; //Couleur écriture des cases
    const Color numero_cases = {27,41,76,255}; //Couleur numéro des cases


    // Ajout d'éléments décoratifs de Noël
    Texture2D snowflake = LoadTexture("snowflake.png"); // Flocon de neige
    Texture2D tree = LoadTexture("tree.png");           // Sapin de Noël
    Texture2D gift = LoadTexture("gift.png");           // Cadeau

    // Ajout de la texture pour le pion Père Noël
    Texture2D santa = LoadTexture("santa.jpg");

    // Ajout du traineau de fin
    Texture2D traineau = LoadTexture("traineau.png");

    // Ajout de la texture pour le fond du plateau
    Texture2D background = LoadTexture("fond2.jpg");
    Texture2D principalBG = LoadTexture("principal.png");
    Texture2D bannerNoel = LoadTexture("perenoel.png");
    Texture2D optionsBG = LoadTexture("option.jpg");


    //-----------------------------------------------------
    // LAYOUT : Plateau centré dans la zone gauche
    //-----------------------------------------------------

    // panneau à droite : moins large (ancré au bord droit)
    Rectangle sidePanel = (Rectangle){1920 - 420, 0, 420, 1080};  // panneau à droite

    int tailleCase = 130;  
    int totalCases = 20;   // 0 à 19

    // d’après ton schéma col/row : 0..7 en X, 0..4 en Y
    int maxCol = 7;
    int maxRow = 4;

    // dimensions réelles du plateau
    int plateauWidth  = (maxCol + 1) * tailleCase;   // 8 cases
    int plateauHeight = (maxRow + 1) * tailleCase;   // 5 cases

    // zone disponible pour le plateau = largeur avant le panneau
    int boardAreaWidth  = (int)sidePanel.x;    // 1320 px
    int boardAreaHeight = 1080;                // full height

    // offsets centrés
    int offsetX = (boardAreaWidth  - plateauWidth)  / 2;
    int offsetY = (boardAreaHeight - plateauHeight) / 2;

    while (!WindowShouldClose()) {
        // Gestion musique ON/OFF
        if (musicEnabled) {
            UpdateMusicStream(music);
            if (!IsMusicStreamPlaying(music)) {
                PlayMusicStream(music);
            }
        } else {
            StopMusicStream(music);
        }

        BeginDrawing();
        ClearBackground(bg1);

        // ------------------------------------------------------
        // Guirlandes du sapin (persistantes sur tous les écrans)
        // ------------------------------------------------------
        {
            const int sapinX = 430;
            const int sapinY = 150;
            const int height = 730;
            const int width  = 500;

            const int lightsCount = 42;
            static Vector2 lightPos[64];
            static int lightColorIndex[64];
            static bool initializedLights = false;

            Color palette[] = {
                {255, 240, 80, 255},
                {255, 60, 60, 255},
                {60, 255, 90, 255},
                {90, 140, 255, 255}
            };
            int paletteCount = 4;

            if (!initializedLights) {
                initializedLights = true;
                for (int i = 0; i < lightsCount; i++) {
                    float t = (float)i / (lightsCount - 1);
                    float rowWidth = width * t;
                    if (rowWidth < 20) rowWidth = 20;

                    float y = sapinY + t * height;
                    float x = sapinX - rowWidth/2 + (rand() % (int)rowWidth);

                    if (y > sapinY + height * 0.88f) {
                        y -= 60 + (rand() % 50);
                    }

                    lightPos[i] = (Vector2){x, y};
                    lightColorIndex[i] = rand() % paletteCount;
                }
            }

            for (int i = 0; i < lightsCount; i++) {
                float t = GetTime()*3.5f + i*0.35f;
                float intensity = 0.4f + 0.6f * (0.5f + 0.5f * sinf(t));

                Color base = palette[lightColorIndex[i]];
                Color glow = (Color){
                    base.r,
                    base.g,
                    base.b,
                    (unsigned char)(190 * intensity)
                };

                DrawCircleV(lightPos[i], 8,
                    (Color){glow.r, glow.g, glow.b, (unsigned char)(70 * intensity)});
                DrawCircleV(lightPos[i], 4, glow);
            }
        }

        switch (state) {

        // ============ MENU ============
        case STATE_MENU: {
            DrawTexturePro(
                principalBG,
                (Rectangle){0, 0, principalBG.width, principalBG.height},
                (Rectangle){0, 0, 1920, 1080},
                (Vector2){0, 0},
                0,
                WHITE
            );
//--------------------------------------------------------------
// ✨ Guirlandes du sapin — version corrigée
//--------------------------------------------------------------
{
    const int sapinX = 430;      // centre horizontal du sapin
    const int sapinY = 150;      // haut du sapin
    const int height = 730;      // hauteur visible
    const int width  = 500;      // largeur max

    const int lightsCount = 42;
    static Vector2 lightPos[64];
    static int lightColorIndex[64];
    static bool initialized = false;

    // ---- Palette LED ----
    Color palette[] = {
        {255, 240, 80, 255},   // jaune chaud
        {255, 60, 60, 255},    // rouge
        {60, 255, 90, 255},    // vert
        {90, 140, 255, 255}    // bleu
    };
    int paletteCount = 4;

    // ---------------------------------------------------
    // Initialisation une seule fois
    // ---------------------------------------------------
    if (!initialized) {
        initialized = true;

        for (int i = 0; i < lightsCount; i++) {

            float t = (float)i / (lightsCount - 1); // progression 0 → 1

            // Largeur de la couche courante (triangle)
            float rowWidth = width * t;
            if (rowWidth < 20) rowWidth = 20;

            float y = sapinY + t * height;
            float x = sapinX - rowWidth/2 + (rand() % (int)rowWidth);

            // ⛔ SUPPRESSION DES LED TROP BASSES (zone cadeau/tapis)
            if (y > sapinY + height * 0.88f) {
                // on remonte la LED automatiquement pour éviter les excès
                y -= 60 + (rand() % 50);
            }

            lightPos[i] = (Vector2){x, y};
            lightColorIndex[i] = rand() % paletteCount;
        }
    }

    // ---------------------------------------------------
    // Dessin animé
    // ---------------------------------------------------
    for (int i = 0; i < lightsCount; i++) {

        float t = GetTime()*3.5f + i*0.35f;   // ⚡ scintillement + rapide
        float intensity = 0.4f + 0.6f * (0.5f + 0.5f * sinf(t));

        Color base = palette[lightColorIndex[i]];
        Color glow = (Color){
            base.r,
            base.g,
            base.b,
            (unsigned char)(190 * intensity)
        };

        // Halo
        DrawCircleV(lightPos[i], 8,
            (Color){glow.r, glow.g, glow.b, (unsigned char)(70 * intensity)});

        // LED
        DrawCircleV(lightPos[i], 4, glow);
    }
}
            // --- AFFICHAGE DE LA BANNIÈRE DE NOËL ---
            float scaleBanner = 0.35f;

            float bannerW = bannerNoel.width * scaleBanner;
            float bannerH = bannerNoel.height * scaleBanner;

            float offsetX = -20;  // Ajustage horizontal banière

            float bannerX = 1920/2 - bannerW/2 + offsetX;
            float bannerY = 40;

            DrawTextureEx(
                bannerNoel,
                (Vector2){ bannerX, bannerY },
                0.0f,
                scaleBanner,
                WHITE
            );

            // --- Nouveaux boutons stylés ---
            Rectangle btnPlay  = (Rectangle){ 800, 500, 320, 90 };
            Rectangle btnOpt   = (Rectangle){ 800, 620, 320, 90 };
            Rectangle btnQuit  = (Rectangle){ 800, 740, 320, 90 };

            // Bouton JOUER
            if (DrawFancyButton(fontCandy, "JOUER", btnPlay)) {
                state = STATE_BOARD;
            }

            // Bouton OPTIONS
            if (DrawFancyButton(fontCandy, "OPTIONS", btnOpt)) {
                state = STATE_OPTIONS;
            }

            // Bouton QUITTER
            if (DrawFancyButton(fontCandy, "QUITTER", btnQuit)) {
                shouldExit = true;
            }

        } break;

        // ============ OPTIONS ============
        case STATE_OPTIONS: {
            // Fond image pour l'ecran d'options
            DrawTexturePro(
                optionsBG,
                (Rectangle){0, 0, optionsBG.width, optionsBG.height},
                (Rectangle){0, 0, 1920, 1080},
                (Vector2){0, 0},
                0,
                WHITE
            );

            // Grande carte centrale en verre dépoli
            Rectangle card = (Rectangle){ 460, 140, 1000, 800 };
            Color glass = (Color){255, 255, 255, 180};
            DrawRectangleRounded(card, 0.12f, 20, glass);
            DrawRectangleRoundedLines(card, 0.12f, 20, (Color){255,255,255,220});

            // Bordure extérieure en guirlande (lignes vertes/rouges)
            DrawRectangleLinesEx((Rectangle){card.x-6, card.y-6, card.width+12, card.height+12},
                                 4, (Color){0,150,0,200});
            DrawRectangleLinesEx((Rectangle){card.x-14, card.y-14, card.width+28, card.height+28},
                                 3, (Color){220,40,40,200});

            // Quelques flocons en fond léger
            for (int i = 0; i < 40; i++) {
                int x = GetRandomValue(0, 1920);
                int y = GetRandomValue(0, 1080);
                DrawTexture(snowflake, x, y, (Color){255,255,255,80});
            }

            // Titre OPTIONS centré en haut de la carte
            const char *optTitle = "OPTIONS";
            int optSize = 120;
            Vector2 optS = MeasureTextEx(fontSantabold, optTitle, optSize, 0);

            float optX = card.x + (card.width  - optS.x) / 2.0f;
            float optY = card.y + 40;

            DrawTextEx(fontSantabold, optTitle, (Vector2){optX, optY}, optSize, 0,
                       (Color){180, 20, 40, 255});

            // Ligne décorative sous le titre
            float decoW = optS.x + 40;
            float decoX = card.x + (card.width - decoW) / 2.0f;
            float decoY = optY + optS.y + 10;
            DrawRectangle(decoX, decoY, decoW, 4, (Color){0,140,60,255});
            DrawRectangle(decoX, decoY + 5, decoW, 2, (Color){255, 230, 150, 255});

            // Boutons centrés dans la carte
            float btnWidth  = 420;
            float btnHeight = 100;
            float centerX   = card.x + card.width / 2.0f;

            Rectangle musicBtn = (Rectangle){
                centerX - btnWidth/2,
                decoY + 80,
                btnWidth,
                btnHeight
            };

            Rectangle backBtn = (Rectangle){
                centerX - btnWidth/2,
                musicBtn.y + btnHeight + 60,
                btnWidth,
                btnHeight
            };

            const char *musicLabel = musicEnabled ? "Musique : ON" : "Musique : OFF";

            // Bouton musique ON/OFF (même style que les gros boutons du menu)
            if (DrawFancyButton(fontSantabold, musicLabel, musicBtn)) {
                musicEnabled = !musicEnabled;
            }

            // Bouton retour
            if (DrawFancyButton(fontSantabold, "Retour", backBtn)) {
                state = STATE_MENU;
            }
        } break;

        // ============ PLATEAU ============
        case STATE_BOARD: {
            // Dessin du fond du plateau
            DrawTextureEx(background, (Vector2){0, 0}, 0.0f, 1.0f, WHITE);

            // Afficher la bannière du Pere Noel en haut, entre le sapin et le tableau de bord
            float scaleBoardBanner = 0.33f;
            float boardBannerW = bannerNoel.width * scaleBoardBanner;
            float boardBannerH = bannerNoel.height * scaleBoardBanner;

            // On la place vers la droite, avant le panneau latéral
            float boardBannerX = sidePanel.x - boardBannerW - 160; // 40 px de marge avant le panneau
            float boardBannerY = 40;                              // en haut de l'écran

            DrawTextureEx(bannerNoel,
                          (Vector2){ boardBannerX, boardBannerY },
                          0.0f,
                          scaleBoardBanner,
                          WHITE);

            // Dessin des flocons de neige en arrière-plan (décor random)
            for (int i = 0; i < 10; i++) {
                DrawTexture(snowflake, rand() % 1920, rand() % 1080, WHITE);
            }

            // plateau gauche
            Draw3DBoard(font, totalCases, offsetX, offsetY, tailleCase,
                caseA, caseB, accent, accent);
    

            // Ajout de sapins autour du plateau
            for (int i = 0; i < 4; i++) {
                DrawTexture(tree, offsetX - 50 + i * 500, offsetY - 100, WHITE);
            }

            // Ajout de cadeaux sur certaines cases
            Vector2 giftPos = CaseToPos(3, offsetX, offsetY, tailleCase);
            DrawTexture(gift, giftPos.x + 20, giftPos.y + 20, WHITE);

            // Traineau de Noël en bas à gauche
            Vector2 posTraineau = {975, 790}; // position x,y sur l’écran
            float scaletraineau = 0.5f;               // ajuster la taille
            DrawTextureEx(traineau, posTraineau, 0.0f, scaletraineau, WHITE);

            // pion 
            float scale = (float)tailleCase / santa.width;
            Vector2 pPos = CaseToPos(player.pos, offsetX, offsetY, tailleCase);

            // Petit saut quand le pion se déplace
            float jumpOffset = 0.0f;
            if (player.isMoving) {
                float t = player.animTimer;
                if (t < 1.0f) {
                    float s = sinf(t * 3.1415926f);
                    jumpOffset = -s * (tailleCase * 0.25f);
                }
            }

            if (!tpInvisible7) {
                DrawTextureEx(
                    santa,
                    (Vector2){
                        pPos.x + (tailleCase - santa.width * scale) / 2,
                        pPos.y + (tailleCase - santa.height * scale) / 2 + jumpOffset
                    },
                    0.0f,scale,WHITE
                );
            }
            

            // --- Panneau latéral en verre dépoli (glassmorphism) ---
            Color glass = (Color){255,255,255,160};  // semi transparent
            DrawRectangleRounded(
                (Rectangle){sidePanel.x, sidePanel.y, sidePanel.width, sidePanel.height},
                0.08f, 20, glass
            );

            // Contour lumineux
            DrawRectangleRoundedLines(
                (Rectangle){sidePanel.x, sidePanel.y, sidePanel.width, sidePanel.height},
                0.08f, 20, (Color){255,255,255,200}
            );

            // Petite ombre à gauche
            DrawRectangle(sidePanel.x - 8, sidePanel.y, 8, sidePanel.height, (Color){0,0,0,50});


                        // ----- TITRE TABLEAU DE BORD -----
            // texte centré horizontalement
            const char *dashTitle = "Tableau de bord";
            float dashSize = 75;
            Vector2 dashMeasure = MeasureTextEx(font, dashTitle, dashSize, 0);

            // centre dans le panneau
            float dashX = sidePanel.x + (sidePanel.width - dashMeasure.x) / 2 + 50;
            float dashY = 80;

            DrawTextEx(customFont, dashTitle, (Vector2){dashX, dashY}, dashSize, 0, DARKBLUE);

            // ---- Soulignement décoratif (ligne verte + ligne bleue) ----
            float underlineWidth = dashMeasure.x - 50 ;    // petite marge
            float underlineX = sidePanel.x + (sidePanel.width - underlineWidth) / 2;
            float underlineY = dashY + dashMeasure.y + 6;

            DrawRectangle(underlineX, underlineY, underlineWidth, 4, accent);
            DrawRectangle(underlineX, underlineY + 4, underlineWidth, 2, DARKBLUE);

            // ----- Carte OBJECTIF -----
            const char *obj = "Objectif : atteindre la case 19 !";

            // Taille du texte
            float objSize = 50;
            Vector2 objMeasure = MeasureTextEx(fontSantabold, obj, objSize, 0);

            // Position du texte (plus de rectangle de fond)
            float objX = sidePanel.x + (sidePanel.width - objMeasure.x) / 2;
            float objY = underlineY + 40;

            // Texte objectif seul, sans carte blanche
            DrawTextEx(fontSantabold, obj, (Vector2){objX, objY}, objSize, 0, DARKBLUE);


            // Déplacement + arrivée ce frame (avance OU recule vers la cible)
            bool arrived = false;
            if (player.isMoving) {
                player.animTimer += GetFrameTime() * 3.0f;
                if (player.animTimer >= 1.0f) {
                    player.animTimer = 0.0f;

                    if (player.pos == player.cible) {
                        player.isMoving = false;
                        arrived = true;
                    } else {
                        if (player.pos < player.cible && player.pos < totalCases - 1) {
                            player.pos++;
                        } else if (player.pos > player.cible && player.pos > 0) {
                            player.pos--;
                        }

                        if (player.pos == player.cible) {
                            player.isMoving = false;
                            arrived = true;
                        }
                    }
                }
            }
            // Téléportation case 7 → 13
            if (!player.isMoving && player.pos == 7) {
                HandleTeleport7(&player, totalCases, font,
                                &tpActive7, &tpTimer7, &tpInvisible7);
            }

            // Piège case 9 : recule
            if (!player.isMoving && player.pos == 9) {
                HandleCaseNine(&player, totalCases, font, &trapActive, &trapTimer);
            }

            // ---- CASE 5 : RETOUR AU DEPART ----
            if (!player.isMoving && player.pos == 5) {
                HandleCaseFive(&player, totalCases, font, &trapActive5, &trapTimer5);
            }

            // Case 15 → avance de 4 cases
            if (!player.isMoving && player.pos == 15) {
                HandleCaseFifteen(&player, totalCases, font, &trapActive15, &trapTimer15);
            }
            // Case 4 : avance de 2 cases
            if (!player.isMoving && player.pos == 4) {
                HandleCaseFour(&player, totalCases, font, &trapActive4, &trapTimer4);
            }

            // Piège case 18 : retour départ
            if (!player.isMoving && player.pos == 18) {
                HandleCaseEighteen(&player, totalCases, font, &trapActive18, &trapTimer18);
            }

            // Arrivée sur une case Sudoku → lancer le jeu (uniquement SUR ARRIVÉE)
            if (arrived &&
                (player.pos == 1 || player.pos == 10 || player.pos == 17)) {

                // Choisir le niveau selon la case
                if (player.pos == 1)  SudokuSetLevel(1);   // Facile
                if (player.pos == 10) SudokuSetLevel(2);   // Moyen
                if (player.pos == 17) SudokuSetLevel(3);   // Difficile

                EndDrawing();
                StartSudoku(font);   // lance le mini-jeu

                // on revient au plateau sans relancer automatiquement
                continue;
            }


            // Arrivée case 8 ou 14 → Morpion
            if (arrived && (player.pos == 8 || player.pos == 14)) {
                EndDrawing();
                system("morpion.exe");
                continue;
            }
            
            // Arrivée sur une case PENDU (3, 11, 16)
            if (arrived && (player.pos == 3 || player.pos == 11 || player.pos == 16)) {

                EndDrawing();
                system("pendu.exe");  
                continue;             
            }

            // Arrivée sur une case DEMINEUR → lancer le bon niveau
            if (arrived && (player.pos == 2 || player.pos == 6 || player.pos == 13)) {

                int level = 1;
                if (player.pos == 6) level = 2;
                if (player.pos == 13) level = 3;

                EndDrawing();

                int won = StartDemineur(level);

                if (won != 1) {
                    // Le joueur a perdu → recule (ou autre logique)
                    player.pos -= 1;
                    if (player.pos < 0) player.pos = 0;
                }

                continue; // IMPORTANT !
            }

            // Victoire
            if (!player.isMoving && player.pos == totalCases - 1) {
                state = STATE_WIN;
            }

            // Dés affichés uniquement si pas de Sudoku en cours
            {

            int diceY = sidePanel.height - 250;
            Vector2 mouse = GetMousePosition();
            
            // -----------------------------------------------------------
            // VALEUR TRUQUEE — joli et bien centré au-dessus du dé truqué
            // -----------------------------------------------------------
            char txtTriche[64];
            snprintf(txtTriche, sizeof(txtTriche), "Valeur truquee : %d", tricheValue);

            int textSize = 36;
            Vector2 textMeasure = MeasureTextEx(fontSantabold, txtTriche, textSize, 0);

            float centerX = sidePanel.x + sidePanel.width / 2.0f;
            float tricheX = centerX - (textMeasure.x / 2.0f);
            float tricheY = diceY - 260;

            // Texte simple, sans ombre ni glow
            DrawTextEx(fontSantabold, txtTriche,
                       (Vector2){tricheX, tricheY}, textSize, 0,
                       (Color){0, 100, 0, 255});

            // Clique droit → changer valeur
            if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
                tricheValue++;
                if (tricheValue > 19) tricheValue = 1;
            }

                // Dé inférieur (normal) — centré dans le panneau
                Rectangle diceRect = (Rectangle){
                    centerX - 180.0f/2.0f,
                    diceY,
                    180,
                    180
                };

                DrawRectangleRounded(diceRect, 0.35f, 8, WHITE);
                DrawRectangleRoundedLines(diceRect, 0.35f, 8, accent);

                if (CheckCollisionPointRec(mouse, diceRect)) {
                    DrawDiceHighlight(diceRect);
                }

                DrawDiceDots(diceRect, diceValue, accentLight);

                const char *label1 = "De normal";
                Vector2 s1 = MeasureTextEx(font, label1, 26, 0);
                DrawTextEx(font, label1,
                           (Vector2){
                               diceRect.x + diceRect.width/2 - s1.x/2,
                               diceRect.y + diceRect.height - 35
                           },
                           26, 0, DARKGRAY);

                if (CheckCollisionPointRec(mouse, diceRect) &&
                    IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                    if (player.pos != totalCases - 1) {
                        diceValue = LaunchDice3D(font);
                        player.cible = player.pos + diceValue;
                        if (player.cible >= totalCases)
                            player.cible = totalCases - 1;
                        player.isMoving = true;
                    }
                }

                // Dé supérieur (truqué) — centré dans le panneau
                Rectangle fixedDiceRect = (Rectangle){
                    centerX - 180.0f/2.0f,
                    diceY - 200,
                    180,
                    180
                };

                DrawRectangleRounded(fixedDiceRect, 0.35f, 8, WHITE);
                DrawRectangleRoundedLines(fixedDiceRect, 0.35f, 8, accent);

                // effet bleu au survol
                if (CheckCollisionPointRec(mouse, fixedDiceRect)) {
                    DrawDiceHighlight(fixedDiceRect);
                }

                DrawDiceDots(fixedDiceRect, 1, accentLight);

                const char *label2 = "De truque";
                Vector2 s2 = MeasureTextEx(font, label2, 26, 0);
                DrawTextEx(font, label2,
                           (Vector2){
                               fixedDiceRect.x + fixedDiceRect.width/2 - s2.x/2,
                               fixedDiceRect.y + fixedDiceRect.height - 35
                           },
                           26, 0, DARKGRAY);

                           if (CheckCollisionPointRec(mouse, fixedDiceRect) &&
                            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {

                            if (player.pos != totalCases - 1) {

                                fakeRoll = tricheValue;               // ← UTILISÉ SEULEMENT POUR LE MOUVEMENT
                                player.cible = player.pos + fakeRoll;

                                if (player.cible >= totalCases)
                                    player.cible = totalCases - 1;

                                player.isMoving = true;
                            }
                        }

                       
            }

        } break;

        // ============ ÉCRAN DE VICTOIRE ============
        case STATE_WIN: {
            DrawRectangleGradientV(0, 0, 1920, 1080,
                                   (Color){255,220,160,255}, (Color){255,170,100,255});
            DrawTextEx(customFont, "BRAVO !", (Vector2){800, 380}, 110, 0, accent);
            DrawTextEx(font, "Tu as atteint la derniere case !",
                       (Vector2){640, 540}, 42, 0, DARKGRAY);

            if (DrawButton(font, "RETOUR MENU",
                           (Rectangle){820, 700, 280, 100},
                           accent, accentLight, 50)) {

                state = STATE_MENU;
                SudokuReset();
                player = (Player){0,0,false, 0.0f};
                diceValue = 1;
            }
        } break;

        } // fin switch(state)

        EndDrawing();
        if (shouldExit) break;
    } // fin while

    UnloadTexture(snowflake);
    UnloadTexture(tree);
    UnloadTexture(gift);
    UnloadTexture(santa);
    UnloadTexture(traineau);
    UnloadTexture(background);
    UnloadTexture(principalBG);
    UnloadTexture(optionsBG);
    UnloadFont(customFont);
    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
