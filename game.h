#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>
#include <time.h>

// Window size
#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

#define Attempt 7 // Number of attempts given to the player
int Attempts = Attempt;

int X, Y; // Used for tracking the mouse X and Y values

// Defines the sizes of the appropriate boxes
int letterX, letterY;           // Position of the letters
int MmenuX, MmenuY;             // Position of the MainMenu buttons
int backX, backY;               // Position of the back button
int TmenuX = 450, TmenuY = 200; // Position of the Title on the main menu
int lineX, lineY;               // Position of the underlines

typedef enum buttonpos // The position for the boxes on the main menu
{
    FirstColumn = 450,
    SecondColumn = 850,
    FirstRow = 400,
    SecondRow = 600
} ButtonPos;

// Game parameters
bool WrongGuess = false;
int CorrectGuess = 0;
int CorrectLetter = 0;
int result = 0;

// Game-related
bool menu = true;
bool playing = false;

// Difficulty
typedef enum difficulty
{
    None = 0,
    Easy = 1,
    Medium = 2,
    Hard = 3
} Difficulty;

int difficulty = None;

// Variables for when a letter is clicked
typedef enum result
{
    Win = 1,
    Lose = 2,
    Correct = 3,
    Wrong = 4
} Result;

// Related to the letters and the word selected
int text_length = 0; // Length of selected word
char word[20];       // Word selected
char guessed[26];    // Letters that have been guessed

char letter[26] = {'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', 'Z', 'X', 'C', 'V', 'B', 'N', 'M'};

char EasyWords[20][24] = {"ORANGE", "SQUARE", "OCEAN", "STONE", "STAPLE", "CODING", "LAPTOP", "PURPLE", "SCRIPT", "POINTER", "INTEGER", "BACKPACK", "CHAMBER",
                          "HANGMAN", "CHEQUE", "ANCIENT", "SUMMIT", "RETURN", "STRING", "IGNEOUS"};

char MedWords[20][24] = {"SPHINX", "PHYSICAL", "QUARTZ", "EXERCISE", "APPOINTMENT", "CALCULATOR", "KNIGHT", "RHYTHM", "DISCORDANCE", "BACKSPACE", "SNOOZE",
                         "DUPLEX", "TELEVISION", "TECHNOLOGY", "BEEKEEPER", "COORDINATE", "KNITTER", "WEDNESDAY", "YACHT", "CONSONANT"};

char HardWords[20][24] = {"KINDERGARTEN", "PYGMY", "ONOMATOPOEIA", "GYMNASTICS", "LYMPHATIC", "HYPERACTIVITY", "MEGAHERTZ", "MNEMONIC", "ZEPHYR", "LARYNX",
                          "SPRITZ", "APOSTROPHE", "JIUJITSU", "GROGGINESS", "PTERODACTYL", "PHLEGM", "BOURGEOISIE", "SYZYGY", "ECCENTRIC", "TEMPORARILY"};

// SDL FUNCTIONS

// Draws the background
void draw_background(SDL_Renderer *renderer, SDL_Texture *background)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_Rect rect;
    rect.x = 0;
    rect.y = 0;
    rect.w = WINDOW_WIDTH;
    rect.h = WINDOW_HEIGHT;
    SDL_RenderCopy(renderer, background, NULL, &rect);
}

// Draws the main menu buttons
void main_button(const char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color)
{
    SDL_Rect btnRect;
    btnRect.x = MmenuX;
    btnRect.y = MmenuY;
    btnRect.w = 300;
    btnRect.h = 100;
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &btnRect);

    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = {MmenuX + 20, MmenuY + 10, texW, texH};

    int button[4] = {MmenuX, MmenuY, MmenuX + 300, MmenuY + 100};
    if (X >= button[0] && X <= button[2] && Y > button[1] && Y < button[3])
    {
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderDrawLine(renderer, button[0], button[1], button[2], button[1]);
        SDL_RenderDrawLine(renderer, button[2], button[1], button[2], button[3]);
        SDL_RenderDrawLine(renderer, button[2], button[3], button[0], button[3]);
        SDL_RenderDrawLine(renderer, button[0], button[3], button[0], button[1]);
    }

    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// Draws the title on the main menu
void main_title(const char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color)
{
    SDL_Rect btnRect;
    btnRect.x = TmenuX;
    btnRect.y = TmenuY;
    btnRect.w = 700;
    btnRect.h = 150;
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &btnRect);

    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = {TmenuX + 75, TmenuY + 5, texW, texH};

    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// Draws the back button
void back_button(const char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color)
{
    SDL_Rect btnRect;
    btnRect.x = backX;
    btnRect.y = backY;
    btnRect.w = 200;
    btnRect.h = 75;
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &btnRect);

    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = {backX + 20, backY + 10, texW, texH};

    int button[4] = {backX, backY, backX + 200, backY + 75};
    if (X >= button[0] && X <= button[2] && Y > button[1] && Y < button[3])
    {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawLine(renderer, button[0], button[1], button[2], button[1]);
        SDL_RenderDrawLine(renderer, button[2], button[1], button[2], button[3]);
        SDL_RenderDrawLine(renderer, button[2], button[3], button[0], button[3]);
        SDL_RenderDrawLine(renderer, button[0], button[3], button[0], button[1]);
    }

    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// Draws the letters in the game
void draw_letter(const char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color)
{
    // Draw ButtonRect
    SDL_Rect btnRect;
    btnRect.x = letterX;
    btnRect.y = letterY;
    btnRect.w = 100;
    btnRect.h = 75;
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    SDL_RenderFillRect(renderer, &btnRect);

    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = {letterX + 30, letterY + 12, texW, texH};

    int button[4] = {letterX, letterY, letterX + 100, letterY + 75};
    if (X >= button[0] && X <= button[2] && Y > button[1] && Y < button[3])
    {
        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        SDL_RenderDrawLine(renderer, button[0], button[1], button[2], button[1]);
        SDL_RenderDrawLine(renderer, button[2], button[1], button[2], button[3]);
        SDL_RenderDrawLine(renderer, button[2], button[3], button[0], button[3]);
        SDL_RenderDrawLine(renderer, button[0], button[3], button[0], button[1]);
    }

    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// Draws the underlines on the playing screen
void draw_line(const char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = {lineX, lineY, texW, texH};

    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// Draws the answer after a guess is made on the lines
void draw_guess(const char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = {lineX, lineY + 30, texW, texH};

    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// Draws the attempt counter
void draw_attempts(const char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int texW = 0, texH = 0;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    SDL_Rect dstrect = {1300, 50, texW, texH};

    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// Draws an image based on the outcome of the game
void draw_result(const char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, SDL_Texture *texture)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *Ttexture = SDL_CreateTextureFromSurface(renderer, surface);
    int texW = 0, texH = 0;
    int posX, posY;
    SDL_QueryTexture(Ttexture, NULL, NULL, &texW, &texH);
    if (strcmp(text, "You Win!") == 0)
        posX = 550, posY = 100;
    else
        posX = 550, posY = 70;
    SDL_Rect dstrect = {posX, posY, texW, texH};
    SDL_RenderCopy(renderer, Ttexture, NULL, &dstrect);

    SDL_Rect rect;
    rect.x = 600;
    rect.y = 250;
    rect.w = 400;
    rect.h = 400;
    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_DestroyTexture(Ttexture);
    SDL_FreeSurface(surface);
}

// Draws the correct word if the outcome is lose
void draw_correct(const char *text, SDL_Renderer *renderer, TTF_Font *font, SDL_Color color)
{
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    int texW = 0, texH = 0;
    int posX, posY;
    SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
    posX = (630 - text_length * 15), posY = 185;
    SDL_Rect dstrect = {posX, posY, texW, texH};
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

// Used to initilialize the word
void start(int text_length)
{
    menu = false;
    playing = true;
    Attempts = Attempt;
    CorrectLetter = 0;
    result = 0;
    WrongGuess = 0;
    for (int i = 0; i < 26; i++)
    {
        for (int k = 0; k < text_length; k++)
        {
            if (letter[i] == word[k])
            {
                CorrectLetter++;
                break;
            }
        }
    }
}

// Checking system to see if the letter chosen is correct
void check(int i)
{
    guessed[i] = letter[i];
    for (int k = 0; k < text_length; k++)
    {
        if (guessed[i] == word[k])
        {
            WrongGuess = false;
            CorrectGuess++;
            result = Correct;
            break;
        }
        else
        {
            WrongGuess = true;
            result = Wrong;
        }
    }
}

// Handles the keyboard inputs and calls check function
void InputKey(SDL_KeyboardEvent *key)
{

    if (strlen(SDL_GetKeyName(key->keysym.sym)) == 1)
    {
        for (int i = 0; i < 26; i++)
        {
            if (SDL_GetKeyName(key->keysym.sym)[0] == letter[i] && SDL_GetKeyName(key->keysym.sym)[0] != guessed[i])
            {
                check(i);
            }
        }
    }
    else if (strcmp(SDL_GetKeyName(key->keysym.sym), "Escape") == 0)
    {
        difficulty = None;
        playing = false;
        menu = true;
    }
}
