#ifndef ZF4_GAME_H
#define ZF4_GAME_H

#include <zf4_scenes.h>

typedef struct {
    int windowInitWidth;
    int windowInitHeight;
    const char* windowTitle;
    bool windowResizable;
    bool windowHideCursor;

    int sceneTypeCnt;
    ZF4SceneTypeInfoLoader sceneTypeInfoLoader;
} ZF4UserGameInfo;

void zf4_run_game(const ZF4UserGameInfo* const userInfo);

#endif
