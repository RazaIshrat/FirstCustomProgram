#include <stdio.h>
#include "game.h"

int main(int argc, char *args[])
{
    srand(time(NULL));
    SDL_Init(SDL_INIT_EVERYTHING);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG);

    SDL_Window *window = SDL_CreateWindow(
        "Hangman",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    // Text Font Related
    TTF_Font *font = TTF_OpenFont("resources/arial.ttf", 50);   // Font
    TTF_Font *Tfont = TTF_OpenFont("resources/arial.ttf", 125); // Title Font
    TTF_Font *Mfont = TTF_OpenFont("resources/arial.ttf", 50);  // Menu button Font
    TTF_Font *MMfont = TTF_OpenFont("resources/arial.ttf", 75); // Main Menu Button Fnt
    TTF_Font *Lfont = TTF_OpenFont("resources/arial.ttf", 100); // Underline Font
    TTF_Font *Wfont = TTF_OpenFont("resources/chalk.ttf", 100); // Guessed Word Font

    // Text Colour Related
    SDL_Color color = {255, 255, 255};  // Colour
    SDL_Color Tcolor = {0, 0, 0};       // Colour of the Title Text
    SDL_Color Rcolor = {255, 0, 0};     // Colour of incorrect letter
    SDL_Color Gcolor = {0, 255, 0};     // Colour of correct letter
    SDL_Color Lcolor = {252, 252, 252}; // Colour of the guessed letter

    // Load All Sprite
    SDL_Surface *img_win = IMG_Load("resources/win.png");
    SDL_Surface *img_lose = IMG_Load("resources/lose.png");
    SDL_Surface *img_background = IMG_Load("resources/background.png");

    // Load Surface
    SDL_Texture *texture_win = SDL_CreateTextureFromSurface(renderer, img_win);
    SDL_Texture *texture_lose = SDL_CreateTextureFromSurface(renderer, img_lose);
    SDL_Texture *texture_background = SDL_CreateTextureFromSurface(renderer, img_background);

    // Clear Sprite
    SDL_FreeSurface(img_win);
    SDL_FreeSurface(img_lose);
    SDL_FreeSurface(img_background);

    // Load Audio
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024);
    Mix_Chunk *audio_win = Mix_LoadWAV("Resources/win.wav");
    Mix_Chunk *audio_lose = Mix_LoadWAV("Resources/lose.wav");
    Mix_Chunk *audio_correct = Mix_LoadWAV("Resources/correct.wav");
    Mix_Chunk *audio_wrong = Mix_LoadWAV("Resources/wrong.wav");
    Mix_Chunk *audio_select = Mix_LoadWAV("Resources/select.wav");
    Mix_Music *audio_background = Mix_LoadMUS("Resources/background.wav");

    // Application state
    bool isRunning = true;
    bool win = false;
    bool lose = false;

    Mix_PlayMusic(audio_background, -1);
    SDL_Event event;

    while (isRunning)
    {
        SDL_PumpEvents();
        SDL_GetMouseState(&X, &Y);

        if (SDL_PollEvent(&event) > 0)
        {
            switch (event.type)
            {
            // Handles OS "Exit" event
            case SDL_QUIT:
                isRunning = false;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    int mouseX = event.motion.x;
                    int mouseY = event.motion.y;

                    // Start buttons
                    if (menu)
                    {
                        if (mouseX >= FirstColumn && mouseX <= FirstColumn + 300 && mouseY >= FirstRow && mouseY <= FirstRow + 100) // Easy button
                        {
                            difficulty = Easy;
                            Mix_PlayChannel(-1, audio_select, 0);
                        }
                        else if (mouseX >= SecondColumn && mouseX <= SecondColumn + 300 && mouseY >= FirstRow && mouseY <= FirstRow + 100) // Medium button
                        {
                            difficulty = Medium;
                            Mix_PlayChannel(-1, audio_select, 0);
                        }
                        else if (mouseX >= FirstColumn && mouseX <= FirstColumn + 300 && mouseY >= SecondRow && mouseY <= SecondRow + 100) // Hard button
                        {
                            difficulty = Hard;
                            Mix_PlayChannel(-1, audio_select, 0);
                        }
                        else if (mouseX >= SecondColumn && mouseX <= SecondColumn + 300 && mouseY >= SecondRow && mouseY <= SecondRow + 100) // Quit button
                        {
                            isRunning = false;
                        }
                    }

                    // Playing buttons
                    if (playing)
                    {
                        // Back button
                        if (mouseX <= backX + 200 && mouseX >= backX && mouseY <= backY + 50 && mouseY >= backY)
                        {
                            difficulty = None;
                            playing = false;
                            menu = true;
                            Mix_PlayChannel(-1, audio_select, 0);
                        }
                        if (mouseX <= WINDOW_WIDTH && mouseX >= 0 && mouseY <= WINDOW_HEIGHT && mouseY >= 0 && ((win && !lose) || (!win && lose))) // When the player wins or loses, clicking will bring
                        {                                                                                                                          // them back to the menu
                            difficulty = None;
                            win = false;
                            lose = false;
                            playing = false;
                            menu = true;
                            for (int i = 0; i < 26; i++)
                                guessed[i] = 0;
                            for (int i = 0; i < 20; i++)
                                word[i] = 0L;
                        }
                        if (!win && !lose)
                        {
                            // Letter buttons
                            for (int i = 0; i < 10; i++)
                            {
                                letterX = 220 + i * 100 + i * 20;
                                letterY = 600;

                                if (mouseX >= letterX && mouseX <= letterX + 100 && mouseY >= letterY && mouseY <= letterY + 75 && guessed[i] != letter[i])
                                {
                                    mouseX = 0, mouseY = 0;
                                    check(i);
                                }
                            }
                            for (int i = 10; i < 19; i++)
                            {
                                letterX = 280 + (i - 10) * 100 + (i - 10) * 20;
                                letterY = 700;

                                if (mouseX <= letterX + 100 && mouseX >= letterX && mouseY <= letterY + 75 && mouseY >= letterY && guessed[i] != letter[i])
                                {
                                    mouseX = 0, mouseY = 0;
                                    check(i);
                                }
                            }
                            for (int i = 19; i < 26; i++)
                            {
                                letterX = 400 + (i - 19) * 100 + (i - 19) * 20;
                                letterY = 800;

                                if (mouseX <= letterX + 100 && mouseX >= letterX && mouseY <= letterY + 75 && mouseY >= letterY && guessed[i] != letter[i])
                                {
                                    mouseX = 0, mouseY = 0;
                                    check(i);
                                }
                            }
                        }
                    }
                }
                break;

            case SDL_KEYDOWN:
                if (playing)
                {
                    if (!win && !lose)
                        InputKey(&event.key);

                    if ((win && !lose) || (!win && lose))
                    {
                        difficulty = None;
                        playing = false;
                        menu = true;
                        win = false;
                        lose = false;
                        for (int i = 0; i < 26; i++)
                            guessed[i] = 0;
                        for (int i = 0; i < 20; i++)
                            word[i] = 0L;
                    }
                }
                break;
            }
        }

        // Main menu loop
        if (menu)
        {
            win = false;
            lose = false;

            int select = rand() % 20;

            for (int i = 0; i < 26; i++)
                guessed[i] = 0;
            for (int i = 0; i < 20; i++)
                word[i] = 0L;

            // Draw Background background
            draw_background(renderer, texture_background);

            main_title("Hangman", renderer, Tfont, Tcolor);

            MmenuX = FirstColumn;
            MmenuY = FirstRow;
            main_button("Easy", renderer, MMfont, Tcolor);

            MmenuX = SecondColumn;
            MmenuY = FirstRow;
            main_button("Medium", renderer, MMfont, Tcolor);

            MmenuX = FirstColumn;
            MmenuY = SecondRow;
            main_button("Hard", renderer, MMfont, Tcolor);

            MmenuX = SecondColumn;
            MmenuY = SecondRow;
            main_button("Quit", renderer, MMfont, Tcolor);

            switch (difficulty)
            {
            case Easy:
                strcpy(word, EasyWords[select]);
                text_length = strlen(EasyWords[select]);
                start(text_length);
                break;

            case Medium:
                strcpy(word, MedWords[select]);
                text_length = strlen(MedWords[select]);
                start(text_length);
                break;

            case Hard:
                strcpy(word, HardWords[select]);
                text_length = strlen(HardWords[select]);
                start(text_length);
                break;

            case None:
                for (int i = 0; i < 26; i++)
                    guessed[i] = 0;
                for (int i = 0; i < 20; i++)
                    word[i] = 0;
                break;
            }

            // Renders the Screen
            SDL_RenderPresent(renderer);
        }

        // Gameplay loop
        if (playing)
        {
            // Draw Background background
            draw_background(renderer, texture_background);

            // Draw Exit Button
            back_button("EXIT", renderer, Mfont, Tcolor);
            backX = 50;
            backY = 50;

            // Loop with Array for letters
            for (int i = 0; i < 10; i++)
            {
                letterX = 220 + i * 100 + i * 20;
                letterY = 600;
                char text[32];
                sprintf(text, "%c", letter[i]);
                if (guessed[i] != letter[i])
                    draw_letter(text, renderer, font, color);
                else
                    for (int k = 0; k < text_length; k++)
                    {
                        if (guessed[i] == word[k])
                        {
                            draw_letter(text, renderer, font, Gcolor);
                            break;
                        }
                        else
                        {
                            draw_letter(text, renderer, font, Rcolor);
                        }
                    }
            }

            for (int i = 10; i < 19; i++)
            {
                letterX = 280 + (i - 10) * 100 + (i - 10) * 20;
                letterY = 700;
                char text[32];
                sprintf(text, "%c", letter[i]);
                if (guessed[i] != letter[i])
                    draw_letter(text, renderer, font, color);
                else
                    for (int k = 0; k < text_length; k++)
                    {
                        if (guessed[i] == word[k])
                        {
                            draw_letter(text, renderer, font, Gcolor);
                            break;
                        }
                        else
                        {
                            draw_letter(text, renderer, font, Rcolor);
                        }
                    }
            }

            for (int i = 19; i < 26; i++)
            {
                letterX = 400 + (i - 19) * 100 + (i - 19) * 20;
                letterY = 800;
                char text[32];
                sprintf(text, "%c", letter[i]);
                if (guessed[i] != letter[i])
                    draw_letter(text, renderer, font, color);
                else
                    for (int k = 0; k < text_length; k++)
                    {
                        if (guessed[i] == word[k])
                        {
                            draw_letter(text, renderer, font, Gcolor);
                            break;
                        }
                        else
                        {
                            draw_letter(text, renderer, font, Rcolor);
                        }
                    }
            }

            // Drawing the underlines for guessing
            for (int i = 0; i < text_length; i++)
            {
                int distance = (WINDOW_WIDTH / 2) / text_length;
                lineX = (WINDOW_WIDTH - (distance * text_length + 25 * (text_length + 1))) / 2 + ((distance * i) + (25 * (i + 1)));
                lineY = 400;
                draw_line("_", renderer, Lfont, Lcolor);
            }

            // Drawing correct letters on the underlines
            for (int j = 0; j < 26; j++)
            {
                for (int w = 0; w < text_length; w++)
                {
                    if (guessed[j] == word[w])
                    {
                        int distance = (WINDOW_WIDTH / 2) / text_length;
                        lineX = (WINDOW_WIDTH - (distance * text_length + 25 * (text_length + 1))) / 2 + ((distance * w) + (25 * (w + 1)));
                        lineY = 400;
                        char text[32];
                        sprintf(text, "%c", word[w]);
                        draw_guess(text, renderer, Wfont, Lcolor);
                    }
                }
            }

            if (!win && !lose)
            {
                char text[32];
                sprintf(text, "Attempts: %d", Attempts);
                draw_attempts(text, renderer, Mfont, Lcolor);
            }

            if (WrongGuess)
            {
                Attempts--;
                WrongGuess = false;
            }

            if (CorrectGuess == CorrectLetter)
            {
                win = true;
                result = Win;
            }

            if (Attempts == 0)
            {
                lose = true;
                result = Lose;
            }

            switch (result)
            {
            case Win:
                Mix_PlayChannel(-1, audio_win, 0);
                result = 0;
                break;
            case Lose:
                Mix_PlayChannel(-1, audio_lose, 0);
                result = 0;
                break;
            case Correct:
                Mix_PlayChannel(-1, audio_correct, 0);
                result = 0;
                break;
            case Wrong:
                Mix_PlayChannel(-1, audio_wrong, 0);
                result = 0;
                break;
            default:
                result = 0;
                break;
            }

            if (win)
            {
                draw_result("You Win!", renderer, Tfont, Tcolor, texture_win);
                CorrectGuess = 0;
            }

            if (lose)
            {
                char text[32];
                sprintf(text, "The word was %s", word);
                draw_result("You lost!", renderer, Tfont, Tcolor, texture_lose);
                draw_correct(text, renderer, Mfont, Tcolor);
                Attempts = -1;
            }
            // Renders the Screen
            SDL_RenderPresent(renderer);
        }
    }

    // Cleanup
    SDL_DestroyTexture(texture_win);
    SDL_DestroyTexture(texture_lose);
    SDL_DestroyTexture(texture_background);
    Mix_FreeChunk(audio_win);
    Mix_FreeChunk(audio_lose);
    Mix_FreeChunk(audio_correct);
    Mix_FreeChunk(audio_wrong);
    Mix_FreeChunk(audio_select);
    Mix_FreeMusic(audio_background);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    IMG_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
