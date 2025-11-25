#ifndef FIVE_H
#define FIVE_H

#include "raylib.h"
#include "player.h"

// Case 5 : retour direct à la case départ (0) avec message + pause
void HandleCaseFive(Player *player, int totalCases, Font font,
                    bool *trapActive5, float *trapTimer5);

#endif
