#if !defined(PIXELSCRATCHER_RENDER_H)

enum render_command_type
{
    RenderCommandType_Clear,
    RenderCommandType_Rect,
};

struct render_clear
{
    v4 Color;
};

struct render_rect
{
    rect2 Bounds;
    v4 Color;
};

struct render_command_buffer
{
    u32 WindowWidth;
    u32 WindowHeight;
    u32 Width;
    u32 Height;
    
    mem_arena Arena;
};

#define PIXELSCRATCHER_RENDER_H
#endif
