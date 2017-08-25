#ifndef CHARACTER_H
#define CHARACTER_H

#include <pthread.h>

#include "map.h"

typedef struct {
  unsigned int IsLeft: 1;

} Hand;

typedef struct {

} Head;

typedef struct {
  unsigned int IsLeft: 1;
} Foot;

typedef struct {
  unsigned int Gender: 1;
  unsigned int Race: 4;
  unsigned int Class: 4;
  unsigned int Level;

  char *Name;

  Map *Location;
  int X;
  int Y;

  Head *head;
  Hand *hands[2];
  Foot *feet[2];
} Character;

Character *NewCharacter(
  unsigned int,
  unsigned int,
  unsigned int,
  char*
);
void DestroyCharacter(Character *);
void SetLocation(Map *, Character *);
int *GetPosition(Character *);

#endif
