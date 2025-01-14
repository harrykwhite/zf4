#pragma once

#include <zf4c_math.h>

namespace zf4 {
    const char* const gk_assetsFileName = "assets.dat";

    constexpr Vec2DI gk_texSizeLimit = {2048, 2048};
    constexpr int gk_texChannelCnt = 4;
    constexpr int gk_texPxLimit = gk_texSizeLimit.x * gk_texSizeLimit.y;
    constexpr int gk_texPxDataSizeLimit = gk_texChannelCnt * gk_texPxLimit;

    constexpr int gk_fontCharRangeBegin = 32;
    constexpr int gk_fontCharRangeLen = 95;

    constexpr int gk_shaderSrcLenLimit = 4095;

    constexpr int gk_audioSamplesPerChunk = 44100;

    constexpr int gk_soundSampleLimit = 441000;

    using AudioSample = float;

    enum AssetType {
        AssetType_Texture,
        AssetType_Font,
        AssetType_ShaderProg,
        AssetType_Sound,
        AssetType_Music,

        AssetTypeCnt
    };

    struct FontCharsArrangementInfo {
        int horOffsets[gk_fontCharRangeLen];
        int verOffsets[gk_fontCharRangeLen];
        int horAdvances[gk_fontCharRangeLen];

        RectI srcRects[gk_fontCharRangeLen];

        int kernings[gk_fontCharRangeLen * gk_fontCharRangeLen];
    };

    struct FontArrangementInfo {
        int lineHeight;
        FontCharsArrangementInfo chars;
    };

    struct AudioInfo {
        int channelCnt;
        long long sampleCntPerChannel;
        int sampleRate;
    };
}
