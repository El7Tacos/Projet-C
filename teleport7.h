#ifndef TELEPORT7_H
#define TELEPORT7_H

#include "raylib.h"
#include "player.h"

void HandleTeleport7(Player *player, int totalCases, Font font,
                     bool *tpActive, float *tpTimer, bool *tpInvisible);

#endif
