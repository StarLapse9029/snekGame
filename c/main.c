#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>


// Structs
typedef struct{
  int xcor;
  int ycor;
  struct Node * next;
} Node;

// Constants
int WINDOW_HEIGHT = 800;
int WINDOW_WIDTH = 1200;
int SEG_SIZE = 50;
int DELAY = 100;

// Function Declarations
bool events();
SDL_Window* initWindow(int x, int y);
SDL_Renderer* initRenderer(SDL_Window * window);
void drawSquare(int x, int y, SDL_Renderer * renderer);
Node *initSnake();
void *freeSnake(Node *snake);
void *drawSnake(Node *snake, SDL_Renderer * renderer);
void drawFruit(SDL_Renderer * renderer);
// Main 
int main(void){
  srand(time(NULL));
  bool runnning = true;
  Node* snake = initSnake();
  if(snake == NULL){
    return 42;
  }
  
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
printf("Could not initialize SDL: %s\n", SDL_GetError());
    return 1;
}
  SDL_Window *window = initWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
  SDL_Renderer * renderer = initRenderer(window);
  
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);

  while(runnning){
    runnning = events();
    drawSnake(snake, renderer);
    drawFruit(renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(DELAY);
  }


  freeSnake(snake);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

bool events(){
  SDL_Event event;
      while(SDL_PollEvent(&event)){
        switch (event.type) {
            case SDL_QUIT:
                return false;
            case SDL_KEYDOWN:
              switch(event.key.keysym.sym){
                case SDLK_ESCAPE:
                  return false;
                default:
                  break;
            }
            default:
              break;
          }
      }
  return true;  
}

SDL_Window *initWindow(int x, int y){
  SDL_Window* window = SDL_CreateWindow(
    "...",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    x, y,
    SDL_WINDOW_SHOWN
);
  
  if (!window) {
    printf("Could not create window: %s\n", SDL_GetError());
    SDL_Quit();
  }
  return window;
}

SDL_Renderer *initRenderer(SDL_Window *window){
  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    printf("Could not create renderer: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
  }
  return renderer;
}

void drawSquare(int x, int y, SDL_Renderer * renderer){
  SDL_Rect rect = {x, y, SEG_SIZE, SEG_SIZE};
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &rect);
}

Node *initSnake(){
  Node *head = (Node*)malloc(sizeof(Node));
  head->xcor = WINDOW_WIDTH/2;
  head->ycor = WINDOW_HEIGHT/2;
  head->next = NULL;

  return head;
}

void *freeSnake(Node *snake){
  if(snake == NULL){
    return 0;
  }
  freeSnake((Node*)snake->next);
  free(snake);
  return 0;
}

void *drawSnake(Node * snake, SDL_Renderer * renderer){
  Node *tmp = snake;
  while(tmp){
    drawSquare(tmp->xcor, tmp->ycor, renderer);
    tmp = (Node*)tmp->next;
  }
  return 0;
}

void drawFruit(SDL_Renderer * renderer){
  int x = rand() % 1175 + 26;
  int y = rand() % 775 + 26;
  SDL_Rect fruit = {x, y, (int)(SEG_SIZE * 2/3), (int)(SEG_SIZE * 2/3)};
  SDL_SetRenderDrawColor(renderer, 255, 15, 15, 255);
  SDL_RenderFillRect(renderer, &fruit);
}
