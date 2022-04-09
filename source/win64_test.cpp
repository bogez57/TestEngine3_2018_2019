/*

    ToDo List:

*/

#define BGZ_ERRHANDLING_ON true
#define BGZ_LOGGING_ON true

#include <Windows.h>
#include <windowsx.h>
#include <xinput.h>
#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include "GL/glew.h"
#include "GL/wglew.h"
#include <gl/gl.h>
#include "boagz/timing.h"
#include "boagz/error_handling.h"
#include "stb/stb_image.h"

#include "atomic_types.h"
#include "array.h"
#include "boagz/memory_handling.h"
#include "my_math.h"
#include "utilities.h"
#include "renderer_stuff.h"
#include "win64_test.h"
#include "shared.h"
#include "software_rendering.h"
#include "opengl.h"

#define UTILITIES_IMPL
#include "utilities.h"
#define MY_MATH_IMPL
#include "my_math.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define PLATFORM_RENDERER_STUFF_IMPL
#include "renderer_stuff.h"
#define MEMORY_HANDLING_IMPL
#include "boagz/memory_handling.h"

global_variable u32 globalWindowWidth { 1280 };
global_variable u32 globalWindowHeight { 720 };
global_variable Win32_Offscreen_Buffer globalBackBuffer_forSoftwareRendering;
global_variable bgz::MemoryBlock gameMemory;
global_variable bool  GameRunning {};

local_func void
Win32_LogErr(const char* ErrMessage)
{
    BGZ_CONSOLE("Windows error code: %d\n", GetLastError());
    BGZ_CONSOLE(ErrMessage);
};

local_func void*
Win32_Malloc(sizet Size)
{
    void* Result {};
    
    Result = malloc(Size);
    
    return Result;
};

local_func void*
Win32_Calloc(sizet Count, sizet Size)
{
    void* Result {};
    
    Result = calloc(Count, Size);
    
    return Result;
};

local_func void*
Win32_Realloc(void* ptr, sizet size)
{
    void* result {};
    
    result = realloc(ptr, size);
    
    return result;
};

local_func auto
Win32_Free(void* PtrToFree) -> void
{
    free(PtrToFree);
};

local_func void Win32_FreeFileMemory(void* FileMemory)
{
    if (FileMemory)
    {
        VirtualFree(FileMemory, 0, MEM_RELEASE);
    }
};

local_func void Win32_Sleep(unsigned int milliseconds)
{
    Sleep(milliseconds);
};

local_func bool
Win32_WriteEntireFile(const char* FileName, void* memory, u32 MemorySize)
{
    b32 Result = false;
    
    HANDLE FileHandle = CreateFile(FileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (FileHandle != INVALID_HANDLE_VALUE)
    {
        DWORD BytesWritten;
        if (WriteFile(FileHandle, memory, MemorySize, &BytesWritten, 0))
        {
            //File read successfully
            Result = (BytesWritten == MemorySize);
        }
        else
        {
            Win32_LogErr("Unable to write to file!");
            InvalidCodePath;
        }
        
        CloseHandle(FileHandle);
    }
    else
    {
        Win32_LogErr("Unable to create file handle!");
        InvalidCodePath;
    }
    
    return (Result);
};

local_func unsigned char*
Win32_ReadEntireFile(s32&& length, const char* FilePath)
{
    unsigned char* data;
    FILE* file;
    
    BGZ_ASSERT(fopen_s(&file, FilePath, "rb") == 0);//File path used is incorrect or doesn't exist!"
    
    fseek(file, 0, SEEK_END);
    length = (s32)ftell(file);
    fseek(file, 0, SEEK_SET);
    
    data = (unsigned char*)malloc(length + 1);
    fread(data, 1, length, file);
    data[length] = 0;
    
    fclose(file);
    
    return data;
};

local_func u8*
Win32_LoadBGRABitmap(const char* ImagePath, s32&& width, s32&& height)
{
    stbi_set_flip_vertically_on_load(true); //So first byte stbi_load() returns is bottom left instead of top-left of image (which is stb's default)
    
    s32 numOfLoadedChannels {};
    s32 desiredChannels { 4 }; //Since I still draw assuming 4 byte pixels I need 4 channels
    
    //Returns RGBA
    unsigned char* imageData = stbi_load(ImagePath, &width, &height, &numOfLoadedChannels, desiredChannels);
    BGZ_ASSERT(imageData);//Invalid image data!"
    
    s32 totalPixelCountOfImg = width * height;
    u32* imagePixel = (u32*)imageData;
    
    //Swap R and B channels of image
    for (int i = 0; i < totalPixelCountOfImg; ++i)
    {
        auto color = UnPackPixelValues(*imagePixel, RGBA);
        
        //Pre-multiplied alpha
        f32 alphaBlend = color.a / 255.0f;
        color.rgb *= alphaBlend;
        
        u32 newSwappedPixelColor = (((u8)color.a << 24) | ((u8)color.r << 16) | ((u8)color.g << 8) | ((u8)color.b << 0));
        
        *imagePixel++ = newSwappedPixelColor;
    }
    
    return (u8*)imageData;
}

local_func void
Win32_UseConsole()
{
    //Create a console for this application
    AllocConsole();
    
    // Get STDOUT handle
    HANDLE ConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    int SystemOutput = _open_osfhandle(intptr_t(ConsoleOutput), _O_TEXT);
    FILE* COutputHandle = _fdopen(SystemOutput, "w");
    
    // Get STDERR handle
    HANDLE ConsoleError = GetStdHandle(STD_ERROR_HANDLE);
    int SystemError = _open_osfhandle(intptr_t(ConsoleError), _O_TEXT);
    FILE* CErrorHandle = _fdopen(SystemError, "w");
    
    // Get STDIN handle
    HANDLE ConsoleInput = GetStdHandle(STD_INPUT_HANDLE);
    int SystemInput = _open_osfhandle(intptr_t(ConsoleInput), _O_TEXT);
    FILE* CInputHandle = _fdopen(SystemInput, "r");
    
    // Redirect the CRT standard input, output, and error handles to the console
    freopen_s(&CInputHandle, "CONIN$", "r", stdin);
    freopen_s(&COutputHandle, "CONOUT$", "w", stdout);
    freopen_s(&CErrorHandle, "CONOUT$", "w", stderr);
};

inline FILETIME
Win32_GetFileTime(const char* FileName)
{
    FILETIME TimeFileWasLastWrittenTo {};
    WIN32_FILE_ATTRIBUTE_DATA FileData {};
    
    if (GetFileAttributesEx(FileName, GetFileExInfoStandard, &FileData))
    {
        TimeFileWasLastWrittenTo = FileData.ftLastWriteTime;
    }
    
    return TimeFileWasLastWrittenTo;
};

local_func Win32_Game_Code
Win32_LoadGameCodeDLL(const char* GameCodeDLL)
{
    Win32_Game_Code GameCode {};
    
    GameCode.DLLHandle = LoadLibrary(GameCodeDLL);
    BGZ_ASSERT(GameCode.DLLHandle);
    
    GameCode.UpdateFunc = (GameUpdateFuncPtr)GetProcAddress(GameCode.DLLHandle, "GameUpdate");
    BGZ_ASSERT(GameCode.UpdateFunc);//Dll function not loading!"
    
    return GameCode;
};

local_func void
Win32_FreeGameCodeDLL(Win32_Game_Code&& GameCode, Platform_Services&& platformServices)
{
    if (GameCode.DLLHandle != INVALID_HANDLE_VALUE)
    {
        FreeLibrary(GameCode.DLLHandle);
        GameCode.DLLHandle = 0;
        GameCode.UpdateFunc = nullptr;
        platformServices.DLLJustReloaded = true;
    };
};

local_func void
Win32_InitInputRecording(Win32_Game_Replay_State&& GameReplayState)
{
    GameReplayState.InputRecording = true;
    GameReplayState.TotalInputStructsRecorded = 0;
    GameReplayState.InputCount = 0;
    memcpy(GameReplayState.OriginalRecordedGameState, gameMemory.permanentStorage, gameMemory.totalSize);
};

local_func void
Win32_RecordInput(const Game_Input* Input, Win32_Game_Replay_State&& GameReplayState)
{
    GameReplayState.RecordedInputs[GameReplayState.InputCount] = *Input;
    ++GameReplayState.InputCount;
    ++GameReplayState.TotalInputStructsRecorded;
};

local_func void
Win32_InitInputPlayBack(Win32_Game_Replay_State&& GameReplayState)
{
    GameReplayState.InputPlayBack = true;
    GameReplayState.InputCount = 0;
    //Set game state back to when it was first recorded for proper looping playback
    memcpy(gameMemory.permanentStorage, GameReplayState.OriginalRecordedGameState, gameMemory.totalSize);
}

local_func void
Win32_EndInputPlayBack(Game_Input&& Input, Win32_Game_Replay_State&& GameReplayState)
{
    GameReplayState.InputPlayBack = false;
    for (u32 ControllerIndex { 0 }; ControllerIndex < ArrayCount(Input.Controllers); ++ControllerIndex)
    {
        for (u32 ButtonIndex { 0 }; ButtonIndex < ArrayCount(Input.Controllers[ControllerIndex].Buttons); ++ButtonIndex)
        {
            Input.Controllers[ControllerIndex].Buttons[ButtonIndex].Pressed = false;
        }
    }
};

local_func void
Win32_PlayBackInput(Game_Input&& Input, Win32_Game_Replay_State&& GameReplayState)
{
    if (GameReplayState.InputCount < GameReplayState.TotalInputStructsRecorded)
    {
        Input = GameReplayState.RecordedInputs[GameReplayState.InputCount];
        ++GameReplayState.InputCount;
    }
    else
    {
        GameReplayState.InputCount = 0;
        memcpy(gameMemory.permanentStorage, GameReplayState.OriginalRecordedGameState, gameMemory.totalSize);
    }
}

local_func Win32_Window_Dimension
Win32_GetWindowDimension(HWND window)
{
    Win32_Window_Dimension Result;
    
    RECT ClientRect;
    GetClientRect(window, &ClientRect);
    
    Result.width = ClientRect.right - ClientRect.left;
    Result.height = ClientRect.bottom - ClientRect.top;
    
    return (Result);
};

local_func void
Win32_ResizeDIBSection(Win32_Offscreen_Buffer&& buffer, int width, int height)
{
    // TODO: Bulletproof this.
    // Maybe don't free first, free after, then free first if that fails.
    
    if (buffer.memory)
    {
        VirtualFree(buffer.memory, 0, MEM_RELEASE);
    }
    
    buffer.width = width;
    buffer.height = height;
    
    buffer.bytesPerPixel = 4;
    
    // When the biHeight field is negative, this is the clue to
    // Windows to treat this bitmap as top-down, not bottom-up, meaning that
    // the first three bytes of the image are the color for the top left pixel
    // in the bitmap, not the bottom left!
    buffer.Info.bmiHeader.biSize = sizeof(buffer.Info.bmiHeader);
    buffer.Info.bmiHeader.biWidth = buffer.width;
    buffer.Info.bmiHeader.biHeight = buffer.height;
    buffer.Info.bmiHeader.biPlanes = 1;
    buffer.Info.bmiHeader.biBitCount = 32;
    buffer.Info.bmiHeader.biCompression = BI_RGB;
    
    int BitmapMemorySize = (buffer.width * buffer.height) * buffer.bytesPerPixel;
    buffer.memory = VirtualAlloc(0, BitmapMemorySize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    buffer.pitch = width * buffer.bytesPerPixel;
    
    // TODO: Probably clear this to black
}

local_func void Win32_DisplayBackBuffer(HDC deviceContext, int windowWidth, int windowHeight)
{
    bool renderThroughHardware { true };
    if (renderThroughHardware)
    {
        SwapBuffers(deviceContext);
    }
    else
    {
        //Performs screen clear so resizing window doesn't screw up the image displayed
        PatBlt(deviceContext, 0, 0, windowWidth, 0, BLACKNESS);
        PatBlt(deviceContext, 0, globalBackBuffer_forSoftwareRendering.height, windowWidth, windowHeight, BLACKNESS);
        PatBlt(deviceContext, 0, 0, 0, windowHeight, BLACKNESS);
        PatBlt(deviceContext, globalBackBuffer_forSoftwareRendering.width, 0, windowWidth, windowHeight, BLACKNESS);
        
        { //Switched around coordinates and things here so I can treat drawing in game as bottom-up instead of top down
            v2i displayRect_BottomLeftCoords { 0, 0 };
            v2i displayRect_Dimensions {};
            displayRect_Dimensions.width = globalBackBuffer_forSoftwareRendering.width;
            displayRect_Dimensions.height = globalBackBuffer_forSoftwareRendering.height;
            
            //Copy game's rendered back buffer to whatever display area size you want
            StretchDIBits(deviceContext,
                          displayRect_BottomLeftCoords.x, displayRect_BottomLeftCoords.y, displayRect_Dimensions.width, displayRect_Dimensions.height, //Dest - Area to draw to within window's window
                          0, 0, globalBackBuffer_forSoftwareRendering.width, globalBackBuffer_forSoftwareRendering.height, //Source - The dimensions/coords of the back buffer the game rendered to
                          globalBackBuffer_forSoftwareRendering.memory,
                          &globalBackBuffer_forSoftwareRendering.Info,
                          DIB_RGB_COLORS, SRCCOPY);
        };
    };
};

local_func void Win32_RenderToBackBuffer(Rendering_Info&& renderingInfo, RenderCmdBuffer&& bufferToRender, bgz::Memory_Partition* platformMemoryPart, int windowWidth, int windowHeight, Platform_Services platformServices)
{
    bool renderThroughHardware { true };
    if (renderThroughHardware)
    {
        RenderViaHardware($(renderingInfo), $(bufferToRender), platformMemoryPart, windowWidth, windowHeight);
    }
    else
    {
        //RenderViaSoftware($(renderingInfo), globalBackBuffer_forSoftwareRendering.memory, v2i { globalBackBuffer_forSoftwareRendering.width, globalBackBuffer_forSoftwareRendering.height }, globalBackBuffer_forSoftwareRendering.pitch, &platformServices);
    };
    
    //Clear out render command buffer
    bufferToRender.usedAmount = 0;
};

local_func void
Win32_ProcessKeyboardMessage(Button_State&& NewState, b32 IsDown)
{
    if (NewState.Pressed != IsDown)
    {
        NewState.Pressed = IsDown;
        ++NewState.NumTransitionsPerFrame;
    }
}

local_func void
Win32_ProcessPendingMessages(Game_Input&& Input, HWND window, Win32_Game_Replay_State&& GameReplayState)
{
    MSG Message;
    while (PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
    {
        switch (Message.message)
        {
            case WM_QUIT: {
                GameRunning = false;
            }
            break;
            
            default: {
                TranslateMessage(&Message);
                DispatchMessageA(&Message);
            }
            break;
        }
    }
};

LRESULT CALLBACK
Win32_ProgramWindowCallback(HWND WindowHandle, UINT Message, WPARAM wParam, LPARAM lParam)
{
    LRESULT Result { 0 };
    
    //For hardware rendering
    HDC WindowContext = GetDC(WindowHandle);
    
    switch (Message)
    {
        case WM_CREATE: {
            if (WindowContext)
            {
                { //Init OpenGL
                    //Pixel format you would like to have if possible
                    PIXELFORMATDESCRIPTOR DesiredPixelFormat {};
                    //Pixel format that windows actually gives you based on your desired pixel format and what the system
                    //acutally has available (32 bit color capabilites? etc.)
                    PIXELFORMATDESCRIPTOR SuggestedPixelFormat {};
                    
                    DesiredPixelFormat.nSize = sizeof(DesiredPixelFormat);
                    DesiredPixelFormat.nVersion = 1;
                    DesiredPixelFormat.iPixelType = PFD_TYPE_RGBA;
                    DesiredPixelFormat.dwFlags = PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
                    DesiredPixelFormat.cColorBits = 32;
                    DesiredPixelFormat.cAlphaBits = 8;
                    DesiredPixelFormat.iLayerType = PFD_MAIN_PLANE;
                    
                    int SuggestedPixelFormatIndex = ChoosePixelFormat(WindowContext, &DesiredPixelFormat);
                    DescribePixelFormat(WindowContext, SuggestedPixelFormatIndex, sizeof(SuggestedPixelFormat), &SuggestedPixelFormat);
                    
                    if (SetPixelFormat(WindowContext, SuggestedPixelFormatIndex, &SuggestedPixelFormat))
                    {
                        HGLRC OpenGLRenderingContext = wglCreateContext(WindowContext);
                        
                        if (OpenGLRenderingContext)
                        {
                            if (wglMakeCurrent(WindowContext, OpenGLRenderingContext))
                            {
                                //Success! We have a current openGL context. Now setup glew
                                if (glewInit() == GLEW_OK)
                                {
                                    if (WGLEW_EXT_swap_control)
                                    {
                                        //Turn Vsync on/off
                                        wglSwapIntervalEXT(0);
                                    }
                                    else
                                    {
                                        Win32_LogErr("Failed to find opengl swap interval extention on computer!");
                                    }
                                }
                                else
                                {
                                    ReleaseDC(WindowHandle, WindowContext);
                                    InvalidCodePath;
                                }
                            }
                            else
                            {
                                ReleaseDC(WindowHandle, WindowContext);
                                Win32_LogErr("Unable to make opengl context the current context!");
                                InvalidCodePath;
                            }
                        }
                        else
                        {
                            ReleaseDC(WindowHandle, WindowContext);
                            Win32_LogErr("Unable to create an opengl rendering context!");
                            InvalidCodePath;
                        }
                    }
                    else
                    {
                        ReleaseDC(WindowHandle, WindowContext);
                        Win32_LogErr("Unable to set the pixel format for potential opengl window!");
                        InvalidCodePath;
                    }
                } //Init OpenGL
            }
            else
            {
                Win32_LogErr("Unable to get window context from window!");
                InvalidCodePath;
            }
        }
        break;
        
        case WM_PAINT: {
            //To understand why you need a display buffer call here as well as game loop, see this post:
            //https://hero.handmade.network/forums/code-discussion/t/825-wm_paint_question_beginner
            PAINTSTRUCT Paint;
            HDC deviceContext = BeginPaint(WindowHandle, &Paint);
#if 0
            Win32_Window_Dimension windowDimension = GetWindowDimension(WindowHandle);
            Win32_DisplayBufferInWindow(deviceContext, windowDimension.width, windowDimension.height);
#endif
            EndPaint(WindowHandle, &Paint);
        }
        break;
        
        case WM_DESTROY: {
            GameRunning = false;
        }
        break;
        
        case WM_CLOSE: {
            GameRunning = false;
        }
        break;
        
        case WM_ACTIVATEAPP: {
        }
        break;
        
        default: {
            Result = DefWindowProc(WindowHandle, Message, wParam, lParam);
        }
        break;
    }
    
    return Result;
};

local_func auto
Win32_ProcessXInputDigitalButton(DWORD XInputButtonState, Button_State&& GameButtonState, DWORD XInputButtonBit)
{
    if (GameButtonState.Pressed != ((XInputButtonState & XInputButtonBit) == XInputButtonBit))
    {
        GameButtonState.Pressed = ((XInputButtonState & XInputButtonBit) == XInputButtonBit);
        ++GameButtonState.NumTransitionsPerFrame;
    }
}

local_func f32
Win32_NormalizeAnalogStickValue(SHORT Value, SHORT DeadZoneThreshold)
{
    f32 Result = 0;
    
    if (Value < -DeadZoneThreshold)
    {
        Result = (f32)((Value + DeadZoneThreshold) / (32768.0f - DeadZoneThreshold));
    }
    else if (Value > DeadZoneThreshold)
    {
        Result = (f32)((Value - DeadZoneThreshold) / (32767.0f - DeadZoneThreshold));
    }
    
    return (Result);
}

struct Work_Queue_Entry
{
    platform_work_queue_callback* callback;
    void* data;
};

struct Work_Queue
{
    HANDLE semaphoreHandle;
    s32 volatile entryCompletionGoal;
    s32 volatile entryCompletionCount;
    s32 volatile nextEntryToWrite;
    s32 volatile nextEntryToRead;
    Work_Queue_Entry entries[256];
};

global_variable Work_Queue globalWorkQueue;

struct Thread_Info
{
    s32 logicalThreadIndex;
};

void AddToWorkQueue(platform_work_queue_callback* callback, void* data)
{
    s32 newNextEntryToWrite = (globalWorkQueue.nextEntryToWrite + 1) % ArrayCount(globalWorkQueue.entries);
    Assert(newNextEntryToWrite != globalWorkQueue.nextEntryToRead);
    
    Work_Queue_Entry entry { callback, data };
    globalWorkQueue.entries[globalWorkQueue.nextEntryToWrite] = entry;
    ++globalWorkQueue.entryCompletionGoal;
    
    _WriteBarrier();
    _mm_sfence();
    
    ++globalWorkQueue.nextEntryToWrite = newNextEntryToWrite;
    ReleaseSemaphore(globalWorkQueue.semaphoreHandle, 1, 0);
};

bool DoWork();
void FinishAllWork()
{
    while (globalWorkQueue.entryCompletionGoal != globalWorkQueue.entryCompletionCount)
    {
        DoWork();
    };
    
    globalWorkQueue.entryCompletionCount = 0;
    globalWorkQueue.entryCompletionGoal = 0;
};

bool DoWork()
{
    bool isThereStillWork {};
    
    s32 originalNextEntryToRead = globalWorkQueue.nextEntryToRead;
    s32 newNextEntryToRead = (originalNextEntryToRead + 1) % ArrayCount(globalWorkQueue.entries);
    if (originalNextEntryToRead != globalWorkQueue.nextEntryToWrite)
    {
        s32 entryIndex = _InterlockedCompareExchange((LONG volatile*)&globalWorkQueue.nextEntryToRead, newNextEntryToRead, originalNextEntryToRead);
        _ReadBarrier();
        
        if (entryIndex == originalNextEntryToRead)
        {
            Work_Queue_Entry entry = globalWorkQueue.entries[entryIndex];
            entry.callback(entry.data);
            _InterlockedIncrement((LONG volatile*)&globalWorkQueue.entryCompletionCount);
        };
        
        isThereStillWork = true;
    }
    else
    {
        isThereStillWork = false;
    }
    
    return isThereStillWork;
};

DWORD WINAPI
ThreadProc(LPVOID param)
{
    Thread_Info* info = (Thread_Info*)param;
    
    while (1)
    {
        if (DoWork())
        {
            //Keep doing work
        }
        else
        {
            WaitForSingleObjectEx(globalWorkQueue.semaphoreHandle, INFINITE, FALSE);
        }
    };
    
    return 0;
};

int CALLBACK WinMain(HINSTANCE CurrentProgramInstance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode)
{
    Win32_UseConsole();
    
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    
    Thread_Info threadInfo[8] = {};
    s32 threadCount = ArrayCount(threadInfo);
    
    s32 initialThreadCount = 0;
    globalWorkQueue.semaphoreHandle = CreateSemaphoreExA(0, initialThreadCount, threadCount, 0, 0, SEMAPHORE_ALL_ACCESS);
    
    for (s32 threadIndex {}; threadIndex < ArrayCount(threadInfo); ++threadIndex)
    {
        Thread_Info* info = threadInfo + threadIndex;
        info->logicalThreadIndex = threadIndex;
        
        DWORD threadID;
        HANDLE myThread = CreateThread(0, 0, ThreadProc, info, 0, &threadID);
    };
    
    //Set scheduler granularity to help ensure we are able to put thread to sleep by the amount of time specified and no longer
    UINT DesiredSchedulerGranularityMS = 1;
    BGZ_ASSERT(timeBeginPeriod(DesiredSchedulerGranularityMS) == TIMERR_NOERROR);//Error when trying to set windows granularity!"
    
    Win32_ResizeDIBSection($(globalBackBuffer_forSoftwareRendering), globalWindowWidth, globalWindowHeight);
    
    WNDCLASS WindowProperties {};
    WindowProperties.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW; //TODO: Check if OWNDC/HREDRAW/VEDRAW matter
    WindowProperties.lpfnWndProc = Win32_ProgramWindowCallback;
    WindowProperties.hInstance = CurrentProgramInstance;
    WindowProperties.lpszClassName = "TestEngineClass";
    WindowProperties.hCursor = LoadCursor(NULL, IDC_ARROW);
    
    bool mouseButtonHeld{};
    
    if (RegisterClass(&WindowProperties))
    {
        //Since CreateWindowEx function expects the TOTAL window size (including pixels for title bar, borders etc.)
        //we will specify the rect size we actually want the game to run in and then us window's AdjustWindowRectEx
        //to properly adjust rect coordinates so no clipping of game window occurs.
        DWORD windowStyles = WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
        RECT rect = { 0, 0, (LONG)globalWindowWidth, (LONG)globalWindowHeight };
        BOOL success = AdjustWindowRectEx(&rect, windowStyles, false, 0);
        if (NOT success) InvalidCodePath;
        
        HWND window = CreateWindowEx(0, WindowProperties.lpszClassName, "Test Engine", WS_VISIBLE | WS_OVERLAPPEDWINDOW,
                                     CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, 0, 0, CurrentProgramInstance, 0);
        
        HDC WindowContext = GetDC(window);
        
        if (window && WindowContext)
        {
#if DEVELOPMENT_BUILD
            void* baseAddress { (void*)Terabytes(2) };
#else
            void* baseAddress { (void*)0 };
#endif
            
            Game_Input Input {};
            Game_Sound_Output_Buffer SoundBuffer {};
            Rendering_Info renderingInfo {};
            Platform_Services platformServices {};
            Win32_Game_Replay_State GameReplayState {};
            Win32_Game_Code GameCode { Win32_LoadGameCodeDLL("bin/gamecode.dll") };
            
            void* gameMemoryPtr = VirtualAlloc(baseAddress, Gigabytes(1) + Megabytes(64), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE); //TODO: Add large page support?)
            if(!gameMemoryPtr) Win32_LogErr("Virtual Alloc Failed!");
            
            bgz::InitMemoryBlock($(gameMemory), Gigabytes(1) /* total Memsize */, Megabytes(64) /* size of permanent store */, gameMemoryPtr);
            
            //Partition game memory
            bgz::CreatePartitionFromMemoryBlock($(gameMemory), Megabytes(100), "frame");
            bgz::CreatePartitionFromMemoryBlock($(gameMemory), Megabytes(100), "level");
            bgz::CreatePartitionFromMemoryBlock($(gameMemory), Megabytes(100), "platform");
            bgz::CreatePartitionFromMemoryBlock($(gameMemory), Megabytes(10), "RenderCmdBuffer");
            
            bgz::Memory_Partition* platformMemoryPart = GetMemoryPartition(&gameMemory, "platform");
            
            { //Init game render command buffer and other render stuff
                renderingInfo.gameCmdBuffer.baseAddress = (u8*)(GetMemoryPartition(&gameMemory, "RenderCmdBuffer"))->baseAddress;
                renderingInfo.gameCmdBuffer.size = Megabytes(10);
                renderingInfo.gameCmdBuffer.entryCount = 0;
                renderingInfo.gameCmdBuffer.usedAmount = 0;
                
                renderingInfo._pixelsPerMeter = globalBackBuffer_forSoftwareRendering.height * .10f;
                renderingInfo.initialWidthOfScreen_pixels = globalWindowWidth;
                renderingInfo.initialHeightOfScreen_pixels = globalWindowHeight;
            }
            
            { //Init input recording and replay services
                GameReplayState.RecordedInputs = (Game_Input*)VirtualAlloc(0, (sizeof(Game_Input) * Win32_MaxAllowableRecordedInputs), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
                GameReplayState.OriginalRecordedGameState = VirtualAlloc(0, gameMemory.totalSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            }
            
            { //Init game services
                platformServices.WriteEntireFile = &Win32_WriteEntireFile;
                platformServices.ReadEntireFile = &Win32_ReadEntireFile;
                platformServices.FreeFileMemory = &Win32_FreeFileMemory;
                platformServices.LoadBGRABitmap = &Win32_LoadBGRABitmap;
                platformServices.Malloc = &Win32_Malloc;
                platformServices.Calloc = &Win32_Calloc;
                platformServices.Realloc = &Win32_Realloc;
                platformServices.Free = &Win32_Free;
                platformServices.AddWorkQueueEntry = &AddToWorkQueue;
                platformServices.FinishAllWork = &FinishAllWork;
                platformServices.Sleep = &Win32_Sleep;
            }
            
            auto UpdateInput = [window](Game_Input&& Input, Win32_Game_Replay_State&& GameReplayState) -> void {
                
                for (u32 ControllerIndex = 0; ControllerIndex < ArrayCount(Input.Controllers); ++ControllerIndex)
                    ClearTransitionCounts(&Input.Controllers[ControllerIndex]);
                
                ClearTransitionCounts(Input.mouseButtons);
                
                {//Update mouse position/button state
                    POINT mousePos;
                    GetCursorPos(&mousePos);
                    ScreenToClient(window, &mousePos);
                    
                    Input.mouseX = mousePos.x;
                    Input.mouseY = mousePos.y;
                    
                    Win32_ProcessKeyboardMessage($(Input.mouseButtons[LEFT_CLICK]), GetKeyState(VK_LBUTTON) & (1 << 15));
                    Win32_ProcessKeyboardMessage($(Input.mouseButtons[RIGHT_CLICK]), GetKeyState(VK_RBUTTON) & (1 << 15));
                    Win32_ProcessKeyboardMessage($(Input.mouseButtons[WHEEL_CLICK]), GetKeyState(VK_MBUTTON) & (1 << 15));
                    
#if 0
                    //Ex. of detecting key states using GetAsyncKeyState
                    //This first way will detect for single presses of key
                    if((GetAsyncKeyState(VK_CONTROL) & (1 << 0)))
                    {
                    }
                    //This way will detect if key is pressed and held
                    else if(GetAsyncKeyState(VK_CONTROL) & (1 << 15))
                    {
                    }
#endif
                };
                
                //Poll Keyboard Input
                Win32_ProcessPendingMessages($(Input), window, $(GameReplayState));
                
                //Or you can process key input this way. Not quite sure which one is preferred and when
#if 0
                Win32_ProcessKeyboardMessage($(Input.Controllers[0].MoveUp), GetAsyncKeyState('W') & (1 << 15));
                Win32_ProcessKeyboardMessage($(Input.Controllers[0].MoveUp), GetAsyncKeyState('S') & (1 << 15));
                Win32_ProcessKeyboardMessage($(Input.Controllers[0].MoveUp), GetAsyncKeyState('A') & (1 << 15));
                Win32_ProcessKeyboardMessage($(Input.Controllers[0].MoveUp), GetAsyncKeyState('D') & (1 << 15));
                Win32_ProcessKeyboardMessage($(Input.Controllers[0].MoveUp), GetAsyncKeyState('X') & (1 << 15));
                Win32_ProcessKeyboardMessage($(Input.Controllers[0].MoveUp), GetAsyncKeyState('Z') & (1 << 15));
                Win32_ProcessKeyboardMessage($(Input.Controllers[0].MoveUp), GetAsyncKeyState('I') & (1 << 15));
                Win32_ProcessKeyboardMessage($(Input.Controllers[0].MoveUp), GetAsyncKeyState(VK_SHIFT) & (1 << 15));
#endif
                Win32_ProcessKeyboardMessage($(Input.Controllers[0].Back), GetAsyncKeyState(VK_SPACE) & (1 << 15));
                //....etc
                if(Input.Controllers[0].Back.Pressed)
                {
                    //do something
                };
                
                { //Poll GamePad Input(s)
                    for (DWORD ControllerIndex = 0; ControllerIndex < ArrayCount(Input.Controllers); ++ControllerIndex)
                    {
                        Game_Controller* MyGamePad = &Input.Controllers[ControllerIndex + 1]; //Since index 0 is reserved for keyboard
                        
                        XINPUT_STATE ControllerState;
                        if (XInputGetState(ControllerIndex, &ControllerState) == ERROR_SUCCESS)
                        {
                            if (ControllerIndex == ArrayCount(Input.Controllers)) //Since index 0 is reserved for keyboard
                                break;
                            
                            //This controller is plugged in
                            XINPUT_GAMEPAD* XGamePad = &ControllerState.Gamepad;
                            MyGamePad->IsConnected = true;
                            
                            MyGamePad->LThumbStick.x = Win32_NormalizeAnalogStickValue(XGamePad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                            MyGamePad->LThumbStick.y = Win32_NormalizeAnalogStickValue(XGamePad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                            
                            if ((MyGamePad->LThumbStick.x != 0.0f) || (MyGamePad->LThumbStick.y != 0.0f))
                            {
                                MyGamePad->IsAnalog = true;
                            }
                            
                            if (XGamePad->wButtons & XINPUT_GAMEPAD_DPAD_UP)
                            {
                                MyGamePad->LThumbStick.y = 1.0f;
                                MyGamePad->IsAnalog = false;
                            }
                            
                            if (XGamePad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
                            {
                                MyGamePad->LThumbStick.y = -1.0f;
                                MyGamePad->IsAnalog = false;
                            }
                            
                            if (XGamePad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
                            {
                                MyGamePad->LThumbStick.x = -1.0f;
                                MyGamePad->IsAnalog = false;
                            }
                            
                            if (XGamePad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
                            {
                                MyGamePad->LThumbStick.x = 1.0f;
                                MyGamePad->IsAnalog = false;
                            }
                            
                            Win32_ProcessXInputDigitalButton(XGamePad->wButtons, $(MyGamePad->MoveUp), XINPUT_GAMEPAD_DPAD_UP);
                            Win32_ProcessXInputDigitalButton(XGamePad->wButtons, $(MyGamePad->MoveDown), XINPUT_GAMEPAD_DPAD_DOWN);
                            Win32_ProcessXInputDigitalButton(XGamePad->wButtons, $(MyGamePad->MoveLeft), XINPUT_GAMEPAD_DPAD_LEFT);
                            Win32_ProcessXInputDigitalButton(XGamePad->wButtons, $(MyGamePad->MoveRight), XINPUT_GAMEPAD_DPAD_RIGHT);
                            Win32_ProcessXInputDigitalButton(XGamePad->wButtons, $(MyGamePad->LeftShoulder), XINPUT_GAMEPAD_LEFT_SHOULDER);
                            Win32_ProcessXInputDigitalButton(XGamePad->wButtons, $(MyGamePad->RightShoulder), XINPUT_GAMEPAD_RIGHT_SHOULDER);
                            Win32_ProcessXInputDigitalButton(XGamePad->wButtons, $(MyGamePad->ActionUp), XINPUT_GAMEPAD_Y);
                            Win32_ProcessXInputDigitalButton(XGamePad->wButtons, $(MyGamePad->ActionDown), XINPUT_GAMEPAD_A);
                            Win32_ProcessXInputDigitalButton(XGamePad->wButtons, $(MyGamePad->ActionLeft), XINPUT_GAMEPAD_X);
                            Win32_ProcessXInputDigitalButton(XGamePad->wButtons, $(MyGamePad->ActionRight), XINPUT_GAMEPAD_B);
                            Win32_ProcessXInputDigitalButton(XGamePad->wButtons, $(MyGamePad->Start), XINPUT_GAMEPAD_START);
                            Win32_ProcessXInputDigitalButton(XGamePad->wButtons, $(MyGamePad->Back), XINPUT_GAMEPAD_BACK);
                        }
                        else
                        {
                            //Controller not available
                            MyGamePad->IsConnected = false;
                        }
                    }
                };
            };
            
            u32 MonitorRefreshRate = bgz::MonitorRefreshHz();
            int GameRefreshRate {};
            f32 TargetSecondsPerFrame {};
            
            switch (MonitorRefreshRate)
            {
                case 30: {
                    GameRefreshRate = MonitorRefreshRate;
                    TargetSecondsPerFrame = 1.0f / (f32)GameRefreshRate;
                }
                break;
                
                case 40: {
                    GameRefreshRate = MonitorRefreshRate;
                    TargetSecondsPerFrame = 1.0f / (f32)GameRefreshRate;
                }
                break;
                
                case 60: {
                    GameRefreshRate = MonitorRefreshRate;
                    TargetSecondsPerFrame = 1.0f / (f32)GameRefreshRate;
                }
                break;
                case 120: {
                    GameRefreshRate = MonitorRefreshRate / 2;
                    TargetSecondsPerFrame = 1.0f / (f32)GameRefreshRate;
                }
                break;
                default:
                InvalidCodePath; //Unkown monitor refresh rate
            };
            
            GameRunning = true;
            
            while (GameRunning)
            {
                //TODO: Currently globalWindowWidth and height aren't updated dynamically. Need to see about doing this since other things rely on renderingInfo stuff currently
                Win32_Window_Dimension windowDimension = Win32_GetWindowDimension(window);
                globalWindowWidth = windowDimension.width;
                globalWindowHeight = windowDimension.height;
                
                renderingInfo.widthOfScreen_pixels = globalWindowWidth;
                renderingInfo.heightOfScreen_pixels = globalWindowHeight;
                
                HDC deviceContext = GetDC(window);
                Win32_ResizeDIBSection($(globalBackBuffer_forSoftwareRendering), windowDimension.width, windowDimension.height);
                renderingInfo._pixelsPerMeter = globalBackBuffer_forSoftwareRendering.height * .10f;
                
#if 1
                //helps to prevent overly large detlatimes from getting passed when using debugger and breakpoints
                if (platformServices.prevFrameTimeInSecs > 1.0f / 30.0f)
                    platformServices.prevFrameTimeInSecs = 1.0f / 30.0f;
#endif
                
                //TODO: Should we poll more frequently?
                UpdateInput($(Input), $(GameReplayState));
                
                if (GameReplayState.InputRecording)
                    Win32_RecordInput(&Input, $(GameReplayState));
                if (GameReplayState.InputPlayBack)
                    Win32_PlayBackInput($(Input), $(GameReplayState));
                
                GameCode.UpdateFunc(&gameMemory, &platformServices, &renderingInfo, &SoundBuffer, &Input);
                
                Input = Input;
                GameReplayState = GameReplayState;
                
                //TODO: Move this out of platform layer. Only hear because I call RendertoBackBuffer twice and this used to be in that function
                if(renderingInfo.userWantsToClearDepthBuf)
                {
                    glClearColor(renderingInfo.clearColor.r, renderingInfo.clearColor.g, renderingInfo.clearColor.b,  0.0f);
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                }
                else
                {
                    glClearColor(renderingInfo.clearColor.r, renderingInfo.clearColor.g, renderingInfo.clearColor.b,  0.0f);
                    glClear(GL_COLOR_BUFFER_BIT);
                };
                
                Win32_RenderToBackBuffer($(renderingInfo), $(renderingInfo.gameCmdBuffer), platformMemoryPart, windowDimension.width, windowDimension.height, platformServices);
                
                IsAllTempMemoryCleared(*platformMemoryPart);
                
                //BGZ_CONSOLE("frame time secs: %f\n", debugTiming.frameTime_inSeconds);
                
                Win32_DisplayBackBuffer(deviceContext, windowDimension.width, windowDimension.height);
            };
            
            PostMessage(window, WM_CLOSE, 0, 0);
            
            //Hardware Rendering shutdown procedure
            wglMakeCurrent(NULL, NULL);
            ReleaseDC(window, WindowContext);
        }
        else
        {
            Win32_LogErr("Window could not be created!");
            InvalidCodePath;
        }
    }
    else
    {
        Win32_LogErr("Window properties could not be registered!");
        InvalidCodePath;
    }
    
    return 0;
}
