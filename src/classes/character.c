#include <string.h>
#include <stdlib.h>

#include "helpers.h"
#include "class.h"
#include "character.h"

Character *NewCharacter(
  unsigned int class,
  unsigned int gender,
  unsigned int race,
  char *name
){
  switch (class) {
    case CLASS_WARRIOR:
    case CLASS_MAGE:
    case CLASS_HUNTER:
      // pass
    default:
      class = CLASS_WARRIOR;
  }
  Character *character = calloc(1, sizeof(Character));

  character->Name = name;
  character->Class = class;
  character->Gender = gender;
  character->Race = race;
  character->Location = NULL;
  character->X = 0;
  character->Y = 0;

  character->head = calloc(1, sizeof(Head));

  // Set hands
  Hand *hands = calloc(2, sizeof(Hand));
  hands[0].IsLeft = 1;
  memcpy(character->hands, &hands, sizeof(hands));
  free(hands);

  Foot *feet = calloc(2, sizeof(Foot));
  feet[0].IsLeft = 1;
  memcpy(character->feet, &feet, sizeof(feet));
  free(feet);

  return character;
}

void DestroyCharacter(Character *character){
  free(character);
}

void SetLocation(Map *map, Character *character){
  character->Location = map;
  return;
}

int *GetPosition(Character *character){
  int *position = calloc(2, sizeof(int));
  position[0] = character->X;
  position[1] = character->Y;
  return position;
}
