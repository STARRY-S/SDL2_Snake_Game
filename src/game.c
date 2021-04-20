#include <stdio.h>
#include <SDL.h>
#include <math.h>

#include "game.h"
#include "controller.h"
#include "renderer.h"

/*
 * Create snake game..
 */
struct snake *create_snake_game(void)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
                fprintf(stderr, "SDL INIT ERROR: %s\n", SDL_GetError());
                return NULL;
        }

        struct snake *game = NULL;
        if ((game = (struct snake *) malloc(sizeof(struct snake))) == NULL) {
                fprintf(stderr, "Malloc error.\n");
                return NULL;
        }
	bzero(game, sizeof(struct snake));

        return game;
}

/*
 * Initialize game.
 * Create window and renderer.
 * generate food and snake's head.
 */
int init_game(struct snake *game, int argc, char **argv)
{
	game->speed = 5;
	if (game->screen_width == 0) {
		game->screen_width = WINDOW_WIDTH;
		game->screen_height = WINDOW_HEIGHT;
	}
        game->grid_width = 25;
	if (game->screen_width % game->grid_width != 0) {
		game->grid_width++;
	}
        game->grid_height = game->grid_width;
	game->size = 1;
        game->body =
		(struct snake_point*) malloc(sizeof(struct snake_point) * 512);
        if (game->body == NULL) {
                fprintf(stderr, "Malloc error.\n");
        }
        game->alive = true;
	game->isrunning = true;
	game->isgrowing = false;
	game->direction = SDIR_LEFT;

	if (game->fps == 0) {
		game->fps = FRAMERATE;
	}

	game->window = SDL_CreateWindow(
		"",
                SDL_WINDOWPOS_CENTERED,
                SDL_WINDOWPOS_CENTERED,
                game->screen_width,
                game->screen_height,
                0
        );

	char title[64];
	sprintf(title, "%dfps -- Score: %d -- Snake",
		game->fps, game->size);
	SDL_SetWindowTitle(game->window, title);

        if (game->window == NULL) {
                fprintf(stderr, "Failed to create window: %s\n",
			SDL_GetError());
        }

	game->renderer = SDL_CreateRenderer(
		game->window, -1, SDL_RENDERER_ACCELERATED);
	if (game->renderer == NULL)
	{
		fprintf(stderr, "Render error: %s\n", SDL_GetError());
		return 0;
	}

	SDL_SetWindowResizable(game->window, SDL_FALSE);

	/*
	 * Generate head first, then generate food,
	 * avoid head is generated in food.
	 */
	generate_head(game);
	generate_food(game);

	return 0;
}

/*
 * Main loop.
 * Calcuate game FPS.
 * handle input and render game.
 */
int snake_game_run(struct snake *game)
{
	float ms_per_frame = 1000.0f / game->fps;
	Uint32 ms_per_move = 200;

	Uint32 frame_start;
	Uint32 frame_end;
	Uint32 frame_duration;
	Uint32 frame_time_stamp;
	Uint32 move_time_stamp;
	Uint32 frame_count = 0;

	move_time_stamp = frame_time_stamp = SDL_GetTicks();
        while (game->isrunning)
        {
		frame_start = SDL_GetTicks();

                handle_input(game);
		render_game(game);

		if (frame_start - move_time_stamp
				> ms_per_move - game->speed)
		{
			update_snake_game(game);
			move_time_stamp = frame_start;
		}

		frame_end = SDL_GetTicks();
		frame_count++;
		frame_duration = frame_end - frame_start;

		if (frame_end - frame_time_stamp >= 1000) {
			char title[64];
			sprintf(title, "%dfps -- Score: %d -- Snake",
				frame_count, game->size);
			SDL_SetWindowTitle(game->window, title);
			frame_count = 0;
			frame_time_stamp = frame_end;
		}

		/* If the frame time is smaller than ms per frame, delay */
		if (frame_duration < ms_per_frame) {
			float delay_time = ms_per_frame - frame_duration;
			float floor_time = floor(delay_time);
			if (delay_time - floor_time >= 0.5f)
				floor_time += 1.0;
			SDL_Delay(floor_time);
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

	move_snake_body(game);

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
			printf("======== Game over ========\n");
			printf("Score: %d.\n", game->size);
			printf("Press 'ENTER' to restart.\n");
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
			game->speed += 5;
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
	int limit_x = game->grid_width;
	int limit_y = game->grid_height;
	bool regen;

	do {
		game->food.x = rand() % limit_x;
		game->food.y = rand() % limit_y;

		regen = false;
		for (int i = 0; i < game->size; i++)
		{
			if (game->food.x == game->body[i].x
			    && game->food.y == game->body[i].y)
			{
				regen = true;
			}
		}
	} while (regen);
	// printf("generate food at (%d, %d)\n", game->food.x, game->food.y);
	return 0;
}

int generate_head(struct snake *game)
{
	int limit_x = game->grid_width;
	int limit_y = game->grid_height;
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
        return 0;
}
