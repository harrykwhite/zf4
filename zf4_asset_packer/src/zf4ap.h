#ifndef ZF4AP_H
#define ZF4AP_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <assert.h>
#include <zf4c.h>

#define SRC_ASSET_FILE_PATH_BUF_SIZE 256

typedef bool (*AssetTypePacker)(FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, const cJSON* const cjAssets);

typedef struct {
    FILE* outputFS;
    char* instrsFileChars;
    cJSON* instrsCJ;
} AssetPacker;

bool run_asset_packer(AssetPacker* const packer, const char* const srcDir, const char* const outputDir);
void clean_asset_packer(AssetPacker* const packer, const bool packingSuccess);

bool complete_asset_file_path(char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, const char* const relPath);

bool pack_textures(FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, const cJSON* const cjTextures);
bool pack_fonts(FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, const cJSON* const cjFonts);
bool pack_sounds(FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, const cJSON* const cjSnds);
bool pack_music(FILE* const outputFS, char* const srcAssetFilePathBuf, const int srcAssetFilePathStartLen, const cJSON* const cjMusic);

#endif
