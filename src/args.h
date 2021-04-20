#ifndef __ARGS_H__
#define __ARGS_H__

#include "game.h"

int handle_args(int argc, char **argv, struct snake *game);
int getarg(int argc, char **argv, const char *const opt_string);
void print_help(char *name);

#endif
