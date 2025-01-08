#include <zf4_assets.h>

#include <cstdlib>
#include <AL/alext.h>

namespace zf4 {
    static void set_up_gl_tex(const GLuint glID, const Vec2DI size, const unsigned char* const pxData) {
        glBindTexture(GL_TEXTURE_2D, glID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pxData);
    }

    static GLuint create_shader_from_src(const char* const src, const bool frag) {
        const GLuint glID = glCreateShader(frag ? GL_FRAGMENT_SHADER : GL_VERTEX_SHADER);
        glShaderSource(glID, 1, &src, nullptr);
        glCompileShader(glID);

        GLint compileSuccess;
        glGetShaderiv(glID, GL_COMPILE_STATUS, &compileSuccess);

        if (!compileSuccess) {
            glDeleteShader(glID);
            return 0;
        }

        return glID;
    }

    static GLuint create_shader_prog_from_srcs(const char* const vertShaderSrc, const char* const fragShaderSrc) {
        const GLuint vertShaderGLID = create_shader_from_src(vertShaderSrc, false);

        if (!vertShaderGLID) {
            return 0;
        }

        const GLuint fragShaderGLID = create_shader_from_src(fragShaderSrc, true);

        if (!fragShaderGLID) {
            glDeleteShader(vertShaderGLID);
            return 0;
        }

        const GLuint progGLID = glCreateProgram();
        glAttachShader(progGLID, vertShaderGLID);
        glAttachShader(progGLID, fragShaderGLID);
        glLinkProgram(progGLID);

        // We no longer need the shaders, as they are now part of the program.
        glDeleteShader(vertShaderGLID);
        glDeleteShader(fragShaderGLID);

        return progGLID;
    }

    bool AssetManager::load(MemArena* const memArena) {
        assert(!s_loaded);

        FILE* const fs = fopen(gk_assetsFileName, "rb");

        if (!fs) {
            log_error("Failed to open \"%s\"!", gk_assetsFileName);
            return false;
        }

        if (!load_textures(&s_textures, memArena, fs)) {
            log_error("Failed to load textures!");
            fclose(fs);
            return false;
        }

        if (!load_fonts(&s_fonts, memArena, fs)) {
            log_error("Failed to load fonts!");
            fclose(fs);
            return false;
        }

        if (!load_shader_progs(&s_shaderProgs, memArena, fs)) {
            log_error("Failed to load shader programs!");
            fclose(fs);
            return false;
        }

        if (!load_sounds(&s_sounds, memArena, fs)) {
            log_error("Failed to load sounds!");
            fclose(fs);
            return false;
        }

        if (!load_music(&s_music, memArena, fs)) {
            log_error("Failed to load music!");
            fclose(fs);
            return false;
        }

        fclose(fs);

        s_loaded = true;

        return true;
    }

    void AssetManager::unload() {
        if (s_sounds.cnt > 0) {
            alDeleteBuffers(s_sounds.cnt, s_sounds.bufALIDs);
        }

        zero_out(&s_sounds);

        for (int i = 0; i < s_shaderProgs.cnt; ++i) {
            glDeleteProgram(s_shaderProgs.glIDs[i]);
        }

        zero_out(&s_shaderProgs);

        if (s_fonts.cnt > 0) {
            glDeleteTextures(s_fonts.cnt, s_fonts.texGLIDs);
        }

        zero_out(&s_fonts);

        if (s_textures.cnt > 0) {
            glDeleteTextures(s_textures.cnt, s_textures.glIDs);
        }

        zero_out(&s_textures);

        s_loaded = false;
    }

    bool load_textures(Textures* const textures, MemArena* const memArena, FILE* const fs) {
        read_from_fs<int>(&textures->cnt, fs);

        if (textures->cnt > 0) {
            // Reserve space in the arena for texture data.
            textures->glIDs = memArena->push<GLuint>(textures->cnt);

            if (!textures->glIDs) {
                return false;
            }

            textures->sizes = memArena->push<Vec2DI>(textures->cnt);

            if (!textures->sizes) {
                return false;
            }

            textures->pxDatas = memArena->push<unsigned char*>(textures->cnt);

            if (!textures->pxDatas) {
                return false;
            }

            // Load textures.
            if (textures->cnt > 0) {
                glGenTextures(textures->cnt, textures->glIDs);

                for (int i = 0; i < textures->cnt; ++i) {
                    read_from_fs<Vec2DI>(&textures->sizes[i], fs);

                    const int pxDataSize = gk_texChannelCnt * textures->sizes[i].x * textures->sizes[i].y;
                    textures->pxDatas[i] = memArena->push<unsigned char>(pxDataSize);

                    if (!textures->pxDatas[i]) {
                        return false;
                    }

                    read_from_fs<unsigned char>(textures->pxDatas[i], fs, pxDataSize);

                    set_up_gl_tex(textures->glIDs[i], textures->sizes[i], textures->pxDatas[i]);
                }
            }
        }

        return true;
    }

    bool load_fonts(Fonts* const fonts, MemArena* const memArena, FILE* const fs) {
        read_from_fs<int>(&fonts->cnt, fs);

        if (fonts->cnt > 0) {
            // Reserve space in the arena for font data.
            fonts->arrangementInfos = memArena->push<FontArrangementInfo>(fonts->cnt);

            if (!fonts->arrangementInfos) {
                return false;
            }

            fonts->texGLIDs = memArena->push<GLuint>(fonts->cnt);

            if (!fonts->texGLIDs) {
                return false;
            }

            fonts->texSizes = memArena->push<Vec2DI>(fonts->cnt);

            if (!fonts->texSizes) {
                return false;
            }

            // Allocate memory for pixel data, to be reused for all font textures.
            const auto pxData = alloc<unsigned char>(gk_texPxDataSizeLimit);

            if (!pxData) {
                return false;
            }

            // Load fonts.
            glGenTextures(fonts->cnt, fonts->texGLIDs);

            for (int i = 0; i < fonts->cnt; ++i) {
                read_from_fs<FontArrangementInfo>(&fonts->arrangementInfos[i], fs);
                read_from_fs<Vec2DI>(&fonts->texSizes[i], fs);
                read_from_fs<unsigned char>(pxData, fs, gk_texPxDataSizeLimit);
                set_up_gl_tex(fonts->texGLIDs[i], fonts->texSizes[i], pxData);
            }

            free(pxData);
        }

        return true;
    }

    bool load_shader_progs(ShaderProgs* const progs, MemArena* const memArena, FILE* const fs) {
        read_from_fs<int>(&progs->cnt, fs);

        if (progs->cnt > 0) {
            // Reserve space in the arena for shader program GL IDs.
            progs->glIDs = memArena->push<GLuint>(progs->cnt);

            if (!progs->glIDs) {
                return false;
            }

            // Allocate buffers to store shader source code.
            const auto vertShaderSrcBuf = alloc<char>(gk_shaderSrcLenLimit + 1);

            if (!vertShaderSrcBuf) {
                return false;
            }

            const auto fragShaderSrcBuf = alloc<char>(gk_shaderSrcLenLimit + 1);

            if (!fragShaderSrcBuf) {
                free(vertShaderSrcBuf);
                return false;
            }

            for (int i = 0; i < progs->cnt; ++i) {
                // Get vertex shader source.
                int vertShaderSrcLen;
                read_from_fs(&vertShaderSrcLen, fs);

                read_from_fs(vertShaderSrcBuf, fs, vertShaderSrcLen);
                vertShaderSrcBuf[vertShaderSrcLen] = '\0';

                // Get fragment shader source.
                int fragShaderSrcLen;
                read_from_fs(&fragShaderSrcLen, fs);

                read_from_fs(fragShaderSrcBuf, fs, fragShaderSrcLen);
                fragShaderSrcBuf[fragShaderSrcLen] = '\0';

                // Create the program.
                progs->glIDs[i] = create_shader_prog_from_srcs(vertShaderSrcBuf, fragShaderSrcBuf);

                if (!progs->glIDs[i]) {
                    free(fragShaderSrcBuf);
                    free(vertShaderSrcBuf);
                    return false;
                }
            }

            free(fragShaderSrcBuf);
            free(vertShaderSrcBuf);
        }

        return true;
    }

    bool load_sounds(Sounds* const snds, MemArena* const memArena, FILE* const fs) {
        read_from_fs<int>(&snds->cnt, fs);

        if (snds->cnt > 0) {
            snds->bufALIDs = memArena->push<ALuint>(snds->cnt);

            if (!snds->bufALIDs) {
                return false;
            }

            const auto samples = alloc<float>(gk_soundSampleLimit);

            if (!samples) {
                return false;
            }

            alGenBuffers(snds->cnt, snds->bufALIDs);

            for (int i = 0; i < snds->cnt; ++i) {
                AudioInfo audioInfo;
                read_from_fs<AudioInfo>(&audioInfo, fs);

                const long long sampleCnt = audioInfo.sampleCntPerChannel * audioInfo.channelCnt;
                read_from_fs<float>(samples, fs, sampleCnt);

                const ALenum format = audioInfo.channelCnt == 1 ? AL_FORMAT_MONO_FLOAT32 : AL_FORMAT_STEREO_FLOAT32;
                alBufferData(snds->bufALIDs[i], format, samples, sizeof(*samples) * sampleCnt, audioInfo.sampleRate);
            }

            free(samples);
        }

        return true;
    }

    bool load_music(Music* const music, MemArena* const memArena, FILE* const fs) {
        read_from_fs<int>(&music->cnt, fs);

        if (music->cnt > 0) {
            music->infos = memArena->push<AudioInfo>(music->cnt);

            if (!music->infos) {
                return false;
            }

            music->sampleDataFilePositions = memArena->push<int>(music->cnt);

            if (!music->sampleDataFilePositions) {
                return false;
            }

            for (int i = 0; i < music->cnt; ++i) {
                read_from_fs<AudioInfo>(&music->infos[i], fs);
                music->sampleDataFilePositions[i] = ftell(fs);
            }
        }

        return true;
    }

    static TexturedQuadShaderProg load_textured_quad_shader_prog() {
        const char* const vertShaderSrc =
            "#version 430 core\n"
            "layout (location = 0) in vec2 a_vert;\n"
            "layout (location = 1) in vec2 a_pos;\n"
            "layout (location = 2) in vec2 a_size;\n"
            "layout (location = 3) in float a_rot;\n"
            "layout (location = 4) in float a_texIndex;\n"
            "layout (location = 5) in vec2 a_texCoord;\n"
            "layout (location = 6) in float a_alpha;\n"
            "\n"
            "out flat int v_texIndex;\n"
            "out vec2 v_texCoord;\n"
            "out float v_alpha;\n"
            "\n"
            "uniform mat4 u_view;\n"
            "uniform mat4 u_proj;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    float rotCos = cos(a_rot);\n"
            "    float rotSin = -sin(a_rot);\n"
            "\n"
            "    mat4 model = mat4(\n"
            "        vec4(a_size.x * rotCos, a_size.x * rotSin, 0.0f, 0.0f),\n"
            "        vec4(a_size.y * -rotSin, a_size.y * rotCos, 0.0f, 0.0f),\n"
            "        vec4(0.0f, 0.0f, 1.0f, 0.0f),\n"
            "        vec4(a_pos.x, a_pos.y, 0.0f, 1.0f)\n"
            "    );\n"
            "\n"
            "    gl_Position = u_proj * u_view * model * vec4(a_vert, 0.0f, 1.0f);\n"
            "\n"
            "    v_texIndex = int(a_texIndex);\n"
            "    v_texCoord = a_texCoord;\n"
            "    v_alpha = a_alpha;\n"
            "}\n";

        const char* const fragShaderSrc =
            "#version 430 core\n"
            "\n"
            "in flat int v_texIndex;\n"
            "in vec2 v_texCoord;\n"
            "in float v_alpha;\n"
            "\n"
            "out vec4 o_fragColor;\n"
            "\n"
            "uniform sampler2D u_textures[32];\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 texColor = texture(u_textures[v_texIndex], v_texCoord);\n"
            "    o_fragColor = texColor * vec4(1.0f, 1.0f, 1.0f, v_alpha);\n"
            "}\n";

        TexturedQuadShaderProg prog = {
            .glID = create_shader_prog_from_srcs(vertShaderSrc, fragShaderSrc)
        };

        assert(prog.glID);

        prog.projUniLoc = glGetUniformLocation(prog.glID, "u_proj");
        prog.viewUniLoc = glGetUniformLocation(prog.glID, "u_view");
        prog.texturesUniLoc = glGetUniformLocation(prog.glID, "u_textures");

        return prog;
    }

    static TestShaderProg load_test_shader_prog() {
        const char* const vertShaderSrc =
            "#version 430 core\n"
            "layout (location = 0) in vec2 a_vert;\n"
            "layout (location = 1) in vec2 a_texCoord;\n"
            "\n"
            "out vec2 v_texCoord;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_Position = vec4(a_vert, 0.0f, 1.0f);\n"
            "    v_texCoord = a_texCoord;\n"
            "}\n";

        const char* const fragShaderSrc =
            "#version 430 core\n"
            "\n"
            "in vec2 v_texCoord;\n"
            "out vec4 o_fragColor;\n"
            "\n"
            "uniform sampler2D u_tex;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    vec4 texColor = texture(u_tex, v_texCoord);\n"
            "    o_fragColor = vec4(texColor.r, texColor.g * 0.5f, texColor.b * 0.5f, texColor.a);\n"
            "}\n";

        const TestShaderProg prog = {
            .glID = create_shader_prog_from_srcs(vertShaderSrc, fragShaderSrc)
        };

        assert(prog.glID);

        return prog;
    }

    InternalShaderProgs load_internal_shader_progs() {
        return {
            .texturedQuad = load_textured_quad_shader_prog(),
            .test = load_test_shader_prog()
        };
    }

    void unload_internal_shader_progs(InternalShaderProgs* const progs) {
        glDeleteBuffers(1, &progs->texturedQuad.glID);
        glDeleteBuffers(1, &progs->test.glID);

        zero_out(progs);
    }
}
