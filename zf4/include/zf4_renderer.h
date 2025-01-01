#pragma once

#include <zf4c.h>
#include <zf4_assets.h>
#include <zf4_shader_progs.h>
#include <zf4_window.h>

namespace zf4 {
    constexpr int gk_spriteBatchSlotLimit = 4096;
    constexpr int gk_charBatchSlotLimit = 1024;
    constexpr int gk_texUnitLimit = 16;

    typedef enum {
        FONT_HOR_ALIGN_LEFT,
        FONT_HOR_ALIGN_CENTER,
        FONT_HOR_ALIGN_RIGHT
    } FontHorAlign;

    typedef enum {
        FONT_VER_ALIGN_TOP,
        FONT_VER_ALIGN_CENTER,
        FONT_VER_ALIGN_BOTTOM
    } FontVerAlign;

    typedef struct {
        GLuint vertArrayGLID;
        GLuint vertBufGLID;
        GLuint elemBufGLID;
    } QuadBuf;

    typedef struct {
        int slotsUsed;
        int texUnitTexIDs[gk_texUnitLimit];
        int texUnitsInUse;
    } SpriteBatchTransients;

    typedef struct {
        int texIndex;
        Vec2D pos;
        Rect srcRect;
        Vec2D origin;
        float rot;
        Vec2D scale;
        float alpha;
    } SpriteBatchWriteInfo;

    typedef struct {
        int fontIndex;
        Vec2D pos;
        float rot;
        Vec4D blend;
    } CharBatchDisplayProps;

    typedef struct {
        QuadBuf quadBuf;
        int slotCnt;
        CharBatchDisplayProps displayProps;
    } CharBatch;

    typedef struct {
        int layerIndex;
        int batchIndex;
    } CharBatchID;

    typedef struct {
        int spriteBatchCnt;
        int charBatchCnt;
    } RenderLayerProps;

    typedef void (*RenderLayerPropsInitializer)(RenderLayerProps* const props, const int layerIndex);

    typedef struct {
        QuadBuf* spriteBatchQuadBufs;
        SpriteBatchTransients* spriteBatchTransients;
        int spriteBatchesFilled;

        CharBatch* charBatches;
        Byte* charBatchActivityBitset;

        RenderLayerProps props;
    } RenderLayer;

    typedef struct {
        Vec2D pos;
        float scale;
    } Camera;

    typedef struct {
        RenderLayer* layers;
        int layerCnt;
        int camLayerCnt;
        Vec3D bgColor;
        Camera cam;
    } Renderer;

    bool load_renderer(Renderer* const renderer, MemArena* const memArena, const int layerCnt, const int camLayerCnt, const RenderLayerPropsInitializer layerPropsInitializer);
    void clean_renderer(Renderer* const renderer);
    void render_all(const Renderer* const renderer, const ShaderProgs* const shaderProgs);

    void empty_sprite_batches(Renderer* const renderer);
    void write_to_sprite_batch(Renderer* const renderer, const int layerIndex, const SpriteBatchWriteInfo* const info);

    CharBatchID activate_any_char_batch(Renderer* const renderer, const int layerIndex, const int slotCnt, const int fontIndex);
    void deactivate_char_batch(Renderer* const renderer, const CharBatchID id);
    void write_to_char_batch(Renderer* const renderer, const CharBatchID id, const char* const text, const FontHorAlign horAlign, const FontVerAlign verAlign);
    void clear_char_batch(const Renderer* const renderer, const CharBatchID id);

    inline CharBatchDisplayProps* const get_char_batch_display_props(const Renderer* const renderer, const CharBatchID id) {
        return &renderer->layers[id.layerIndex].charBatches[id.batchIndex].displayProps;
    }

    inline Vec2D get_camera_size(const Camera* const cam) {
        const Vec2D size = {get_window_size().x / cam->scale, get_window_size().y / cam->scale};
        return size;
    }

    inline Vec2D get_camera_top_left(const Camera* const cam) {
        const Vec2D size = get_camera_size(cam);
        const Vec2D topLeft = {cam->pos.x - (size.x / 2.0f), cam->pos.y - (size.y / 2.0f)};
        return topLeft;
    }

    inline Vec2D get_camera_bottom_right(const Camera* const cam) {
        const Vec2D size = get_camera_size(cam);
        const Vec2D bottomRight = {cam->pos.x + (size.x / 2.0f), cam->pos.y + (size.y / 2.0f)};
        return bottomRight;
    }

    inline Vec2D camera_to_screen_pos(const Vec2D pos, const Camera* const cam) {
        const Vec2D topLeft = get_camera_top_left(cam);
        return {(pos.x - topLeft.x) * cam->scale, (pos.y - topLeft.y) * cam->scale};
    }

    inline Vec2D screen_to_camera_pos(const Vec2D pos, const Camera* const cam) {
        const Vec2D topLeft = get_camera_top_left(cam);
        return {(pos.x / cam->scale) + topLeft.x, (pos.y / cam->scale) + topLeft.y};
    }
}
