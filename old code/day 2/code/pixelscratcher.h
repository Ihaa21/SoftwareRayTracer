#if !defined(PIXELSCRATCHER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Ihor Szlachtycz $
   $Notice: (C) Copyright 2014 by Dream.Inc, Inc. All Rights Reserved. $
   ======================================================================== */

// TODO: Remove this
#include <cmath>

enum ModelType
{
    ModelType_Rect3,
    ModelType_Plane,
    ModelType_Sphere,
};

struct world_object
{
    u32 ModelType;

    union
    {
        rect3 Rect3;
        plane Plane;
        sphere Sphere;
    };
};

struct game_state
{
    mem_arena Arena;
    v3* RayDir;

    u32 ObjectArraySize;
    world_object* ObjectArray;
};

#define PIXELSCRATCHER_H
#endif
