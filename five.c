#include "five.h"
#include "raylib.h"

// Case 5 : retour au départ avec pause + message rouge
void HandleCaseFive(Player *player, int totalCases, Font font,
                    bool *trapActive5, float *trapTimer5)
{
    const float pauseTime = 1.2f;

    // 1) Première entrée dans le piège → juste lancer la pause
    if (!(*trapActive5)) {
        *trapActive5 = true;
        *trapTimer5 = pauseTime;
        return;
    }

    // 2) Tant que la pause n’est pas finie → afficher le message et stopper le mouvement
    if (*trapTimer5 > 0.0f) {
        *trapTimer5 -= GetFrameTime();

        const char *txt = "MALUS ! Retour au DEPART !";
        int size = 55;

        Vector2 s = MeasureTextEx(font, txt, size, 0);
        float x = (1920 - s.x) / 2;
        float y = 900;

        DrawTextEx(font, txt, (Vector2){x, y}, size, 0,
                   (Color){200,0,0,255}); // rouge malus

        return;  // on attend la fin du timer
    }

    // 3) Timer écoulé → on lance le retour (une seule fois)
    if (!player->isMoving) {
        player->cible = 0;
        player->isMoving = true;

        *trapActive5 = false; // piège fini
    }

    // 4) Pendant le retour, continuer d'afficher
    const char *txt = "MALUS ! Retour au DEPART !";
    int size = 55;
    Vector2 s = MeasureTextEx(font, txt, size, 0);
    float x = (1920 - s.x) / 2;
    float y = 900;

    DrawTextEx(font, txt, (Vector2){x, y}, size, 0,
               (Color){200,0,0,255});
}
