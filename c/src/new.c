#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct{
  int x;
  int y;
} coord;

typedef struct Segment{
  coord *posxy ;
  struct Segment *next;
  void (*snek)(struct Segment *self);
} snake_segment;

void snek(snake_segment *self){
  printf("works");
}

snake_segment *snake_constructor(coord *current_position){
  snake_segment *segment = malloc(sizeof(snake_segment));
  if(segment == NULL) return 0;
  segment->next = NULL;
  coord position = {current_position->x, current_position->y};
  segment->posxy = &position;
  segment->snek = snek;
  return segment;
}
coord current_position = {0, 0};

int main(void){
  snake_segment *a = snake_constructor(&current_position);
  if(a == NULL) return 0;
  a->snek(a);
  free(a);
  return 1;
}

