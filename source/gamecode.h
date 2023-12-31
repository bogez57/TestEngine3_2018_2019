#pragma once

#include "atlas.h"
#include "shared.h"
#include "2d_skeleton.h"
#include "2d_animation.h"
#include "fighter.h"

struct Game_Camera
{
    v2 lookAt{};
    v2 dilatePoint_inScreenCoords{};
    v2 dilatePointOffset_normalized{};
    f32 zoomFactor{};
};

struct Stage_Data
{
    Bitmap backgroundImg;
    v2 size{};
    v2 centerPoint{};
    Fighter player;
    Fighter enemy;
    Fighter enemy2;
    Game_Camera camera;
};

struct Game_State
{
    Rect myRect{};
    Cube myCube{};
    Camera3D camera3d{};
    Bitmap composite;
    Bitmap normalMap;
    Atlas* atlas;
    f32 lightAngle{};
    f32 lightThreshold{};
    Stage_Data stage;
    b isLevelOver{false};
};
