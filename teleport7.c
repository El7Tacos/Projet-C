#include "teleport7.h"
#include "raylib.h"

// Téléportation case 7 → 13 avec disparition du pion
void HandleTeleport7(Player *player, int totalCases, Font font,
                     bool *tpActive, float *tpTimer, bool *tpInvisible)
{
    const float pauseTime = 3.0f;  // disparition pendant 3 secondes

    // Première activation → on lance le timer et on rend invisible
    if (!(*tpActive)) {
        *tpActive = true;
        *tpTimer = pauseTime;
        *tpInvisible = true;
        return;
    }

    // Tant que le timer tourne → afficher message + pion invisible
    if (*tpTimer > 0.0f) {
        *tpTimer -= GetFrameTime();

        const char *txt = "TELEPORTATION MAGIQUE !";
        int size = 55;

        Vector2 s = MeasureTextEx(font, txt, size, 0);
        float x = (1920 - s.x) / 2;
        float y = 900;

        DrawTextEx(font, txt, (Vector2){x, y}, size, 0, (Color){180,40,200,255});
        return;
    }

    // Fin du timer → on réapparaît à la case 13
    if (!player->isMoving) {
        player->pos = 13;          // téléportation immédiate
        *tpInvisible = false;      // réapparition
        *tpActive = false;         // reset du piège
    }

    // Message affiché pendant la frame de réapparition
    const char *txt = "TELEPORTATION MAGIQUE !";
    int size = 55;

    Vector2 s = MeasureTextEx(font, txt, size, 0);
    float x = (1920 - s.x) / 2;
    float y = 900;

    DrawTextEx(font, txt, (Vector2){x, y}, size, 0, (Color){180,40,200,255});
}
