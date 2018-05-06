#include <stdio.h>
#include <windows.h>
#include <dsound.h>

#include "pixelscratcher_platform.h"
#include "win32_pixelscratcher.h"

//
//
//

#define DIRECT_SOUND_CREATE(name) HRESULT WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND* ppDS, LPUNKNOWN pUnkOuter);
typedef DIRECT_SOUND_CREATE(direct_sound_create);

//
//
//

global b32 GlobalRunning = true;
global b32 GlobalShowCursor = true;

global i64 GlobalTimerFrequency;

global LPDIRECTSOUNDBUFFER GlobalSecondaryBuffer;
global b32 GlobalIsSoundPlaying = true;

global HDC GlobalDeviceContext;

//
//
//

inline LARGE_INTEGER Win32GetClock()
{
    LARGE_INTEGER Result;
    QueryPerformanceCounter(&Result);

    return Result;
}

inline f32 Win32GetSecondsBetween(LARGE_INTEGER End, LARGE_INTEGER Start)
{
    f32 Result = ((f32)(End.QuadPart - Start.QuadPart) / (f32)GlobalTimerFrequency);
    return Result;
}

inline FILETIME Win32GetLastFileTime(char* FileName)
{
    FILETIME LastWriteTime = {};

    WIN32_FILE_ATTRIBUTE_DATA FileData;
    if (GetFileAttributesEx(FileName, GetFileExInfoStandard, &FileData))
    {
        LastWriteTime = FileData.ftLastWriteTime;
    }

    return LastWriteTime;
}

internal void Win32DisplayBufferInWindow(HWND Window, win32_display* Display)
{
    RECT WindowRect;
    GetClientRect(Window, &WindowRect);
    u32 WindowWidth = WindowRect.right - WindowRect.left;
    u32 WindowHeight = WindowRect.bottom - WindowRect.top;
        
    StretchDIBits(GlobalDeviceContext,
                  0, 0, WindowWidth, WindowHeight,
                  0, 0, Display->Width, Display->Height,
                  Display->FrameBuffer, &Display->BitmapInfo, DIB_RGB_COLORS, SRCCOPY);
}

internal LRESULT CALLBACK Win32MainWindowCallBack(HWND Window, UINT Message, WPARAM WParam,
                                                  LPARAM LParam)
{
    LRESULT Result = 0;

    switch (Message)
    {
        case WM_CLOSE:
        case WM_DESTROY:
        {
            GlobalRunning = false;
        } break;

        case WM_SETCURSOR:
        {
            if (GlobalShowCursor)
            {
                Result = DefWindowProc(Window, Message, WParam, LParam);
            }
            else
            {
                SetCursor(0);
            }
        } break;

        case WM_MOVE:
        {
#if 1
            if (GlobalSecondaryBuffer)
            {
                GlobalSecondaryBuffer->Stop();
                GlobalIsSoundPlaying = false;
            }
#endif
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;

#if 0
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            Win32DisplayBufferInWindow(Window);
            EndPaint(Window, &Paint);
        } break;
#endif
        
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }

    return Result;
}

inline DWORD Win32GetNearestFrameByte(DWORD SoundBytesPerFrame, DWORD CurrentSoundByte)
{
    // TODO: Implement a floor function here for NearestFrameCount
    DWORD NearestFrameCount = (DWORD)((CurrentSoundByte + SoundBytesPerFrame) / SoundBytesPerFrame);
    DWORD Result = NearestFrameCount*SoundBytesPerFrame;
    return Result;
}

inline i16* Win32WriteSoundSamples(i16* SourceSample, DWORD RegionSize, DWORD BytesPerSample,
                                   i16* DestSample)
{
    i16* Result = SourceSample;
    DWORD RegionSampleCount = RegionSize / BytesPerSample;
    for (DWORD SampleIndex = 0; SampleIndex < RegionSampleCount; ++SampleIndex)
    {
        *DestSample++ = *Result++;
        *DestSample++ = *Result++;
    }

    return Result;
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    
    WNDCLASSA WindowClass = {};
    WindowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    WindowClass.lpfnWndProc = Win32MainWindowCallBack;
    WindowClass.hInstance = hInstance;
    WindowClass.hCursor = LoadCursor(0, IDC_ARROW);
    WindowClass.lpszClassName = "PixelscratcherClass";

    if (!RegisterClassA(&WindowClass))
    {
        // TODO: Display error message
        InvalidCodePath;
    }

    HWND Window = CreateWindowExA(0,
                                  WindowClass.lpszClassName,
                                  "Haw Yeah",
                                  WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                                  CW_USEDEFAULT,
                                  CW_USEDEFAULT,
                                  256*2,
                                  256*2,
                                  0,
                                  0,
                                  hInstance,
                                  0);

    if (!Window)
    {
        // TODO: Display error message
        InvalidCodePath;
    }

    GlobalDeviceContext = GetDC(Window);
    mm ProgramMemorySize = 0;

    //
    // NOTE: Create bitmap that we are going to draw to
    //

    win32_display Display = {};
    Display.Width = 256;
    Display.Height = 256;
    Display.TargetFrameRate = 60;
    Display.FrameBuffer = 0;
    mm RenderMemorySize = Display.Width * Display.Height * 4;
    
    BITMAPINFOHEADER* BitmapHeader = &Display.BitmapInfo.bmiHeader;
    BitmapHeader->biSize = sizeof(Display.BitmapInfo.bmiHeader);
    BitmapHeader->biWidth = Display.Width;
    BitmapHeader->biHeight = Display.Height;
    BitmapHeader->biPlanes = 1;
    BitmapHeader->biBitCount = 32;
    BitmapHeader->biCompression = BI_RGB;

    ProgramMemorySize += RenderMemorySize;
    
    //
    // NOTE: Init our sound buffers
    //

    // TODO: Currently, we set the latency sound bytes to be the start value of our CurrentSoundByte.
    // It may be better on the first or second frame (or before that) to measure the latency and
    // cap this value to the nearest next frame
    DWORD SamplesPerSecond = 48000;
    DWORD BytesPerSample = sizeof(i16)*2;
    DWORD SoundBytesPerFrame = (SamplesPerSecond / Display.TargetFrameRate) * BytesPerSample;
    DWORD CurrentSoundByte = 2*SoundBytesPerFrame;
    DWORD SampleBufferSize = SamplesPerSecond*BytesPerSample;
    DWORD PrevWriteCursor = 0;
    i16* SampleBuffer = 0;

    ProgramMemorySize += (mm)SampleBufferSize;
    
    HMODULE DSoundLib = LoadLibraryA("dsound.dll");
    if (!DSoundLib)
    {
        // TODO: Display error message
        InvalidCodePath;
    }

    direct_sound_create* DirectSoundCreate = (direct_sound_create*)GetProcAddress(DSoundLib, "DirectSoundCreate");
    LPDIRECTSOUND DirectSound = 0;
    // NOTE: DSSCL_PRIORITY lets us set the sound format
    if (!(DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &DirectSound, 0)) &&
          SUCCEEDED(DirectSound->SetCooperativeLevel(Window, DSSCL_PRIORITY))))
    {
        // TODO: Display error message
        InvalidCodePath;
    }

    DSBUFFERDESC BufferDescription = {};
    BufferDescription.dwSize = sizeof(BufferDescription);
    BufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;
    
    WAVEFORMATEX WaveFormat = {}; // NOTE: For more than 2 channels, use WAVEFORMATEXTENSIBLE
    WaveFormat.wFormatTag = WAVE_FORMAT_PCM; // NOTE: This gives us 2 channels only
    WaveFormat.nChannels = 2;
    WaveFormat.nSamplesPerSec = SamplesPerSecond;
    WaveFormat.wBitsPerSample = 16;
    WaveFormat.nBlockAlign = (WaveFormat.nChannels*WaveFormat.wBitsPerSample) / 8;
    WaveFormat.nAvgBytesPerSec = WaveFormat.nSamplesPerSec*WaveFormat.nBlockAlign;

    LPDIRECTSOUNDBUFFER PrimaryBuffer;
    if (!(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &PrimaryBuffer, 0)) &&
          SUCCEEDED(PrimaryBuffer->SetFormat(&WaveFormat))))
    {
        // TODO: Display error message
        InvalidCodePath;
    }

    BufferDescription = {};
    BufferDescription.dwSize = sizeof(BufferDescription);
    BufferDescription.dwFlags = DSBCAPS_GETCURRENTPOSITION2;
    BufferDescription.dwBufferBytes = (DWORD)SampleBufferSize;
    BufferDescription.lpwfxFormat = &WaveFormat;
 
    if (!(SUCCEEDED(DirectSound->CreateSoundBuffer(&BufferDescription, &GlobalSecondaryBuffer, 0))))
    {
        // TODO: Display error message
        InvalidCodePath;
    }

    GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);

    //
    // NOTE: Setup game executable
    //

    win32_game_code GameCode = {};

    char SourceDLLPath[256] = "W:\\pixelscratcher\\build_win32\\pixelscratcher.dll";
    GameCode.SourceDLLPath = SourceDLLPath;

    char TempDLLPath[256] = "W:\\pixelscratcher\\build_win32\\pixelscratcher_temp.dll";
    GameCode.TempDLLPath = TempDLLPath;

    char LockFilePath[256] = "W:\\pixelscratcher\\build_win32\\lock.tmp";
    GameCode.LockFilePath = LockFilePath;
    
    //
    // NOTE: Game memory
    //

    game_memory GameMem = {};
    GameMem.PermanentMemSize = MegaBytes(100);
    GameMem.TransientMemSize = MegaBytes(100);
    GameMem.DebugMemSize = 0;
    GameMem.IsInitialized = false;
    
    ProgramMemorySize += GameMem.PermanentMemSize + GameMem.TransientMemSize + GameMem.DebugMemSize;

    //
    // NOTE: Allocate program memory
    //

    void* ProgramMemory = VirtualAlloc(0, ProgramMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!ProgramMemory)
    {
        // TODO: Display error message
        InvalidCodePath;
    }

    Display.FrameBuffer = (u32*)ProgramMemory;
    SampleBuffer = (i16*)((u8*)Display.FrameBuffer + RenderMemorySize);
    GameMem.PermanentMem = (void*)((u8*)SampleBuffer + SampleBufferSize);
    GameMem.TransientMem = (void*)((u8*)GameMem.PermanentMem + GameMem.PermanentMemSize);
    GameMem.DebugMem = (void*)((u8*)GameMem.TransientMem + GameMem.TransientMemSize);

    // NOTE: Set windows timing state to increase resolution of timing calls
    //UINT TimingResolutionMS = 1;
    //b32 HighResSleep = timeBeginPeriod(TimingResolutionMS) == TIMERR_NOERROR;

    LARGE_INTEGER TimerFrequency;
    QueryPerformanceFrequency(&TimerFrequency);
    GlobalTimerFrequency = TimerFrequency.QuadPart;
    
    LARGE_INTEGER LastFrameTime = Win32GetClock();
            
    while (GlobalRunning)
    {
        if (!GlobalIsSoundPlaying)
        {
            GlobalSecondaryBuffer->Play(0, 0, DSBPLAY_LOOPING);
            GlobalIsSoundPlaying = true;
        }

        // NOTE: Update platform api function pointers
        //GameMem.PlatformApi.RenderCommandsToOutput = GLRenderCommandsToOutput;
        
        // NOTE: Reload game code DLL if the DLL changed
        FILETIME NewDLLFileTime = Win32GetLastFileTime(GameCode.SourceDLLPath);
        if (CompareFileTime(&NewDLLFileTime, &GameCode.LastDLLFileTime) != 0)
        {
            // NOTE: Unload the game code
            if (GameCode.GameCodeDLL)
            {
                FreeLibrary(GameCode.GameCodeDLL);
                GameCode.GameCodeDLL = 0;
            }

            GameCode.UpdateAndRender = 0;
            GameCode.GetSoundSamples = 0;

            // NOTE: Load the game code
            b32 IsValid = false;
            for (u32 LoadTryIndex = 0; LoadTryIndex < 100 && !IsValid; ++LoadTryIndex)
            {
                WIN32_FILE_ATTRIBUTE_DATA Ignored;
    
                // NOTE: We check if the lock file exists. The lock file is there so that we
                // don't load the dll before the pdb
                if (!GetFileAttributesEx(GameCode.LockFilePath, GetFileExInfoStandard, &Ignored))
                {
                    GameCode.LastDLLFileTime = Win32GetLastFileTime(GameCode.SourceDLLPath);
                    CopyFileA(GameCode.SourceDLLPath, GameCode.TempDLLPath, FALSE);
                    GameCode.GameCodeDLL = LoadLibraryA(GameCode.TempDLLPath);

                    if (!GameCode.GameCodeDLL)
                    {
                        // TODO: Logging
                        InvalidCodePath;
                    }

                    // NOTE: Load in the functions from our DLL
                    GameCode.UpdateAndRender = (game_update_and_render*)GetProcAddress(GameCode.GameCodeDLL, "GameUpdateAndRender");
                    GameCode.GetSoundSamples = (game_get_sound_samples*)GetProcAddress(GameCode.GameCodeDLL, "GameGetSoundSamples");

                    IsValid = true;
                }
                
                Sleep(100);
            }
        }
        
        // NOTE: Input 
        MSG Message;
        while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            switch (Message.message)
            {
                case WM_QUIT:
                {
                    GlobalRunning = false;
                } break;

                case WM_SYSKEYDOWN:
                case WM_SYSKEYUP:
                case WM_KEYDOWN:
                case WM_KEYUP:
                {
                    u32 VKCode = (u32)Message.wParam;
                    b32 WasDown = (Message.lParam & (1 << 30)) != 0;
                    b32 IsDown = (Message.lParam & (1 << 31)) == 0;
                } break;
                
                default:
                {
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }
            }
        }

        // NOTE: Sound
        // NOTE: Write out sample sound data
        DWORD PlayCursor = 0;
        DWORD WriteCursor = 0;
        DWORD ByteToLock = 0;
        DWORD BytesToWrite = 0;
        if (!(GlobalSecondaryBuffer->GetCurrentPosition(&PlayCursor, &WriteCursor) == DS_OK))
        {
            // TODO: Display error message
            InvalidCodePath;            
        }
        
        DWORD UnWrappedWriteCursor = WriteCursor;
        if (PrevWriteCursor > WriteCursor)
        {
            UnWrappedWriteCursor += (DWORD)SampleBufferSize;
        }

        // NOTE: The CurrentSoundByte is unwrapped
        DWORD FrameEndByte = Win32GetNearestFrameByte(SoundBytesPerFrame, UnWrappedWriteCursor);        
        FrameEndByte %= SampleBufferSize;
        ByteToLock = CurrentSoundByte;
        if (FrameEndByte > CurrentSoundByte)
        {
            ByteToLock = FrameEndByte;
        }
        
        //ByteToLock = ((FrameCount + 2)*SoundBytesPerFrame);
        ByteToLock %= SampleBufferSize;
        BytesToWrite = SoundBytesPerFrame;

#if 1
        {
            char TextBuffer[256];
            _snprintf_s(TextBuffer, sizeof(TextBuffer), "Play: %d Write: %d\n",
                        PlayCursor, WriteCursor);//, FrameEndByte);
            OutputDebugStringA(TextBuffer);            
        }
#endif
        
#if 1
        {
            char TextBuffer[256];
            _snprintf_s(TextBuffer, sizeof(TextBuffer), "ByteToLock: %d BytesToWrite: %d\n",
                        ByteToLock, BytesToWrite, CurrentSoundByte);
            OutputDebugStringA(TextBuffer);            
        }
#endif
        
        Assert(BytesToWrite >= 0);
        CurrentSoundByte = ByteToLock + BytesToWrite;

        if (GameCode.GetSoundSamples)
        {
            game_sound_buffer SoundBuffer = {};
            SoundBuffer.SamplesPerSecond = SamplesPerSecond;
            SoundBuffer.BytesPerSample = BytesPerSample;
            SoundBuffer.SamplesToWrite = BytesToWrite / BytesPerSample;
            SoundBuffer.Memory = SampleBuffer;
            
            GameCode.GetSoundSamples(&SoundBuffer);
        }
        
        VOID* Region1;
        DWORD Region1Size;
        VOID* Region2;
        DWORD Region2Size;
        if (SUCCEEDED(GlobalSecondaryBuffer->Lock(ByteToLock, BytesToWrite, &Region1, &Region1Size,
                                                  &Region2, &Region2Size, 0)))
        {
            // TODO: Pass double pointer so that we don't need a return value for Win32WriteSoundSamples
            i16* SourceSample = SampleBuffer;
            SourceSample = Win32WriteSoundSamples(SourceSample, Region1Size, BytesPerSample, (i16*)Region1);
            Win32WriteSoundSamples(SourceSample, Region2Size, BytesPerSample, (i16*)Region2);
            GlobalSecondaryBuffer->Unlock(Region1, Region1Size, Region2, Region2Size);
        }
        else
        {
            // TODO: Display error message            
            InvalidCodePath;
        }
        
        // NOTE: Rendering
        if (GameCode.UpdateAndRender)
        {
            RECT WindowRect;
            GetClientRect(Window, &WindowRect);
            u32 WindowWidth = WindowRect.right - WindowRect.left;
            u32 WindowHeight = WindowRect.bottom - WindowRect.top;

            game_display GameDisplay = {};
            GameDisplay.Width = Display.Width;
            GameDisplay.Height = Display.Height;
            GameDisplay.TargetFrameRate = Display.TargetFrameRate;
            GameDisplay.FrameBuffer = Display.FrameBuffer;
            
            GameCode.UpdateAndRender(&GameMem, &GameDisplay);
        }
        
        Win32DisplayBufferInWindow(Window, &Display);
        
#if 0
        // NOTE: Wait so that we run at 60fps
        {
            i64 FrameTimeMicro = Win32GetTimeMicro() - LastFrameTime;
            i64 FrameTimeMilli = FrameTimeMicro / 1000;

            char TextBuffer[256];
            _snprintf_s(TextBuffer, sizeof(TextBuffer), "GAME Microseconds: %ld Milliseconds: %ld\n",
                        FrameTimeMicro, FrameTimeMilli);
        
            OutputDebugStringA(TextBuffer);
        }
#endif

        // NOTE: Wait so that we run at 60fps
        f32 FrameTimeSeconds = Win32GetSecondsBetween(Win32GetClock(), LastFrameTime);
        while (FrameTimeSeconds < (1.0f / (f32)Display.TargetFrameRate))
        {
            FrameTimeSeconds = Win32GetSecondsBetween(Win32GetClock(), LastFrameTime);
        }

        LARGE_INTEGER TempLastFrameTime = LastFrameTime;
        LastFrameTime = Win32GetClock();

        // NOTE: This caused audio bugs so we have to include it in the next frame not the current
        // TODO: Figure out why this was causing audio bugs (frame time might have been misreported??)
#if 1
        {
            f32 FrameTimeSeconds = Win32GetSecondsBetween(Win32GetClock(), TempLastFrameTime);
            i32 FrameTimeMilliseconds = (i32)(FrameTimeSeconds * 1000.0f);
            i32 FrameTimeMicroseconds = (i32)(FrameTimeSeconds * 1000000.0f);
            char TextBuffer[256];
            _snprintf_s(TextBuffer, sizeof(TextBuffer), "WAIT Microseconds: %ld Milliseconds: %ld\n",
                        FrameTimeMicroseconds, FrameTimeMilliseconds);
        
            OutputDebugStringA(TextBuffer);
        }
#endif
                
    }

//timeEndPeriod(TimingResolutionMS);
    
    return 0;
}
