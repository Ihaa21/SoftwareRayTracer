/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Ihor Szlachtycz $
   $Notice: (C) Copyright 2014 by Dream.Inc, Inc. All Rights Reserved. $
   ======================================================================== */

#include <gl/glext.h>
#include <gl/glcorearb.h>

#if 0
#define GL_SRGB8_ALPHA8 0x8C43
#define GL_FRAMEBUFFER_SRGB 0x8DB9

#define GL_SHADING_LANGUAGE_VERSION 0x8B8C
#endif

global PFNGLGENFRAMEBUFFERSPROC glGenFramebuffers;
global PFNGLBINDFRAMEBUFFERPROC glBindFramebuffer;
global PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
global PFNGLBLITFRAMEBUFFERPROC glBlitFramebuffer;
global PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffers;
global PFNGLDRAWBUFFERSPROC glDrawBuffers;
global PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;

struct opengl_info
{
    char* Vendor;
    char* Renderer;
    char* Version;
    char* ShadingLanguageVersion;
    char* Extensions;
};

internal void GLInit()
{   
    opengl_info GLInfo = {};
    GLInfo.Vendor = (char*)glGetString(GL_VENDOR);
    GLInfo.Renderer = (char*)glGetString(GL_RENDERER);
    GLInfo.Version = (char*)glGetString(GL_VERSION);
    GLInfo.ShadingLanguageVersion = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);    
    GLInfo.Extensions = (char*)glGetString(GL_EXTENSIONS);

    b32 SupportsFBO = false;
    char* At = GLInfo.Extensions;
    while (*At)
    {
        // TODO: Make this into a function?
        while (IsWhiteSpace(*At))
        {
            ++At;
        }

        char* End = At;

        while (*End && !IsWhiteSpace(*End))
        {
            ++End;
        }

        umm Count = End - At;

#if 0
        if (StringsAreEqual(Count, At, "GL_EXT_framebuffer_sRGB"))
        {
            Result.GL_EXT_framebuffer_sRGB = true;
        }
        else if (StringsAreEqual(Count, At, "GL_EXT_texture_sRGB"))
        {
            Result.GL_EXT_texture_sRGB = true;
        }
        else if (StringsAreEqual(Count, At, "GL_ARB_framebuffer_sRGB"))
        {
            Result.GL_EXT_framebuffer_sRGB = true;
        }
#endif
        if (StringsAreEqual(Count, At, "GL_ARB_framebuffer_object"))
        {
            SupportsFBO = true;
        }

        At = End;
    }

    Assert(SupportsFBO);
    
    OpenGLDefaultInternalTextureFormat = GL_RGBA8;
}

inline void GLSetScreenSpace(i32 Width, i32 Height)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    f32 a = 2.0f / (f32)Width;
    f32 b = 2.0f / (f32)Height;
    f32 Proj[] =
        {
             a,   0,  0,  0,
             0,   b,  0,  0,
             0,   0,  1,  0,
            -1,  -1,  0,  1,
        };
    glLoadMatrixf(Proj);
}

inline void GLRectangle(u32 Height, rect2 Rect, v4 Color, v2 MinUV = V2(0, 0), v2 MaxUV = V2(1, 1))
{
    v2 MinP = Rect.Min;
    v2 MaxP = Rect.Max;
    
    MinP.y = (f32)Height - MinP.y;
    MaxP.y = (f32)Height - MaxP.y;
    
    glBegin(GL_TRIANGLES);

    glColor4f(Color.r, Color.g, Color.b, Color.a);
                
    glTexCoord2f(MinUV.x, MinUV.y); 
    glVertex2f(MinP.x, MinP.y);
                
    glTexCoord2f(MaxUV.x, MinUV.y);
    glVertex2f(MaxP.x, MinP.y);
                
    glTexCoord2f(MaxUV.x, MaxUV.y);
    glVertex2f(MaxP.x, MaxP.y);
    
    glTexCoord2f(MinUV.x, MinUV.y); 
    glVertex2f(MinP.x, MinP.y);
                
    glTexCoord2f(MaxUV.x, MaxUV.y);
    glVertex2f(MaxP.x, MaxP.y);
                
    glTexCoord2f(MinUV.x, MaxUV.y);
    glVertex2f(MinP.x, MaxP.y);
                
    glEnd();
}

inline void GLCheckError()
{
    GLuint InvalidEnum = GL_INVALID_ENUM;
    GLuint InvalidValue = GL_INVALID_VALUE;
    GLuint InvalidOperation = GL_INVALID_OPERATION;
    GLuint InvalidFrameBufferOperation = GL_INVALID_FRAMEBUFFER_OPERATION;
    GLuint OutOfMemory = GL_OUT_OF_MEMORY;

    GLuint ErrorCode = glGetError();
    Assert(ErrorCode == GL_NO_ERROR);
}

global GLuint GlobalFrameTexture = 1;
global GLuint GlobalFrameBuffer = 1; 

RENDER_COMMANDS_TO_OUTPUT(GLRenderCommandsToOutput)
{
    mm CurrentByte = 0;
    mem_arena RenderArena = RenderCommandBuffer->Arena;
    
    //glViewport(0, 0, RenderCommandBuffer->WindowWidth, RenderCommandBuffer->WindowHeight);
    glViewport(0, 0, RenderCommandBuffer->Width, RenderCommandBuffer->Height);
    glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    glGenTextures(1, &GlobalFrameTexture);
    glBindTexture(GL_TEXTURE_2D, GlobalFrameTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, OpenGLDefaultInternalTextureFormat,
                 RenderCommandBuffer->Width, RenderCommandBuffer->Height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glGenFramebuffers(1, &GlobalFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, GlobalFrameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, GlobalFrameTexture, 0);

    Assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    GLSetScreenSpace(RenderCommandBuffer->Width, RenderCommandBuffer->Height);
    glBindFramebuffer(GL_FRAMEBUFFER, GlobalFrameBuffer);

    while (CurrentByte < RenderArena.Used)
    {
        u32* CommandType = (u32*)((u8*)RenderArena.Mem + CurrentByte);
        CurrentByte += sizeof(u32);

        void* RenderCommand = (u8*)RenderArena.Mem + CurrentByte;
        
        switch (*CommandType)
        {
            case RenderCommandType_Clear:
            {
                render_clear* RenderClear = (render_clear*)RenderCommand;
                CurrentByte += sizeof(render_clear);

                v4 Color = RenderClear->Color;
                glClearColor(Color.r, Color.g, Color.b, Color.a);
                glClear(GL_COLOR_BUFFER_BIT);
            } break;

            case RenderCommandType_Rect:
            {
                render_rect* RenderRect = (render_rect*)RenderCommand;
                CurrentByte += sizeof(render_rect);

                GLRectangle(RenderCommandBuffer->Height, RenderRect->Bounds, RenderRect->Color);
            } break;
        }
    }
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // NOTE: Draw our frame buffer texture as a fullscreen quad
    u32 WindowWidth = RenderCommandBuffer->WindowWidth;
    u32 WindowHeight = RenderCommandBuffer->WindowHeight;
    
    glBindTexture(GL_TEXTURE_2D, GlobalFrameTexture);
    glViewport(0, 0, WindowWidth, WindowHeight);
    GLSetScreenSpace(WindowWidth, WindowHeight);
    GLRectangle(RenderCommandBuffer->WindowHeight,
                RectMinMax(V2(0, 0), V2((f32)WindowWidth, (f32)WindowHeight)), V4(1, 1, 1, 1));
    glBindTexture(GL_TEXTURE_2D, 0);
    
#if 1
    glDeleteTextures(1, &GlobalFrameTexture);
    glDeleteFramebuffers(1, &GlobalFrameBuffer);
#endif

    GLCheckError();
}
