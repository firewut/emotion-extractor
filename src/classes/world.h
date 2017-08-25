#ifndef WORLD_H
#define WORLD_H

#include "character.h"
#include "map.h"

#define MAX_MAPS 1000
#define MAX_ONLINE_CHARACTERS 10000

typedef struct {
  char *Id;
  char *Name;

  pthread_mutex_t characters_lock;
  int CharactersOnline;
  Character *Characters[MAX_ONLINE_CHARACTERS];

  pthread_mutex_t maps_lock;
  Map *Maps[MAX_MAPS];
} World;


World *NewWorld(char *, char*);
void DestroyWorld(World *);
int GetWorldOnline(World *);

Map *WorldAddRandomMap(World *);
int WorldAddMap(World *, Map *);

Character *WorldAddRandomCharacter(World *);
int WorldAddCharacter(World *, Character *);
int WorldRemoveCharacter(World *, Character *);

#endif
