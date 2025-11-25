#include "eighteen.h"
#include "raylib.h"

// Case 18 : Retour immédiat au départ avec message + pause
void HandleCaseEighteen(Player *player, int totalCases, Font font,
                        bool *trapActive18, float *trapTimer18)
{
    const float pauseTime = 1.2f;

    // 1) Première entrée dans le piège → on déclenche juste la pause
    if (!(*trapActive18)) {
        *trapActive18 = true;
        *trapTimer18 = pauseTime;
        return;
    }

    // 2) Tant que le timer n’est pas écoulé → afficher le message et ne pas bouger
    if (*trapTimer18 > 0.0f) {
        *trapTimer18 -= GetFrameTime();

        const char *txt = "PIEGE ! Retour au DEPART !";
        int size = 55;

        Vector2 s = MeasureTextEx(font, txt, size, 0);
        float x = (1920 - s.x) / 2;
        float y = 900;

        DrawTextEx(font, txt, (Vector2){x, y}, size, 0, (Color){220,40,40,255});
        return;
    }

    // 3) Une fois la pause finie → on lance le mouvement vers 0
    if (!player->isMoving) {
        player->cible = 0;    // cible = case départ
        player->isMoving = true;
        *trapActive18 = false;
    }

    // 4) Pendant le mouvement, on continue d'afficher le message
    const char *txt = "PIEGE ! Retour au DEPART !";
    int size = 55;

    Vector2 s = MeasureTextEx(font, txt, size, 0);
    float x = (1920 - s.x) / 2;
    float y = 900;

    DrawTextEx(font, txt, (Vector2){x, y}, size, 0, (Color){220,40,40,255});
}
