#include "nine.h"
#include "raylib.h"

// Fonction appelée quand on arrive sur la case 5
void HandleCaseNine(Player *player, int totalCases, Font font, bool *trapActive, float *trapTimer)
{
    // Durée de la pause avant le recul
    const float pauseBeforeMove = 1.2f;   // lisible mais pas trop long

    // 🔥 1) Activation initiale du piège — on PAUSE et on NE BOUGE PAS
    if (!(*trapActive)) {
        *trapActive = true;
        *trapTimer = pauseBeforeMove;     // compte à rebours avant de reculer
        return;                            // stop ici → pas de mouvement maintenant
    }

    // 🔥 2) Tant que la pause n’est PAS finie → on affiche juste le message
    if (*trapTimer > 0.0f) {
        *trapTimer -= GetFrameTime();

        // Message géant et centré
        int msgSize = 55;
        const char *txt = "PIEGE ! Tu recules de 6 cases !";

        // Centrage horizontal
        Vector2 txtSize = MeasureTextEx(font, txt, msgSize, 0);
        float msgX = (1920 - txtSize.x) / 2;
        float msgY = 900; // bas de l'écran comme tu voulais

        DrawTextEx(font, txt, (Vector2){msgX, msgY}, msgSize, 0,
                   (Color){0,120,0,255});  // VERT SAPIN STYLE DEPART

        return; // toujours aucune mise en mouvement
    }

    // 🔥 3) La pause est terminée → lancer le recul d'une case
    if (!player->isMoving) {

        int recul = 6;

        if (player->pos - recul >= 0)
            player->cible = player->pos - recul;
        else
            player->cible = 0;  // jamais sous zéro
        

        player->isMoving = true;  // maintenant on bouge
        *trapActive = false;      // piège terminé
    }

    // 🔥 4) Affichage du message pendant qu'il commence à reculer
    int msgSize = 55;
    const char *txt = "PIEGE ! Tu recules de 6 cases !";
    Vector2 txtSize = MeasureTextEx(font, txt, msgSize, 0);
    float msgX = (1920 - txtSize.x) / 2;
    float msgY = 900;

    DrawTextEx(font, txt, (Vector2){msgX, msgY}, msgSize, 0,
               (Color){0,120,0,255});
}
