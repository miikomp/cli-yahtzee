#ifndef SCORETABLE_H
#define SCORETABLE_H

#include <stddef.h>

#define SCORE_EMPTY -1

enum ScoreCategory {
  SCORE_ONES,
  SCORE_TWOS,
  SCORE_THREES,
  SCORE_FOURS,
  SCORE_FIVES,
  SCORE_SIXES,
  SCORE_THREE_OF_A_KIND,
  SCORE_FOUR_OF_A_KIND,
  SCORE_FULL_HOUSE,
  SCORE_SMALL_STRAIGHT,
  SCORE_LARGE_STRAIGHT,
  SCORE_YAHTZEE,
  SCORE_CHANCE,
  SCORE_CATEGORY_COUNT,
};

typedef struct {
  size_t n_players;
  int (*values)[SCORE_CATEGORY_COUNT];
} ScoreTable;

void drawScoreTable(const ScoreTable scores, const char *const player_names[],
                    size_t current_player, size_t selected_category,
                    const int possible_scores[SCORE_CATEGORY_COUNT],
                    int selected);
int initializeScoreTable(ScoreTable *scores, size_t n_players);
void resetScoreTable(ScoreTable *scores);
int scoreTableComplete(const ScoreTable scores);
size_t winningPlayer(const ScoreTable scores);
int playerScoreTotal(const ScoreTable scores, size_t player);

#endif // SCORETABLE_H
