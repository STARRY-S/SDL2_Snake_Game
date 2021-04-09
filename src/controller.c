#include <SDL.h>
#include <stdlib.h>
#include <stdbool.h>

#include "game.h"
#include "controller.h"

int handle_input(struct snake *game)
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			game->isrunning = false;
		}
		if (e.type == SDL_KEYDOWN)
		{
			int key = e.key.keysym.sym;
			switch (key)
			{
			case SDLK_ESCAPE:
			{
				// printf("ESC.\n");
				game->isrunning = false;
				break;
			}
			case SDLK_q:
			{
				// printf("q.\n");
				game->isrunning = false;
				break;
			}
			case SDLK_KP_ENTER:
			case 0x0D:
			{
				if (!game->alive)
				{
					game->alive = true;
					game->size = 1;
					game->speed = 0.1;
				}
				break;
			}
			default:
				change_direction(game, key);
			}
		}
	}
	return 0;
}

int change_direction(struct snake *game, int type)
{
	switch (type)
	{
	case SDLK_w:
	case SDLK_UP:
	{
		// printf("Up.\n");
		if (game->direction != SDIR_DOWN || game->size == 1)
		{
			game->direction = SDIR_UP;
		}
		break;
	}
	case SDLK_s:
	case SDLK_DOWN:
	{
		// printf("Down.\n");
		if (game->direction != SDIR_UP || game->size == 1)
		{
			game->direction = SDIR_DOWN;
		}
		break;
	}
	case SDLK_a:
	case SDLK_LEFT:
	{
		// printf("LEFT.\n");
		if (game->direction != SDIR_RIGHT || game->size == 1)
		{
			game->direction = SDIR_LEFT;
		}
		break;
	}
	case SDLK_d:
	case SDLK_RIGHT:
	{
		// printf("RIGHT.\n");
		if (game->direction != SDIR_LEFT || game->size == 1)
		{
			game->direction = SDIR_RIGHT;
		}
		break;
	}
	default:
		break;
	};

	return 0;
}
