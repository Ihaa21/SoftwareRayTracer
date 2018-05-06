/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Ihor Szlachtycz $
   $Notice: (C) Copyright 2014 by Dream.Inc, Inc. All Rights Reserved. $
   ======================================================================== */

#include "pixelscratcher_platform.h"
#include "pixelscratcher.h"

#include "pixelscratcher_render.cpp"

global platform_api PlatformApi;

internal void ClearScreen(game_display* Display)
{    
    u32* PixelRow = Display->FrameBuffer;
    for (u32 Y = 0; Y < Display->Height; ++Y)
    {
        u32* Pixel = PixelRow;
        for (u32 X = 0; X < Display->Width; ++X)
        {
            *Pixel = 0xFFAAFF00;
            ++Pixel;
        }

        PixelRow += Display->Width;
    }
}

#if 0
inline b32 RaySphereIntersect(v3 RayOrig, v3 RayDir, sphere S, f32* tReturn)
{
    v3 L = S.Center - RayOrig;
    f32 tCA = Inner(L, RayDir);
    if (tCA < 0.0f)
    {
        return false;
    }

    f32 d = SquareRoot(Inner(L, L) - Square(tCA));
    if (d > S.Radius)
    {
        return false;
    }

    f32 tHC = SquareRoot(Square(S.Radius) - Square(d));

    f32 t0 = tCA - tHC;
    f32 t1 = tCA + tHC;

    if (t0 > 0 || t1 > 0)
    {
        i32 i = 0;
    }
    
    f32 t = Max(t0, t1);
    if (t >= 0.0f)
    {
        *tReturn = t;
        return true;
    }

    return false;
}
#else

#if 0
inline void SolveQuadratic(f32 a, f32 b, f32 c, f32* Root1, f32* Root2)
{
    f32 Discriminant = Square(b) - 4*a*c;
    if (Discriminant > 0)
    {
        *Root1 = ((-b) + SquareRoot(Discriminant)) / (2*a);
        *Root2 = ((-b) - SquareRoot(Discriminant)) / (2*a);
    }
    else if (Discriminant == 0)
    {
        *Root1 = ((-b) / (2*a));
    }
}
#else
inline void SolveQuadratic(f32 a, f32 b, f32 c, f32* Root1, f32* Root2)
{
    f32 Discriminant = Square(b) - 4*a*c;
    if (Discriminant > 0.0f)
    {
        f32 q = ((b > 0.0f) ? -0.5f*(b + SquareRoot(Discriminant)) :
                 -0.5f*(b - SquareRoot(Discriminant)));
        *Root1 = q / a;
        *Root2 = c / q;
    }
    else if (Discriminant == 0.0f)
    {
        *Root1 = *Root2 = -0.5f * b / a;
    }
}
#endif

inline b32 RaySphereIntersect(v3 RayOrig, v3 RayDir, sphere S, f32* tReturn)
{
    b32 Result = false;
    
    v3 L = RayOrig - S.Center;
    f32 a = Inner(RayDir, RayDir); // 1.0f
    f32 b = 2*Inner(RayDir, L);
    f32 c = Inner(L, L) - Square(S.Radius);

    f32 Root1 = F32_MIN;
    f32 Root2 = F32_MIN;
    SolveQuadratic(a, b, c, &Root1, &Root2);

    if (Root1 > Root2)
    {
        f32 Temp = Root1;
        Root1 = Root2;
        Root2 = Temp;
    }

    if (Root1 < 0.0f)
    {
        Root1 = Root2;
        if (Root1 < 0.0f)
        {
            return false;
        }
    }

    *tReturn = Root1;
    return true;
    
#if 0
    // TODO: This doesn't account for intersections at t = 0.0f
    f32 t = Min(Max(0.0f, Root1), Max(0.0f, Root2));
    
    if (t > 0.0f)
    {
        *tReturn = t;
        Result = true;
    }
#endif
    
//    return Result;
}
#endif

inline b32 RayPlaneIntersect(v3 RayOrig, v3 RayDir, plane P, f32* tReturn)
{
    b32 Result = false;
    f32 Denominator = Inner(RayDir, P.Normal);

    if (Denominator > 0.0000001f)
    {
        f32 Numerator = Inner((P.Pos - RayOrig), P.Normal);
        f32 t = Numerator / Denominator;
            
        if (t >= 0.0f)
        {
            *tReturn = t;
            Result = true;
        }
    }

    return Result;
}

inline b32 RayRectIntersect(v3 RayOrig, v3 RayDir, rect3 B, f32* tReturn)
{
    b32 Result = false;
    
    f32 MinT = F32_MIN;
    f32 MaxT = F32_MAX;

    if (RayDir.x != 0.0f)
    {
        f32 t0 = (B.Min.x - RayOrig.x) / RayDir.x;
        f32 t1 = (B.Max.x - RayOrig.x) / RayDir.x;

        MinT = Max(MinT, Min(t0, t1));
        MaxT = Min(MaxT, Max(t0, t1));
    }

    if (RayDir.y != 0.0f)
    {
        f32 t0 = (B.Min.y - RayOrig.y) / RayDir.y;
        f32 t1 = (B.Max.y - RayOrig.y) / RayDir.y;

        MinT = Max(MinT, Min(t0, t1));
        MaxT = Min(MaxT, Max(t0, t1));
    }

    if (RayDir.z != 0.0f)
    {
        f32 t0 = (B.Min.z - RayOrig.z) / RayDir.z;
        f32 t1 = (B.Max.z - RayOrig.z) / RayDir.z;

        MinT = Max(MinT, Min(t0, t1));
        MaxT = Min(MaxT, Max(t0, t1));
    }    

    Result = MaxT >= MinT;
    if (Result)
    {
        if (MinT < 0.0f)
        {
            *tReturn = MaxT;
        }
        else
        {
            *tReturn = MinT;
        }
    }
        
    return Result;
}

internal f32 TraceIndirectRay(game_state* GameState, ray Ray)
{
    f32 Result = 0.0f;
    f32 MinT = F32_MAX;
    
    camera Camera = GameState->Camera;
                
    for (u32 ObjectIndex = 0; ObjectIndex < GameState->ObjectArraySize; ++ObjectIndex)
    {
        world_object* Object = GameState->ObjectArray + ObjectIndex;

        f32 t = 0.0f;
        switch (Object->ModelType)
        {
            case ModelType_Plane:
            {
                plane P = Object->Plane;
                P.Pos -= Camera.Pos;
                        
                if (RayPlaneIntersect(Ray.Origin, Ray.Dir, P, &t))
                {
                    if (t > 0.0f)
                    {
                        MinT = Min(MinT, t);
                    }
                }                        
            } break;

            case ModelType_Rect3:
            {
                rect3 R = Translate(Object->Rect3, -Camera.Pos);
                        
                if (RayRectIntersect(Ray.Origin, Ray.Dir, R, &t))
                {
                    if (t > 0.0f)
                    {
                        MinT = Min(MinT, t);
                    }
                }
            } break;

            case ModelType_Sphere:
            {
                sphere S = Object->Sphere;
                S.Center -= Camera.Pos;
                
                if (RaySphereIntersect(Ray.Origin, Ray.Dir, S, &t))
                {
                    if (t > 0.0f)
                    {
                        MinT = Min(MinT, t);
                    }
                }   
            } break;
        }
    }

    Assert(MinT >= 0.0f);
    if (MinT < 1.0f)
    {
        Result = 0.0f;
    }
    else
    {
        Result = 1.0f;
    }

    return Result;
}

internal f32 SampleIndirectRays(game_state* GameState, ray Ray, v3 Origin)
{
    f32 Result = 0.0f;
    
    for (u32 LightIndex = 0; LightIndex < GameState->LightArraySize; ++LightIndex)
    {
        world_object* Light = GameState->LightArray + LightIndex;
        
        // TODO: Figure out a good epsilon
        ray IndirectRay = {};
        IndirectRay.Origin = Origin;
        IndirectRay.Dir = Light->PointLight.Pos - Origin;
                    
        Result += 0.5f*TraceIndirectRay(GameState, IndirectRay);
    }

    return Result;
}

struct trace_ray_result
{
    b32 Intersected;
    v3 Normal;
    v3 HitPos;
    u32 Color;
};

#define MAX_Z 1024.0f

internal trace_ray_result TraceCameraRay(game_state* GameState, ray Ray)
{
    trace_ray_result Result = {};
    
    camera Camera = GameState->Camera;
    
    f32 t = F32_MIN;
    b32 Intersected = false;
                
    for (u32 ObjectIndex = 0; ObjectIndex < GameState->ObjectArraySize && !Intersected;
         ++ObjectIndex)
    {
        Intersected = false;
                
        world_object* Object = GameState->ObjectArray + ObjectIndex;

        switch (Object->ModelType)
        {
            case ModelType_Plane:
            {
                plane P = Object->Plane;
                P.Pos -= Camera.Pos;
                
                if (RayPlaneIntersect(Ray.Origin, Ray.Dir, P, &t))
                {
                    Result.HitPos = Ray.Origin + t*Ray.Dir;
                    Result.Normal = Object->Plane.Normal;

                    // TODO: Figure out a good epsilon
                    v3 Origin = Ray.Origin + (t - 0.0001f)*Ray.Dir;
                    f32 Intensity = SampleIndirectRays(GameState, Ray, Origin);
                    Result.Color = (u32)(255.0f*Intensity) << 8;
                    
                    Result.Intersected = true;
                    Intersected = true;
                }                        
            } break;

            case ModelType_Rect3:
            {
                rect3 R = Translate(Object->Rect3, -Camera.Pos);
                
                if (RayRectIntersect(Ray.Origin, Ray.Dir, R, &t))
                {
                    Result.HitPos = Ray.Origin + t*Ray.Dir;
                    //Result.Normal = ;
                    //Result.Color = 0x1AA0;

                    Result.Intersected = true;
                    Intersected = true;
                }
            } break;

            case ModelType_Sphere:
            {
                sphere S = Object->Sphere;
                S.Center -= Camera.Pos;
                
                if (RaySphereIntersect(Ray.Origin, Ray.Dir, S, &t))
                {
                    Result.HitPos = Ray.Origin + t*Ray.Dir;
                    Result.Normal = Normalize(Result.HitPos - Object->Sphere.Center);

                    // TODO: Figure out a good epsilon
                    v3 Origin = Ray.Origin + (t - 0.0001f)*Ray.Dir;
                    f32 Intensity = SampleIndirectRays(GameState, Ray, Origin);
                    Result.Color = (u32)(255.0f*Intensity);
                    
                    Result.Intersected = true;
                    Intersected = true;
                }   
            } break;
        }
    }

    return Result;
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    game_state* GameState = (game_state*)GameMem->PermanentMem;
    PlatformApi = GameMem->PlatformApi;

    if (!GameMem->IsInitialized)
    {
        GameState->Arena = InitArena((u8*)GameState + sizeof(game_state),
                                     GameMem->PermanentMemSize - sizeof(game_state));

        // NOTE: Init camera
        GameState->Camera.Pos = V3(0, 0, 0);

        // NOTE: Init list of lights in our world
        GameState->LightArraySize = 1;
        GameState->LightArray = PushArray(&GameState->Arena, world_object, GameState->LightArraySize);

        world_object* CurrLight = GameState->LightArray;
        CurrLight->ModelType = ModelType_PointLight;
        CurrLight->PointLight.Pos = V3(0, 20, 20);
        CurrLight->PointLight.Color = V3(1, 0, 0);
        ++CurrLight;

#if 0
        CurrLight->ModelType = ModelType_PointLight;
        CurrLight->PointLight.Pos = V3(-20, 0, 20);
        CurrLight->PointLight.Color = V3(1, 0, 0);
        ++CurrLight;
#endif
        
        // NOTE: Init list of objects in our world
        GameState->ObjectArraySize = 2;
        GameState->ObjectArray = PushArray(&GameState->Arena, world_object, GameState->ObjectArraySize);

        world_object* CurrObject = GameState->ObjectArray;

#if 1
        CurrObject->ModelType = ModelType_Sphere;
        CurrObject->Sphere.Center = V3(0, 0, 40);
        CurrObject->Sphere.Radius = 10.0f;
        ++CurrObject;
#endif
        
#if 0
        CurrObject->ModelType = ModelType_Rect3;
        CurrObject->Rect3 = RectCenterDim(V3(-20, 0, 10), V3(1, 1, 1));
        ++CurrObject;
#endif
        
        CurrObject->ModelType = ModelType_Plane;
        CurrObject->Plane.Normal = Normalize(V3(0, -1, 0));
        CurrObject->Plane.Pos = V3(0, -1, 0);
        
        // NOTE: Init ray direction vectors
        GameState->RayDir = PushArray(&GameState->Arena, v3, Display->Width*Display->Height);

        v2 PixelDim = V2(2, 2) / V2i(Display->Width, Display->Height);
        v3 RayOrigin = {};
        for (u32 Y = 0; Y < Display->Height; ++Y)
        {
            for (u32 X = 0; X < Display->Width; ++X)
            {
                v3* RayDir = GameState->RayDir + Display->Width*Y + X;
                RayDir->x = -1.0f + 0.5f*PixelDim.x + X*PixelDim.x;
                RayDir->y = -1.0f + 0.5f*PixelDim.y + Y*PixelDim.y;
                RayDir->z = 1.0f;

                *RayDir = *RayDir - RayOrigin;
                *RayDir = Normalize(*RayDir);
            }
        }
        
        GameMem->IsInitialized = true;
    }

    camera* Camera = &GameState->Camera;
    ClearScreen(Display);

    local_global f32 SINT = 0.0f;

    world_object* CurrLight = GameState->LightArray;
    //CurrLight->PointLight.Pos = V3(10.0f, 0.0f, 10.0f*sinf(SINT) + 5.0f);
    CurrLight->PointLight.Pos = V3(-1000.0f, 0.0f, 20.0f);
    ++CurrLight;

    //CurrLight->PointLight.Pos = V3(-10.0f, 0.0f, 10.0f*cosf(SINT) + -5.0f);
    //CurrLight->PointLight.Pos = V3(-10.0f, 0.0f, 40.0f);
    
    SINT += 0.1f;
    
    //local_global f32 t = 0.0f;
    //Camera->Pos = 100.0f*V3(sinf(t), 0.0f, cosf(t)) + V3(0, 0, -50);
    //t += 0.01f;
        
    v3 RayOrigin = {};
    for (u32 Y = 0; Y < Display->Height; ++Y)
    {
        for (u32 X = 0; X < Display->Width; ++X)
        {
            v3 RayDir = GameState->RayDir[Display->Width*Y + X];

            ray Ray = {};
            Ray.Dir = RayDir;

            if (X == 134 && Y == 95)
            {
                i32 i = 0;
            }
            
            trace_ray_result Result = TraceCameraRay(GameState, Ray);

            if (Result.Intersected)
            {
                u32* Pixel = Display->FrameBuffer + Y*Display->Width + X;

                *Pixel = Result.Color;

                if (X == 134 && Y == 95)
                {
                    *Pixel = 0xFFFFFFFF;
                }
                
                //*Pixel = (u32)((Result.HitPos.z / 30.0f) * 1024.0f);
                
                //*Pixel = (u32)((Phit.z / MAX_Z) * 255.0f);
            
                //Display->FrameBuffer[Y*Display->Width + X] = (u32)(RayDir.x*RayDir.y*9999.0f) & 0xFF;
            }          
        }
    }
}

extern "C" GAME_GET_SOUND_SAMPLES(GameGetSoundSamples)
{
#if 0
    local_global f32 tSine = 0.0f;

    i16* DestSample = SoundBuffer->Memory;
    for (u32 SampleIndex = 0; SampleIndex < SoundBuffer->SamplesToWrite; ++SampleIndex)
    {
        i16 SourceSample = (i16)(sinf(tSine) * 3000.0f);
        *DestSample++ = SourceSample;
        *DestSample++ = SourceSample;

        tSine += 0.03f;
        if (tSine > Tau32)
        {
            tSine = 0.0f;
        }
    }
#endif
}
