#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "map.h"

Map *NewMap(char *id, char *name, int size, unsigned int theme){
  Map *map = calloc(1, sizeof(Map));

  map->Id = id;
  map->Name = name;
  map->Theme = theme;
  map->Size = size;

  if (pthread_mutex_init(&map->online_lock, NULL) != 0){
    printf("\n mutex init failed\n");
    return NULL;
  }

  return map;
}

int GetMapOnline(Map *map){
  pthread_mutex_lock(&map->online_lock);

  int online = map->CharactersOnline;

  pthread_mutex_unlock(&map->online_lock);

  return online;
};


void DestroyMap(Map *map){
  pthread_mutex_destroy(&map->online_lock);
  free(map);

  return;
}
