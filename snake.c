#include "local_libraries/queue.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct coordinate {
    int x, y;

} coordinate;

typedef enum { up, down, left, right, directions_count } directions;

// screen macros defines start
#define WIDTH 900
#define HEIGHT 600
#define COLOR_SCREEN 0x10
// screen macros define end

// grid macros defines start
#define CELL_SIZE 30
#define ROWS HEIGHT / CELL_SIZE
#define COLUMNS WIDTH / CELL_SIZE
#define COLOR_GRID 0x778899ff
#define GRID_LINE_WIDTH 2
#define DRAW_GRID() draw_grid(surface)
// grid macros define end

// cell macros deines start
#define MAKE_CELL_FREE(x, y) fill_cell(surface, x, y, COLOR_SCREEN)
// cell macros defines end

// snake macros defines start
#define COLOR_SNAKE 0x00FF00FF
#define SNAKE(x, y) fill_cell(surface, x, y, COLOR_SNAKE)
#define MAX_PICES WIDTH* HEIGHT / CELL_SIZE + 5
// snake macros defines end

// apple macros defines start
#define COLOE_APPLE 0x00ff0000
#define APPLE(x, y) fill_cell(surface, x, y, COLOE_APPLE)
// apple macros defines end

// functions calls macros define start

#define GET_INPUT() getInput(event)
#define MAKE_MOVE() makeMove(surface)
#define CHECK_GAME_OVER() checkGameOver()
#define CHECK_APPLE_EATEN() checkAppleEaten(surface, window)
#define DRAW_SNAKE() paintSnakCells(surface)
#define DRAW_APPLE() paintAppleCells(surface)

// functions calls macros define end

// global variables start
int pices = 1, score = 0, gameDelay = 110;
coordinate apple, snake[MAX_PICES];
directions pices_directions[MAX_PICES];
Queue* moves;
bool game = true, wantToPlayAgain = true;
directions current_direction;

// global variables end

int getRandomInRange(int min, int max) {
    return min + rand() % (max - min + 1);
}

void fill_cell(SDL_Surface* surface, int x, int y, Uint32 color) {

    SDL_Rect cell = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
    SDL_FillRect(surface, &cell, color);
}

void setWindowTitle(SDL_Window* window, char title[]) {

    SDL_SetWindowTitle(window, title);
}

void updateScore(SDL_Window* window, int newScore) {
    char title[50];
    snprintf(title, sizeof(title), "Classic Snake {%d}", newScore);
    setWindowTitle(window, title);
}

bool isOutOfBound(coordinate snake) {

    return (

        (snake.x * CELL_SIZE < 0 || snake.x * CELL_SIZE >= WIDTH) ||
        (snake.y * CELL_SIZE < 0 || snake.y * CELL_SIZE >= HEIGHT)

    );
}

bool isTouchHisBody(){

    for(int i = 1; i < pices ; i++){

        if( (snake[0].x == snake[i].x) && (snake[0].y == snake[i].y) )
            return true;

    }

    return false;

}

void reallocApple() {
    bool flag = false;
    while (!flag) {
        flag = true;
        apple.x = getRandomInRange(0, WIDTH / CELL_SIZE - 1);
        apple.y = getRandomInRange(0, HEIGHT / CELL_SIZE - 1);
        for (int i = 0; i < pices; i++) {

            if (snake[i].x == apple.x && snake[i].y == apple.y) {

                flag = false;
                break;
            }
        }
    }
}

void getInput(SDL_Event event) {
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            game = false;
            wantToPlayAgain = false;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_DOWN: if (current_direction != up) push(moves, down); break;
                case SDLK_UP: if (current_direction != down) push(moves, up); break;
                case SDLK_LEFT: if (current_direction != right) push(moves, left); break;
                case SDLK_RIGHT: if (current_direction != left) push(moves, right); break;
            }
        }
    }
}

void makeMove(SDL_Surface* surface) {

    if (!isEmpty(moves)) {
        current_direction = pop(moves);
    }

    for (int i = 0; i < pices; i++) {
        MAKE_CELL_FREE(snake[i].x, snake[i].y);
    }

    // make every cell `i` update his direction to be the same to direction cell
    // `i - 1` except the root (cell 0) , make the direction of root equal to
    // direction of current input

    for (int i = pices - 1; i > 0; i--) {
        pices_directions[i] = pices_directions[i - 1];
    }

    // update root (cell 0) after update other pices
    // because we want second cell take old direction to the root

    pices_directions[0] = current_direction;

    // move each cell with her direction
    for (int i = 0; i < pices; i++) {
       switch (pices_directions[i]) {
            case up: snake[i].y--; break;
            case down: snake[i].y++; break;
            case left: snake[i].x--; break;
            case right: snake[i].x++; break;
            default: break;
        }    
    }
}

void checkGameOver() {
    if (isOutOfBound(snake[0]) || isTouchHisBody()) {
        SDL_Delay(1000);
        game = false;
        wantToPlayAgain = false;
    }
}

void addNewCell() {
    pices++;
    snake[pices - 1] = snake[pices - 2];
    if (pices_directions[pices - 2] == up) {
        snake[pices - 1].y++;
    } else if (pices_directions[pices - 2] == down) {
        snake[pices - 1].y--;
    } else if (pices_directions[pices - 2] == left) {
        snake[pices - 1].x++;
    } else if (pices_directions[pices - 2] == right) {
        snake[pices - 1].x--;
    }
}

void changeGameSpeed(){
    if(score % 10 == 0){
        if(gameDelay > 50)
            gameDelay -= 10;
        else if(gameDelay > 25)
            gameDelay -= 5;
    }
}

void checkAppleEaten(SDL_Surface* surface, SDL_Window* window) {

    if (snake[0].x == apple.x && snake[0].y == apple.y) {
        MAKE_CELL_FREE(apple.x, apple.y);
        score++;
        changeGameSpeed();
        updateScore(window, score);
        addNewCell();
        reallocApple();
    }
}

void paintSnakCells(SDL_Surface* surface) {
    for (int i = 0; i < pices; i++) {
        SNAKE(snake[i].x, snake[i].y);
    }
}

void initializeVariables() {
    if (moves) free(moves);
    moves = (Queue*)malloc(sizeof(Queue));
    initQueue(moves);

    snake[0].x = 8;
    snake[0].y = 10;
    pices = 1;
    score = 0;
    for (int i = 0; i < pices; i++) {
        pices_directions[i] = current_direction;
    }
    reallocApple();
    current_direction = (directions)(getRandomInRange(0, directions_count - 1));
    game = true;
}

void paintAppleCells(SDL_Surface* surface) { APPLE(apple.x, apple.y); }

void draw_grid(SDL_Surface* surface) {

    

    SDL_Rect line = {0, 0, GRID_LINE_WIDTH, HEIGHT};

    // draw columns
    for (line.x = 0; line.x < WIDTH; line.x += CELL_SIZE)
        SDL_FillRect(surface, &line, COLOR_GRID);

    line.w = WIDTH;
    line.x = 0;
    line.h = GRID_LINE_WIDTH;

    // draw rows
    for (line.y = 0; line.y < HEIGHT; line.y += CELL_SIZE)
        SDL_FillRect(surface, &line, COLOR_GRID);

    
}

void resetScreen(SDL_Surface *surface){
    
    SDL_Rect free_screen = {0,0, WIDTH, HEIGHT};
    SDL_FillRect(surface, &free_screen, COLOR_SCREEN);


}

void showStartScreen(SDL_Surface* surface, SDL_Window* window) {
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));  
    SDL_UpdateWindowSurface(window);

    TTF_Font* font = TTF_OpenFont("Arial.ttf", 24);
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
        return;
    }

    SDL_Color textColor = {255, 255, 255};
    SDL_Event event;
    bool waiting = true;

    const char* messages[] = {
        "Avoid hitting corners or yourself to survive!",
        "Every 10 apples increase your speed!",
        "Press Enter to Start",
        "Made with <3 by gom3a :)) in C",
        "" __DATE__
    };

    while (waiting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) exit(0);
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) waiting = false;
        }

        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));

        for (int i = 0; i < 5; i++) {
            SDL_Surface* textSurface = TTF_RenderText_Solid(font, messages[i], textColor);
            if (!textSurface) {
                printf("Error rendering text: %s\n", TTF_GetError());
                break;
            }
            SDL_Rect textRect = {WIDTH / 2 - textSurface->w / 2, HEIGHT / 2 - 60 + i * 40, textSurface->w, textSurface->h};
            SDL_BlitSurface(textSurface, NULL, surface, &textRect);
            SDL_FreeSurface(textSurface);
        }

        SDL_UpdateWindowSurface(window);
        SDL_Delay(20);
    }

    TTF_CloseFont(font);
}
void showEndScreen(SDL_Surface* surface, SDL_Window* window, int score) {
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 0, 0));  
    SDL_UpdateWindowSurface(window);

    TTF_Font* font = TTF_OpenFont("Arial.ttf", 24);
    if (!font) {
        printf("Error loading font: %s\n", TTF_GetError());
        return;
    }

    SDL_Color textColor = {255, 255, 255};
    char scoreText[50];
    sprintf(scoreText, "Your Score: %d", score);

    SDL_Event event;
    bool waiting = true;

    while (waiting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE)) 
            {

                exit(0); 
            }
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_RETURN) {
                waiting = false; 
                wantToPlayAgain = true;
                initializeVariables();
            }
        }

        SDL_Surface* scoreSurface = TTF_RenderText_Solid(font, scoreText, textColor);
        SDL_Surface* restartSurface = TTF_RenderText_Solid(font, "Press ENTER to Restart", textColor);
        SDL_Surface* exitSurface = TTF_RenderText_Solid(font, "Press ESC to Exit", textColor);

        if (!scoreSurface || !restartSurface || !exitSurface) {
            printf("Error rendering text: %s\n", TTF_GetError());
            break;
        }

        SDL_Rect scoreRect = {WIDTH / 2 - scoreSurface->w / 2, HEIGHT / 2 - 50, scoreSurface->w, scoreSurface->h};
        SDL_Rect restartRect = {WIDTH / 2 - restartSurface->w / 2, HEIGHT / 2, restartSurface->w, restartSurface->h};
        SDL_Rect exitRect = {WIDTH / 2 - exitSurface->w / 2, HEIGHT / 2 + 50, exitSurface->w, exitSurface->h};

        SDL_BlitSurface(scoreSurface, NULL, surface, &scoreRect);
        SDL_BlitSurface(restartSurface, NULL, surface, &restartRect);
        SDL_BlitSurface(exitSurface, NULL, surface, &exitRect);

        SDL_UpdateWindowSurface(window);

        SDL_FreeSurface(scoreSurface);
        SDL_FreeSurface(restartSurface);
        SDL_FreeSurface(exitSurface);

        SDL_Delay(20);
    }

    TTF_CloseFont(font);
}



int main(int args, char* argv[]) {
    srand(time(NULL)); // for random values

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window =
        SDL_CreateWindow("Classic Snake {0}", SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    SDL_Surface* surface = SDL_GetWindowSurface(window);
    SDL_FillRect(surface, NULL, COLOR_SCREEN);
    SDL_Event event;

    initializeVariables();

    showStartScreen(surface, window);
    resetScreen(surface);

    do {
        Uint32 lastUpdate = SDL_GetTicks();

        while (game) {
            Uint32 now = SDL_GetTicks();
            if (now - lastUpdate >= gameDelay) { 
                GET_INPUT();
                MAKE_MOVE();
                CHECK_GAME_OVER();
                CHECK_APPLE_EATEN();
                DRAW_SNAKE();
                DRAW_APPLE();
                DRAW_GRID();
                SDL_UpdateWindowSurface(window);

                lastUpdate = now; 
            }

            SDL_Delay(1); 
        }

        showEndScreen(surface, window, score);
        resetScreen(surface);

    } while (wantToPlayAgain);

    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

