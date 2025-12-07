#include "fifteen.h"
#include "raylib.h"
#include "player.h"

void HandleCaseFifteen(Player *player, int totalCases, Font font,
                       bool *trapActive15, float *trapTimer15)
{
    const float pauseBeforeMove = 1.2f;

    // Première fois sur la case : petite pause avec message
    if (!(*trapActive15)) {
        *trapActive15 = true;
        *trapTimer15 = pauseBeforeMove;
        return;
    }

    // Tant que le timer n'est pas terminé, on affiche le message
    if (*trapTimer15 > 0.0f) {
        *trapTimer15 -= GetFrameTime();

        const char *txt = "BONUS ! Tu avances de 4 cases !";
        int size = 55;

        Vector2 s = MeasureTextEx(font, txt, size, 0);
        float x = (1920 - s.x) / 2;
        float y = 900;

        DrawTextEx(font, txt, (Vector2){ x, y }, size, 0,
                   (Color){ 0, 120, 200, 255 });  // bleu cadeau

        return;
    }

    // Ensuite, on fait avancer de 4 cases
    if (!player->isMoving) {
        int avance = 4;
        if (player->pos + avance < totalCases)
            player->cible = player->pos + avance;
        else
            player->cible = totalCases - 1;

        player->isMoving = true;
        *trapActive15 = false;
    }

    // On garde le message affiché pendant le mouvement
    const char *txt = "BONUS ! Tu avances de 4 cases !";
    int size = 55;

    Vector2 s = MeasureTextEx(font, txt, size, 0);
    float x = (1920 - s.x) / 2;
    float y = 900;

    DrawTextEx(font, txt, (Vector2){ x, y }, size, 0,
               (Color){ 0, 120, 200, 255 });
}
