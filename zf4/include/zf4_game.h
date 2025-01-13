#pragma once

#include <zf4_window.h>
#include <zf4_assets.h>
#include <zf4_rendering.h>
#include <zf4_audio.h>
#include <zf4_sprites.h>
#include <zf4_scenes.h>

namespace zf4 {
    constexpr int gk_glVersionMajor = 4;
    constexpr int gk_glVersionMinor = 3;

    struct GamePtrs {
        const Assets* assets;
        Renderer* renderer;
        SoundSrcManager* soundSrcManager;
        MusicSrcManager* musicSrcManager;
        const zf4::Array<Sprite>& sprites;
        const zf4::Array<ComponentType>& compTypes;
        const zf4::Array<SceneType>& sceneTypes;
    };

    using GameInit = bool (*)(const GamePtrs& gamePtrs);
    using GameCleanup = void (*)();

    struct UserGameInfo {
        GameInit init;
        GameCleanup cleanup;

        int memArenaSize;

        const Vec2DI windowInitSize;
        const char* windowTitle;
        WindowFlags windowFlags;

        int spriteCnt;
        SpriteInitializer spriteInitializer;

        int componentTypeCnt;
        ComponentTypeInitializer componentTypeInitializer;

        int sceneTypeCnt;
        SceneTypeInitializer sceneTypeInitializer;
    };

    using UserGameInfoInitializer = void (*)(UserGameInfo* const userInfo);

    bool run_game(const UserGameInfoInitializer userInfoInitializer);
}
