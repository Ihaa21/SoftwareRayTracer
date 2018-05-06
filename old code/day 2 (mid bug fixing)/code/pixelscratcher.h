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

    ModelType_PointLight,
};

struct camera
{
    v3 Target;
    v3 Up;
    v3 Pos;
};

struct point_light
{
    v3 Pos;
    v3 Color;
};

struct world_object
{
    u32 ModelType;

    union
    {
        rect3 Rect3;
        plane Plane;
        sphere Sphere;

        point_light PointLight;
    };
};

struct game_state
{
    mem_arena Arena;
    v3* RayDir;

    camera Camera;

    u32 LightArraySize;
    world_object* LightArray;
    
    u32 ObjectArraySize;
    world_object* ObjectArray;
};

#define PIXELSCRATCHER_H
#endif
