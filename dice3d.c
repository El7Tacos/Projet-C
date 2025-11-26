#include "dice3d.h"
#include "raylib.h"
#include "rlgl.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

// ============================================================
// NEIGE AUTOUR DU DÉ
// ============================================================
#define SNOW_COUNT 200

typedef struct {
    Vector3 position;
    float speed;     // vitesse verticale
    float radius;    // taille du flocon
    float driftX;    // mouvement horizontal X (vent)
    float driftZ;    // mouvement horizontal Z (vent)
} Snowflake;

static Snowflake snowflakes[SNOW_COUNT];

// Initialise les flocons de neige
void InitSnowflakes(float area) {
    for (int i = 0; i < SNOW_COUNT; i++) {
        snowflakes[i].position = (Vector3){
            GetRandomValue(-area*100, area*100)/100.0f,
            GetRandomValue(0, 500)/100.0f,
            GetRandomValue(-area*100, area*100)/100.0f
        };
        snowflakes[i].speed = GetRandomValue(10, 30)/10.0f;
        snowflakes[i].radius = GetRandomValue(1, 3)/50.0f;
        snowflakes[i].driftX = GetRandomValue(-20, 20)/100.0f;
        snowflakes[i].driftZ = GetRandomValue(-20, 20)/100.0f;
    }
}

// Met à jour les flocons chaque frame
void UpdateSnowflakes(float dt, float area) {
    for (int i = 0; i < SNOW_COUNT; i++) {
        snowflakes[i].position.y -= snowflakes[i].speed * dt;
        snowflakes[i].position.x += snowflakes[i].driftX * dt;
        snowflakes[i].position.z += snowflakes[i].driftZ * dt;

        if (snowflakes[i].position.y < 0 ||
            fabsf(snowflakes[i].position.x) > area ||
            fabsf(snowflakes[i].position.z) > area) 
        {
            snowflakes[i].position.y = 5.0f;
            snowflakes[i].position.x = GetRandomValue(-area*100, area*100)/100.0f;
            snowflakes[i].position.z = GetRandomValue(-area*100, area*100)/100.0f;
            snowflakes[i].speed = GetRandomValue(10, 30)/10.0f;
            snowflakes[i].radius = GetRandomValue(1, 3)/50.0f;
            snowflakes[i].driftX = GetRandomValue(-20, 20)/100.0f;
            snowflakes[i].driftZ = GetRandomValue(-20, 20)/100.0f;
        }
    }
}

// Dessine tous les flocons
void DrawSnowflakes(void)
{
    for (int i = 0; i < SNOW_COUNT; i++) {
        DrawSphere(snowflakes[i].position, snowflakes[i].radius, WHITE);
    }
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
// ORIENTATION DE LA FACE OPPOSÉE AU SOL (FACE GAGNANTE VERS LE BAS)
// ============================================================
static Vector3 GetFaceRotation(int face)
{
    switch (face) {
        case 1: return (Vector3){90, 0, 0};
        case 2: return (Vector3){-90, 0, 0};
        case 3: return (Vector3){0, 0, 90};
        case 4: return (Vector3){0, 0, -90};
        case 5: return (Vector3){180, 0, 0};
        case 6: return (Vector3){0, 0, 0};
        default: return (Vector3){0, 0, 0};
    }
}

// ============================================================
// DESSIN DU DÉ AVEC POINTS
// ============================================================
static void DrawFlatDot(Vector3 pos, Vector3 normal, float radius, Color color)
{
    Vector3 top = (Vector3){
        pos.x + normal.x * 0.01f,
        pos.y + normal.y * 0.01f,
        pos.z + normal.z * 0.01f
    };

    DrawCylinderEx(pos, top, radius, radius, 20, color);
}

static void DrawLightHalo(Vector3 pos)
{
    DrawCircle3D((Vector3){pos.x, pos.y - 0.49f, pos.z}, 0.8f, (Vector3){1,0,0}, 90, Fade((Color){255,200,50,255}, 0.2f));
}



void DrawDiceWithDots(Vector3 pos, float size, Color bodyColor, Color dotColor)
{
    float r = size / 2.0f;
    float d = r * 0.45f;
    float dotRadius = size * 0.12f;

    // Cube du dé
    DrawCube(pos, size, size, size, bodyColor);
    DrawCubeWires(pos, size, size, size, BLACK);

    // POINTS (disques dorés)
    // FACE +Z (1)
    DrawFlatDot((Vector3){pos.x, pos.y, pos.z + r}, (Vector3){0, 0, 1}, dotRadius * 1.2f, dotColor);
    // FACE -Z (2)
    DrawFlatDot((Vector3){pos.x - d, pos.y + d, pos.z - r}, (Vector3){0, 0, -1}, dotRadius, dotColor);
    DrawFlatDot((Vector3){pos.x + d, pos.y - d, pos.z - r}, (Vector3){0, 0, -1}, dotRadius, dotColor);
    // FACE +X (3)
    DrawFlatDot((Vector3){pos.x + r, pos.y + d, pos.z + d}, (Vector3){1, 0, 0}, dotRadius, dotColor);
    DrawFlatDot((Vector3){pos.x + r, pos.y,     pos.z},     (Vector3){1, 0, 0}, dotRadius, dotColor);
    DrawFlatDot((Vector3){pos.x + r, pos.y - d, pos.z - d}, (Vector3){1, 0, 0}, dotRadius, dotColor);
    // FACE -X (4)
    DrawFlatDot((Vector3){pos.x - r, pos.y + d, pos.z + d}, (Vector3){-1, 0, 0}, dotRadius, dotColor);
    DrawFlatDot((Vector3){pos.x - r, pos.y + d, pos.z - d}, (Vector3){-1, 0, 0}, dotRadius, dotColor);
    DrawFlatDot((Vector3){pos.x - r, pos.y - d, pos.z + d}, (Vector3){-1, 0, 0}, dotRadius, dotColor);
    DrawFlatDot((Vector3){pos.x - r, pos.y - d, pos.z - d}, (Vector3){-1, 0, 0}, dotRadius, dotColor);
    // FACE +Y (5)
    DrawFlatDot((Vector3){pos.x - d, pos.y + r, pos.z - d}, (Vector3){0, 1, 0}, dotRadius, dotColor);
    DrawFlatDot((Vector3){pos.x + d, pos.y + r, pos.z - d}, (Vector3){0, 1, 0}, dotRadius, dotColor);
    DrawFlatDot((Vector3){pos.x,     pos.y + r, pos.z},     (Vector3){0, 1, 0}, dotRadius, dotColor);
    DrawFlatDot((Vector3){pos.x - d, pos.y + r, pos.z + d}, (Vector3){0, 1, 0}, dotRadius, dotColor);
    DrawFlatDot((Vector3){pos.x + d, pos.y + r, pos.z + d}, (Vector3){0, 1, 0}, dotRadius, dotColor);

    // FACE -Y (6) - réajustement pour que les points ne se touchent pas
    float d6 = d * 1.1f;
    float r6 = dotRadius * 0.8f;

    DrawFlatDot((Vector3){pos.x - d6, pos.y - r, pos.z - d6}, (Vector3){0, -1, 0}, r6, dotColor);
    DrawFlatDot((Vector3){pos.x + d6, pos.y - r, pos.z - d6}, (Vector3){0, -1, 0}, r6, dotColor);
    DrawFlatDot((Vector3){pos.x - d6, pos.y - r, pos.z + d6}, (Vector3){0, -1, 0}, r6, dotColor);
    DrawFlatDot((Vector3){pos.x + d6, pos.y - r, pos.z + d6}, (Vector3){0, -1, 0}, r6, dotColor);
    DrawFlatDot((Vector3){pos.x - d6, pos.y - r, pos.z},     (Vector3){0, -1, 0}, r6, dotColor);
    DrawFlatDot((Vector3){pos.x + d6, pos.y - r, pos.z},     (Vector3){0, -1, 0}, r6, dotColor);
}

// ============================================================
// LANCEMENT DU DÉ 3D
// ============================================================
int LaunchDice3D(Font font)
{
    // Caméra
    Camera3D camera = {0};
    camera.position = (Vector3){4.0f, 3.0f, 4.0f};
    camera.target   = (Vector3){0.0f, 0.0f, 0.0f};
    camera.up       = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy     = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Dé
    Dice3D dice = {0};
    dice.rotation = (Vector3){GetRandomValue(0, 360), GetRandomValue(0, 360), GetRandomValue(0, 360)};
    dice.rotationSpeed = (Vector3){GetRandomValue(400, 700), GetRandomValue(400, 700), GetRandomValue(400, 700)};
    dice.position = (Vector3){0, 0, 0};
    dice.velocity = (Vector3){GetRandomValue(-2, 2)/10.0f, 4.0f, GetRandomValue(-2, 2)/10.0f};
    dice.duration = 2.0f + GetRandomValue(0, 40)/100.0f;
    dice.rolling = true;

    int finalResult = GetRandomValue(1, 6);
    float camAngle = 0.0f;
    float camZoom = 4.0f;

    // NEIGE - initialisation avec zone de 5x5
    InitSnowflakes(5.0f);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        // Mise à jour du dé
        if (dice.rolling) {
            dice.timer += dt;
            float t = dice.timer / dice.duration;
            float slow = 1.0f - powf(t, 3.5f);
            if (slow < 0) slow = 0;

            dice.rotation.x += dice.rotationSpeed.x * dt * slow;
            dice.rotation.y += dice.rotationSpeed.y * dt * slow;
            dice.rotation.z += dice.rotationSpeed.z * dt * slow;

            dice.velocity.y -= 9.8f * dt;
            dice.position.y += dice.velocity.y * dt;

            if (dice.position.y <= 0) {
                dice.position.y = 0;
                dice.velocity.y *= -0.45f;
                dice.rotationSpeed.x *= 0.8f;
                dice.rotationSpeed.y *= 0.8f;
                dice.rotationSpeed.z *= 0.8f;
            }

            dice.position.x += dice.velocity.x * dt;
            dice.position.z += dice.velocity.z * dt;
            dice.velocity.x *= 0.98f;
            dice.velocity.z *= 0.98f;

            if (t >= 1.0f && fabsf(dice.velocity.y) < 0.2f) {
                dice.rolling = false;
                Vector3 target = GetFaceRotation(finalResult);

                float blend = fminf(1.0f, (dice.timer - dice.duration)/0.4f);
                if (blend < 0.0f) blend = 0.0f;

                dice.rotation.x = dice.rotation.x * (1 - blend) + target.x * blend;
                dice.rotation.y = dice.rotation.y * (1 - blend) + target.y * blend;
                dice.rotation.z = dice.rotation.z * (1 - blend) + target.z * blend;

                if (blend >= 1.0f) {
                    dice.rotation = target;
                    dice.position = (Vector3){0, 0.5f, 0};
                }
            }
        } else {
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                break;
        }

        // Caméra
        camAngle += dt * 1.0f;
        float targetZoom = dice.rolling ? 3.0f : 4.0f;
        camZoom += (targetZoom - camZoom) * dt * 2.0f;
        camera.position.x = cosf(camAngle) * camZoom;
        camera.position.z = sinf(camAngle) * camZoom;
        camera.position.y = 2.5f + 0.3f * sinf(dice.timer*1.5f);

        // Mettre à jour la neige
        UpdateSnowflakes(dt, 5.0f);

        // Rendu
        BeginDrawing();
        ClearBackground((Color){27,41,76,255});
        BeginMode3D(camera);

        // Sol
        DrawCube((Vector3){0, -0.51f, 0}, 12.0f, 0.1f, 12.0f, (Color){230, 240, 255, 255});

        // Ombre douce
        DrawCircle3D((Vector3){dice.position.x,-0.49f,dice.position.z}, 0.6f,
                     (Vector3){1,0,0}, 90, Fade(BLACK,0.2f));

        rlPushMatrix();
        rlTranslatef(dice.position.x, dice.position.y + 0.5f, dice.position.z);
        rlRotatef(dice.rotation.y,0,1,0);
        rlRotatef(dice.rotation.x,1,0,0);
        rlRotatef(dice.rotation.z,0,0,1);

        DrawDiceWithDots((Vector3){0,0,0}, 1.0f,
                         (Color){200,30,30,255}, (Color){230,200,60,255});
        rlPopMatrix();

        // Dessiner les flocons
        DrawSnowflakes();

        EndMode3D();

        // Texte
        DrawTextEx(font, "LANCER DE DE", (Vector2){440,40}, 40, 0, BROWN);
        if (dice.rolling) {
            int alpha = 150 + (int)(105 * sinf(GetTime()*4.0f));
            DrawTextEx(font, "Le de roule...", (Vector2){540,720}, 30, 0, (Color){80,80,80,alpha});
        } else {
            DrawTextEx(font, TextFormat("Resultat : %d (ENTREE ou clic pour revenir)", finalResult),
                       (Vector2){260,720}, 30, 0, DARKBROWN);
        }

        EndDrawing();
    }

    return finalResult;
}
