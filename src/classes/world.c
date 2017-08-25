#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <SDL2/SDL.h>

#include "helpers.h"
#include "class.h"
#include "race.h"
#include "gender.h"
#include "character.h"
#include "world.h"
#include "map.h"

Map *WorldAddRandomMap(World *world){
  Map *map = NewMap(
    randstring(10),
    randstring(10),
    100,
    MAP_BINARY
  );

  if ( WorldAddMap(world, map) != 0 ) {
    printf("\nNo Empty slots for new Map\n");
  }

  return map;
};

int WorldAddMap(World *world, Map *map){
  pthread_mutex_lock(&world->maps_lock);

  int no_empty_slots = 1;

  for(int i=0; i<MAX_MAPS;i++){
    // Check if this map slot is available
    if(world->Maps[i] == NULL){
      world->Maps[i] = map;

      no_empty_slots = 0;
      printf("\nAdding map %s\n", map->Id);
      break;
    }
  }

  pthread_mutex_unlock(&world->maps_lock);
  return no_empty_slots;
};

int WorldAddCharacter(World *world, Character *character){
  pthread_mutex_lock(&world->characters_lock);

  int no_empty_slots = 1;

  for(int i=0; i<MAX_ONLINE_CHARACTERS;i++){
    // Check if this map slot is available
    if(world->Characters[i] == NULL){
      world->Characters[i] = character;

      no_empty_slots = 0;
      world->CharactersOnline += 1;
      break;
    }
  }

  printf(
    "\nAdding character %s...",
    character->Name
  );

  pthread_mutex_unlock(&world->characters_lock);
  return no_empty_slots;
}

int WorldRemoveCharacter(World *world, Character *character){
  pthread_mutex_lock(&world->characters_lock);

  int no_empty_slots = 1;

  for(int i=0; i<MAX_ONLINE_CHARACTERS;i++){
    // Check if this map slot is available
    if(world->Characters[i] == NULL){
      free(world->Characters[i]);

      no_empty_slots = 0;
      world->CharactersOnline -= 1;
      break;
    }
  }

  pthread_mutex_unlock(&world->characters_lock);
  return no_empty_slots;
}

Character* WorldAddRandomCharacter(World *world){
  Character *character = NewCharacter(
    CLASS_WARRIOR,
    GENDER_MALE,
    RACE_HUMAN,
    randstring(25)
  );

  if ( WorldAddCharacter(world, character) != 0 ) {
    printf("\nERROR CHARACTER RANDOM ADD\n");
  }

  return character;
}

World* NewWorld(char* id, char* name){
  World *world = calloc(1, sizeof(World));
  if (pthread_mutex_init(&world->characters_lock, NULL) != 0){
    printf("\n mutex init failed\n");
    return NULL;
  }
  if (pthread_mutex_init(&world->maps_lock, NULL) != 0){
    printf("\n mutex init failed\n");
    return NULL;
  }

  world->CharactersOnline = 0;
  world->Id = id;
  world->Name = name;
  *world->Maps = calloc(MAX_MAPS, sizeof(Map*));
  *world->Characters = calloc(MAX_ONLINE_CHARACTERS, sizeof(Character*));

  return world;
}

void DestroyWorld(World *world){
  for (int i=0; i<world->CharactersOnline; i++){
    Character *character = world->Characters[i];
    DestroyCharacter(character);
  };

  pthread_mutex_destroy(&world->characters_lock);
  pthread_mutex_destroy(&world->maps_lock);
  free(world);

  return;
}

int GetWorldOnline(World *world){
  pthread_mutex_lock(&world->characters_lock);
  int online = world->CharactersOnline;
  pthread_mutex_unlock(&world->characters_lock);

  return online;
}
