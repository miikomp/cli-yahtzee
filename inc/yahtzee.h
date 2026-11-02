#ifndef YAHTZEE_H
#define YAHTZEE_H

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>

#include "die_faces.h"
#include "scoretable.h"
#include "dies.h"

enum Input {
  INPUT_NONE,
  INPUT_ROLL,
  INPUT_LEFT,
  INPUT_RIGHT,
  INPUT_UP,
  INPUT_DOWN,
  INPUT_TOGGLE_FOCUS,
  INPUT_CONFIRM,
  INPUT_EXIT
};

enum Sound {
  SOUND_CLICK,
  SOUND_CLUNK,
  SOUND_ROLL,
};

enum Input readInput(void);
int shouldQuit(void);
void playSound(enum Sound sound);
void setupTerminal(void);
void resetTerminal(void);

#endif // YAHTZEE_H