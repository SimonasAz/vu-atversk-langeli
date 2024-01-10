#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game.h"
#include "hidden-char.h"
#include "saving.h"
#include "utils.h"

int game_loop(struct Game game) {
    int first_pick = -1;
    int second_pick = -1;

    while (true) {
        clear_terminal();
        saveGameState(game);

        if (game.tries_left <= 0) {
            printf("skill issue :[, goodbye\n");
            return 0;
        }

        if (HiddenChar_all_picked(game.characters, game.character_len)) {
            printf("ure built different ;], goodbye\n");
            return 0;
        }

        for (int i = 0; i < game.character_len; i++) {
            HiddenChar_print_box(game.characters[i], i);
        }

        // printing player status
        printf("[\n");
        if (game.state == FIRST_PICK) {
            printf("  FIRST_PICK\n");
        } else {
            printf("  SECOND_PICK\n");
        }
        printf("  tries_left: %2d\n", game.tries_left);
        printf("]\n");
        printf("your choice: ");

        if (game.state == FIRST_PICK) {
            // when both are picked, let the player check his results until the
            // first pick again
            if (first_pick != -1 && second_pick != -1) {
                if (game.characters[first_pick].character !=
                    game.characters[second_pick].character) {
                    game.characters[first_pick].picked = false;
                    game.characters[second_pick].picked = false;
                }
            }

            scanf("%i", &first_pick);
            first_pick = clamp_int(first_pick, 0, game.character_len - 1);

            game.state = SECOND_PICK;
            game.characters[first_pick].picked = true;

            continue;
        }

        if (game.state == SECOND_PICK) {
            scanf("%i", &second_pick);
            second_pick = clamp_int(second_pick, 0, game.character_len - 1);

            game.state = FIRST_PICK;
            game.characters[second_pick].picked = true;
            game.tries_left--;

            continue;
        }
    }
}

int main() {
    clear_terminal();
    srand(time(NULL));

    int CHARACTERS_LENGTH = 26;
    int CHARACTERS[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
                        'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
                        's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};

    char settingsFileName[100];
    printf("File name to read: ");
    scanf("%s", settingsFileName);

    FILE* settingsFile = fopen(settingsFileName, "r");

    if (settingsFile == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    int HOW_MANY_CHARACTERS;
    int TRIES_LEFT;

    if (fscanf(settingsFile, "%d %d", &HOW_MANY_CHARACTERS, &TRIES_LEFT) != 2) {
        printf("Error reading the value from file.\n");
        fclose(settingsFile);
        return 1;
    }

    fclose(settingsFile);

    // Print the read values for verification
    printf("Read HOW_MANY_CHARACTERS: %d\n", HOW_MANY_CHARACTERS);
    printf("Read TRIES_LEFT: %d\n", TRIES_LEFT);

    HOW_MANY_CHARACTERS = clamp_int(HOW_MANY_CHARACTERS, 2, 26);
    TRIES_LEFT = clamp_int(TRIES_LEFT, 1, INT_MAX);

    int arr_length = HOW_MANY_CHARACTERS * 2;

    struct HiddenChar* hidden_char_arr = HiddenChar_create_arr(arr_length);

    for (int i = 0; i < arr_length; i++) {
        struct HiddenChar hidden_char;
        hidden_char.picked = false;
        hidden_char.character = CHARACTERS[i / 2];

        hidden_char_arr[i] = hidden_char;
    }

    HiddenChar_shuffle_arr(hidden_char_arr, arr_length);

    struct Game game;

    game.state = FIRST_PICK;
    game.tries_left = TRIES_LEFT;
    game.characters = hidden_char_arr;
    game.character_len = arr_length;

    char CONTINUE_FROM_PREV;

    printf("continue from previous run? [y/n]\n");
    scanf("%s", &CONTINUE_FROM_PREV);

    if (CONTINUE_FROM_PREV == 121) {
        readGameState(&game);
    }

    int exit_code = game_loop(game);
    free(hidden_char_arr);

    return exit_code;
}
