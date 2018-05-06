#if !defined(WIN32_PIXELSCRATCHER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Ihor Szlachtycz $
   $Notice: (C) Copyright 2014 by Dream.Inc, Inc. All Rights Reserved. $
   ======================================================================== */

struct win32_display
{
    u32 Width;
    u32 Height;
    u32 TargetFrameRate;
    u32* FrameBuffer;
    BITMAPINFO BitmapInfo;
};

struct win32_game_code
{
    char* SourceDLLPath;
    char* TempDLLPath;
    char* LockFilePath;
    
    HMODULE GameCodeDLL;
    FILETIME LastDLLFileTime;

    // IMPORTANT: Either of the callbacks can be null
    // You must check before calling
    game_update_and_render* UpdateAndRender;
    game_get_sound_samples* GetSoundSamples;
};

#define WIN32_PIXELSCRATCHER_H
#endif
