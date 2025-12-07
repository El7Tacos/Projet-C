#include "five.h"
#include "raylib.h"

// Case 5 : retour au départ avec petite pause et message
void HandleCaseFive(Player *player, int totalCases, Font font,
                    bool *trapActive5, float *trapTimer5)
{
    const float pauseTime = 1.2f;

    // Première fois sur la case : on lance la pause
    if (!(*trapActive5)) {
        *trapActive5 = true;
        *trapTimer5 = pauseTime;
        return;
    }

    // Tant que la pause n'est pas finie, on affiche le message et le pion ne bouge pas
    if (*trapTimer5 > 0.0f) {
        *trapTimer5 -= GetFrameTime();

        const char *txt = "MALUS ! Retour au DEPART !";
        int size = 55;

        Vector2 s = MeasureTextEx(font, txt, size, 0);
        float x = (1920 - s.x) / 2;
        float y = 900;

        DrawTextEx(font, txt, (Vector2){ x, y }, size, 0,
                   (Color){ 200, 0, 0, 255 });  // rouge malus

        return;  // on attend la fin du timer
    }

    // Timer écoulé : on renvoie le joueur au départ (une seule fois)
    if (!player->isMoving) {
        player->cible = 0;
        player->isMoving = true;

        *trapActive5 = false;  // piège terminé
    }

    // Pendant le retour, on garde le message affiché
    const char *txt = "MALUS ! Retour au DEPART !";
    int size = 55;

    Vector2 s = MeasureTextEx(font, txt, size, 0);
    float x = (1920 - s.x) / 2;
    float y = 900;

    DrawTextEx(font, txt, (Vector2){ x, y }, size, 0,
               (Color){ 200, 0, 0, 255 });
}
