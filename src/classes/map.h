#ifndef MAP_H
#define MAP_H

#include <pthread.h>

#define MAP_BINARY    0x0000
#define MAP_FOREST    0x0001
#define MAP_WINTER    0x0010

typedef struct {
  unsigned int IsWall: 1;

} MapNode;

typedef struct {
  char *Id;
  char *Name;

  unsigned int Theme: 4;
  int Size;
  MapNode *nodes[0][0];

  pthread_mutex_t online_lock;
  int CharactersOnline;
} Map;

Map *NewMap(char *, char *, int, unsigned int);
void DestroyMap(Map *);
int GetMapOnline(Map *);

#endif
