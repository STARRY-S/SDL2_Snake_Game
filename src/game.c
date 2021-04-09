#include <stdio.h>
#include <SDL.h>
#include <math.h>

#include "game.h"
#include "controller.h"
#include "renderer.h"

/*
 * Create snake game.
 * create SDL Window and SDL Renderer.
 */
struct snake *create_snake_game(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
                fprintf(stderr, "SDL INIT ERROR: %s\n", SDL_GetError());
                return NULL;
        }

        struct snake *game = NULL;
        if ((game = malloc(sizeof(struct snake))) == NULL) {
                fprintf(stderr, "Malloc error.\n");
                return NULL;
        }

        game->window = SDL_CreateWindow(
		"Snake",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                WINDOW_WIDTH,
                WINDOW_HEIGHT,
                0
        );

        if (game->window == NULL) {
                fprintf(stderr, "Failed to create window: %s\n",
			SDL_GetError());
        }

	game->renderer = SDL_CreateRenderer(
		game->window, -1, SDL_RENDERER_ACCELERATED);
	if (game->renderer == NULL)
	{
		fprintf(stderr, "SDL Render Create Failed: %s\n", SDL_GetError());
		return 0;
	}

	SDL_SetWindowResizable(game->window, SDL_FALSE);

        return game;
}

/*
 * Initialize game.
 * generate food and snake's head.
 */
int init_game(struct snake *game, int argc, char **argv)
{
	game->speed = 0.1;
	game->move_count = 0.0;
	game->screen_width = WINDOW_WIDTH;
	game->screen_height = WINDOW_HEIGHT;
        game->grid_width = 25;
        game->grid_height = 25;
	game->size = 1;
        game->body = malloc(sizeof(struct snake_point) * 512);
        if (game->body == NULL) {
                fprintf(stderr, "Malloc error.\n");
        }
        game->alive = true;
	game->isrunning = true;
	game->isgrowing = false;
	game->direction = SDIR_LEFT;

	/*
	 * Generate head first, then generate food,
	 * to avoid head was generated in food.
	 */
	generate_head(game);
	generate_food(game);

	// For debug
	// for (int i = 1; i < game->size; i++)
	// {
	// 	game->body[i].x = game->body[0].x + i;
	// 	game->body[i].y = game->body[0].y;
	// }

	return 0;
}

/*
 * Main loop.
 * Calcuate game FPS.
 * handle input and render game.
 */
int snake_game_run(struct snake *game)
{
	float ms_per_frame = 1000.0f / FRAMERATE;

	Uint32 frame_start;
	Uint32 frame_end;
	Uint32 frame_duration;
	Uint32 timestamp;
	Uint32 frame_count = 0;

	timestamp = SDL_GetTicks();
        while (game->isrunning)
        {
		frame_start = SDL_GetTicks();

                handle_input(game);
		render_game(game);
		update_snake_game(game);

		frame_end = SDL_GetTicks();
		frame_count++;
		frame_duration = frame_end - frame_start;

		// Update title every second.
		if (frame_end - timestamp >= 1000) {
			// TODO: update title.
			frame_count = 0;
			timestamp = frame_end;
		}

		/* If the frame time is smaller than ms per frame, delay */
		if (frame_duration < ms_per_frame) {
			SDL_Delay(ms_per_frame - frame_duration);
		}
        }

	return 0;
}

/*
 * Update snake, move snake head and body,
 * generate food, increase its size
 * move snake head and body.
 */
int update_snake_game(struct snake *game)
{
	if (!game->alive)
		return 0;

	game->move_count += game->speed;
	if (game->move_count >= 1.0)
	{
		move_snake_body(game);
		game->move_count = 0;
	}

	/* Check eat food */
	if (game->body[0].x == game->food.x && game->body[0].y == game->food.y)
	{
		// printf("Eat food.\n");
		game->isgrowing = true;
		generate_food(game);
	}

	/* Check if head is in body */
	for (int i = 1; game->alive && i < game->size; i++)
	{
		if (game->body[0].x == game->body[i].x
			&& game->body[0].y == game->body[i].y)
		{
			game->alive = false;
			printf(" ======== Game over ========\n");
			printf(" Score: %d.\n", game->size);
			printf(" Press 'ENTER' to restart.\n");
		}
	}

	return 0;
}

int move_snake_body(struct snake *game)
{
	/*
	 * If the snake's lenth is 512, stop growing.
	 * (nobody can achieve this because the largest length is 400)
	 */
	if (game->isgrowing && game->size < 512)
	{
		++game->size;
		game->isgrowing = false;

		if (game->size % 5 == 0)
		{
			game->speed += 0.005;
		}
	}

	// example: right direction
	// head  body1 body2 body3 body4
	// (3,0) (2,0) (1,0) (1,1) (1,2)
	// (4,0) (3,0) (2,0) (1,0) (1,1)

	for (int i = game->size-1; i > 0; i--)
	{
		game->body[i] = game->body[i-1];
	}

	/* Move snake's head */
	switch (game->direction)
	{
	case SDIR_DOWN:
	{
		game->body[0].y += 1;
		break;
	}
	case SDIR_UP:
	{
		game->body[0].y -= 1;
		break;
	}
	case SDIR_RIGHT:
	{
		game->body[0].x += 1;
 		break;
	}
	case SDIR_LEFT:
	{
		game->body[0].x -= 1;
		break;
	}
	default:
		break;
	}

	/* Make sure game's head is in screen. */
	if (game->body[0].x < 0)
	{
		game->body[0].x = game->grid_width-1;
	}

	if (game->body[0].x > game->grid_width-1)
	{
		game->body[0].x = 0;
	}

	if (game->body[0].y < 0)
	{
		game->body[0].y = game->grid_width-1;
	}
	if (game->body[0].y > game->grid_width-1)
	{
		game->body[0].y = 0;
	}

	return 0;
}

int generate_food(struct snake *game)
{
	int limit_x = game->screen_width / game->grid_width;
	int limit_y = game->screen_height / game->grid_height;
	bool regen;

	do {
		game->food.x = rand() % limit_x;
		game->food.y = rand() % limit_y;

		regen = false;
		for (int i = 0; i < game->size; i++)
		{
			if (abs(game->food.x - game->body[i].x) < 1.0
				&& abs(game->food.y - game->body[i].y) < 1.0)
			{
				regen = true;
			}
		}
	} while (regen);
	// printf("generate food at (%d, %d)\n", game->food_x, game->food_y);
	return 0;
}

int generate_head(struct snake *game)
{
	int limit_x = game->screen_width / game->grid_width;
	int limit_y = game->screen_height / game->grid_height;
	game->body[0].x = SDL_GetTicks() % limit_x;
	game->body[0].y = SDL_GetTicks() % limit_y;
	return 0;
}

/*
 * Destroy window and renderer.
 * Free memory.
 */
int destroy_snake_game(struct snake *game)
{
        if (game == NULL)
                return 0;

        SDL_DestroyWindow(game->window);
	SDL_DestroyRenderer(game->renderer);
        free(game->body);
        free(game);
	printf("Bye.\n");
        return 0;
}
