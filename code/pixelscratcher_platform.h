#if !defined(PIXELSCRATCHER_PLATFORM_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Ihor Szlachtycz $
   $Notice: (C) Copyright 2014 by Dream.Inc, Inc. All Rights Reserved. $
   ======================================================================== */

#include <stdint.h>
#include <cfloat>

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef size_t mm;
typedef uintptr_t umm;

typedef int32_t b32;

#define internal static
#define global static
#define local_global static

#define F32_MAX FLT_MAX
#define F32_MIN -F32_MAX
#define Pi32 3.14159265359f
#define Tau32 6.28318530717958647692f

#define Assert(Expression) if (!(Expression)) {*(int*)0 = 0;}
#define InvalidCodePath Assert(!"Invalid Code Path")
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

#define KiloBytes(Val) ((Val)*1024LL)
#define MegaBytes(Val) (KiloBytes(Val)*1024LL)
#define GigaBytes(Val) (MegaBytes(Val)*1024LL)

#include "pixelscratcher_string.h"
#include "pixelscratcher_math.cpp"
#include "pixelscratcher_memory.h"
#include "pixelscratcher_render.h"

//
//
//

struct game_input
{
    v2 MouseP;
    
    union
    {
        struct
        {
            b32 MoveLeftDown;
            b32 MoveRightDown;
            b32 MoveUpDown;
            b32 MoveDownDown;
        };

        b32 ButtonsPressed[4];
    };
};

struct game_display
{
    u32 Width;
    u32 Height;
    u32 TargetFrameRate;
    u32* FrameBuffer;
};

struct game_memory
{
    mm PermanentMemSize;
    void* PermanentMem;

    b32 IsInitialized;
};

//
//
//

#define GAME_UPDATE_AND_RENDER(name) void name(game_memory* GameMem, game_input* Input, game_display* Display)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);

//
//
//

#define PIXELSCRATCHER_PLATFORM_H
#endif
