#include "game.h"
#include "args.h"

int main(int argc, char **argv)
{
        struct snake *snake_game = NULL;
        snake_game = create_snake_game();

        if (handle_args(argc, argv, snake_game) != 0) {
                destroy_snake_game(snake_game);
                return 0;
        }

	init_game(snake_game, argc, argv);
        snake_game_run(snake_game);
        destroy_snake_game(snake_game);

        return 0;
}
