#ifndef OPENGL_INCLUDE_H
#define OPENGL_INCLUDE_H

#include "renderer_stuff.h"
#include "memory_handling.h"
#include "my_math.h"

const char* basicVertexShaderCode =
R"HereDoc(

#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec4 color;
in layout(location=2) vec2 texCoord;
in layout(location=3) vec2 normal;

out vec2 fragTexCoord;
out vec4 fragColor;

uniform mat4 transformationMatrix;

void main()
{

  gl_Position = vec4(position, 1.0) * transformationMatrix;//vector is on the left side because my matrices are row major
fragTexCoord = texCoord;
fragColor = color;

};

)HereDoc";

const char* basicFragmentShaderCode =
R"HereDoc(

#version 430

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 color;

uniform sampler2D textureSlotToSampleFrom;//This is the texture slot you want to sample from (not the texture id! Modern opengl has a max of 32 texture slots you can use I believe);
uniform bool userWantsToDrawFromTexture;
uniform vec4 colorChange;

void main()
{

if(userWantsToDrawFromTexture)
{
vec4 vcolor = colorChange;
 color = vcolor * texture(textureSlotToSampleFrom, fragTexCoord);
}
else
{
color = colorChange;
};

};

)HereDoc";

const char* textVertexShaderCode =
R"HereDoc(

#version 430

in layout(location=0) vec3 unitSquareVerts;

uniform vec2 min_texCoordinates;
uniform float width_texCoordinates;
uniform float height_texCoordinates;
uniform mat4 transformationMatrix;
uniform vec2 charOffset_topLeft;
uniform float widthOfChar_normalized;
uniform float heightOfChar_normalized;

out vec2 texCoordinates;

void main()
{

float texCoordinates_x = (unitSquareVerts.x * width_texCoordinates) + min_texCoordinates.x;
float texCoordinates_y = (unitSquareVerts.y * height_texCoordinates) + min_texCoordinates.y;
texCoordinates = vec2(texCoordinates_x, texCoordinates_y);

vec2 charPos_0To1;
charPos_0To1.x = (unitSquareVerts.x * widthOfChar_normalized) + charOffset_topLeft.x;
charPos_0To1.y = (unitSquareVerts.y * heightOfChar_normalized) + charOffset_topLeft.y;

  gl_Position = vec4(charPos_0To1, 0.0, 1.0) * transformationMatrix;

};

)HereDoc";

const char* textFragmentShaderCode =
R"HereDoc(

#version 430

 in vec2 texCoordinates;
uniform sampler2D textureSlotToSampleFrom;

out vec4 fragColor;

void main()
{

 fragColor = texture(textureSlotToSampleFrom, texCoordinates);

};

)HereDoc";

void GLAPIENTRY MyOpenGLErrorCallbackFunc(GLenum source, GLenum debugErrorType, GLuint errorID, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
    switch(debugErrorType)
    {
        case GL_DEBUG_TYPE_ERROR:
        {
            BGZ_CONSOLE("GL Type error: %s\nGL error id: 0x%x\n", message, errorID);
#if _MSC_VER
            __debugbreak();
#endif
        }break;
        
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        {
            BGZ_CONSOLE("GL deprecated gl function usage error: %s\nGL error id: 0x%x\n", message, errorID);
#if _MSC_VER
            __debugbreak();
#endif
        }break;
        
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        {
            BGZ_CONSOLE("GL undefined behavior error: %s\nGL error id: 0x%x\n", message, errorID);
            
#if _MSC_VER
            __debugbreak();
#endif
        }break;
        
        case GL_DEBUG_TYPE_PERFORMANCE:
        {
            BGZ_CONSOLE("GL performance warning/error: %s\nGL error id: 0x%x\n", message, errorID);
            
        }break;
        
        case GL_DEBUG_TYPE_PORTABILITY:
        {
            BGZ_CONSOLE("GL portability warning/error: %s\nGL error id: 0x%x\n", message, errorID);
            
        }break;
        
        case GL_DEBUG_TYPE_OTHER:
        {
            if(errorID == 0x20071)//Ignores the warning: Buffer object 1 (bound to WHATEVER_BUFFER, usage hint is GL_STATIC_DRAW) will use VIDEO memory.... Apparently this doesn't mean much
            {
                //Ignore
            }
            else
            {
                BGZ_CONSOLE("GL other error: %s\nGL error id: 0x%x\n", message, errorID);
            };
        }break;
    };
};

local_func void
DrawLine(v2 minPoint, v2 maxPoint, v3 color, f32 lineThickness)
{
    glLineWidth(lineThickness);
    glBegin(GL_LINES);
    glColor3f(color.r, color.g, color.b);
    glVertex2f(minPoint.x, minPoint.y);
    glColor3f(color.r, color.g, color.b);
    glVertex2f(maxPoint.x, maxPoint.y);
    glEnd();
    glFlush();
};

local_func void
GLInit(int windowWidth, int windowHeight)
{
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);//This makes things so debug callback function doesn't get called from a thread other than which one I need to view the call stack in debugger
    glDebugMessageCallback(MyOpenGLErrorCallbackFunc, 0);
    
    glViewport(0, 0, windowWidth, windowHeight);
    
    //If this is set to GL_MODULATE instead then you might get unwanted texture coloring.
    //In order to avoid that in GL_MODULATE mode you need to constantly set glcolor to white after drawing.
    //For more info: https://stackoverflow.com/questions/53180760/all-texture-colors-affected-by-colored-rectangle-opengl
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glFrontFace(GL_CW);//What ordering of indices indicates front facing triangles. In most cases you want front facing triangles to be visible so this setting will indicate visible triangles
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);//Defaults to CCW ordering if glFrontFace is not set
    glCullFace(GL_BACK);//Culls only back faces (faces facing away from viewer)
}

Mat4x4 ProduceScreenOverlayMatrix(f32 width, f32 height, f32 n, f32 f)
{
    Mat4x4 result =
    {
        {
            {(2.0f/width), 0,          0,         0},
            {0,            (2/height), 0,         0},
            {0,            0,          1/(f - n), 0},
            {0,            0,          n/(n-f),   1}
        },
    };
    
    return result;
};

//TODO: enable texture drawing
void Draw(bgz::Memory_Partition* memPart, u32 id, s32 textureID, RunTimeArr<s16> meshIndicies)
{
    glBindVertexArray(id);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawElements(GL_TRIANGLES, (s32)meshIndicies.Size(), GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
};

//TODO: Need to separate buffering of data and drawing. Running out of video memory if program runs too long
void DrawQuad(Array<v3, 4> quadVerts_objSpace, Array<v2, 2> uvs)
{
    GLfloat verts[4 * 5] = {};
    s32 i{};
    //Bottom Left verts and tex coords
    verts[i++] = quadVerts_objSpace[0].x;
    verts[i++] = quadVerts_objSpace[0].y;
    verts[i++] = quadVerts_objSpace[0].z;
    verts[i++] = uvs[0].elem[0];
    verts[i++] = uvs[0].elem[1];
    
    //Bottom Right
    verts[i++] = quadVerts_objSpace[1].x;
    verts[i++] = quadVerts_objSpace[1].y;
    verts[i++] = quadVerts_objSpace[1].z;
    verts[i++] = uvs[1].elem[0];
    verts[i++] = uvs[0].elem[1];
    
    //Top Right
    verts[i++] = quadVerts_objSpace[2].x;
    verts[i++] = quadVerts_objSpace[2].y;
    verts[i++] = quadVerts_objSpace[2].z;
    verts[i++] = uvs[1].elem[0];
    verts[i++] = uvs[1].elem[1];
    
    //Top Left
    verts[i++] = quadVerts_objSpace[3].x;
    verts[i++] = quadVerts_objSpace[3].y;
    verts[i++] = quadVerts_objSpace[3].z;
    verts[i++] = uvs[0].elem[0];
    verts[i++] = uvs[1].elem[1];
    
    GLuint bufferID;
    glGenBuffers(1, &bufferID);
    glBindBuffer(GL_ARRAY_BUFFER, bufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (char*)(sizeof(GLfloat)*3));
    
    GLushort indicies[] =
    {
        0, 1, 2,
        0, 2, 3
    };
    
    GLuint indexBufferID;
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
};

global_variable int basicShader{};
global_variable int textShader{};

void CheckCompileStatus(GLuint shaderID)
{
    GLint compileStatus;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);
    
    if(compileStatus != GL_TRUE)
    {
        GLint infoLogLength;
        glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        GLchar buffer[512] = {};
        GLsizei bufferSize;
        glGetShaderInfoLog(shaderID, infoLogLength, &bufferSize, buffer);
        
        BGZ_CONSOLE("%s", buffer);
        InvalidCodePath;
    };
};

void CheckLinkStatus(GLuint programID)
{
    GLint linkStatus;
    glGetProgramiv(programID, GL_LINK_STATUS, &linkStatus);
    
    if(linkStatus != GL_TRUE)
    {
        GLint infoLogLength;
        glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        GLchar buffer[512] = {};
        GLsizei bufferSize;
        glGetProgramInfoLog(programID, infoLogLength, &bufferSize, buffer);
        
        BGZ_CONSOLE("%s", buffer);
        InvalidCodePath;
    };
};

int CreateShader(const char* vertexShaderCode, const char* fragmentShaderCode)
{
    int result_shaderProgramID{};
    
    GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    
    const char* adapter[1];
    adapter[0] = vertexShaderCode;
    glShaderSource(vertexShaderID, 1, adapter, 0);
    adapter[0] = fragmentShaderCode;
    glShaderSource(fragmentShaderID, 1, adapter, 0);
    
    glCompileShader(vertexShaderID);
    glCompileShader(fragmentShaderID);
    CheckCompileStatus(vertexShaderID);
    CheckCompileStatus(fragmentShaderID);
    
    result_shaderProgramID = glCreateProgram();
    glAttachShader(result_shaderProgramID, vertexShaderID);
    glAttachShader(result_shaderProgramID, fragmentShaderID);
    glLinkProgram(result_shaderProgramID);
    
    CheckLinkStatus(result_shaderProgramID);
    
    return result_shaderProgramID;
};

void SetUniformValue_Bool(int shaderProgramID, const char* nameOfUniformInShader, bool boolVal)
{
    GLint uniformLocation = glGetUniformLocation(shaderProgramID, nameOfUniformInShader);
    BGZ_ASSERT(uniformLocation != -1);
    glUniform1i(uniformLocation, boolVal);
};


void SetUniformValue_Mat4(int shaderProgramID, const char* nameOfUniformInShader, Mat4x4 matrixData)
{
    GLint uniformLocation = glGetUniformLocation(shaderProgramID, nameOfUniformInShader);
    BGZ_ASSERT(uniformLocation != -1);
    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, &matrixData.elem[0][0]);
};

void SetUniformValue_4fv(int shaderProgramID, const char* nameOfUniformInShader, v4 vecData)
{
    GLint uniformLocation = glGetUniformLocation(shaderProgramID, nameOfUniformInShader);
    BGZ_ASSERT(uniformLocation != -1);
    glUniform4fv(uniformLocation, 1, &vecData.elem[0]);
};

void SetUniformValue_3fv(int shaderProgramID, const char* nameOfUniformInShader, v3 vecData)
{
    GLint uniformLocation = glGetUniformLocation(shaderProgramID, nameOfUniformInShader);
    BGZ_ASSERT(uniformLocation != -1);
    glUniform3fv(uniformLocation, 1, &vecData.elem[0]);
};

void SetUniformValue_2fv(int shaderProgramID, const char* nameOfUniformInShader, v2 vecData)
{
    GLint uniformLocation = glGetUniformLocation(shaderProgramID, nameOfUniformInShader);
    BGZ_ASSERT(uniformLocation != -1);
    glUniform2fv(uniformLocation, 1, &vecData.elem[0]);
};

void SetUniformValue_1f(int shaderProgramID, const char* nameOfUniformInShader, f32 floatVal)
{
    GLint uniformLocation = glGetUniformLocation(shaderProgramID, nameOfUniformInShader);
    BGZ_ASSERT(uniformLocation != -1);
    glUniform1f(uniformLocation, floatVal);
};

void RenderViaHardware(Rendering_Info&& renderingInfo, RenderCmdBuffer&& bufferToRender, bgz::Memory_Partition* platformMemoryPart, int windowWidth_pixels, int windowHeight_pixels)
{
    local_persist bool glIsInitialized { false };
    if (NOT glIsInitialized)
    {
        GLInit(windowWidth_pixels, windowHeight_pixels);
        glIsInitialized = true;
        basicShader = CreateShader(basicVertexShaderCode, basicFragmentShaderCode);
        textShader = CreateShader(textVertexShaderCode, textFragmentShaderCode);
    };
    
    u8* currentRenderBufferEntry = bufferToRender.baseAddress;
    Camera3D camera3d = renderingInfo.camera3d;
    
    f32 pixelsPerMeter = renderingInfo._pixelsPerMeter;
    v2i screenSize = { windowWidth_pixels, windowHeight_pixels };
    v2 screenSize_meters = CastV2IToV2F(screenSize) / pixelsPerMeter;
    
    Mat4x4 xRotMatrix = XRotation(camera3d.rotation.x);
    Mat4x4 yRotMatrix = YRotation(camera3d.rotation.y);
    Mat4x4 zRotMatrix = ZRotation(camera3d.rotation.z);
    Mat4x4 fullRotMatrix = xRotMatrix * yRotMatrix * zRotMatrix;
    v3 xAxis = GetColumn(fullRotMatrix, 0);
    v3 yAxis = GetColumn(fullRotMatrix, 1);
    v3 zAxis = GetColumn(fullRotMatrix, 2);
    
    Mat4x4 camTransformMatrix = ProduceCameraTransformMatrix(xAxis, yAxis, zAxis, camera3d.worldPos);
    Mat4x4 projectionMatrix = ProduceProjectionTransformMatrix_UsingFOV(renderingInfo.fov, renderingInfo.aspectRatio, renderingInfo.nearPlane, renderingInfo.farPlane);
    
    glEnable(GL_TEXTURE_2D);
    
    for (s32 entryNumber = 0; entryNumber < bufferToRender.entryCount; ++entryNumber)
    {
        RenderEntry_Header* entryHeader = (RenderEntry_Header*)currentRenderBufferEntry;
        switch (entryHeader->type)
        {
            case EntryType_InitVertexData:
            {
                RenderEntry_InitVertexData vertexData = *(RenderEntry_InitVertexData*)currentRenderBufferEntry;
                
                u32 vertexArrayID{};
                glGenVertexArrays(1, &vertexArrayID);
                glBindVertexArray(vertexArrayID);
                BGZ_ASSERT(vertexData.vertObjectID == vertexArrayID);
                
                GLuint bufferID;
                glGenBuffers(1, &bufferID);
                glBindBuffer(GL_ARRAY_BUFFER, bufferID);
                glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertexData.interleavedVertAttribData.Size(), vertexData.interleavedVertAttribData.elements, GL_DYNAMIC_DRAW);
                
                //Always need vertex position data
                BGZ_ASSERT(vertexData.vertexAttributes.pos.exists);
                if(vertexData.vertexAttributes.pos.exists) {
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, vertexData.vertexAttributes.pos.numFloats, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * vertexData.stride, 0);//position
                }
                
                if(vertexData.vertexAttributes.color.exists) {
                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(1, vertexData.vertexAttributes.color.numFloats, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * vertexData.stride, (char*)(sizeof(GLfloat)*3));//color
                };
                
                if(vertexData.vertexAttributes.texCoord.exists) {
                    glEnableVertexAttribArray(2);
                    glVertexAttribPointer(2, vertexData.vertexAttributes.texCoord.numFloats, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * vertexData.stride, (char*)(sizeof(GLfloat)*6));//tex coords
                };
                
                if(vertexData.vertexAttributes.normal.exists) {
                    glEnableVertexAttribArray(3);
                    glVertexAttribPointer(3, vertexData.vertexAttributes.normal.numFloats, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * vertexData.stride, (char*)(sizeof(GLfloat)*6));//normals
                };
                
                GLuint indexBufferID;
                glGenBuffers(1, &indexBufferID);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(s16) * vertexData.indicies.Size(), vertexData.indicies.elements, GL_DYNAMIC_DRAW);
                
                s64 size = vertexData.indicies.Size();
                
                glBindVertexArray(0);
                
                currentRenderBufferEntry += sizeof(RenderEntry_InitVertexData);
            }break;
            
            case EntryType_LoadTexture:
            {
                RenderEntry_LoadTexture loadTexEntry = *(RenderEntry_LoadTexture*)currentRenderBufferEntry;
                
                u32 textureID {};
                glEnable(GL_TEXTURE_2D);
                glGenTextures(1, &textureID);
                BGZ_ASSERT(textureID == loadTexEntry.id);
                
                glActiveTexture(GL_TEXTURE0);//This specifies which texture slot we want to use. I'm not passing this down at the moment so any texture used is assumed to be in slot 0
                glBindTexture(GL_TEXTURE_2D, textureID);
                
                //This call allocates space on gpu and takes the texture data to send down
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, loadTexEntry.texture.width_pxls, loadTexEntry.texture.height_pxls, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, loadTexEntry.texture.data);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                
                //Enable alpha channel for transparency
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBindTexture(GL_TEXTURE_2D, 0);
                
                currentRenderBufferEntry += sizeof(RenderEntry_LoadTexture);
            }break;
            
            case EntryType_DrawText:
            {
                RenderEntry_DrawText textEntry = *(RenderEntry_DrawText*)currentRenderBufferEntry;
                
                Mat4x4 transformToNeg1To1Space =
                {
                    {
                        {2, 0, 0, -1},
                        {0, 2, 0, -1},
                        {0, 0, 1, 0},
                        {0, 0, 0, 1}
                    },
                };
                
                Mat4x4 fullTransformMatrix = transformToNeg1To1Space;
                
                glUseProgram(textShader);
                
                glBindTexture(GL_TEXTURE_2D, renderingInfo.textTextureID);
                glBindVertexArray(renderingInfo.textVertObjID);
                
                SetUniformValue_Mat4(textShader, "transformationMatrix", fullTransformMatrix);
                
                for(int i{}; i < textEntry.characters.length; ++i)
                {
                    CharRenderInfo charToRender = textEntry.characters[i];
                    f32 width_UV = charToRender.maxUV.x - charToRender.minUV.x;
                    f32 height_UV = charToRender.maxUV.y - charToRender.minUV.y;
                    
                    //Perform flip from top left origin/+y down to opengl's expected bottom left origin/+y up coord system
                    charToRender.pos_pixelCoords.y = charToRender.textBaselinePos_y - (charToRender.pos_pixelCoords.y - charToRender.textBaselinePos_y);
                    charToRender.pos_pixelCoords.y = (renderingInfo.heightOfScreen_pixels - charToRender.textBaselinePos_y) - (charToRender.textBaselinePos_y - charToRender.pos_pixelCoords.y);
                    charToRender.minUV.y = charToRender.maxUV.y;
                    height_UV = -height_UV;
                    
                    //Set to 0 to 1 values to match verts stored on GPU currently
                    SetUniformValue_2fv(textShader, "charOffset_topLeft", v2{charToRender.pos_pixelCoords.x / renderingInfo.initialWidthOfScreen_pixels, charToRender.pos_pixelCoords.y / renderingInfo.initialHeightOfScreen_pixels});
                    SetUniformValue_1f(textShader, "widthOfChar_normalized", charToRender.width_pixels / renderingInfo.initialWidthOfScreen_pixels);
                    SetUniformValue_1f(textShader, "heightOfChar_normalized", charToRender.height_pixels / renderingInfo.initialHeightOfScreen_pixels);
                    //Uv stuff
                    SetUniformValue_2fv(textShader, "min_texCoordinates", charToRender.minUV);
                    SetUniformValue_1f(textShader, "width_texCoordinates", width_UV);
                    SetUniformValue_1f(textShader, "height_texCoordinates", height_UV);
                    
                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
                }
                
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);
                
                currentRenderBufferEntry += sizeof(RenderEntry_DrawText);
            }break;
            
            case EntryType_DrawRect:
            {
                RenderEntry_DrawRect rectEntry = *(RenderEntry_DrawRect*)currentRenderBufferEntry;
                
                Mat4x4 worldTransformMatrix = ProduceWorldTransformMatrix(rectEntry.worldTransform.translation, rectEntry.worldTransform.rotation, rectEntry.worldTransform.scale);
                Mat4x4 fullTransformMatrix = projectionMatrix * camTransformMatrix * worldTransformMatrix;
                
                bool userWantsToDrawFromTexture{};
                if(rectEntry.textureID)
                    userWantsToDrawFromTexture = true;
                
                glUseProgram(basicShader);
                
                SetUniformValue_Mat4(basicShader, "transformationMatrix", fullTransformMatrix);
                SetUniformValue_Bool(basicShader, "userWantsToDrawFromTexture", userWantsToDrawFromTexture);
                SetUniformValue_4fv(basicShader, "colorChange", rectEntry.color);
                
                glDepthFunc(GL_LESS);
                
                glBindTexture(GL_TEXTURE_2D, rectEntry.textureID);
                glBindVertexArray(renderingInfo.rectVertObjID);
                
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
                
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);
                
                currentRenderBufferEntry += sizeof(RenderEntry_DrawRect);
            }break;
            
            case EntryType_DrawRectOverlay:
            {
                RenderEntry_DrawRectOverlay rectEntryOverlay = *(RenderEntry_DrawRectOverlay*)currentRenderBufferEntry;
                
                //Perform flip from top left origin/+y down to opengl's expected bottom left origin/+y up coord system
                f32 rectWidth = AbsoluteValFloat(rectEntryOverlay._localMax.x - rectEntryOverlay._localMin.x);
                f32 rectHeight = AbsoluteValFloat(rectEntryOverlay._localMax.y - rectEntryOverlay._localMin.y);
                rectEntryOverlay.transform.translation.y = AbsoluteValFloat(rectEntryOverlay.transform.translation.y - renderingInfo.heightOfScreen_pixels) - rectHeight;
                
                //verts currently stored on GPU for rect overlay are between 0 and 1 so need to convert transform to reflect this
                rectWidth *= rectEntryOverlay.transform.scale.x;
                rectHeight *= rectEntryOverlay.transform.scale.y;
                rectWidth /= renderingInfo.initialWidthOfScreen_pixels;
                rectHeight /= renderingInfo.initialHeightOfScreen_pixels;
                rectEntryOverlay.transform.scale.x = rectWidth;
                rectEntryOverlay.transform.scale.y = rectHeight;
                rectEntryOverlay.transform.translation.x /= renderingInfo.initialWidthOfScreen_pixels;
                rectEntryOverlay.transform.translation.y /= renderingInfo.initialHeightOfScreen_pixels;
                
                Mat4x4 worldTransform = ProduceWorldTransformMatrix(rectEntryOverlay.transform.translation, rectEntryOverlay.transform.rotation, rectEntryOverlay.transform.scale);
                
                //Use this matrix to easily setup my points to -1 to 1 space which opengl expects my verts to be in (aka NDC space). Normally, with 3d objects,
                //this step is done by openGL after you transform verts to projected space and openGL then divides all verts by w coordinate sometime
                //between vertex and fragment shaders
                Mat4x4 transformToNeg1To1Space =
                {
                    {
                        {2, 0, 0, -1},
                        {0, 2, 0, -1},
                        {0, 0, 1, 0},
                        {0, 0, 0, 1}
                    },
                };
                
                Mat4x4 fullTransformMatrix = transformToNeg1To1Space * worldTransform;
                
                bool userWantsToDrawFromTexture{};
                if(rectEntryOverlay.textureID)
                    userWantsToDrawFromTexture = true;
                
                glUseProgram(basicShader);
                
                SetUniformValue_Mat4(basicShader, "transformationMatrix", fullTransformMatrix);
                SetUniformValue_Bool(basicShader, "userWantsToDrawFromTexture", userWantsToDrawFromTexture);
                SetUniformValue_4fv(basicShader, "colorChange", rectEntryOverlay.colorChange);
                
                glDepthFunc(GL_ALWAYS);
                
                glBindTexture(GL_TEXTURE_2D, rectEntryOverlay.textureID);
                glBindVertexArray(renderingInfo.rectVertObjID);
                
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
                
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);
                
                currentRenderBufferEntry += sizeof(RenderEntry_DrawRectOverlay);
            }break;
            
            case EntryType_Line:
            {
                RenderEntry_Line lineEntry = *(RenderEntry_Line*)currentRenderBufferEntry;
                
#if 0
                v2 lineMinPoint_camera = CameraTransform(lineEntry.minPoint, *camera2d);
                v2 lineMaxPoint_camera = CameraTransform(lineEntry.maxPoint, *camera2d);
                
                v2 lineMinPoint_screen = ProjectionTransform_Ortho(lineMinPoint_camera, pixelsPerMeter);
                v2 lineMaxPoint_screen = ProjectionTransform_Ortho(lineMaxPoint_camera, pixelsPerMeter);
                lineEntry.minPoint = lineMinPoint_screen;
                lineEntry.maxPoint = lineMaxPoint_screen;
                
                glDisable(GL_TEXTURE_2D);
                DrawLine(lineEntry.minPoint, lineEntry.maxPoint, lineEntry.color, lineEntry.thickness);
                glEnable(GL_TEXTURE_2D);
#endif
                
                currentRenderBufferEntry += sizeof(RenderEntry_Line);
            }break;
            
            case EntryType_DrawCube:
            {
                RenderEntry_DrawCube cube = *(RenderEntry_DrawCube*)currentRenderBufferEntry;
                
                Mat4x4 worldTransformMatrix = ProduceWorldTransformMatrix(cube.worldTransform.translation, cube.worldTransform.rotation, cube.worldTransform.scale);
                Mat4x4 fullTransformMatrix = projectionMatrix * camTransformMatrix * worldTransformMatrix;
                
                bool userWantsToDrawFromTexture{};
                if(cube.textureID)
                    userWantsToDrawFromTexture = true;
                
                glUseProgram(basicShader);
                
                SetUniformValue_Mat4(basicShader, "transformationMatrix", fullTransformMatrix);
                SetUniformValue_Bool(basicShader, "userWantsToDrawFromTexture", userWantsToDrawFromTexture);
                SetUniformValue_4fv(basicShader, "colorChange", cube.color);
                
                glDepthFunc(GL_LESS);
                
                glBindTexture(GL_TEXTURE_2D, cube.textureID);
                glBindVertexArray(renderingInfo.cubeVertObjID);
                
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
                
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);
                
                currentRenderBufferEntry += sizeof(RenderEntry_DrawCube);
            }break;
            
            case EntryType_DrawMesh:
            {
                bgz::ScopedMemory scope{platformMemoryPart};
                
                RenderEntry_DrawMesh meshEntry = *(RenderEntry_DrawMesh*)currentRenderBufferEntry;
                
                //Setup full transform matrix
                Mat4x4 fullTransformMatrix = projectionMatrix * camTransformMatrix * meshEntry.worldTransform;
                
                bool userWantsToDrawFromTexture{};
                if(meshEntry.textureID)
                    userWantsToDrawFromTexture = true;
                
                glUseProgram(basicShader);
                
                SetUniformValue_Mat4(basicShader, "transformationMatrix", fullTransformMatrix);
                SetUniformValue_Bool(basicShader, "userWantsToDrawFromTexture", userWantsToDrawFromTexture);
                SetUniformValue_3fv(basicShader, "colorChange", v3{1.0f, 0.0f, 0.0f});
                
                glDepthFunc(GL_LESS);
                
                glBindTexture(GL_TEXTURE_2D, meshEntry.textureID);
                glBindVertexArray(meshEntry.meshID);
                
                glDrawElements(GL_TRIANGLES, meshEntry.renderIndexLength, GL_UNSIGNED_SHORT, 0);
                
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
                glUseProgram(0);
                
                currentRenderBufferEntry += sizeof(RenderEntry_DrawMesh);
            }break;
            
            InvalidDefaultCase;
        };
    }
    
    bufferToRender.entryCount = 0;
};

#endif //OPENGL_INCLUDE_H
