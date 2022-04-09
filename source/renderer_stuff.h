#ifndef RENDERER_STUFF_INCLUDE
#define RENDERER_STUFF_INCLUDE

#ifndef BYTES_PER_PIXEL
#define BYTES_PER_PIXEL 4
#endif

#include "stb/stb_truetype.h"

#include "my_math.h"
#include "memory_handling.h"
#include "dynamic_array.h"
#include "debug_array.h"
#include "runtime_array.h"

/*

    Current Renderer assumptions:

    1.) Renderer expects all verts to be in meters and not pixels.
    2.) Y axis is going up
3.) Assuming clockwise index ordering for verts

*/

//TODO: Separate out transform from pushtexture so that user pushes transform and textures separately

struct Camera3D
{
    v3 worldPos{};
    v3 rotation{};
};

enum struct Origin
{
    CENTER,
    BOTTOM_CENTER,
    BOTTOM_LEFT,
    TOP_LEFT
};

struct Transform_v3
{
    v3 translation{0.0f, 0.0f, 0.0f};
    v3 rotation{0.0f, 0.0f, 0.0f};
    v3 scale{1.0f, 1.0f, 1.0f};
    Origin originP{Origin::BOTTOM_LEFT};
};

struct Quad
{
    union
    {
        Array<v3, 4> vertices;
        struct
        {
            v3 bottomLeft;
            v3 bottomRight;
            v3 topRight;
            v3 topLeft;
        };
    };
};

struct QuadV2
{
    union
    {
        Array<v2, 4> vertices;
        struct
        {
            v2 bottomLeft;
            v2 bottomRight;
            v2 topRight;
            v2 topLeft;
        };
    };
};

struct Color
{
    int r, g, b, a;
    int elem[4];
};

struct Cube
{
    v3 _localMin{};
    v3 _localMax{};
    Color color{};
    Transform_v3 worldTransform{};
};

struct Rect
{
    v2 _localMin;
    v2 _localMax;
    Origin origin;
    Color color{};
    v2 pos{};
    v2 scale{1.0f, 1.0f};
    v2 rotation{};
    Transform_v3 _worldTransform{};//Not used for rect overlay stuff. Still here for regular rect stuff
};

struct Mesh
{
    //TODO: Will I need to store this for world to local transformations in gamecode???
    //RunTimeArr<f32> vertPositions{};
    Mat4x4 worldTransform{};
};

struct RenderCmdBuffer
{
    u8* baseAddress { nullptr };
    s32 usedAmount {};
    s32 size {};
    s32 entryCount {};
    u32 textureCount{};
};

struct Bitmap
{
    u8* data { nullptr };
    f32 aspectRatio {};
    s32 width_pxls {};
    s32 height_pxls {};
    s32 pitch_pxls {};
    f32 opacity { 1.0f };
    v2 scale { 1.0f, 1.0f };
    bool isLoadedOnGPU{false};//TODO: Eventually remove
};

struct Rendering_Info
{
    RenderCmdBuffer gameCmdBuffer;
    RenderCmdBuffer profilerCmdBuffer;
    int initialWidthOfScreen_pixels{};
    int initialHeightOfScreen_pixels{};
    int widthOfScreen_pixels{};
    int heightOfScreen_pixels{};
    Camera3D camera3d{};
    v3 clearColor{};
    u32 textTextureID{};
    s32 textVertObjID{};
    s32 rectVertObjID{};
    s32 cubeVertObjID{};
    bool userWantsToClearDepthBuf{false};
    f32 fov{};
    f32 aspectRatio{};
    f32 nearPlane{};
    f32 farPlane{};
    f32 _pixelsPerMeter {};
    s32 numTotalRenderablesLoaded{};//TODO: Eventually use this to determine array length I think
    
    //Font stuff
    stbtt_packedchar cdata[96]; // ASCII 32..126 is 95 glyphs
    stbtt_fontinfo fontInfo{}; //Contains pointer to font file data
    float fontHeight{};
    Bitmap fontAtlas{};
};

struct NormalMap
{
    u8* mapData { nullptr };
    f32 rotation {};
    v2 scale { 1.0f, 1.0f };
    f32 lightAngle {};
    f32 lightThreshold {};
};

struct Coordinate_Space
{
    v2 origin {};
    v2 xBasis {};
    v2 yBasis {};
};

struct Object_Transform
{
    f32 rotation {};
    v2 pos {};
    v2 scale {};
    Origin originP{};
};

enum Render_Entry_Type
{
    EntryType_InitVertexData,
    EntryType_DrawRectOverlay,
    EntryType_Line,
    EntryType_DrawRect,
    EntryType_DrawText,
    EntryType_DrawRectOutline,
    EntryType_DrawCube,
    EntryType_DrawMesh,
    EntryType_Texture,
    EntryType_LoadTexture
};

struct RenderEntry_Header
{
    Render_Entry_Type type;
};

struct RenderEntry_LoadTexture
{
    RenderEntry_Header header;
    Bitmap texture;
    u32 id{};
};

struct CharRenderInfo
{
    char character{};
    f32 width_pixels{};
    f32 height_pixels{};
    v2 pos_pixelCoords{};
    v2 minUV{};
    v2 maxUV{};
    f32 depth{};
    f32 textBaselinePos_y{};
};

struct RenderEntry_DrawText
{
    RenderEntry_Header header;
    bgz::DbgArray<CharRenderInfo, 100> characters;
    f32 depth{};
};

struct RenderEntry_DrawRectOverlay
{
    RenderEntry_Header header;
    v2 _localMin;
    v2 _localMax;
    Origin origin{};
    Transform_v3 transform{};
    v4 colorChange {};
    u32 textureID{};
    s32 vertObjID{};
    f32 zDepth{};
};

struct RenderEntry_DrawRect
{
    RenderEntry_Header header;
    Transform_v3 worldTransform{};
    v4 color{};
    u32 textureID{};
};

struct RenderEntry_DrawRectOutline
{
    RenderEntry_Header header;
    v2 min_normalized{};
    v2 max_normalized{};
    v4 color{};
    f32 lineThickness_0_to_1{};
};

struct RenderEntry_DrawCube
{
    RenderEntry_Header header;
    Transform_v3 worldTransform{};
    v4 color;
    u32 textureID{};
};

struct RenderEntry_DrawMesh
{
    RenderEntry_Header header;
    int renderIndexLength{};
    u32 meshID{};
    s32 textureID{};
    Mat4x4 worldTransform{};
};

//TODO: need way to verify your setting right num floats to correct attribute
struct VertexAttributeList
{
    struct VertexAttributeInfo {
        bool exists{};
        int numFloats{};
    };
    
    VertexAttributeInfo pos{};
    VertexAttributeInfo color{};
    VertexAttributeInfo texCoord{};
    VertexAttributeInfo normal{};
};

struct RenderEntry_InitVertexData
{
    RenderEntry_Header header;
    RunTimeArr<f32> interleavedVertAttribData{};
    RunTimeArr<s16> indicies{};
    VertexAttributeList vertexAttributes;
    int stride{};
    s32 vertObjectID{};
};

struct RenderEntry_Texture
{
    RenderEntry_Header header;
    const char* name;
    u8* colorData { nullptr };
    NormalMap normalMap {};
    v2i size {};
    s32 pitch_pxls {};
    v2 dimensions {};
    Array<v2, 2> uvBounds;
    Quad targetRect_worldCoords;
    bool isLoadedOnGPU{false};//TODO: Eventually remove
};

struct RenderEntry_Line
{
    RenderEntry_Header header;
    v2 minPoint;
    v2 maxPoint;
    v4 color;
    f32 thickness;
};

Bitmap LoadBitmap_BGRA(const char* fileName);

//Helpers
f32 BitmapWidth_Meters(Bitmap bitmap);
f32 BitmapHeight_Meters(Rendering_Info info, Bitmap bitmap);
v2 viewPortDimensions_Meters(Rendering_Info&& renderingInfo);
Rect CreateRect(f32 width, f32 height, v2 startingPos, Origin origin, Color color);
Cube CreateCube(v3 radius, v3 startingTranslation, Color color);
f32 Width(Rect rect);
f32 Height(Rect rect);
v4 ConvertColorTo0to1Values(Color colorToConvert);

//TODO: Need to add more things to init (like current window width/height in pxls) which right now happen in platform layer. Probably just need to move this call from gamecode to platform layer
void GPU_InitRenderer(Rendering_Info* renderingInfo, f32 fov, f32 aspectRatio, f32 nearPlane, f32 farPlane);
void GPU_SetCamera3D(Rendering_Info* renderingInfo, v3 cameraStartingPos, v3 camStartingRotation);
void GPU_Update3DCamera(RenderCmdBuffer* rendererCmdBuffer, v3 camWorldPos, v3 camRotation);

//All push commands
s32  GPUCmd_SendVertexData(Rendering_Info* renderingInfo, RenderCmdBuffer* cmdBuffer, bgz::Memory_Partition* memPart, RunTimeArr<f32> vertAttributes, int stride,  RunTimeArr<s16> indicies, VertexAttributeList vertAttribList);
void GPUCmd_SendRectVertexData(Rendering_Info* renderingInfo, RenderCmdBuffer* renderCmdBuffer, bgz::Memory_Partition* memPart, Color initialColor);
u32  GPUCmd_SendTextureData(RenderCmdBuffer* cmdBuffer, Bitmap bitmap);
void GPUCmd_SendFontAtlas(Rendering_Info* renderingInfo, RenderCmdBuffer* cmdBuffer, bgz::Memory_Partition* memPart);
s32  GPUCmd_SendBaseTextVertexData(Rendering_Info* renderingInfo, RenderCmdBuffer* renderCmdBuffer,  bgz::Memory_Partition* memPart);
void GPUCmd_Clear(Rendering_Info* renderingInfo, Color clearColor, bool clearDepthBuffer);
void GPUCmd_DrawRect(RenderCmdBuffer* cmdBuffer, Rect rect, f32 depth, u32 textureID);
void GPUCmd_DrawCube(RenderCmdBuffer* cmdBuffer, Cube cubeToDraw, u32 textureID);
void GPUCmd_DrawRectOutline(Rendering_Info* renderinInfo, RenderCmdBuffer* cmdBuffer, Rect rectToOutline, f32 depthOfOutline, f32 lineThickness, Color color);
void GPUCmd_DrawRectOutline(RenderCmdBuffer* cmdBuffer, v2 pos, Color color, f32 width, f32 height, f32 lineThickness, f32 depth);
void GPUCmd_DrawLine(RenderCmdBuffer* rendererCmdBuffer, v2 minPoint, v2 maxPoint, Color color, f32 thickness);
void GPUCmd_DrawMesh(Rendering_Info* rendererCmdBuffer, s32 id, s32 textureID, Mat4x4 worldTransformMatrix);

void GPUCmd_Overlay_DrawRect(RenderCmdBuffer* cmdBuffer, Rect rectToDraw, f32 depth, u32 textureID = 0);
void GPUCmd_Overlay_DrawRect(Rendering_Info* renderingInfo, RenderCmdBuffer* cmdBuffer, v2 pos, Color color, f32 width, f32 height, Origin origin, f32 depth, u32 textureID = 0);
void GPUCmd_Overlay_DrawRectOutline(RenderCmdBuffer* cmdBuffer, Rect rectToOutline, f32 depthOfOutline, f32 lineThickness, Color color);
void GPUCmd_Overlay_DrawRectOutline(RenderCmdBuffer* cmdBuffer, v2 pos, Color color, f32 width, f32 height, f32 lineThickness, f32 depth);
void GPUCmd_Overlay_DrawText(Rendering_Info* renderingInfo, RenderCmdBuffer* rendererCmdBuffer, const char* string, v2 screenPos, f32 depth,  f32 maxScreenPos_x, int pixelHeight = 0);

void ConvertNegativeToPositiveAngle_Radians(f32&& angle);
void ConvertToCorrectPositiveRadian(f32&& angle);
//void RenderToImage(Bitmap&& renderTarget, Bitmap sourceImage, Quad targetArea);
Quad WorldTransform(Quad localCoords, Object_Transform transformInfo_world);
Quad ProjectionTransform_Ortho(Quad cameraCoords, f32 pixelsPerMeter);
v2 ProjectionTransform_Ortho(v2 cameraCoords, f32 pixelsPerMeter);
Quad _ProduceQuadFromBottomLeftPoint(v2 originPoint, f32 width, f32 height);
Quad _ProduceQuadFromBottomMidPoint(v2 originPoint, f32 width, f32 height);
Quad ProduceQuadFromCenterPoint(v2 originPoint, f32 width, f32 height);
v2 Max(Rect rect);
v2 Min(Rect rect);

#endif //RENDERER_STUFF_INCLUDE_H

#ifdef GAME_RENDERER_STUFF_IMPL

v4 ConvertColorTo0to1Values(Color colorToConvert)
{
    v4 result{};
    result.r = (f32)colorToConvert.r / 255.0f;
    result.g = (f32)colorToConvert.g / 255.0f;
    result.b = (f32)colorToConvert.b / 255.0f;
    result.a = (f32)colorToConvert.a / 255.0f;
    
    return result;
};

//TODO: How to handle cube origins? Do I just give 12 different origin point options?(e.g. Front BL, Front center, mid center, Back BL)
Cube CreateCube(v3 radius, v3 startingTranslation, Color color)
{
    Cube result{};
    
    //Assuming center origin point for now
    result._localMin.x -= radius.x;
    result._localMin.y -= radius.y;
    result._localMin.z += radius.z;
    result._localMax.x += radius.x;
    result._localMax.y += radius.y;
    result._localMax.z -= radius.z;
    result.color = color;
    result.worldTransform.translation = startingTranslation;
    
    return result;
};

struct MinMaxPair { v2 min{}; v2 max{}; };
MinMaxPair GenerateMinMaxPositions(f32 width, f32 height, v2 pos, Origin originPoint)
{
    MinMaxPair result{};
    switch(originPoint)
    {
        case Origin::CENTER:
        {
            result.min = v2{pos.x - (width / 2.0f), pos.y - (height / 2.0f)};
            result.max = v2{pos.x + (width / 2.0f), pos.y + (height / 2.0f)};
        }break;
        
        case Origin::BOTTOM_CENTER:
        {
            result.min = v2{pos.x - (width / 2.0f), pos.y};
            result.max = v2{pos.x + (width / 2.0f), pos.y + height};
        }break;
        
        case Origin::BOTTOM_LEFT:
        {
            result.min = v2{pos.x, pos.y};
            result.max = v2{pos.x + width, pos.y + height};
        }break;
        
        case Origin::TOP_LEFT:
        {
            result.min = v2{pos.x, pos.y - height};
            result.max = v2{pos.x + width, pos.y};
        }break;
        
        InvalidDefaultCase;
    };
    
    return result;
};

void SetWidth(Rect&& rect, f32 newWidth)
{
    switch(rect.origin)
    {
        case Origin::CENTER:
        {
            f32 oldWidth = Width(rect);
            f32 widthDiff = (newWidth - oldWidth) / 2.0f;
            rect._localMin.x -= widthDiff;
            rect._localMax.x += widthDiff;
        }break;
        
        case Origin::BOTTOM_CENTER:
        {
        }break;
        
        case Origin::BOTTOM_LEFT:
        {
            rect._localMax.x = rect._localMin.x + newWidth;
        }break;
        
        case Origin::TOP_LEFT:
        {
            rect._localMax.x = rect._localMin.x + newWidth;
        }break;
        
        InvalidDefaultCase;
    };
};

void SetHeight(Rect&& rect, f32 newHeight)
{
    f32 currentRectHeight = Height(rect);
    switch(rect.origin)
    {
        case Origin::CENTER:
        {
            f32 heightDiff = (newHeight - currentRectHeight) / 2.0f;
            rect._localMin.y -= heightDiff;
            rect._localMax.y += heightDiff;
        }break;
        
        case Origin::BOTTOM_CENTER:
        {
        }break;
        
        case Origin::BOTTOM_LEFT:
        {
            rect._localMax.y = rect._localMin.y + newHeight;
        }break;
        
        case Origin::TOP_LEFT:
        {
            rect._localMin.y += (currentRectHeight - newHeight);
        }break;
        
        InvalidDefaultCase;
    };
};

Rect CreateRect(f32 width, f32 height, v2 startingTranslation, Origin origin, Color color)
{
    Rect result{};
    
    //TODO: Currently we always have to place rect (or any geometry) at world origin 0, 0, 0 because otherwise the object won't rotate/scale correctly.
    //Is this the proper way to do it? Or is there an alternative where you can start the point at some other world location and have things rotatte/scale/translate from there?
    MinMaxPair minMax = GenerateMinMaxPositions(width, height, v2{0.0f, 0.0f}, origin);
    
    result._localMin = minMax.min;
    result._localMax = minMax.max;
    result.color = color;
    result.origin = origin;
    result.pos = startingTranslation;
    
    return result;
};

f32 Width(Rect rect)
{
    Mat4x4 worldTransform = ProduceWorldTransformMatrix(v3{rect.pos, 0.0f}, v3{rect.rotation, 0.0f}, v3{rect.scale, 0.0f});
    v4 worldMin = worldTransform * v4{rect._localMin, 0.0f, 1.0f};
    v4 worldMax = worldTransform * v4{rect._localMax, 0.0f, 1.0f};
    
    return (worldMax.x) - (worldMin.x);
};

f32 Height(Rect rect)
{
    Mat4x4 worldTransform = ProduceWorldTransformMatrix(v3{rect.pos, 0.0f}, v3{rect.rotation, 0.0f}, v3{rect.scale, 1.0f});
    v4 worldMin = worldTransform * v4{rect._localMin, 0.0f, 1.0f};
    v4 worldMax = worldTransform * v4{rect._localMax, 0.0f, 1.0f};
    
    return (worldMax.y) - (worldMin.y);
};

v2 Min(Rect rect)
{
    Mat4x4 worldTransform = ProduceWorldTransformMatrix(v3{rect.pos, 0.0f}, v3{rect.rotation, 0.0f}, v3{rect.scale, 1.0f});
    v4 currentWorldMinPos = worldTransform * v4{rect._localMin, 0.0f, 1.0f};
    
    return currentWorldMinPos.xy;
};

v2 Max(Rect rect)
{
    Mat4x4 worldTransform = ProduceWorldTransformMatrix(v3{rect.pos, 0.0f}, v3{rect.rotation, 0.0f}, v3{rect.scale, 1.0f});
    v4 currentWorldMaxPos = worldTransform * v4{rect._localMax, 0.0f, 1.0f};
    
    return currentWorldMaxPos.xy;
};

void SetMinMax(Rect&& rect, v2 newMin, v2 newMax)
{
    BGZ_ASSERT(newMax.x >= newMin.x);
    BGZ_ASSERT(newMax.y >= newMin.y);
    
    Mat4x4 worldTransformMatrix = ProduceWorldTransformMatrix(v3{rect.pos, 0.0f}, v3{rect.rotation, 0.0f}, v3{rect.scale, 1.0f});
    Mat4x4 invertedMatrix = InvertMatrix(worldTransformMatrix);
    
    v4 localMax_v4 = invertedMatrix * v4{newMax, 0.0f, 1.0f};
    v4 localMin_v4 = invertedMatrix * v4{newMin, 0.0f, 1.0f};
    rect._localMax = localMax_v4.xy;
    rect._localMin = localMin_v4.xy;
};

void* _RenderCmdBuf_Push(RenderCmdBuffer* commandBuf, s32 sizeOfCommand)
{
    BGZ_ASSERT(commandBuf->usedAmount < commandBuf->size);//Not enough space on render buffer!"
    
    void* memoryPointer = (void*)(commandBuf->baseAddress + commandBuf->usedAmount);
    commandBuf->usedAmount += (sizeOfCommand);
    return memoryPointer;
};
#define RenderCmdBuf_Push(commandBuffer, commandType) (commandType*)_RenderCmdBuf_Push(commandBuffer, sizeof(commandType))

void GPU_InitRenderer(Rendering_Info* renderingInfo, f32 fov, f32 aspectRatio, f32 nearPlane, f32 farPlane)
{
    //ProjectionTransform
    //f32 fov = (InvTanR(1.0f/camMagnification) * 2.0f) / (PI / 180.0f); //For if you want to use a magnification value to calculate fov 1x, 2x, 10x, etc. Adjusting fov too much causes distortion or 'fish eye' effect though
    renderingInfo->fov = fov;
    renderingInfo->aspectRatio = aspectRatio;
    renderingInfo->nearPlane = nearPlane;
    renderingInfo->farPlane = farPlane;
};

s32  GPUCmd_SendVertexData(Rendering_Info* renderingInfo, RenderCmdBuffer* cmdBuffer, bgz::Memory_Partition* memPart, RunTimeArr<f32> vertAttribs, int stride, RunTimeArr<s16> indicies, VertexAttributeList vertAttribList)
{
    RenderEntry_InitVertexData* vertexData = RenderCmdBuf_Push(cmdBuffer, RenderEntry_InitVertexData);
    
    InitArr($(vertexData->interleavedVertAttribData), memPart, vertAttribs.Size());
    InitArr($(vertexData->indicies), memPart, indicies.Size());
    
    vertexData->header.type = EntryType_InitVertexData;
    vertexData->stride = stride;
    vertexData->vertexAttributes = vertAttribList;
    vertexData->vertObjectID = ++renderingInfo->numTotalRenderablesLoaded;
    vertexData->interleavedVertAttribData = CopyArray(memPart, vertAttribs);
    vertexData->indicies = CopyArray(memPart, indicies);
    
    ++cmdBuffer->entryCount;
    
    return renderingInfo->numTotalRenderablesLoaded;
};

//Rect vertex data should be sent to gpu only once and all rect draw calls will manipulate these values.
//Initial rect vertex position will be min 0, 0 (bottom left of screen) and max 1, 1 (top right of screen)
void GPUCmd_SendRectVertexData(Rendering_Info* renderingInfo, RenderCmdBuffer* cmdBuffer, bgz::Memory_Partition* memPart, Color initialColor)
{
    //Already loaded rect vertexData for all calls to use
    if(NOT renderingInfo->rectVertObjID)
    {
        Array<v3, 4> vertPositions{ v3{0.0f, 0.0f, 0.0f}, v3{1.0f, 0.0f, 0.0f}, v3{1.0f, 1.0f, 0.0f}, v3{0.0f, 1.0f, 0.0f} };
        Array<v4, 4> vertColors{ v4{ConvertColorTo0to1Values(initialColor)}, v4{ConvertColorTo0to1Values(initialColor)}, v4{ConvertColorTo0to1Values(initialColor)}, v4{ConvertColorTo0to1Values(initialColor)}};
        Array<v2, 4> vertUVs{ v2{0.0f, 0.0f}, v2{1.0f, 0.0f}, v2{1.0f, 1.0f}, v2{0.0f, 1.0f} };
        
        s64 totalLengthOfVertexBuffer{};
        totalLengthOfVertexBuffer += (vertPositions.Size() * 3);
        totalLengthOfVertexBuffer += (vertColors.Size() * 3);
        totalLengthOfVertexBuffer += (vertUVs.Size() * 2);
        RunTimeArr<f32> allVertexAttribData{memPart, totalLengthOfVertexBuffer};
        
        //Clockwise index ordering is what renderer expects
        RunTimeArr<s16> indicies{memPart, 6};
        int writePos{};
        indicies[writePos++] = 0;
        indicies[writePos++] = 2;
        indicies[writePos++] = 1;
        indicies[writePos++] = 0;
        indicies[writePos++] = 3;
        indicies[writePos++] = 2;
        
        writePos = 0;
        for(int i{}; i < 4; ++i)
        {
            for(int j{}; j < 3; ++j)
                allVertexAttribData[writePos++] = vertPositions[i].elem[j];
            
            for(int j{}; j < 3; ++j)
                allVertexAttribData[writePos++] = vertColors[i].elem[j];
            
            for(int j{}; j < 2; ++j)
                allVertexAttribData[writePos++] = vertUVs[i].elem[j];
        };
        
        int stride = 3 + 3 + 2;
        VertexAttributeList vertAttribs { /*pos*/{true, 3}, /*color*/{true, 3}, /*texCoords*/{true, 2} };
        renderingInfo->rectVertObjID = GPUCmd_SendVertexData(renderingInfo, cmdBuffer, memPart, allVertexAttribData, stride, indicies, vertAttribs);
    }
};

void GPUCmd_SendCubeVertexData(Rendering_Info* renderingInfo, RenderCmdBuffer* cmdBuffer, bgz::Memory_Partition* memPart, Color initialColor)
{
    //if hadn't already loaded cube vertexData for all calls to use
    if(NOT renderingInfo->cubeVertObjID)
    {
        Array<v3, 8> vertPositions {
            v3{-0.5f, 0.5f, -0.5f }, //0
            v3{+0.5f, 0.5f, -0.5f }, //1
            v3{-0.5f, -0.5f, -0.5f },//2
            v3{+0.5f, -0.5f, -0.5f },//3
            v3{+0.5f, -0.5f, +0.5f },//4
            v3{+0.5f, +0.5f, +0.5f },//5
            v3{-0.5f, +0.5f, +0.5f },//6
            v3{-0.5f, -0.5f, +0.5f } //7
        };
        Array<v4, 8> vertColors {
            v4{ConvertColorTo0to1Values(initialColor)},
            v4{ConvertColorTo0to1Values(initialColor)},
            v4{ConvertColorTo0to1Values(initialColor)},
            v4{ConvertColorTo0to1Values(initialColor)},
            v4{ConvertColorTo0to1Values(initialColor)},
            v4{ConvertColorTo0to1Values(initialColor)},
            v4{ConvertColorTo0to1Values(initialColor)},
            v4{ConvertColorTo0to1Values(initialColor)}
        };
        
        int writePos{};
        RunTimeArr<s16> indices{memPart, 36};
        //Front Side
        indices[writePos++] = 0; indices[writePos++] = 2; indices[writePos++] = 1; indices[writePos++] = 3; indices[writePos++] = 1; indices[writePos++] = 2;
        //Right Side
        indices[writePos++] = 1; indices[writePos++] = 3; indices[writePos++] = 4; indices[writePos++] = 4; indices[writePos++] = 5; indices[writePos++] = 1;
        //Back Side
        indices[writePos++] = 4; indices[writePos++] = 7; indices[writePos++] = 5; indices[writePos++] = 5; indices[writePos++] = 7; indices[writePos++] = 6;
        //Left Side
        indices[writePos++] = 0; indices[writePos++] = 6; indices[writePos++] = 7; indices[writePos++] = 2; indices[writePos++] = 0; indices[writePos++] = 7;
        //Top Side
        indices[writePos++] = 6; indices[writePos++] = 0; indices[writePos++] = 5; indices[writePos++] = 1; indices[writePos++] = 5; indices[writePos++] = 0;
        //Bottom Side
        indices[writePos++] = 3; indices[writePos++] = 2; indices[writePos++] = 4; indices[writePos++] = 2; indices[writePos++] = 7; indices[writePos++] = 4;
        
        s64 totalLengthOfVertexBuffer{};
        totalLengthOfVertexBuffer += (vertPositions.Size() * 3);
        totalLengthOfVertexBuffer += (vertColors.Size() * 3);
        RunTimeArr<f32> allVertexAttribData{memPart, totalLengthOfVertexBuffer};
        
        writePos = 0;
        for(int i{}; i < 8; ++i)
        {
            for(int j{}; j < 3; ++j)
                allVertexAttribData[writePos++] = vertPositions[i].elem[j];
            
            for(int j{}; j < 3; ++j)
                allVertexAttribData[writePos++] = vertColors[i].elem[j];
        };
        
        int stride = 3 + 3;
        VertexAttributeList vertAttribs { /*pos*/{true, 3}, /*color*/{true, 3} };
        renderingInfo->cubeVertObjID = GPUCmd_SendVertexData(renderingInfo, cmdBuffer, memPart, allVertexAttribData, stride, indices, vertAttribs);
    };
};

//Verts I send to gpu for text will be between 0 and 1 for easier text transformations
s32 GPUCmd_SendBaseTextVertexData(Rendering_Info* renderingInfo, RenderCmdBuffer* cmdBuffer,  bgz::Memory_Partition* memPart)
{
    Array<v3, 4> vertPositions{ v3{0.0f, 0.0f, 0.0f}, v3{1.0f, 0.0f, 0.0f}, v3{1.0f, 1.0f, 0.0f}, v3{0.0f, 1.0f, 0.0f} };
    
    s64 totalLengthOfVertexBuffer{};
    totalLengthOfVertexBuffer += (vertPositions.Size() * 3);
    
    RunTimeArr<f32> allVertexAttribData{memPart, totalLengthOfVertexBuffer};
    RunTimeArr<s16> indicies{memPart, 6};
    int writePos{};
    indicies[writePos++] = 0;
    indicies[writePos++] = 2;
    indicies[writePos++] = 1;
    indicies[writePos++] = 0;
    indicies[writePos++] = 3;
    indicies[writePos++] = 2;
    
    writePos = 0;
    for(int i{}; i < vertPositions.Size(); ++i)
    {
        for(int j{}; j < 3; ++j)
            allVertexAttribData[writePos++] = vertPositions[i].elem[j];
    };
    
    int stride = 3;
    VertexAttributeList vertAttribs { /*pos*/{true, 3} };
    renderingInfo->textVertObjID = GPUCmd_SendVertexData(renderingInfo, cmdBuffer, memPart, allVertexAttribData, stride, indicies, vertAttribs);
    
    return renderingInfo->textVertObjID;
};

u32 GPUCmd_SendTextureData(RenderCmdBuffer* cmdBuffer, Bitmap bitmap)
{
    BGZ_ASSERT(bitmap.data);//Invalid/null texture data!"
    
    RenderEntry_LoadTexture* loadTextureEntry = RenderCmdBuf_Push(cmdBuffer, RenderEntry_LoadTexture);
    
    loadTextureEntry->header.type = EntryType_LoadTexture;
    loadTextureEntry->texture = bitmap;
    loadTextureEntry->id = ++cmdBuffer->textureCount;
    
    ++cmdBuffer->entryCount;
    
    return cmdBuffer->textureCount;
};

void GPUCmd_SendFontAtlas(Rendering_Info* renderingInfo, RenderCmdBuffer* cmdBuffer, bgz::Memory_Partition* memPart)
{
    renderingInfo->textTextureID = GPUCmd_SendTextureData(cmdBuffer, renderingInfo->fontAtlas);
    GPUCmd_SendBaseTextVertexData(global_renderingInfo, cmdBuffer, memPart);
};

void GPUCmd_Clear(Rendering_Info* renderingInfo, Color clearColor, bool clearDepthThisFrame)
{
    v4 clearColorV4 = ConvertColorTo0to1Values(clearColor);
    renderingInfo->clearColor = clearColorV4.rgb;
    renderingInfo->userWantsToClearDepthBuf = clearDepthThisFrame;
};

void GPUCmd_DrawLine(RenderCmdBuffer* cmdBuffer, v2 minPoint, v2 maxPoint, Color color, f32 thickness)
{
    RenderEntry_Line* lineEntry = RenderCmdBuf_Push(cmdBuffer, RenderEntry_Line);
    
    lineEntry->header.type = EntryType_Line;
    lineEntry->minPoint = minPoint;
    lineEntry->maxPoint = maxPoint;
    lineEntry->color = ConvertColorTo0to1Values(color);
    lineEntry->thickness = thickness;
    
    ++cmdBuffer->entryCount;
};

void GPUCmd_Overlay_DrawText(Rendering_Info* renderingInfo, RenderCmdBuffer* cmdBuffer, const char* string, v2 screenPos, f32 depth, f32 maxScreenPos_x, int pixelHeight)
{
    RenderEntry_DrawText* textEntry = RenderCmdBuf_Push(cmdBuffer, RenderEntry_DrawText);
    
    textEntry->header.type = EntryType_DrawText;
    textEntry->characters.length = 0;
    textEntry->depth = depth;
    
    sizet strLen = strlen(string);
    f32 xScreenPosOfFontBaseline_pixels{screenPos.x}, yScreenPosOfFontBaseline_pixels{screenPos.y + (renderingInfo->fontHeight)};
    f32 startingXPos{};
    for(int i{}; i < strLen; ++i)
    {
        int character = (int)string[i];
        stbtt_aligned_quad q;
        stbtt_GetPackedQuad(renderingInfo->cdata, 512, 512, character - 32, &xScreenPosOfFontBaseline_pixels, &yScreenPosOfFontBaseline_pixels, &q, 0);
        
        if(character == ' ')
            q.x1 = q.x0 + renderingInfo->fontHeight * .4f;
        
        f32 width = q.x1 - q.x0;
        f32 height = q.y1 - q.y0;
        
        if(i == 0)
            startingXPos = q.x0;
        
        CharRenderInfo characterInfo{};
        characterInfo.textBaselinePos_y = yScreenPosOfFontBaseline_pixels;
        characterInfo.minUV = v2{q.s0, q.t0};
        characterInfo.maxUV = v2{q.s1, q.t1};
        characterInfo.height_pixels = height;
        characterInfo.width_pixels = width;
        characterInfo.pos_pixelCoords = v2{ startingXPos, q.y1};
        characterInfo.character = (char)character;
        
        startingXPos += width;/*setup for next char*/
        
        textEntry->characters.Push(characterInfo);
    };
    
    ++cmdBuffer->entryCount;
};

void GPUCmd_DrawCube(RenderCmdBuffer* cmdBuffer, Cube cubeToDraw, u32 textureID = 0)
{
    RenderEntry_DrawCube* cubeEntry = RenderCmdBuf_Push(cmdBuffer, RenderEntry_DrawCube);
    *cubeEntry = {};
    
    //Because there are already verts stored in GPU memmory for a cube we just need to adjust the scale
    //here to adjust those verts
    cubeEntry->header.type = EntryType_DrawCube;
    cubeEntry->worldTransform.scale.x *= AbsoluteValFloat(cubeToDraw._localMax.x - cubeToDraw._localMin.x);
    cubeEntry->worldTransform.scale.y *= AbsoluteValFloat(cubeToDraw._localMax.y - cubeToDraw._localMin.y);
    cubeEntry->worldTransform.scale.z *= AbsoluteValFloat(cubeToDraw._localMax.z - cubeToDraw._localMin.z);
    cubeEntry->worldTransform.translation = cubeToDraw.worldTransform.translation;
    cubeEntry->worldTransform.rotation = cubeToDraw.worldTransform.rotation;
    cubeEntry->color = ConvertColorTo0to1Values(cubeToDraw.color);
    
    ++cmdBuffer->entryCount;
};

void GPUCmd_DrawRect(RenderCmdBuffer* cmdBuffer, Rect rectToDraw, f32 depth, u32 textureID = 0)
{
    RenderEntry_DrawRect* rectEntry = RenderCmdBuf_Push(cmdBuffer, RenderEntry_DrawRect);
    
    {//Setup world transform which takes into account previously stored rect vert information from GPUCMD_SendRectVertData call
        switch(rectToDraw.origin)
        {
            case Origin::TOP_LEFT:
            {
                rectToDraw.pos += (rectToDraw._localMin * rectToDraw._worldTransform.scale.y) - v2{0.0f, 0.0f};//GPU rect verts are assumed to be min 0,0 max 1,1 bottom_left origin
            }break;
            
            case Origin::BOTTOM_LEFT:
            {
                //do nothing since GPU verts are already assumed to be bottom left
            }break;
            
            InvalidDefaultCase;
        };
        
        //Need to adjust scale to work with verts that are currently stored on gpu
        rectToDraw._worldTransform.scale.x *= Width(rectToDraw);
        rectToDraw._worldTransform.scale.y *= Height(rectToDraw);
        
        rectToDraw._worldTransform.translation.z = depth;
    };
    
    rectEntry->header.type = EntryType_DrawRect;
    rectEntry->worldTransform = rectToDraw._worldTransform;
    rectEntry->textureID = textureID;
    rectEntry->color = ConvertColorTo0to1Values(rectToDraw.color);
    
    ++cmdBuffer->entryCount;
};

void GPUCmd_Overlay_DrawRect(RenderCmdBuffer* cmdBuffer, Rect rectToDraw, f32 depth, u32 textureID)
{
    RenderEntry_DrawRectOverlay* rectEntryOverlay = RenderCmdBuf_Push(cmdBuffer, RenderEntry_DrawRectOverlay);
    
    rectEntryOverlay->header.type = EntryType_DrawRectOverlay;
    rectEntryOverlay->textureID = textureID;
    rectEntryOverlay->transform = Transform_v3{v3{rectToDraw.pos, depth}, v3{rectToDraw.rotation, 0.0f}, v3{rectToDraw.scale, 0.0f}};
    rectEntryOverlay->colorChange = ConvertColorTo0to1Values(rectToDraw.color);
    rectEntryOverlay->_localMin = rectToDraw._localMin;
    rectEntryOverlay->_localMax = rectToDraw._localMax;
    rectEntryOverlay->origin = rectToDraw.origin;
    rectEntryOverlay->zDepth = depth;
    
    ++cmdBuffer->entryCount;
};

void GPUCmd_Overlay_DrawRect(Rendering_Info* renderingInfo, RenderCmdBuffer* cmdBuffer, v2 pos, Color color, f32 width, f32 height, Origin origin, f32 depth, u32 textureID)
{
    RenderEntry_DrawRectOverlay* rectEntryOverlay = RenderCmdBuf_Push(cmdBuffer, RenderEntry_DrawRectOverlay);
    
    Rect rectToDraw = CreateRect(width, height, pos, origin, color);
    
    rectEntryOverlay->header.type = EntryType_DrawRectOverlay;
    rectEntryOverlay->textureID = textureID;
    rectEntryOverlay->transform = Transform_v3{v3{rectToDraw.pos, depth}, v3{rectToDraw.rotation, 0.0f}, v3{rectToDraw.scale, 0.0f}};
    rectEntryOverlay->colorChange = ConvertColorTo0to1Values(rectToDraw.color);
    rectEntryOverlay->_localMin = rectToDraw._localMin;
    rectEntryOverlay->_localMax = rectToDraw._localMax;
    rectEntryOverlay->origin = rectToDraw.origin;
    rectEntryOverlay->zDepth = depth;
    
    ++cmdBuffer->entryCount;
};

void GPUCmd_Overlay_DrawRectOutline(RenderCmdBuffer* cmdBuffer, Rect rectToOutline, f32 depth, f32 lineThickness, Color color)
{
    f32 width = lineThickness;
    f32 height = Max(rectToOutline).y - Min(rectToOutline).y;
    Rect leftOutlineRect = CreateRect(width, height, Min(rectToOutline), Origin::BOTTOM_LEFT, color);
    
    width = lineThickness;
    height = Max(rectToOutline).y - Min(rectToOutline).y;
    Rect rightOutlineRect = CreateRect(width, height, v2{Max(rectToOutline).x - width, Min(rectToOutline).y}, Origin::BOTTOM_LEFT, color);
    
    width = Max(rectToOutline).x - Min(rectToOutline).x;
    height = lineThickness;
    Rect topOutlineRect = CreateRect(width, height, v2{Min(rectToOutline).x, Max(rectToOutline).y - lineThickness}, Origin::BOTTOM_LEFT, color);
    
    width = Max(rectToOutline).x - Min(rectToOutline).x;
    height = lineThickness;
    Rect bottomOutlineRect = CreateRect(width, height, v2{Min(rectToOutline).x, Min(rectToOutline).y}, Origin::BOTTOM_LEFT, color);
    
    GPUCmd_Overlay_DrawRect(cmdBuffer, leftOutlineRect, depth);
    GPUCmd_Overlay_DrawRect(cmdBuffer, rightOutlineRect, depth);
    GPUCmd_Overlay_DrawRect(cmdBuffer, topOutlineRect, depth);
    GPUCmd_Overlay_DrawRect(cmdBuffer, bottomOutlineRect, depth);
};

void GPUCmd_Overlay_DrawRectOutline(RenderCmdBuffer* cmdBuffer, v2 pos, Color color, f32 width, f32 height, f32 lineThickness, f32 depth)
{
    Rect rectToOutline = CreateRect(width, height, pos, Origin::BOTTOM_LEFT, color);
    GPUCmd_Overlay_DrawRectOutline(cmdBuffer, rectToOutline, depth, lineThickness, color);
};

void GPUCmd_DrawRectOutline(RenderCmdBuffer* cmdBuffer, Rect rectToOutline, f32 depth, f32 lineThickness_0_to_1, Color color)
{
    lineThickness_0_to_1 *= .1f;
    
    f32 width = lineThickness_0_to_1;
    f32 height = Max(rectToOutline).y - Min(rectToOutline).y;
    v2 test = Min(rectToOutline);
    Rect leftOutlineRect = CreateRect(width, height, Min(rectToOutline), Origin::BOTTOM_LEFT, color);
    
    width = lineThickness_0_to_1;
    height = Max(rectToOutline).y - Min(rectToOutline).y;
    Rect rightOutlineRect = CreateRect(width, height, v2{Max(rectToOutline).x - width, Min(rectToOutline).y}, Origin::BOTTOM_LEFT, color);
    
    width = Max(rectToOutline).x - Min(rectToOutline).x;
    height = lineThickness_0_to_1;
    Rect topOutlineRect = CreateRect(width, height, v2{Min(rectToOutline).x, Max(rectToOutline).y - lineThickness_0_to_1}, Origin::BOTTOM_LEFT, color);
    
    width = Max(rectToOutline).x - Min(rectToOutline).x;
    height = lineThickness_0_to_1;
    Rect bottomOutlineRect = CreateRect(width, height, v2{Min(rectToOutline).x, Min(rectToOutline).y}, Origin::BOTTOM_LEFT, color);
    
    GPUCmd_DrawRect(cmdBuffer, leftOutlineRect, depth);
    GPUCmd_DrawRect(cmdBuffer, rightOutlineRect, depth);
    GPUCmd_DrawRect(cmdBuffer, topOutlineRect, depth);
    GPUCmd_DrawRect(cmdBuffer, bottomOutlineRect, depth);
};

void GPUCmd_DrawMesh(RenderCmdBuffer* cmdBuffer, s32 meshID, s32 textureID, Mat4x4 worldTransform, int renderIndexLength)
{
    RenderEntry_DrawMesh* meshEntry = RenderCmdBuf_Push(cmdBuffer, RenderEntry_DrawMesh);
    
    meshEntry->header.type = EntryType_DrawMesh;
    meshEntry->meshID = meshID;
    meshEntry->textureID = textureID;
    meshEntry->worldTransform = worldTransform;
    meshEntry->renderIndexLength = renderIndexLength;
    
    ++cmdBuffer->entryCount;
};

void GPU_SetCamera3D(Rendering_Info* renderingInfo, v3 cameraStartingPos, v3 camStartingRotation)
{
    renderingInfo->camera3d.worldPos = cameraStartingPos;
    renderingInfo->camera3d.rotation = camStartingRotation;
};

void GPU_Update3DCamera(Rendering_Info* renderingInfo, v3 cameraTranslation, v3 camRotation)
{
    renderingInfo->camera3d.worldPos += cameraTranslation;
    renderingInfo->camera3d.rotation += camRotation;
};

Bitmap LoadBitmap_BGRA(const char* fileName)
{
    Bitmap result;
    
    { //Load image data using stb (w/ user defined read/seek functions and memory allocation functions
        stbi_set_flip_vertically_on_load(true); //So first byte stbi_load() returns is bottom left instead of top-left of image (which is stb's default)
        
        s32 numOfLoadedChannels {};
        s32 desiredChannels { 4 }; //Since I still draw assuming 4 byte pixels I need 4 channels
        
        //Returns RGBA
        unsigned char* bitmapData = stbi_load(fileName, &result.width_pxls, &result.height_pxls, &numOfLoadedChannels, desiredChannels);
        BGZ_ASSERT(bitmapData);//Invalid image data!"
        
        s32 totalPixelCountOfImg = result.width_pxls * result.height_pxls;
        u32* bitmapPixel = (u32*)bitmapData;
        
        //Swap R and B channels of image
        for (int i = 0; i < totalPixelCountOfImg; ++i)
        {
            auto color = UnPackPixelValues(*bitmapPixel, RGBA);
            
            //Pre-multiplied alpha
            f32 alphaBlend = color.a / 255.0f;
            color.rgb *= alphaBlend;
            
            u32 newSwappedPixelColor = (((u8)color.a << 24) | ((u8)color.r << 16) | ((u8)color.g << 8) | ((u8)color.b << 0));
            
            *bitmapPixel++ = newSwappedPixelColor;
        }
        
        result.data = (u8*)bitmapData;
    };
    
    result.aspectRatio = (f32)result.width_pxls / (f32)result.height_pxls;
    result.pitch_pxls = (u32)result.width_pxls * BYTES_PER_PIXEL;
    
    return result;
};

void CreateFontAtlasFromFile_TTF(Rendering_Info* renderingInfo, const char* ttfFontFile, int pixelHeightForFont)
{
    Bitmap result;
    
    renderingInfo->fontHeight = (float)pixelHeightForFont;
    
    s32 length{};
    char* fontFileContents = (char*)globalPlatformServices->ReadEntireFile($(length), ttfFontFile);//Current not freed anywhere
    BGZ_ASSERT(stbtt_InitFont(&renderingInfo->fontInfo, (const unsigned char*)fontFileContents, 0));
    
    //TODO: User should define atlas size
    int bitmapWidth_pxls = 512;
    int bitmapHeight_pxls = 512;
    unsigned char* bitmap = (unsigned char*)globalPlatformServices->Malloc(bitmapWidth_pxls * bitmapHeight_pxls);//Current not freed anywhere
    
    //Make sure text height specified will fit within above bitmap
    int pixelWidth = (int)(pixelHeightForFont * 1.5f);//Just over estimating pixel width per char as I'm sure it will vary and won't be this much per char
    int totalArea = pixelWidth * pixelHeightForFont * 96/*num chars we will load*/;
    BGZ_ASSERT(totalArea < (512*512));
    
    //Create font bitmap
    stbtt_pack_context packContext{};
    BGZ_ASSERT(stbtt_PackBegin(&packContext, bitmap, bitmapWidth_pxls, bitmapHeight_pxls, 0, 1, 0));
    stbtt_PackFontRange(&packContext, (const unsigned char*)fontFileContents, 0/*I think if you have multiple fonts your packing*/, (float)pixelHeightForFont, ' ', 96, renderingInfo->cdata);
    
    unsigned char* bitmapStart = bitmap;
    
    //This flips font bitmap vertically so stored chars are upside down and at bottom of bitmap. Can't really use this stb truetype though
    //since truetype sets up bounding boxes and coords assuming top down, y+ with text at top. Could probably figure it out if you want to
    //take the time but for now I'm just flipping things in opengl layer (uv & char pos coords) to work with opengl's expected bottom left origin, y+=up scheme
#if 0
    for(int y{}; y < bitmapHeight_pxls/2; ++y)
    {
        for(int x{}; x < bitmapWidth_pxls; ++x)
        {
            int temp = bitmap[x + (y * bitmapWidth_pxls)];
            bitmap[x + (y * bitmapWidth_pxls)] = bitmap[x + ((bitmapHeight_pxls - 1) * bitmapWidth_pxls) - (y * bitmapWidth_pxls)];
            bitmap[x + ((bitmapHeight_pxls - 1) * bitmapWidth_pxls) - (y * bitmapWidth_pxls)] = temp;
        };
    };
#endif
    
    stbtt_PackEnd(&packContext);
    
    //Need to convert 1 byte colors from stb's text bitmap to our 4 byte color scheme (keeps things easy)
    s32 totalPixelCountOfImg = bitmapWidth_pxls * bitmapHeight_pxls;
    u8* sourcePixel = (u8*)bitmap;
    u32* destPixel = (u32*)globalPlatformServices->Malloc(totalPixelCountOfImg * sizeof(u32));
    u32* startOfDest = destPixel;
    for (int i = 0; i < totalPixelCountOfImg; ++i)
    {
        u32 newSwappedPixelColor = ((*sourcePixel << 24) | (*sourcePixel << 16) | (*sourcePixel << 8) | (*sourcePixel << 0));
        *destPixel++ = newSwappedPixelColor;
        ++sourcePixel;
    }
    
    //Can use this to test and make sure bitmap came out okay
    //stbi_write_png("out.png", bitmapWidth_pxls, bitmapHeight_pxls, 4, startOfDest, bitmapWidth_pxls * BYTES_PER_PIXEL);
    
    result.data = (u8*)startOfDest;
    result.aspectRatio = (f32)bitmapWidth_pxls / (f32)bitmapHeight_pxls;
    result.width_pxls = bitmapWidth_pxls;
    result.height_pxls = bitmapHeight_pxls;
    result.pitch_pxls = (u32)result.width_pxls * BYTES_PER_PIXEL;
    
    renderingInfo->fontAtlas = result;
};


//TODO: Remove hard coded z value.
Quad ProduceQuadFromCenterPoint(v2 originPoint, f32 width, f32 height)
{
    Quad result;
    
    result.bottomLeft = { originPoint.x - (width / 2.0f), originPoint.y - (height / 2.0f), 0.0f };
    result.bottomRight = { originPoint.x + (width / 2.0f), originPoint.y - (height / 2.0f), 0.0f };
    result.topRight = { originPoint.x + (width / 2.0f), originPoint.y + (height / 2.0f), 0.0f };
    result.topLeft = { originPoint.x - (width / 2.0f), originPoint.y + (height / 2.0f), 0.0f };
    
    return result;
};

f32 BitmapHeight_Meters(Rendering_Info renderingInfo, Bitmap bitmap)
{
    return bitmap.height_pxls / renderingInfo._pixelsPerMeter;
};

Quad _ProduceQuadFromBottomLeftPoint(v2 originPoint, f32 width, f32 height)
{
    Quad Result;
    
    Result.bottomLeft.xy = originPoint;
    Result.bottomRight.xy = { originPoint.x + width, originPoint.y };
    Result.topRight.xy = { originPoint.x + width, originPoint.y + height };
    Result.topLeft.xy = { originPoint.x, originPoint.y + height };
    
    return Result;
};

#endif //GAME_RENDERER_STUFF_IMPL

#ifdef PLATFORM_RENDERER_STUFF_IMPL

v2 sp_DilateAboutArbitraryPoint(v2 PointOfDilation, f32 ScaleFactor, v2 vectorToDilate)
{
    v2 dilatedVector{};
    
    v2 distance = PointOfDilation - vectorToDilate;
    distance *= ScaleFactor;
    dilatedVector = PointOfDilation - distance;
    
    return dilatedVector;
};

auto sp_DilateAboutArbitraryPoint(v2 PointOfDilation, f32 ScaleFactor, Quad QuadToDilate) -> Quad
{
    Quad DilatedQuad {};
    
    for (s32 vertIndex = 0; vertIndex < 4; ++vertIndex)
    {
        v2 Distance = PointOfDilation - QuadToDilate.vertices[vertIndex].xy;
        Distance *= ScaleFactor;
        DilatedQuad.vertices[vertIndex].xy = PointOfDilation - Distance;
    };
    
    return DilatedQuad;
};

v2 ProjectionTransform_Ortho(v2 cameraCoords, f32 pixelsPerMeter)
{
    cameraCoords *= pixelsPerMeter;
    
    return cameraCoords;
};

local_func auto _LinearBlend(u32 foregroundColor, u32 backgroundColor, ChannelType colorFormat)
{
    struct Result
    {
        u8 blendedPixel_R, blendedPixel_G, blendedPixel_B;
    };
    Result blendedColor {};
    
    v4 foreGroundColors = UnPackPixelValues(foregroundColor, colorFormat);
    v4 backgroundColors = UnPackPixelValues(backgroundColor, colorFormat);
    
    f32 blendPercent = foreGroundColors.a / 255.0f;
    
    blendedColor.blendedPixel_R = (u8)Lerp(backgroundColors.r, foreGroundColors.r, blendPercent);
    blendedColor.blendedPixel_G = (u8)Lerp(backgroundColors.g, foreGroundColors.g, blendPercent);
    blendedColor.blendedPixel_B = (u8)Lerp(backgroundColors.b, foreGroundColors.b, blendPercent);
    
    return blendedColor;
};

//TODO: Remove hard coded z value.
Quad _ProduceQuadFromBottomMidPoint(v2 originPoint, f32 width, f32 height)
{
    Quad result;
    
    result.bottomLeft = { originPoint.x - (width / 2.0f), originPoint.y, 0.0f };
    result.bottomRight = { originPoint.x + (width / 2.0f), originPoint.y, 0.0f };
    result.topRight = { originPoint.x + (width / 2.0f), originPoint.y + height, 0.0f };
    result.topLeft = { originPoint.x - (width / 2.0f), originPoint.y + height, 0.0f };
    
    return result;
};

#endif //PLATFORM_RENDERER_STUFF_IMPL