#include "yahtzee.h"

#include <stdlib.h>

#define SCORE_TABLE_COLUMN 75
#define UPPER_CATEGORY_COUNT 6
#define UPPER_BONUS_THRESHOLD 63
#define UPPER_BONUS 35

static const char *const SCORE_LABELS[SCORE_CATEGORY_COUNT] = {
  "Ones",       "Twos",           "Threes",          "Fours",
  "Fives",      "Sixes",          "Three of a Kind", "Four of a Kind",
  "Full House", "Small Straight", "Large Straight",  "Yahtzee",
  "Chance",
};

static void scoreNewLine(void)
{
  printf("\n\x1b[%dG", SCORE_TABLE_COLUMN);
}

static void drawScoreDivider(const size_t n_players, const char *left,
                             const char *join, const char *right)
{
  printf("%s──────────────────", left);
  for (size_t p = 0; p < n_players; p++)
    printf("%s──────────", join);
  printf("%s", right);
  scoreNewLine();
}

static int upperScoreTotal(const ScoreTable scores, const size_t player)
{
  int total = 0;
  int has_scores = 0;

  for (size_t category = SCORE_ONES; category < UPPER_CATEGORY_COUNT;
       category++)
    if (scores.values[player][category] != SCORE_EMPTY)
      {
        total += scores.values[player][category];
        has_scores = 1;
      }

  return has_scores ? total : SCORE_EMPTY;
}

static int upperBonus(const ScoreTable scores, const size_t player)
{
  const int total = upperScoreTotal(scores, player);

  for (size_t category = SCORE_ONES; category < UPPER_CATEGORY_COUNT;
       category++)
    if (scores.values[player][category] == SCORE_EMPTY)
      return SCORE_EMPTY;

  return total >= UPPER_BONUS_THRESHOLD ? UPPER_BONUS : 0;
}

static int scoreTotal(const ScoreTable scores, const size_t player)
{
  int total = 0;
  int has_scores = 0;
  const int bonus = upperBonus(scores, player);

  for (size_t category = 0; category < SCORE_CATEGORY_COUNT; category++)
    if (scores.values[player][category] != SCORE_EMPTY)
      {
        total += scores.values[player][category];
        has_scores = 1;
      }

  if (bonus != SCORE_EMPTY)
    total += bonus;

  return has_scores ? total : SCORE_EMPTY;
}

int playerScoreTotal(const ScoreTable scores, const size_t player)
{
  return scoreTotal(scores, player);
}

int scoreTableComplete(const ScoreTable scores)
{
  for (size_t p = 0; p < scores.n_players; p++)
    for (size_t category = 0; category < SCORE_CATEGORY_COUNT; category++)
      if (scores.values[p][category] == SCORE_EMPTY)
        return 0;

  return 1;
}

size_t winningPlayer(const ScoreTable scores)
{
  size_t winner = 0;
  int highest_score = scoreTotal(scores, winner);

  for (size_t p = 1; p < scores.n_players; p++)
    if (scoreTotal(scores, p) > highest_score)
      {
        winner = p;
        highest_score = scoreTotal(scores, p);
      }

  return winner;
}

static void drawScoreCell(const int score, const int possible_score,
                          const int possible, const int selected)
{
  if (score != SCORE_EMPTY)
    printf("│\x1b[1m %8d \x1b[0m", score);
  else if (selected)
    printf(possible_score == 0 ? "│\x1b[31;7m %8d \x1b[0m" :
                                 "│\x1b[7m %8d \x1b[0m",
           possible_score);
  else if (possible && possible_score > 0)
    printf("│\x1b[32m %8d \x1b[0m", possible_score);
  else if (possible)
    printf("│ %8d ", possible_score);
  else
    printf("│          ");
}

static void drawSummaryCell(const int value)
{
  if (value == SCORE_EMPTY)
    printf("│          ");
  else
    printf("│ %8d ", value);
}

void drawScoreTable(const ScoreTable scores, const char *const player_names[],
                    const size_t current_player, const size_t selected_category,
                    const int possible_scores[SCORE_CATEGORY_COUNT],
                    const int selected)
{
  printf("\x1b[1;%dHScorecard", SCORE_TABLE_COLUMN);
  printf("\x1b[2;%dH", SCORE_TABLE_COLUMN);
  drawScoreDivider(scores.n_players, "┌", "┬", "┐");

  printf("│ %-17s", "Score");
  for (size_t p = 0; p < scores.n_players; p++)
    printf("│ %-9.9s", player_names[p]);
  printf("│");
  scoreNewLine();
  drawScoreDivider(scores.n_players, "├", "┼", "┤");

  for (size_t category = 0; category < SCORE_CATEGORY_COUNT; category++)
    {
      if (category == SCORE_THREE_OF_A_KIND)
        {
          drawScoreDivider(scores.n_players, "├", "┼", "┤");

          printf("│ %-17s", "Sum");
          for (size_t p = 0; p < scores.n_players; p++)
            drawSummaryCell(upperScoreTotal(scores, p));
          printf("│");
          scoreNewLine();

          printf("│ %-17s", "Bonus");
          for (size_t p = 0; p < scores.n_players; p++)
            drawSummaryCell(upperBonus(scores, p));
          printf("│");
          scoreNewLine();

          drawScoreDivider(scores.n_players, "├", "┼", "┤");
        }

      printf("│ %-17s", SCORE_LABELS[category]);
      for (size_t p = 0; p < scores.n_players; p++)
        drawScoreCell(scores.values[p][category],
                      possible_scores == NULL ? 0 : possible_scores[category],
                      p == current_player && possible_scores != NULL,
                      selected && p == current_player &&
                        category == selected_category);
      printf("│");
      scoreNewLine();
    }

  drawScoreDivider(scores.n_players, "├", "┼", "┤");
  printf("│ %-17s", "Total");
  for (size_t p = 0; p < scores.n_players; p++)
    {
      const int total = scoreTotal(scores, p);

      if (total == SCORE_EMPTY)
        printf("│          ");
      else
        printf("│ %8d ", total);
    }
  printf("│");
  scoreNewLine();
  drawScoreDivider(scores.n_players, "└", "┴", "┘");
  fflush(stdout);
}

int initializeScoreTable(ScoreTable *scores, const size_t n_players)
{
  scores->values = malloc(n_players * sizeof(*scores->values));
  if (scores->values == NULL)
    {
      scores->n_players = 0;
      return 0;
    }

  scores->n_players = n_players;
  for (size_t p = 0; p < scores->n_players; p++)
    for (size_t category = 0; category < SCORE_CATEGORY_COUNT; category++)
      scores->values[p][category] = SCORE_EMPTY;

  return 1;
}

void resetScoreTable(ScoreTable *scores)
{
  free(scores->values);
  scores->values = NULL;
  scores->n_players = 0;
}
