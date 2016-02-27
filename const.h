#pragma once

const uint32 SCREEN_W = 320;
const uint32 SCREEN_H = 240;
const int SCREEN_MARGIN = 32;
const int SCALE = 2;

enum class SceneType
{
    None,
    Title,
    Main,
    Clear,
    GameOver,
    Quit
};