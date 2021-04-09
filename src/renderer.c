#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "game.h"
#include "renderer.h"

int render_game(struct snake *game)
{
	SDL_Rect block;
	block.w = game->screen_width / game->grid_width;
	block.h = game->screen_height / game->grid_height;

	// background color
	SDL_SetRenderDrawColor(game->renderer, 0x20, 0x20, 0x20, 0xff);
	SDL_RenderClear(game->renderer);

	// food
	block.x = game->food.x * block.w;
	block.y = game->food.y * block.h;
	SDL_SetRenderDrawColor(game->renderer, 0xaf, 0x20, 0xaf, 0xff);
	SDL_RenderFillRect(game->renderer, &block);

	/* Render body first */
	for (int i = 1; i < game->size; i++)
	{
		block.x = game->body[i].x * block.w;
		block.y = game->body[i].y * block.h;
		SDL_SetRenderDrawColor(game->renderer, 0xee, 0xee, 0xee, 0xff);
		SDL_RenderFillRect(game->renderer, &block);
	}

	/* Render head at last */
	block.x = game->body[0].x * block.w;
	block.y = game->body[0].y * block.h;
	if (game->alive) {
		SDL_SetRenderDrawColor(game->renderer, 0x66, 0x66, 0xee, 0xff);
	} else {
		SDL_SetRenderDrawColor(game->renderer, 0xff, 0x22, 0x22, 0xff);
	}

	SDL_RenderFillRect(game->renderer, &block);
	SDL_RenderPresent(game->renderer);

	return 0;
}
