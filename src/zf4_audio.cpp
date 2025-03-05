#include <zf4_audio.h>

#include <zf4_io.h>

namespace zf4 {
    bool s_audio_system::Init(const s_array<const char* const> snd_file_paths, s_mem_arena& mem_arena) {
        assert(!m_initialized);

        if (ma_engine_init(nullptr, &m_engine) != MA_SUCCESS) {
            LogError("Failed to initialize miniaudio engine!");
            return false;
        }

        if (snd_file_paths.IsInitialized()) {
            m_snds = mem_arena.PushArray<ma_sound>(snd_file_paths.Len());

            if (!m_snds.IsInitialized()) {
                LogError("Failed to reserve memory for miniaudio sounds!");
                return false;
            }

            for (int i = 0; i < snd_file_paths.Len(); ++i) {
                if (ma_sound_init_from_file(&m_engine, snd_file_paths[i], MA_SOUND_FLAG_DECODE, nullptr, nullptr, &m_snds[i]) != MA_SUCCESS) {
                    LogError("Failed to load sound effect with path \"%s\"!", snd_file_paths[i]);
                    return false;
                }
            }
        }

        m_initialized = true;

        return true;
    }

    void s_audio_system::Clean() {
        if (m_snds.IsInitialized()) {
            for (int i = 0; i < m_snds.Len(); ++i) {
                ma_sound_uninit(&m_snds[i]);
            }
        }

        ma_engine_uninit(&m_engine);

        *this = {};
    }

    void s_audio_system::PlaySnd(const int index, const float vol, const float pitch) {
        assert(m_initialized);
        assert(index >= 0 && index < m_snds.Len());
        assert(vol >= 0.0f); // NOTE: Consider disallowing going past 100%?
        assert(pitch > 0.0f);

        ma_sound_set_volume(&m_snds[index], vol);
        ma_sound_set_pitch(&m_snds[index], pitch);
        ma_sound_start(&m_snds[index]);
    }
}
