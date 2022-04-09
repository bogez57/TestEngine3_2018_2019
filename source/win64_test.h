#pragma once
#include <Windows.h>
#include "shared.h"

struct Win32_Offscreen_Buffer
{
    //Pixels are alwasy 32-bits wide, Memory Order BB GG RR XX
    BITMAPINFO Info;
    void *memory;
    int width;
    int height;
    int pitch;
    int bytesPerPixel;
};

struct Win32_Game_Code
{
    HMODULE DLLHandle {};
    void (*UpdateFunc)(bgz::MemoryBlock*, Platform_Services*, Rendering_Info*, Game_Sound_Output_Buffer*, Game_Input*);
    FILETIME PreviousDLLWriteTime {};
};

struct Win32_Window_Dimension
{
    int width;
    int height;
};

using GameUpdateFuncPtr = void (*)(bgz::MemoryBlock*, Platform_Services*, Rendering_Info*, Game_Sound_Output_Buffer*, Game_Input*);

const s32 Win32_MaxAllowableRecordedInputs { 4000 };
struct Win32_Game_Replay_State
{
    Game_Input* RecordedInputs { nullptr };
    s32 InputCount {};
    s32 TotalInputStructsRecorded {};
    void* OriginalRecordedGameState { nullptr };
    
    bool  InputRecording { false };
    bool  InputPlayBack { false };
};