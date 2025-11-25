// player.h
#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

typedef struct {
    int pos;
    int cible;
    bool isMoving;
    float animTimer;
} Player;

#endif
