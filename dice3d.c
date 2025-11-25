#include "dice3d.h"
#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// ============================================================
// POINT PROPRE collé sur une face (pastille en relief)
// ============================================================
static void DrawDotOnFace(Vector3 pos, Vector3 normal, float radius, Color col) {
    rlPushMatrix();
    rlTranslatef(pos.x, pos.y, pos.z);
    
    // Orientation selon la normale de la face
    Vector3 up = {0, 1, 0};
    Vector3 n = Vector3Normalize(normal);
    Vector3 axis = Vector3CrossProduct(up, n);
    float dot = Vector3DotProduct(up, n);
    
    if (fabsf(dot) < 0.999f) {
        float angle = acosf(dot) * RAD2DEG;
        rlRotatef(angle, axis.x, axis.y, axis.z);
    } else if (dot < 0) {
        rlRotatef(180, 1, 0, 0);
    }
    
    // Pastille légèrement en relief
    DrawCylinder((Vector3){0, 0, 0}, radius, radius, 0.04f, 20, col);
    
    rlPopMatrix();
}

// ============================================================
// DESSINER TOUS LES POINTS sur les 6 faces du dé
// ============================================================
static void DrawAllDots(float size) {
    float r = size / 2.0f;
    float d = size / 3.2f;
    float dotRadius = size * 0.09f;
    float offset = r + 0.021f;
    
    Color dotCol = (Color){245, 245, 250, 255}; // Blanc cassé
    
    // === FACE 1 (+Z) - 1 point ===
    Vector3 normalZ = {0, 0, 1};
    DrawDotOnFace((Vector3){0, 0, offset}, normalZ, dotRadius, dotCol);
    
    // === FACE 2 (-Z) - 2 points ===
    Vector3 normalNZ = {0, 0, -1};
    DrawDotOnFace((Vector3){-d, d, -offset}, normalNZ, dotRadius, dotCol);
    DrawDotOnFace((Vector3){d, -d, -offset}, normalNZ, dotRadius, dotCol);
    
    // === FACE 3 (+X) - 3 points ===
    Vector3 normalX = {1, 0, 0};
    DrawDotOnFace((Vector3){offset, d, d}, normalX, dotRadius, dotCol);
    DrawDotOnFace((Vector3){offset, 0, 0}, normalX, dotRadius, dotCol);
    DrawDotOnFace((Vector3){offset, -d, -d}, normalX, dotRadius, dotCol);
    
    // === FACE 4 (-X) - 4 points ===
    Vector3 normalNX = {-1, 0, 0};
    DrawDotOnFace((Vector3){-offset, d, d}, normalNX, dotRadius, dotCol);
    DrawDotOnFace((Vector3){-offset, d, -d}, normalNX, dotRadius, dotCol);
    DrawDotOnFace((Vector3){-offset, -d, d}, normalNX, dotRadius, dotCol);
    DrawDotOnFace((Vector3){-offset, -d, -d}, normalNX, dotRadius, dotCol);
    
    // === FACE 5 (+Y) - 5 points ===
    Vector3 normalY = {0, 1, 0};
    DrawDotOnFace((Vector3){-d, offset, -d}, normalY, dotRadius, dotCol);
    DrawDotOnFace((Vector3){d, offset, -d}, normalY, dotRadius, dotCol);
    DrawDotOnFace((Vector3){-d, offset, d}, normalY, dotRadius, dotCol);
    DrawDotOnFace((Vector3){d, offset, d}, normalY, dotRadius, dotCol);
    DrawDotOnFace((Vector3){0, offset, 0}, normalY, dotRadius, dotCol);
    
    // === FACE 6 (-Y) - 6 points ===
    Vector3 normalNY = {0, -1, 0};
    DrawDotOnFace((Vector3){-d, -offset, -d}, normalNY, dotRadius, dotCol);
    DrawDotOnFace((Vector3){-d, -offset, 0}, normalNY, dotRadius, dotCol);
    DrawDotOnFace((Vector3){-d, -offset, d}, normalNY, dotRadius, dotCol);
    DrawDotOnFace((Vector3){d, -offset, -d}, normalNY, dotRadius, dotCol);
    DrawDotOnFace((Vector3){d, -offset, 0}, normalNY, dotRadius, dotCol);
    DrawDotOnFace((Vector3){d, -offset, d}, normalNY, dotRadius, dotCol);
}

// ============================================================
// DÉ CADEAU MAGNIFIQUE 🎁✨
// ============================================================
static void DrawBeautifulGiftDice(float size, float time) {
    float r = size / 2.0f;
    
    // Couleurs festives
    Color redGift = (Color){220, 50, 70, 255};
    Color gold = (Color){255, 215, 0, 255};
    
    // Cube principal avec coins arrondis simulés
    DrawCube((Vector3){0, 0, 0}, size, size, size, redGift);
    
    // Contour élégant
    DrawCubeWires((Vector3){0, 0, 0}, size + 0.02f, size + 0.02f, size + 0.02f, 
                  (Color){255, 255, 255, 100});
    
    // === RUBANS DORÉS ÉPAIS ===
    float ribbonW = size / 7.0f;
    
    // Ruban vertical
    DrawCube((Vector3){0, 0, 0}, ribbonW, size + 0.03f, size + 0.03f, gold);
    
    // Ruban horizontal
    DrawCube((Vector3){0, 0, 0}, size + 0.03f, ribbonW, size + 0.03f, gold);
    
    // Nœud sur le dessus (décoratif)
    float bowSize = size * 0.25f;
    DrawSphere((Vector3){0, r + bowSize * 0.5f, 0}, bowSize, gold);
    DrawSphere((Vector3){-bowSize * 0.7f, r + bowSize * 0.3f, 0}, bowSize * 0.6f, gold);
    DrawSphere((Vector3){bowSize * 0.7f, r + bowSize * 0.3f, 0}, bowSize * 0.6f, gold);
    
    // === POINTS DU DÉ ===
    DrawAllDots(size);
}

// ============================================================
// STRUCTURE DU DÉ
// ============================================================
typedef struct {
    Vector3 rotation;
    Vector3 rotationSpeed;
    Vector3 position;
    Vector3 velocity;
    float timer;
    float duration;
    bool rolling;
    int result;
} Dice3D;

// ============================================================
// ROTATION FINALE
// ============================================================
static Vector3 GetFaceRotation(int face) {
    switch (face) {
        case 1: return (Vector3){90, 0, 0};
        case 2: return (Vector3){-90, 0, 0};
        case 3: return (Vector3){0, 0, -90};
        case 4: return (Vector3){0, 0, 90};
        case 5: return (Vector3){0, 0, 0};
        case 6: return (Vector3){180, 0, 0};
        default: return (Vector3){0, 0, 0};
    }
}

// ============================================================
// PARTICULES DE NEIGE ❄️
// ============================================================
#define MAX_SNOW 100
typedef struct {
    Vector3 pos;
    float speed;
    float size;
} Snowflake;

static Snowflake snow[MAX_SNOW];
static bool snowInit = false;

static void InitSnow() {
    for (int i = 0; i < MAX_SNOW; i++) {
        snow[i].pos = (Vector3){
            GetRandomValue(-10, 10) / 1.0f,
            GetRandomValue(0, 10) / 1.0f,
            GetRandomValue(-10, 10) / 1.0f
        };
        snow[i].speed = 0.3f + GetRandomValue(0, 20) / 100.0f;
        snow[i].size = 0.03f + GetRandomValue(0, 5) / 100.0f;
    }
    snowInit = true;
}

static void UpdateAndDrawSnow(float dt) {
    for (int i = 0; i < MAX_SNOW; i++) {
        snow[i].pos.y -= snow[i].speed * dt;
        if (snow[i].pos.y < -1.0f) {
            snow[i].pos.y = 10.0f;
            snow[i].pos.x = GetRandomValue(-10, 10) / 1.0f;
            snow[i].pos.z = GetRandomValue(-10, 10) / 1.0f;
        }
        DrawSphere(snow[i].pos, snow[i].size, WHITE);
    }
}

// ============================================================
// LANCEMENT DU DÉ
// ============================================================
int LaunchDice3D(Font font) {
    if (!snowInit) InitSnow();
    
    // Caméra
    Camera3D camera = {0};
    camera.position = (Vector3){5.0f, 3.5f, 5.0f};
    camera.target = (Vector3){0.0f, 0.5f, 0.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;
    
    // Dé
    Dice3D dice = {0};
    dice.rotation = (Vector3){
        GetRandomValue(0, 360),
        GetRandomValue(0, 360),
        GetRandomValue(0, 360)
    };
    dice.rotationSpeed = (Vector3){
        GetRandomValue(500, 900),
        GetRandomValue(500, 900),
        GetRandomValue(500, 900)
    };
    dice.position = (Vector3){0, 2.5f, 0};
    dice.velocity = (Vector3){
        GetRandomValue(-4, 4) / 10.0f,
        0.0f,
        GetRandomValue(-4, 4) / 10.0f
    };
    dice.duration = 2.8f + GetRandomValue(0, 60) / 100.0f;
    dice.rolling = true;
    
    int finalResult = GetRandomValue(1, 6);
    float camAngle = 0.0f;
    float camDistance = 6.0f;
    
    // Boucle
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        // === PHYSIQUE ===
        if (dice.rolling) {
            dice.timer += dt;
            float t = dice.timer / dice.duration;
            float slowdown = 1.0f - powf(t, 3.5f);
            if (slowdown < 0) slowdown = 0;
            
            dice.rotation.x += dice.rotationSpeed.x * dt * slowdown;
            dice.rotation.y += dice.rotationSpeed.y * dt * slowdown;
            dice.rotation.z += dice.rotationSpeed.z * dt * slowdown;
            
            dice.velocity.y -= 14.0f * dt;
            dice.position.y += dice.velocity.y * dt;
            
            if (dice.position.y <= 0.5f) {
                dice.position.y = 0.5f;
                dice.velocity.y *= -0.45f;
                dice.rotationSpeed.x *= 0.82f;
                dice.rotationSpeed.y *= 0.82f;
                dice.rotationSpeed.z *= 0.82f;
            }
            
            dice.position.x += dice.velocity.x * dt;
            dice.position.z += dice.velocity.z * dt;
            dice.velocity.x *= 0.97f;
            dice.velocity.z *= 0.97f;
            
            if (t >= 1.0f && fabsf(dice.velocity.y) < 0.25f) {
                dice.rolling = false;
                dice.rotation = GetFaceRotation(finalResult);
                dice.position.y = 0.5f;
            }
        } else {
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                break;
            }
        }
        
        // === CAMÉRA ===
        camAngle += dt * 0.6f;
        float targetDist = dice.rolling ? 6.5f : 5.0f;
        camDistance += (targetDist - camDistance) * dt * 2.5f;
        
        camera.position.x = cosf(camAngle) * camDistance;
        camera.position.z = sinf(camAngle) * camDistance;
        camera.position.y = 3.5f + 0.3f * sinf(dice.timer * 1.8f);
        camera.target = (Vector3){dice.position.x, 0.6f, dice.position.z};
        
        // === RENDU ===
        BeginDrawing();
        
        // Fond dégradé hivernal
        DrawRectangleGradientV(0, 0, GetScreenWidth(), GetScreenHeight(),
                               (Color){135, 175, 215, 255},
                               (Color){245, 250, 255, 255});
        
        BeginMode3D(camera);
        
        // Sol enneigé élégant
        DrawCube((Vector3){0, -0.51f, 0}, 20, 0.1f, 20, (Color){250, 250, 255, 255});
        DrawCube((Vector3){0, -0.61f, 0}, 20.2f, 0.1f, 20.2f, (Color){220, 230, 245, 255});
        
        // Neige qui tombe
        UpdateAndDrawSnow(dt);
        
        // === DÉ ===
        rlPushMatrix();
        rlTranslatef(dice.position.x, dice.position.y, dice.position.z);
        rlRotatef(dice.rotation.y, 0, 1, 0);
        rlRotatef(dice.rotation.x, 1, 0, 0);
        rlRotatef(dice.rotation.z, 0, 0, 1);
        
        DrawBeautifulGiftDice(1.0f, dice.timer);
        
        rlPopMatrix();
        
        // Ombre douce
        DrawCircle3D((Vector3){dice.position.x, 0.01f, dice.position.z}, 
                     0.6f, (Vector3){1, 0, 0}, 90, (Color){0, 0, 0, 60});
        
        EndMode3D();
        
        // === UI FESTIVE ===
        DrawTextEx(font, "🎁 CADEAU DE NOEL 🎁", 
                   (Vector2){400, 25}, 50, 2, (Color){200, 40, 60, 255});
        
        if (dice.rolling) {
            DrawTextEx(font, "✨ Le cadeau roule... ✨", 
                       (Vector2){460, 705}, 34, 1, (Color){100, 100, 150, 255});
        } else {
            char msg[120];
            snprintf(msg, sizeof(msg), "🎲 Resultat : %d 🎲", finalResult);
            DrawTextEx(font, msg, (Vector2){480, 680}, 38, 1, (Color){220, 50, 70, 255});
            DrawTextEx(font, "[ ENTREE pour continuer ]", 
                       (Vector2){450, 725}, 26, 1, (Color){120, 120, 140, 255});
        }
        
        EndDrawing();
    }
    
    return finalResult;
}