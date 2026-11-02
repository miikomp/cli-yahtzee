#include "yahtzee.h"

enum UiFocus {
  UI_FOCUS_DICES,
  UI_FOCUS_SCORECARD,
};

static const char *const DEFAULT_PLAYER_NAMES[] = {
  "Player 1",
  "Player 2",
};

static void moveScoreSelection(const ScoreTable scores, const size_t player,
                               size_t *selected_category, const int direction)
{
  for (size_t count = 0; count < SCORE_CATEGORY_COUNT; count++)
    {
      if (direction > 0)
        *selected_category = (*selected_category + 1) % SCORE_CATEGORY_COUNT;
      else
        *selected_category = (*selected_category + SCORE_CATEGORY_COUNT - 1) %
                             SCORE_CATEGORY_COUNT;

      if (scores.values[player][*selected_category] == SCORE_EMPTY)
        return;
    }
}

static size_t firstUnscoredCategory(const ScoreTable scores,
                                    const size_t player)
{
  for (size_t category = 0; category < SCORE_CATEGORY_COUNT; category++)
    if (scores.values[player][category] == SCORE_EMPTY)
      return category;

  return 0;
}

static void drawGame(const DiceView dices, const ScoreTable scores,
                     const char *const player_names[],
                     const size_t selected_category, const enum UiFocus focus,
                     const int possible_scores[SCORE_CATEGORY_COUNT])
{
  const int complete = scoreTableComplete(scores);
  const size_t winner = complete ? winningPlayer(scores) : 0;

  drawDies(dices, player_names[dices.current_player],
           complete ? player_names[winner] : NULL,
           complete ? playerScoreTotal(scores, winner) : 0);
  drawScoreTable(scores, player_names, dices.current_player, selected_category,
                 dices.throw_number > 0 ? possible_scores : NULL,
                 focus == UI_FOCUS_SCORECARD);
}

int main(int argc, char **argv)
{
  DiceView dices = { .values = { 0 },
                     .n_values = 5,
                     .locked = { 0 },
                     .selected_idx = 0,
                     .current_player = 0,
                     .throw_number = 0 };
  for (int i = 0; i < argc; i++)
    {
      if (!strcasecmp("--help", argv[i]))
        {
          printf("CLI Yahtzee:\n");
          printf("  Usage %s [PLAYER] ...\n\n", argv[0]);
          return EXIT_SUCCESS;
        }
      else if (*argv[i] == '-')
        {
          printf("Unknown argument: \"%s\". Try --help\n", argv[i]);
          return EXIT_FAILURE;
        }
    }

  ScoreTable scores;
  const size_t n_players = argc > 1 ? (size_t)(argc - 1) : 2;
  const char **player_names = malloc(n_players * sizeof(*player_names));
  int possible_scores[SCORE_CATEGORY_COUNT];
  enum UiFocus focus = UI_FOCUS_DICES;
  size_t selected_category = SCORE_ONES;

  if (player_names == NULL)
    {
      fputs("Unable to allocate player names.\n", stderr);
      return EXIT_FAILURE;
    }

  for (size_t p = 0; p < n_players; p++)
    player_names[p] = argc > 1 ? argv[p + 1] : DEFAULT_PLAYER_NAMES[p];

  srand(time(NULL));
  if (!initializeScoreTable(&scores, n_players))
    {
      fputs("Unable to allocate score table.\n", stderr);
      free(player_names);
      return EXIT_FAILURE;
    }
  setupTerminal();

  drawGame(dices, scores, player_names, selected_category, focus,
           possible_scores);

  while (!shouldQuit())
    {
      if (scoreTableComplete(scores))
        {
          readInput();
          continue;
        }

      /* Handle input */
      switch (readInput())
        {
        case INPUT_LEFT:
          if (focus == UI_FOCUS_DICES)
            {
              if (dices.selected_idx > 0)
                dices.selected_idx--;
              else
                dices.selected_idx = dices.n_values - 1;
              playSound(SOUND_CLICK);
            }
          break;

        case INPUT_RIGHT:
          if (focus == UI_FOCUS_DICES)
            {
              if (dices.selected_idx + 1 < dices.n_values)
                dices.selected_idx++;
              else
                dices.selected_idx = 0;
              playSound(SOUND_CLICK);
            }
          break;

        case INPUT_UP:
          if (focus == UI_FOCUS_SCORECARD)
            {
              moveScoreSelection(scores, dices.current_player,
                                 &selected_category, -1);
              playSound(SOUND_CLICK);
            }
          break;

        case INPUT_DOWN:
          if (focus == UI_FOCUS_SCORECARD)
            {
              moveScoreSelection(scores, dices.current_player,
                                 &selected_category, 1);
              playSound(SOUND_CLICK);
            }
          break;

        case INPUT_TOGGLE_FOCUS:
          if (dices.throw_number > 0)
            focus =
              focus == UI_FOCUS_DICES ? UI_FOCUS_SCORECARD : UI_FOCUS_DICES;
          break;

        case INPUT_CONFIRM:
          if (focus == UI_FOCUS_DICES && dices.throw_number > 0)
            {
              dices.locked[dices.selected_idx] =
                !dices.locked[dices.selected_idx];
              playSound(SOUND_CLUNK);
            }
          else if (focus == UI_FOCUS_SCORECARD)
            {
              const size_t player = dices.current_player;

              if (scores.values[player][selected_category] == SCORE_EMPTY)
                {
                  scores.values[player][selected_category] =
                    possible_scores[selected_category];
                  for (size_t d = 0; d < dices.n_values; d++)
                    {
                      dices.values[d] = 0;
                      dices.locked[d] = 0;
                    }
                  dices.current_player =
                    (dices.current_player + 1) % scores.n_players;
                  dices.throw_number = 0;
                  selected_category =
                    firstUnscoredCategory(scores, dices.current_player);
                  focus = UI_FOCUS_DICES;
                  playSound(SOUND_CLUNK);
                }
            }
          break;

        case INPUT_ROLL:
          if (focus == UI_FOCUS_DICES && dices.throw_number < MAX_THROWS)
            {
              rollDies(dices.values, dices.locked, dices.n_values);
              calculateDiceScores(dices.values, possible_scores);
              dices.throw_number++;
              playSound(SOUND_ROLL);
            }
          break;

        case INPUT_NONE:
          continue;
        }

      drawGame(dices, scores, player_names, selected_category, focus,
               possible_scores);
    }

  resetTerminal();
  resetScoreTable(&scores);
  free(player_names);
  return 0;
}