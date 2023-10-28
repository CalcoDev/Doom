#ifndef ENTITY_H
#define ENTITY_H

#include "defines.h"
#include "dmath.h"

typedef enum EntityFlags
{
    EntityFlag_None = 0b0,
    EntityFlag_Sprite = 0b1
} EntityFlags;

typedef struct Entity
{
    EntityFlags flags;

    v2f position;
    i32 sprite_idx;
} Entity;

#endif