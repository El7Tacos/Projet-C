#include "four.h"
#include "raylib.h"

// Case 4 : avance de 2 cases après une petite pause + message
void HandleCaseFour(Player *player, int totalCases, Font font,
                    bool *trapActive4, float *trapTimer4)
{
    const float pauseTime = 1.2f;

    // 1) Première activation → pause + message
    if (!(*trapActive4)) {
        *trapActive4 = true;
        *trapTimer4 = pauseTime;
        return;
    }

    // 2) Timer pas fini → on affiche le message et on attend
    if (*trapTimer4 > 0.0f) {
        *trapTimer4 -= GetFrameTime();

        const char *txt = "BONUS ! Tu avances de 2 cases !";
        int size = 55;

        Vector2 s = MeasureTextEx(font, txt, size, 0);
        float x = (1920 - s.x) / 2;
        float y = 900;

        DrawTextEx(font, txt, (Vector2){x, y}, size, 0,
                   (Color){0,150,0,255});  // vert bonus

        return;
    }

    // 3) Une fois la pause terminée → on avance !
    if (!player->isMoving) {

        int avance = 2;

        if (player->pos + avance < totalCases)
            player->cible = player->pos + avance;
        else
            player->cible = totalCases - 1;

        player->isMoving = true;
        *trapActive4 = false;
    }

    // 4) On continue d’afficher pendant le mouvement
    const char *txt = "BONUS ! Tu avances de 2 cases !";
    int size = 55;

    Vector2 s = MeasureTextEx(font, txt, size, 0);
    float x = (1920 - s.x) / 2;
    float y = 900;

    DrawTextEx(font, txt, (Vector2){x, y}, size, 0,
               (Color){0,150,0,255});
}
