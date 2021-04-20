#include "args.h"
#include "game.h"

char *argopt = NULL;

int handle_args(int argc, char **argv, struct snake *game)
{
        const char opt_string[] = "hvw:f:";

        int opt = 0;
        while ((opt = getarg(argc, argv, opt_string)) != -1) {
                switch (opt) {
                case 'h':
                        print_help(argv[0]);
                        return 1;
                case 'v':
                        printf("Version 0.1.1 \n");
                        return 1;
                case 'f':
                {
                        if (argopt == NULL) {
                                fprintf(stderr, "No value with -%c.\n", opt);
                                print_help(argv[0]);
                                return 1;
                        }
                        int fps = atol(argopt);
                        if (fps < 25) {
                                fprintf(stderr, "FPS too low.\n");
                                return 1;
                        }
                        printf("Set fps to: %d\n", fps);
                        game->fps = fps;
                        break;
                }
                case 'w':
                {
                        if (argopt == NULL) {
                                fprintf(stderr, "No value with -%c.\n", opt);
                                print_help(argv[0]);
                                return 1;
                        }
                        int width = atol(argopt);
                        if (width < 200) {
                                fprintf(stderr, "Width too small.\n");
                                return 1;
                        }
                        printf("Set width to: %d\n", width);
                        game->screen_width = game->screen_height = width;
                        break;
                }
                case 0:
                        fprintf(stderr, "Invalid value: %s\n", argopt);
                        break;
                default:
                        fprintf(stderr, "Invalid option -%c\n", opt);
                }
        }
        return 0;
}

/* Refer: https://github.com/AlynxZhou/flipclock */
int getarg(int argc, char **argv, const char *const opt_string)
{
        static int i = 1;
        static int j = 1;

        while (i < argc) {
                argopt = NULL;
                if (argv[i][0] != '-') {
                        /* handle args like: snake h */
                        argopt = argv[i++];
                        return 0;
                } else if (argv[i][j] == '\0') {
                        /* handlr args like: snake - -aaa -b */
                        ++i;
                        j = 1;
                        continue;
                } else if (strchr(opt_string, argv[i][j]) == NULL) {
                        /* invalid option */
                        return argv[i][j++];
                } else if (*(strchr(opt_string, argv[i][j]) + 1) != ':') {
                        return argv[i][j++];
                } else if (*(strchr(opt_string, argv[i][j]) + 1) == ':') {
                        /* option followed by a value */
                        if (i + 1 < argc
                                && argv[i + 1][0] != '-'
                                && argv[i][j + 1] == '\0') {
                                int temp_i = i;
                                int temp_j = j;
                                argopt = argv[++i];
                                ++i;
                                j = 1;
                                return argv[temp_i][temp_j];
                        } else {
                                ++j;
                        }
                }
        }
        return -1;
}

void print_help(char *name)
{
        printf("Usage: %s -hv -f [fps] -w width\n", name);
        printf("   -h: show help.\n");
        printf("   -v: show version.\n");
        printf("   -f [fps]: custome fps.\n");
        printf("   -w [width]: custome screen width.\n");
        printf("\nHow to play:\n");
        printf("   Use arrow keys or 'w s a d' to move.\n");
        printf("   Press 'q' or 'esc' to quit.\n");
        printf("   Press 'Enter' to restart when game over.\n");
}
