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
            *Pixel = 0xFF00FF00;
            ++Pixel;
        }

        PixelRow += Display->Width;
    }
}

#if 0
inline b32 RaySphereIntersect(v3 RayOrig, v3 RayDir, sphere S)
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
        *Root1 = -0.5f * b / a;
    }
}
#endif

inline b32 RaySphereIntersect(v3 RayOrig, v3 RayDir, sphere S, f32* tReturn)
{
    b32 Result = false;
    
    v3 L = RayOrig - S.Center;
    f32 a = 1.0f; //Inner(RayDir, RayDir);
    f32 b = 2*Inner(RayDir, L);
    f32 c = Inner(L, L) - Square(S.Radius);

    f32 Root1 = F32_MIN;
    f32 Root2 = F32_MIN;
    SolveQuadratic(a, b, c, &Root1, &Root2);

    f32 t = Max(Root1, Root2);
    if (t >= 0.0f)
    {
        *tReturn = t;
        Result = true;
    }

    return Result;
}
#endif

inline b32 RayPlaneIntersect(v3 RayOrig, v3 RayDir, plane P, f32* tReturn)
{
    b32 Result = false;
    f32 Denominator = Inner(RayDir, P.Normal);

    if (Denominator > 1e-6)
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

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    game_state* GameState = (game_state*)GameMem->PermanentMem;
    PlatformApi = GameMem->PlatformApi;

    if (!GameMem->IsInitialized)
    {
        GameState->Arena = InitArena((u8*)GameState + sizeof(game_state),
                                     GameMem->PermanentMemSize - sizeof(game_state));

        // NOTE: Init list of objects in our world
        GameState->ObjectArraySize = 3;
        GameState->ObjectArray = PushArray(&GameState->Arena, world_object, GameState->ObjectArraySize);

        world_object* CurrObject = GameState->ObjectArray;
        CurrObject->ModelType = ModelType_Sphere;
        CurrObject->Sphere.Center = V3(4, 0, 80);
        CurrObject->Sphere.Radius = 40.0f;
        ++CurrObject;

        CurrObject->ModelType = ModelType_Rect3;
        CurrObject->Rect3 = RectCenterDim(V3(-40, 0, 55), V3(6, 6, 6));
        ++CurrObject;

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
                RayDir->x = -1.0f + 0.5f*PixelDim.x + (X - 1)*PixelDim.x;
                RayDir->y = -1.0f + 0.5f*PixelDim.y + (Y - 1)*PixelDim.y;
                RayDir->z = 1.0f;

                *RayDir = *RayDir - RayOrigin;
                *RayDir = Normalize(*RayDir);
            }
        }
        
        GameMem->IsInitialized = true;
    }
    
    ClearScreen(Display);

    f32 MaxZ = 256.0f;
    f32 Temp = 0.0f;
    
    v3 RayOrigin = {};
    for (u32 Y = 0; Y < Display->Height; ++Y)
    {
        for (u32 X = 0; X < Display->Width; ++X)
        {
            v3 RayDir = GameState->RayDir[Display->Width*Y + X];

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
                        if (RayPlaneIntersect(RayOrigin, RayDir, Object->Plane, &t))
                        {
                            v3 Phit = RayOrigin + t*RayDir;
                            v3 Pnormal = Object->Plane.Normal;

                            Temp = Max(Temp, Phit.z);

                            u32* Pixel = Display->FrameBuffer + Y*Display->Width + X;
                            //*Pixel = (u32)(Pnormal.x*Pnormal.y*Pnormal.z*9999.0f) & 0xFF;
                            *Pixel = (u32)((Phit.z / MaxZ) * 255.0f);
                            //*Pixel = 0xFF00;

                            Intersected = true;
                        }                        
                    } break;

                    case ModelType_Rect3:
                    {
                        if (RayRectIntersect(RayOrigin, RayDir, Object->Rect3, &t))
                        {
                            v3 Phit = RayOrigin + t*RayDir;
                            //v3 Pnormal = ;

                            Temp = Max(Temp, Phit.z);

                            u32* Pixel = Display->FrameBuffer + Y*Display->Width + X;
                            //*Pixel = (u32)(Pnormal.x*Pnormal.y*Pnormal.z*9999.0f) & 0xFF;
                            *Pixel = (u32)((Phit.z / MaxZ) * 255.0f);
                            //*Pixel = 0xFF;

                            Intersected = true;
                        }
                    } break;

                    case ModelType_Sphere:
                    {
                        if (RaySphereIntersect(RayOrigin, RayDir, Object->Sphere, &t))
                        {
                            v3 Phit = RayOrigin + t*RayDir;
                            v3 Pnormal = Normalize(Phit - Object->Sphere.Center);

                            Temp = Max(Temp, Phit.z);

                            u32* Pixel = Display->FrameBuffer + Y*Display->Width + X;
                            //*Pixel = (u32)(Pnormal.x*Pnormal.y*Pnormal.z*9999.0f) & 0xFF;
                            *Pixel = (u32)((Phit.z / MaxZ) * 255.0f);

                            Intersected = true;
                        }   
                    } break;
                }
            }
            
            //Display->FrameBuffer[Y*Display->Width + X] = (u32)(RayDir.x*RayDir.y*9999.0f) & 0xFF;
        }
    }

    i32 i = 0;
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
