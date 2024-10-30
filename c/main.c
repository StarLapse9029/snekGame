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

typedef struct{
  int x;
  int y;
}Direction;

typedef struct{
  int x;
  int y;
} Point;

// Constants
int WINDOW_HEIGHT = 800;
int WINDOW_WIDTH = 1200;
int SEG_SIZE = 50;
int DELAY = 10;

// Function Declarations
bool events(Direction *dir);
SDL_Window* initWindow(int x, int y);
SDL_Renderer* initRenderer(SDL_Window * window);
void drawSquare(int x, int y, SDL_Renderer * renderer);
Node *initSnake();
void *freeSnake(Node *snake);
void *drawSnake(Node *snake, SDL_Renderer * renderer);
int drawFruit(Point a, SDL_Renderer * renderer);
void moveSnake(Node * snake, Direction *dir);
void clearRender(SDL_Renderer * renderer);
Point fruitLocation();
void addSegment(Node * snake);
int eat(Node * snake, Point fruit);
void hitWall(Node * snake);

// Main 
int main(void){
  Direction dir;
  dir.x = 1;
  dir.y = 0;
  Point a = fruitLocation();
  int fruits = 0;
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
    runnning = events(&dir);
    clearRender(renderer);
    hitWall(snake);
    fruits = eat(snake, a);
    moveSnake(snake, &dir);
    drawSnake(snake, renderer);
    if(fruits == 0){
      a = fruitLocation();
    }
    drawFruit(a, renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(DELAY);
  }


  freeSnake(snake);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}

bool events(Direction *dir){
  SDL_Event event;
      while(SDL_PollEvent(&event)){
        switch (event.type) {
            case SDL_QUIT:
                return false;
            case SDL_KEYDOWN:
              switch(event.key.keysym.sym){
                case SDLK_ESCAPE:
                  return false;
                case SDLK_a:
                  dir->x = -1;
                  dir->y = 0;
                  break;
                case SDLK_s:
                  dir->x = 0;
                  dir->y = 1;
                  break;
                case SDLK_d:
                  dir->x = 1;
                  dir->y = 0;
                  break;
                case SDLK_w:
                  dir->x = 0;
                  dir->y = -1;
                  break;
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

void clearRender(SDL_Renderer * renderer){
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
}

// Snake
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

void *drawSnake(Node *snake, SDL_Renderer *renderer) {
    Node *tmp = snake;
    while (tmp != NULL) {
        drawSquare(tmp->xcor, tmp->ycor, renderer);
        tmp = (Node*)tmp->next;
    }
  return 0;
}


void moveSnake(Node * snake, Direction *dir){
  Node * tmp = snake;
  while(tmp){
    tmp->xcor += dir->x*10;
    tmp->ycor += dir->y*10;
    tmp = (Node*)tmp->next;
  }
}

void addSegment(Node * snake){
  Node * tmp = snake;
  Node * new = (Node*)malloc(sizeof(Node));
  if(new == NULL){
    return;
  }
  while(tmp->next != NULL){
    tmp = (Node*)tmp->next;
  }
  new->next = NULL;
  new->xcor = tmp->xcor;
  new->ycor = tmp->ycor;
  tmp->next = (struct Node*)new;
  puts("hi");
}
// Collisions
int eat(Node * snake, Point fruit){
  if(snake->xcor >= fruit.x - 15 && snake->xcor <= fruit.x + 15){
    if (snake->ycor >= fruit.y - 15 && snake->ycor <= fruit.y + 15) {
      addSegment(snake);
      return 0;
    }
  }
  return 1;
}

void hitWall(Node * snake){
  if(snake->xcor >= WINDOW_WIDTH){
    snake->xcor = 0;
  }else if(snake->xcor <= 0){
    snake->xcor = WINDOW_WIDTH;
  }else if(snake->ycor >= WINDOW_HEIGHT){
    snake->ycor = 0;
  }else if(snake->ycor <= 0){
    snake->ycor = WINDOW_HEIGHT;
  }
}

// Fruit

Point fruitLocation(){
  Point a;
  a.x = rand() % (WINDOW_WIDTH - (SEG_SIZE/2)) + SEG_SIZE/2 +1;
  a.y = rand() % (WINDOW_HEIGHT - (SEG_SIZE/2) + SEG_SIZE/2 + 1);
  return a;
}

int drawFruit(Point a, SDL_Renderer * renderer){
  int x = a.x;
  int y = a.y;  
  SDL_Rect fruit = {x, y, (int)(SEG_SIZE * 2/3), (int)(SEG_SIZE * 2/3)};
  SDL_SetRenderDrawColor(renderer, 255, 15, 15, 255);
  SDL_RenderFillRect(renderer, &fruit);
  return 1;
}
