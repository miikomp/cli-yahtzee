#ifndef DIES_H
#define DIES_H

#include <stddef.h>

#include "scoretable.h"

#define MAX_THROWS 3

typedef struct {
  int values[5];
  size_t n_values;
  int locked[5];
  size_t selected_idx;
  size_t current_player;
  int throw_number;
} DiceView;

void drawDies(const DiceView dices, const char *player_name,
              const char *winner_name, int winning_score);
void rollDies(int *values, int *locked, size_t n_values);
void calculateDiceScores(const int *values, int scores[SCORE_CATEGORY_COUNT]);

#endif // DIES_H