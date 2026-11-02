#define _POSIX_C_SOURCE 200809L

#include <signal.h>

#include "yahtzee.h"

static struct termios original_termios;
static volatile sig_atomic_t quit_requested;

static void handleInterrupt(int signal)
{
  (void)signal;
  quit_requested = 1;
}

static void enableInputMode(void)
{
  struct termios mode;

  tcgetattr(STDIN_FILENO, &original_termios);
  mode = original_termios;
  mode.c_lflag &= ~(ICANON | ECHO);
  mode.c_cc[VMIN] = 1;
  mode.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &mode);
}

static void restoreInputMode(void)
{
  tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

static void terminalBell(void)
{
  putchar('\a');
  fflush(stdout);
}

static void soundPause(const long nanoseconds)
{
  const struct timespec duration = {
    .tv_sec = 0,
    .tv_nsec = nanoseconds,
  };

  nanosleep(&duration, NULL);
}

enum Input readInput(void)
{
  unsigned char first;
  unsigned char sequence[2];

  /* Single byte inputs */
  if (read(STDIN_FILENO, &first, 1) != 1)
    return INPUT_NONE;

  if (first == ' ')
    return INPUT_ROLL;

  if (first == '\t')
    return INPUT_TOGGLE_FOCUS;

  if (first == '\n' || first == '\r')
    return INPUT_CONFIRM;

  if (first == 'q')
  {
    quit_requested = 1;
    return INPUT_EXIT;
  }

  if (first != '\x1b')
    return INPUT_NONE;

  /* Multi-byte sequence inputs */
  if (read(STDIN_FILENO, sequence, 2) != 2 || sequence[0] != '[')
    return INPUT_NONE;

  if (sequence[1] == 'D')
    return INPUT_LEFT;

  if (sequence[1] == 'C')
    return INPUT_RIGHT;

  if (sequence[1] == 'A')
    return INPUT_UP;

  if (sequence[1] == 'B')
    return INPUT_DOWN;

  return INPUT_NONE;
}

int shouldQuit(void)
{
  return quit_requested;
}

void playSound(const enum Sound sound)
{
  terminalBell();

  if (sound == SOUND_CLUNK)
    {
      soundPause(70000000);
      terminalBell();
    }
  else if (sound == SOUND_ROLL)
    {
      soundPause(35000000);
      terminalBell();
      soundPause(35000000);
      terminalBell();
    }
}

void setupTerminal(void)
{
  struct sigaction action = {0};

  action.sa_handler = handleInterrupt;
  sigemptyset(&action.sa_mask);
  sigaction(SIGINT, &action, NULL);
  printf("\x1b[?1049h");  // Use secondary screen buffer
  printf("\x1b[2J\x1b[H");  // Clear screen
  printf("\x1b[?25l");  // Hide cursor
  enableInputMode();
  fflush(stdout);
}

void resetTerminal(void)
{
  printf("\x1b[2J\x1b[H");  // Clear the game screen
  printf("\x1b[?25h");  // Show cursor
  printf("\x1b[?1049l");  // Use primary screen buffer
  restoreInputMode();
  fflush(stdout);
}
