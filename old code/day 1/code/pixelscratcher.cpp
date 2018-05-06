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

struct sphere
{
    v3 Center;
    f32 Radius;
};

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

inline b32 RaySphereIntersect(v3 RayOrig, v3 RayDir, sphere S)
{
    v3 L = RayOrig - S.Center;
    f32 a = Inner(RayDir, RayDir);
    f32 b = 2*Inner(RayDir, L);
    f32 c = Inner(L, L) - Square(S.Radius);

    f32 Root1 = F32_MIN;
    f32 Root2 = F32_MIN;
    SolveQuadratic(a, b, c, &Root1, &Root2);

    f32 t = Max(Root1, Root2);
    if (t >= 0.0f)
    {
        return true;
    }

    return false;
}
#endif

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    game_state* GameState = (game_state*)GameMem->PermanentMem;
    PlatformApi = GameMem->PlatformApi;

    if (!GameMem->IsInitialized)
    {
        GameState->Arena = InitArena((u8*)GameState + sizeof(game_state),
                                     GameMem->PermanentMemSize - sizeof(game_state));
        GameState->RayDir = PushArray(&GameState->Arena, v3, Display->Width*Display->Height);

        // NOTE: Init ray direction vectors
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

    sphere S = {};
    S.Center = V3(0, 0, 10);
    S.Radius = 2.0f;

    v3 RayOrigin = {};
    for (u32 Y = 0; Y < Display->Height; ++Y)
    {
        for (u32 X = 0; X < Display->Width; ++X)
        {
            v3 RayDir = GameState->RayDir[Display->Width*Y + X];

            if (RaySphereIntersect(RayOrigin, RayDir, S))
            {
                Display->FrameBuffer[Y*Display->Width + X] = 0;
            }
            
            //Display->FrameBuffer[Y*Display->Width + X] = (u32)(RayDir.x*RayDir.y*9999.0f) & 0xFF;
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
