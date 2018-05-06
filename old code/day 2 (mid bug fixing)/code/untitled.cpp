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

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    game_state* GameState = (game_state*)GameMem->PermanentMem;
    PlatformApi = GameMem->PlatformApi;

    GameState->RenderCommandBuffer = InitRenderCommandBuffer(ScreenBuffer->Width, ScreenBuffer->Height,
                                                             ScreenBuffer->WindowWidth, ScreenBuffer->WindowHeight,
                                                             (u8*)GameState + sizeof(game_state), KiloBytes(100));
    render_command_buffer* RenderCmdBuffer = &GameState->RenderCommandBuffer;

    PushClear(RenderCmdBuffer, V4(1, 0, 1, 1));
    PushRect(RenderCmdBuffer, RectMinMax(V2(4, 4), V2(50, 50)), V4(0, 1, 1, 1));
    
    PlatformApi.RenderCommandsToOutput(RenderCmdBuffer);
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
