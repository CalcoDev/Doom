#ifndef ENTITY_H
#define ENTITY_H

#include "miniaudio.h"

#include "defines.h"
#include "dmath.h"

typedef enum EntityFlags
{
    EntityFlag_None = 0b0,
    EntityFlag_Sprite = 0b1,
    EntityFlag_Audio = 0b10
} EntityFlags;

typedef struct Entity
{
    EntityFlags flags;

    v2f position;
    v2f scale;
    f32 z_transform;

    i32 sound_idx;

    i32 sprite_idx;
} Entity;

// TODO(calco): Something like this for components I guess?
// typedef struct EntityGroup
// {
//     Entity* entity;
//     EntityGroup* next;
// } EntityGroup;

#endif