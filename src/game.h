#ifndef __GAME_H__
#define __GAME_H__

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <SDL.h>
#include <math.h>

#define WINDOW_WIDTH 500
#define WINDOW_HEIGHT 500
#define FRAMERATE 60

enum SNAKE_DIRECTION { SDIR_UP, SDIR_DOWN, SDIR_LEFT, SDIR_RIGHT };

struct snake_point {
	int x;
	int y;
};

struct snake {
        SDL_Window *window;
	SDL_Renderer *renderer;
	struct snake_point *body;
	struct snake_point food;

	int size;		// body length
	int screen_width;
	int screen_height;
	int fps;
        int grid_width;
        int grid_height;
	int speed;

        bool alive;
	bool isrunning;
	bool isgrowing;
	int direction;
};

struct snake *create_snake_game(void);
int init_game(struct snake *game, int argc, char **argv);
int snake_game_run(struct snake *game);
int update_snake_game(struct snake *game);
int move_snake_body(struct snake *game);
int generate_food(struct snake *game);
int generate_head(struct snake *game);
int destroy_snake_game(struct snake *game);

#endif
