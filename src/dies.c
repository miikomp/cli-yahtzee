#include "yahtzee.h"

#include <string.h>

#define DICE_ROW_WIDTH 70

static const char *dieFaceLine(const DiceView dices, const size_t die,
                               const int row)
{
  if (dices.values[die] == 0)
    return DIE_FACE_EMPTY[row];

  if (dices.locked[die])
    return DIE_FACES_LOCKED[dices.values[die] - 1][row];

  return DIE_FACES[dices.values[die] - 1][row];
}

static void drawHint(const char *hint)
{
  const int padding = (DICE_ROW_WIDTH - (int)strlen(hint)) / 2;

  printf("%*s%s\n", padding > 0 ? padding : 0, "", hint);
}

void drawDies(const DiceView dices, const char *player_name,
              const char *winner_name, const int winning_score)
{
  printf("\x1b[H\x1b[2K");
  printf("%s's turn  Throws left: %d\n\n", player_name,
         MAX_THROWS - dices.throw_number);

  /* Loop over rows in dice face */
  for (int r = 0; r < DIE_HEIGHT; r++)
    {
      /* Loop over dices to draw */
      for (size_t d = 0; d < dices.n_values; d++)
        {
          if (d == dices.selected_idx && r == 0)
            printf("%s ", SELECTED_TOP);
          else if (d == dices.selected_idx && r == DIE_HEIGHT - 1)
            printf("%s ", SELECTED_BOTTOM);
          else
            {
              printf("%s ", dieFaceLine(dices, d, r));
            }
        }
      putchar('\n');
    }

  printf("%*s\r", DICE_ROW_WIDTH, "");
  if (winner_name != NULL)
    {
      char winner_hint[80];

      snprintf(winner_hint, sizeof(winner_hint), "%s won with %d points!",
               winner_name, winning_score);
      drawHint(winner_hint);
    }
  else if (dices.throw_number == 0)
    drawHint("Press SPACE to roll");
  else if (dices.throw_number == MAX_THROWS)
    drawHint("Press TAB to select a score");

  fflush(stdout);
}

void rollDies(int *values, int *locked, size_t n_values)
{
  for (size_t d = 0; d < n_values; d++)
    if (!locked[d])
      values[d] = rand() % 6 + 1;
}

void calculateDiceScores(const int *values, int scores[SCORE_CATEGORY_COUNT])
{
  int counts[7] = { 0 };
  int total = 0;

  for (size_t d = 0; d < 5; d++)
    {
      counts[values[d]]++;
      total += values[d];
    }

  for (int value = 1; value <= 6; value++)
    scores[value - 1] = value * counts[value];

  scores[SCORE_THREE_OF_A_KIND] = 0;
  scores[SCORE_FOUR_OF_A_KIND] = 0;
  scores[SCORE_YAHTZEE] = 0;
  for (int value = 1; value <= 6; value++)
    {
      if (counts[value] >= 3)
        scores[SCORE_THREE_OF_A_KIND] = total;
      if (counts[value] >= 4)
        scores[SCORE_FOUR_OF_A_KIND] = total;
      if (counts[value] == 5)
        scores[SCORE_YAHTZEE] = 50;
    }

  scores[SCORE_FULL_HOUSE] = 0;
  for (int first = 1; first <= 6; first++)
    for (int second = 1; second <= 6; second++)
      if (counts[first] == 3 && counts[second] == 2)
        scores[SCORE_FULL_HOUSE] = 25;

  scores[SCORE_SMALL_STRAIGHT] =
    (counts[1] && counts[2] && counts[3] && counts[4]) ||
        (counts[2] && counts[3] && counts[4] && counts[5]) ||
        (counts[3] && counts[4] && counts[5] && counts[6]) ?
      30 :
      0;
  scores[SCORE_LARGE_STRAIGHT] =
    (counts[1] && counts[2] && counts[3] && counts[4] && counts[5]) ||
        (counts[2] && counts[3] && counts[4] && counts[5] && counts[6]) ?
      40 :
      0;
  scores[SCORE_CHANCE] = total;
}