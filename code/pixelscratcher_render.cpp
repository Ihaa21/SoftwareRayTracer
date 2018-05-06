inline render_command_buffer InitRenderCommandBuffer(u32 Width, u32 Height, u32 WindowWidth,
                                                     u32 WindowHeight, void* Mem, mm Size)
{
    render_command_buffer Result = {};
    Result.Width = Width;
    Result.Height = Height;
    Result.WindowWidth = WindowWidth;
    Result.WindowHeight = WindowHeight;

    Result.Arena = InitArena(Mem, Size);

    return Result;
}

inline void PushClear(render_command_buffer* RenderCmdBuffer, v4 Color)
{
    u32* RenderCommandType = PushStruct(&RenderCmdBuffer->Arena, u32);
    render_clear* RenderClear = PushStruct(&RenderCmdBuffer->Arena, render_clear);

    *RenderCommandType = RenderCommandType_Clear;
    RenderClear->Color = Color;
}

inline void PushRect(render_command_buffer* RenderCmdBuffer, rect2 Bounds, v4 Color)
{
    u32* RenderCommandType = PushStruct(&RenderCmdBuffer->Arena, u32);
    render_rect* RenderRect = PushStruct(&RenderCmdBuffer->Arena, render_rect);

    *RenderCommandType = RenderCommandType_Rect;
    RenderRect->Bounds = Bounds;
    RenderRect->Color = Color;
}
