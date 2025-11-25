#ifndef SUDOKU_H
#define SUDOKU_H

#include "raylib.h"
#include <stdbool.h>

// Lance le Sudoku (ou le reprend)
// Retourne true si terminé, false si quitté (ESC)
bool StartSudoku(Font font);

// Reset complet (nouvelle grille)
void SudokuReset(void);

// Indique si le Sudoku est terminé
bool SudokuIsCompleted(void);
// Choisir le niveau de difficulté : 1 = facile, 2 = moyen, 3 = difficile
void SudokuSetLevel(int level);

#endif
