#if !defined(PIXELSCRATCHER_H)

#include <cmath>

#define MAX_Z 1024.0f
#define MAX_RECURSION 4
#define RAY_HIT_BIAS 0.00001f

struct camera
{
    v2 CurrentRotation;
    v3 Pos;
};

enum MaterialType
{
    MaterialType_Diffuse,
    MaterialType_Reflective,
    MaterialType_ReflectiveRefractive,
};

struct material
{
    u32 Type;
    v3 Color;

    union
    {
        // NOTE: Diffuse
        struct
        {
            f32 DiffusePercent;
            f32 SpecularPercent;
        };

        // NOTE: Reflective Refractive
        struct
        {
            f32 RefractionIndex;
        };

        // NOTE: Reflection
        struct
        {
            f32 ReflectionPercent;
        };
        
    };
};

enum ModelType
{
    ModelType_Plane,
    ModelType_Sphere,

    ModelType_PointLight,
};

struct point_light
{
    v3 Pos;

    f32 Intensity;
    v3 Color;
};

struct world_object
{
    u32 ModelType;
 
    union
    {
        struct
        {
            plane Plane;
            material Material;
        };

        struct
        {
            sphere Sphere;
            material Material;
        };
        
        point_light PointLight;
    };
};

struct game_state
{
    mem_arena Arena;
    v3* RayDir;

    u32* ColorBuffer;
    u32* DepthBuffer;
    u32* NormalBuffer;

    v2 PrevMouseP;
    camera Camera;

    u32 MaxLightArrayCount;
    u32 CurrLightArrayCount;
    world_object* LightArray;
    world_object* TransformedObjectArray;
    
    u32 MaxObjectArrayCount;
    u32 CurrObjectArrayCount;
    world_object* ObjectArray;
    world_object* TransformedLightArray;
};

#define PIXELSCRATCHER_H
#endif
