#include "four.h"
#include "raylib.h"

// Case 4 : avance de 2 cases après une courte pause et un message
void HandleCaseFour(Player *player, int totalCases, Font font,
                    bool *trapActive4, float *trapTimer4)
{
    const float pauseTime = 1.2f;

    // Première activation : pause avec message
    if (!(*trapActive4)) {
        *trapActive4 = true;
        *trapTimer4 = pauseTime;
        return;
    }

    // Tant que le timer n'est pas fini, on affiche le message et on attend
    if (*trapTimer4 > 0.0f) {
        *trapTimer4 -= GetFrameTime();

        const char *txt = "BONUS ! Tu avances de 2 cases !";
        int size = 55;

        Vector2 s = MeasureTextEx(font, txt, size, 0);
        float x = (1920 - s.x) / 2;
        float y = 900;

        DrawTextEx(font, txt, (Vector2){ x, y }, size, 0,
                   (Color){ 0, 150, 0, 255 });  // vert bonus

        return;
    }

    // Une fois la pause terminée, on fait avancer de 2 cases
    if (!player->isMoving) {
        int avance = 2;

        if (player->pos + avance < totalCases)
            player->cible = player->pos + avance;
        else
            player->cible = totalCases - 1;

        player->isMoving = true;
        *trapActive4 = false;
    }

    // On garde le message affiché pendant le mouvement
    const char *txt = "BONUS ! Tu avances de 2 cases !";
    int size = 55;

    Vector2 s = MeasureTextEx(font, txt, size, 0);
    float x = (1920 - s.x) / 2;
    float y = 900;

    DrawTextEx(font, txt, (Vector2){ x, y }, size, 0,
               (Color){ 0, 150, 0, 255 });
}
