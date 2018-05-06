#include <stdio.h>
#include <windows.h>

#include "pixelscratcher_platform.h"
#include "win32_pixelscratcher.h"

//
//
//

global b32 GlobalRunning = true;
global b32 GlobalShowCursor = true;
global i64 GlobalTimerFrequency;
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
        
        default:
        {
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
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
    GameMem.IsInitialized = false;
    
    ProgramMemorySize += GameMem.PermanentMemSize;

    //
    // NOTE: Allocate program memory
    //

    void* ProgramMemory = VirtualAlloc(0, ProgramMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!ProgramMemory)
    {
        InvalidCodePath;
    }

    Display.FrameBuffer = (u32*)ProgramMemory;
    GameMem.PermanentMem = (void*)((u8*)Display.FrameBuffer + RenderMemorySize);
    
    game_input Input = {};
    
    LARGE_INTEGER TimerFrequency;
    QueryPerformanceFrequency(&TimerFrequency);
    GlobalTimerFrequency = TimerFrequency.QuadPart;
    
    LARGE_INTEGER LastFrameTime = Win32GetClock();
            
    while (GlobalRunning)
    {
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
                        InvalidCodePath;
                    }

                    // NOTE: Load in the functions from our DLL
                    GameCode.UpdateAndRender = (game_update_and_render*)GetProcAddress(GameCode.GameCodeDLL, "GameUpdateAndRender");
                    
                    IsValid = true;
                }
                
                Sleep(100);
            }
        }
        
        // NOTE: Input
        // NOTE: Get mouse pos in normalized coordinates
        POINT MousePos;
        if (!GetCursorPos(&MousePos))
        {
            InvalidCodePath;
        }

        RECT WindowRect;
        GetWindowRect(Window, &WindowRect);
        Input.MouseP.x = ((f32)(MousePos.x - WindowRect.left) /
                          (f32)(WindowRect.right - WindowRect.left));
        Input.MouseP.y = ((f32)(MousePos.y - WindowRect.bottom) /
                          (f32)(WindowRect.top - WindowRect.bottom));
        Input.MouseP.x = Max(0.0f, Min(1.0f, Input.MouseP.x));
        Input.MouseP.y = Max(0.0f, Min(1.0f, Input.MouseP.y));
        
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
                    
                    switch (VKCode)
                    {
                        case 'W':
                        {
                            Input.MoveUpDown = IsDown;
                        } break;

                        case 'S':
                        {
                            Input.MoveDownDown = IsDown;
                        } break;

                        case 'A':
                        {
                            Input.MoveLeftDown = IsDown;
                        } break;

                        case 'D':
                        {
                            Input.MoveRightDown = IsDown;
                        } break;

                        case VK_ESCAPE:
                        {
                            GlobalRunning = false;
                        } break;
                    }
                } break;
                
                default:
                {
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }
            }
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
            
            GameCode.UpdateAndRender(&GameMem, &Input, &GameDisplay);
        }
        
        Win32DisplayBufferInWindow(Window, &Display);

        // NOTE: Wait so that we run at 60fps
        f32 FrameTimeSeconds = Win32GetSecondsBetween(Win32GetClock(), LastFrameTime);
        while (FrameTimeSeconds < (1.0f / (f32)Display.TargetFrameRate))
        {
            FrameTimeSeconds = Win32GetSecondsBetween(Win32GetClock(), LastFrameTime);
        }

        LARGE_INTEGER TempLastFrameTime = LastFrameTime;
        LastFrameTime = Win32GetClock();

        // NOTE: This caused audio bugs so we have to include it in the next frame not the current
        {
            f32 FrameTimeSeconds = Win32GetSecondsBetween(Win32GetClock(), TempLastFrameTime);
            i32 FrameTimeMilliseconds = (i32)(FrameTimeSeconds * 1000.0f);
            i32 FrameTimeMicroseconds = (i32)(FrameTimeSeconds * 1000000.0f);
            char TextBuffer[256];
            _snprintf_s(TextBuffer, sizeof(TextBuffer), "WAIT Microseconds: %ld Milliseconds: %ld\n",
                        FrameTimeMicroseconds, FrameTimeMilliseconds);
        
            OutputDebugStringA(TextBuffer);
        }
    }
    
    return 0;
}
