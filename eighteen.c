#include "eighteen.h"
#include "raylib.h"

// Case 18 : retour au départ avec message et courte pause
void HandleCaseEighteen(Player *player, int totalCases, Font font,
                        bool *trapActive18, float *trapTimer18)
{
    const float pauseTime = 1.2f;

    // Première fois sur la case : on démarre le piège
    if (!(*trapActive18)) {
        *trapActive18 = true;
        *trapTimer18 = pauseTime;
        return;
    }

    // Tant que le timer n'est pas écoulé, on affiche le message et le pion reste en place
    if (*trapTimer18 > 0.0f) {
        *trapTimer18 -= GetFrameTime();

        const char *txt = "PIEGE ! Retour au DEPART !";
        int size = 55;

        Vector2 s = MeasureTextEx(font, txt, size, 0);
        float x = (1920 - s.x) / 2;
        float y = 900;

        DrawTextEx(font, txt, (Vector2){ x, y }, size, 0, (Color){ 220, 40, 40, 255 });
        return;
    }

    // À la fin de la pause, on renvoie le joueur à la case départ
    if (!player->isMoving) {
        player->cible = 0;  // case départ
        player->isMoving = true;
        *trapActive18 = false;
    }

    // Pendant le mouvement, on garde le message à l'écran
    const char *txt = "PIEGE ! Retour au DEPART !";
    int size = 55;

    Vector2 s = MeasureTextEx(font, txt, size, 0);
    float x = (1920 - s.x) / 2;
    float y = 900;

    DrawTextEx(font, txt, (Vector2){ x, y }, size, 0, (Color){ 220, 40, 40, 255 });
}
