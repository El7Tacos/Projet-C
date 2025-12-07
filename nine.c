#include "nine.h"
#include "raylib.h"

// Case 9 : tu recules de 6 cases après une courte pause
void HandleCaseNine(Player *player, int totalCases, Font font, bool *trapActive, float *trapTimer)
{
    // Durée de la pause avant le recul
    const float pauseBeforeMove = 1.2f;

    // Première activation : on lance la pause, sans bouger le pion
    if (!(*trapActive)) {
        *trapActive = true;
        *trapTimer = pauseBeforeMove;
        return;
    }

    // Tant que la pause n'est pas finie, on affiche le message
    if (*trapTimer > 0.0f) {
        *trapTimer -= GetFrameTime();

        int msgSize = 55;
        const char *txt = "PIEGE ! Tu recules de 6 cases !";

        Vector2 txtSize = MeasureTextEx(font, txt, msgSize, 0);
        float msgX = (1920 - txtSize.x) / 2;
        float msgY = 900;

        DrawTextEx(font, txt, (Vector2){ msgX, msgY }, msgSize, 0,
                   (Color){ 0, 120, 0, 255 });  // vert sapin

        return;
    }

    // Une fois la pause terminée, on recule de 6 cases (sans passer sous 0)
    if (!player->isMoving) {
        int recul = 6;

        if (player->pos - recul >= 0)
            player->cible = player->pos - recul;
        else
            player->cible = 0;

        player->isMoving = true;
        *trapActive = false;
    }

    // On garde le message affiché pendant le mouvement
    int msgSize = 55;
    const char *txt = "PIEGE ! Tu recules de 6 cases !";
    Vector2 txtSize = MeasureTextEx(font, txt, msgSize, 0);
    float msgX = (1920 - txtSize.x) / 2;
    float msgY = 900;

    DrawTextEx(font, txt, (Vector2){ msgX, msgY }, msgSize, 0,
               (Color){ 0, 120, 0, 255 });
}
