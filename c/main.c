#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

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

typedef struct{
  int score;
  char dateTime[25];
  struct scoreRecord * next;
}scoreRecord;

// Constants
int WINDOW_HEIGHT = 800;
int WINDOW_WIDTH = 1200;
int SEG_SIZE = 50;
int DELAY = 100;
SDL_Color WHITE = {255, 255, 255, 255};
SDL_Color BLACK = {0, 0, 0, 255};

// Function Declarations
bool events(Direction *dir);
SDL_Window* initWindow(int x, int y);
SDL_Renderer* initRenderer(SDL_Window * window);
void drawSquare(int x, int y, SDL_Renderer * renderer, int color);
Node *initSnake();
void *freeSnake(Node *snake);
void *drawSnake(Node *snake, SDL_Renderer * renderer, int color);
int drawFruit(Point a, SDL_Renderer * renderer);
void moveSnake(Node * snake, Direction *dir);
void clearRender(SDL_Renderer * renderer, int color);
Point fruitLocation();
void addSegment(Node ** snake);
int eat(Node * snake, Point fruit);
void hitWall(Node * snake);
void moveSegment(Node * snake, int x, int y);
bool hitSelf(Node * snake);
void showScore(SDL_Renderer * renderer, int num, int size, SDL_Color color);
void startScreen(SDL_Renderer * renderer, int alpha);
void endScreen(SDL_Renderer * renderer, int num);
void storeScores(int num);
scoreRecord * readScores();
void printScores(scoreRecord * scores);
void freeScores(scoreRecord * scores);
void transition(int * p, bool change);

// Main 
int main(void){
  Direction dir;
  dir.x = 1;
  dir.y = 0;
  Point a = fruitLocation();
  int fruits = 0;
  int score = 0;
  srand(time(NULL));
  bool runnning = true;
  int color = 255;
  int bg = 0;
  int debuff = 0;
  int p = 0;
  bool change = true;
  int * pp = &p;

  TTF_Init();  

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
  
  for(int i = 0; i < 100; i++){
    startScreen(renderer, (int)(255 - (i*2.55)));
    SDL_Delay(15);
  }
  SDL_Delay(250);

  SDL_SetRenderDrawColor(renderer, 15, 15, 15, 255); 
  SDL_RenderClear(renderer);
  SDL_RenderPresent(renderer);


  int time = 0;
  // Main Loop
  while(runnning){
    debuff = (score/10 < 80 ? score/10 : 20);
    if(events(&dir)){
      runnning = false;
    }
    if(time == 100){
      transition(pp, change);
    }
    if(time > 100){
      change = !change;
      time = 0;
    }
    clearRender(renderer, bg + (p*100));
    
    hitWall(snake);

    fruits = eat(snake, a);

    moveSnake(snake, &dir);
    drawSnake(snake, renderer, (color - p*100));
    if(fruits == 0){
      score++;
      a = fruitLocation();
    }
    drawFruit(a, renderer);
   
    showScore(renderer, score, 26, BLACK);
    showScore(renderer, score, 24, WHITE);


    SDL_RenderPresent(renderer);
    if(hitSelf(snake)){
      runnning = false;
    }
    time++;
    SDL_Delay(DELAY - (int)(debuff*3));
  }
  
  scoreRecord * scoreList = readScores();
  printScores(scoreList);
  freeScores(scoreList);

  endScreen(renderer, score);
  storeScores(score);
  SDL_Delay(1500);

  TTF_Quit();
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
                return true;
            case SDL_KEYDOWN:
              switch(event.key.keysym.sym){
                case SDLK_ESCAPE:
                  return true;
                case SDLK_a:
                  if(dir->x != 1){
                    dir->x = -1;
                    dir->y = 0;
                  }
                  break;
                case SDLK_s:
                  if(dir->y != -1){
                    dir->x = 0;
                    dir->y = 1;
                  }
                  break;
                case SDLK_d:
                  if(dir->x != -1){ 
                    dir->x = 1;
                    dir->y = 0;
                  }
                  break;
                case SDLK_w:
                  if(dir->y != 1){
                    dir->x = 0;
                    dir->y = -1;
                  }
                  break;
                default:
                  break;
            }
            default:
              break;
          }
      }
  return false;  
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

void drawSquare(int x, int y, SDL_Renderer * renderer, int color){
  SDL_Rect rect = {x, y, SEG_SIZE, SEG_SIZE};
  SDL_SetRenderDrawColor(renderer, color, color, color, 255);
  SDL_RenderFillRect(renderer, &rect);
}

void clearRender(SDL_Renderer * renderer, int color){
  SDL_SetRenderDrawColor(renderer, color, color, color, 255);
  SDL_RenderClear(renderer);
}


// Texts
void showScore(SDL_Renderer * renderer, int num, int size, SDL_Color color){
  if(num > 99999){
    return;
  }
  char x[5];
  SDL_itoa(num, x, 10);
  TTF_Font * font = TTF_OpenFont("./fonts/ProggyCleanNerdFontMono-Regular.ttf", size);
  if(font == NULL){
    goto exitLabel;
  }
  SDL_Surface * surfaceMessage = TTF_RenderText_Solid(font, "Score: ", color);
  if(surfaceMessage == NULL){
    goto exitLabel;
  }
  SDL_Texture * Message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
  if(Message == NULL){
    goto exitLabel;
  }
  SDL_Surface * score = TTF_RenderText_Solid(font, x, color);
  if(score == NULL){
    goto exitLabel;
  }
  SDL_Texture * scoreTexture = SDL_CreateTextureFromSurface(renderer, score);
  if(scoreTexture == NULL){
    goto exitLabel;
  }


  SDL_Rect Message_rect = {(int)(WINDOW_WIDTH/2) - (int)(surfaceMessage->w/2), 50, surfaceMessage->w, surfaceMessage->h};
  SDL_Rect score_rect = {(int)(WINDOW_WIDTH /2) + (int)(surfaceMessage->w/2), 50, score->w, score->h};

  SDL_RenderCopy(renderer, Message, NULL, &Message_rect);
  SDL_RenderCopy(renderer, scoreTexture, NULL, &score_rect);

exitLabel:
  TTF_CloseFont(font);
  return;
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

void *drawSnake(Node *snake, SDL_Renderer *renderer, int color){
  Node *tmp = snake;
  while(tmp != NULL){
    drawSquare(tmp->xcor, tmp->ycor, renderer, color);
    tmp = (Node*)tmp->next;
  }
  return 0;
}


void moveSnake(Node * snake, Direction *dir){
  int x = snake->xcor;
  int y = snake->ycor;
  
  snake->xcor += dir->x*SEG_SIZE;
  snake->ycor += dir->y*SEG_SIZE;
  moveSegment((Node*)snake->next, x, y);
}
void moveSegment(Node * segment, int x, int y){
  if(segment == NULL){
    return;
  }
  int newx = segment->xcor;
  int newy = segment->ycor;
  segment->xcor = x;
  segment->ycor = y;
  moveSegment((Node*)segment->next, newx, newy);
}

void addSegment(Node ** snake){
  Node * tmp;
   
  Node * new = (Node*)malloc(sizeof(Node));
  if(new == NULL){
    return;
  }
  new->xcor = (*snake)->xcor;
  new->ycor = (*snake)->ycor;


  if((*snake)->next != NULL){
    tmp = (Node*)(*snake)->next;
    (*snake)->next = (struct Node*)new;
    new->next = (struct Node*)tmp;
  }else{
    (*snake)->next = (struct Node*)new;
  }
}
// Collisions
int eat(Node * snake, Point fruit){
  if(snake->xcor == fruit.x * SEG_SIZE){
    if (snake->ycor == fruit.y * SEG_SIZE){
      addSegment(&snake);
      return 0;
    }
  }
  return 1;
}

void hitWall(Node * snake){
  if(snake->xcor > WINDOW_WIDTH - SEG_SIZE){
    snake->xcor = 0;
  }else if(snake->xcor < 0){
    snake->xcor = WINDOW_WIDTH;
  }else if(snake->ycor > WINDOW_HEIGHT - SEG_SIZE){
    snake->ycor = 0;
  }else if(snake->ycor < 0){
    snake->ycor = WINDOW_HEIGHT;
  }
}
bool hitSelf(Node * snake){
 Node * tmp = (Node*)snake->next;
  while(tmp != NULL){
    if(snake->xcor == tmp->xcor){
      if(snake->ycor == tmp->ycor){
        return true;
      }
    }
    tmp = (Node*)tmp->next;
  }
  return false;
}

// Fruit

Point fruitLocation(){
  Point a;
  a.x = rand() % (int)(WINDOW_WIDTH/SEG_SIZE);
  a.y = rand() % (int)(WINDOW_HEIGHT/SEG_SIZE);
  return a;
}

int drawFruit(Point a, SDL_Renderer * renderer){
  int x = a.x * SEG_SIZE;
  int y = a.y * SEG_SIZE;  
  SDL_Rect fruit = {x, y, SEG_SIZE, SEG_SIZE};
  SDL_SetRenderDrawColor(renderer, 255, 15, 15, 255);
  SDL_RenderFillRect(renderer, &fruit);
  return 1;
}

// Start and End Screens
void startScreen(SDL_Renderer * renderer, int alpha){
  SDL_SetRenderDrawColor(renderer, 0, 0, 10, alpha);
  SDL_RenderClear(renderer);

  SDL_Color  color = {255, 255, 255, alpha};
  TTF_Font * font1 = TTF_OpenFont("./fonts/ProggyCleanNerdFontMono-Regular.ttf", 2*SEG_SIZE);
  SDL_Surface * text1 = TTF_RenderText_Solid(font1, "SnakeGame", color);
  SDL_Texture * message1 = SDL_CreateTextureFromSurface(renderer, text1);
  SDL_Rect rect1 = {(int)(WINDOW_WIDTH/2 - text1->w/2), (int)(WINDOW_HEIGHT/2 - text1->h), text1->w, text1->h };

  SDL_RenderCopy(renderer, message1, NULL, &rect1);

  SDL_RenderPresent(renderer);
}

void endScreen(SDL_Renderer * renderer, int num){
  SDL_SetRenderDrawColor(renderer, 0, 0, 10, 255);
  SDL_RenderClear(renderer);
  
  if(num > 99999){
    return;
  }
  char finalScore[20] = "Final Score: ";
  char score[5];
  SDL_itoa(num, score, 10);
  strcat(finalScore, score);  

  TTF_Font * font1 = TTF_OpenFont("./fonts/ProggyCleanNerdFontMono-Regular.ttf", 2*SEG_SIZE);
  SDL_Surface * text1 = TTF_RenderText_Solid(font1, "Game Over", WHITE);
  SDL_Texture * message1 = SDL_CreateTextureFromSurface(renderer, text1);
  SDL_Rect rect1 = {(int)(WINDOW_WIDTH/2 - text1->w/2), (int)(WINDOW_HEIGHT/2 - text1->h), text1->w, text1->h };

  SDL_RenderCopy(renderer, message1, NULL, &rect1);

  TTF_Font * font2 = TTF_OpenFont("./fonts/ProggyCleanNerdFontMono-Regular.ttf", SEG_SIZE);
  SDL_Surface * text2 = TTF_RenderText_Solid(font2, finalScore, WHITE);
  SDL_Texture * message2 = SDL_CreateTextureFromSurface(renderer, text2);
  SDL_Rect rect2 = {(int)(WINDOW_WIDTH/2 - text2->w/2), (int)(WINDOW_HEIGHT/2 + text1->h), text2->w, text2->h};

  SDL_RenderCopy(renderer, message2, NULL, &rect2);


  SDL_RenderPresent(renderer);
}

void storeScores(int num){
  if(num > 99999) return;

  FILE *fptr = NULL;
  fptr = fopen("scores.csv", "a+");
  if(fptr == NULL){
    return;
  }

  char score[5]; 
  SDL_itoa(num, score, 10);

  time_t  t = time(NULL);

  fprintf(fptr, "%s; %s", score, ctime(&t));
  fclose(fptr);
}

scoreRecord * readScores(){
  FILE * fptr = NULL;
  fptr = fopen("./scores.csv", "r");
  if(fptr == NULL){
    return NULL;
  }
  
  char buffer[32];
  scoreRecord * tmp = NULL;
  scoreRecord * curr = (scoreRecord*)malloc(sizeof(scoreRecord));
  tmp = curr;
  while(fgets(buffer, 32, fptr) != NULL){
    scoreRecord * new = (scoreRecord*)malloc(sizeof(scoreRecord));
    if(new == NULL) return NULL;
    if(sscanf(buffer, "%i; %s", &new->score, new->dateTime) != 2){
      free(new);
      continue;
    }
    tmp->next = (struct scoreRecord*)new;
    tmp = (scoreRecord*)tmp->next;
  }
  tmp->next = NULL;
  return curr;
}
void printScores(scoreRecord * scores){
  if(scores == NULL) return;
  scoreRecord * tmp = (scoreRecord*)scores->next;
  while(tmp != NULL){
    printf("%d   %s\n", tmp->score, tmp->dateTime);
    tmp = (scoreRecord*)tmp->next;
  }
}

void freeScores(scoreRecord * scores){
  if(scores == NULL) return;
  freeScores((scoreRecord*)scores->next);
  free(scores);  
}

void transition(int * p, bool change){
  if(change){
    *p = 0;
  }else{
    *p = 2;
  }
}
