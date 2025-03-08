#include <zf4_game.h>

#include <GLFW/glfw3.h>
#include <zf4_rand.h>
#include <zf4_io.h>

namespace zf4 {
    static constexpr int g_targ_ticks_per_sec = 60;
    static constexpr double g_targ_tick_dur_secs = 1.0 / g_targ_ticks_per_sec;

    struct s_game_cleanup_info {
        s_mem_arena* perm_mem_arena = nullptr;
        s_mem_arena* temp_mem_arena = nullptr;

        bool glfw_initialized = false;
        GLFWwindow* glfw_window = nullptr;

        graphics::s_textures* textures = nullptr;
        graphics::s_pers_render_data* pers_render_data = nullptr;

        s_audio_system* audio_sys = nullptr;

        bool IsValid() const {
            return !(!glfw_initialized && glfw_window);
        }
    };

    static int ToGLFWKeyCode(const e_key_code key_code) {
        switch (key_code) {
            case ek_key_code_space: return GLFW_KEY_SPACE;

            case ek_key_code_0: return GLFW_KEY_0;
            case ek_key_code_1: return GLFW_KEY_1;
            case ek_key_code_2: return GLFW_KEY_2;
            case ek_key_code_3: return GLFW_KEY_3;
            case ek_key_code_4: return GLFW_KEY_4;
            case ek_key_code_5: return GLFW_KEY_5;
            case ek_key_code_6: return GLFW_KEY_6;
            case ek_key_code_7: return GLFW_KEY_7;
            case ek_key_code_8: return GLFW_KEY_8;
            case ek_key_code_9: return GLFW_KEY_9;

            case ek_key_code_a: return GLFW_KEY_A;
            case ek_key_code_b: return GLFW_KEY_B;
            case ek_key_code_c: return GLFW_KEY_C;
            case ek_key_code_d: return GLFW_KEY_D;
            case ek_key_code_e: return GLFW_KEY_E;
            case ek_key_code_f: return GLFW_KEY_F;
            case ek_key_code_g: return GLFW_KEY_G;
            case ek_key_code_h: return GLFW_KEY_H;
            case ek_key_code_i: return GLFW_KEY_I;
            case ek_key_code_j: return GLFW_KEY_J;
            case ek_key_code_k: return GLFW_KEY_K;
            case ek_key_code_l: return GLFW_KEY_L;
            case ek_key_code_m: return GLFW_KEY_M;
            case ek_key_code_n: return GLFW_KEY_N;
            case ek_key_code_o: return GLFW_KEY_O;
            case ek_key_code_p: return GLFW_KEY_P;
            case ek_key_code_q: return GLFW_KEY_Q;
            case ek_key_code_r: return GLFW_KEY_R;
            case ek_key_code_s: return GLFW_KEY_S;
            case ek_key_code_t: return GLFW_KEY_T;
            case ek_key_code_u: return GLFW_KEY_U;
            case ek_key_code_v: return GLFW_KEY_V;
            case ek_key_code_w: return GLFW_KEY_W;
            case ek_key_code_x: return GLFW_KEY_X;
            case ek_key_code_y: return GLFW_KEY_Y;
            case ek_key_code_z: return GLFW_KEY_Z;

            case ek_key_code_escape: return GLFW_KEY_ESCAPE;
            case ek_key_code_enter: return GLFW_KEY_ENTER;
            case ek_key_code_tab: return GLFW_KEY_TAB;

            case ek_key_code_right: return GLFW_KEY_RIGHT;
            case ek_key_code_left: return GLFW_KEY_LEFT;
            case ek_key_code_down: return GLFW_KEY_DOWN;
            case ek_key_code_up: return GLFW_KEY_UP;

            case ek_key_code_f1: return GLFW_KEY_F1;
            case ek_key_code_f2: return GLFW_KEY_F2;
            case ek_key_code_f3: return GLFW_KEY_F3;
            case ek_key_code_f4: return GLFW_KEY_F4;
            case ek_key_code_f5: return GLFW_KEY_F5;
            case ek_key_code_f6: return GLFW_KEY_F6;
            case ek_key_code_f7: return GLFW_KEY_F7;
            case ek_key_code_f8: return GLFW_KEY_F8;
            case ek_key_code_f9: return GLFW_KEY_F9;
            case ek_key_code_f10: return GLFW_KEY_F10;
            case ek_key_code_f11: return GLFW_KEY_F11;
            case ek_key_code_f12: return GLFW_KEY_F12;

            case ek_key_code_left_shift: return GLFW_KEY_LEFT_SHIFT;
            case ek_key_code_left_control: return GLFW_KEY_LEFT_CONTROL;
            case ek_key_code_left_alt: return GLFW_KEY_LEFT_ALT;
            case ek_key_code_right_shift: return GLFW_KEY_RIGHT_SHIFT;
            case ek_key_code_right_control: return GLFW_KEY_RIGHT_CONTROL;
            case ek_key_code_right_alt: return GLFW_KEY_RIGHT_ALT;

            default: return -1;
        }
    }

    static int ToGLFWMouseButtonCode(const e_mouse_button_code button_code) {
        switch (button_code) {
            case ek_mouse_button_code_left: return GLFW_MOUSE_BUTTON_LEFT;
            case ek_mouse_button_code_right: return GLFW_MOUSE_BUTTON_RIGHT;
            case ek_mouse_button_code_middle: return GLFW_MOUSE_BUTTON_MIDDLE;

            default: return -1;
        }
    }

    static inline s_vec_2d_i WindowSize(GLFWwindow* const glfw_window) {
        assert(glfw_window);

        s_vec_2d_i size;
        glfwGetWindowSize(glfw_window, &size.x, &size.y);
        return size;
    }

    static s_window_state LoadWindowState(GLFWwindow* const glfw_window) {
        assert(glfw_window);

        return {
            .size = WindowSize(glfw_window),
            .fullscreen = glfwGetWindowMonitor(glfw_window) != nullptr
        };
    }

    static a_keys_down_bits LoadKeysDown(GLFWwindow* const glfw_window) {
        assert(glfw_window);

        a_keys_down_bits bits = 0;

        for (int i = 0; i < eks_key_code_cnt; ++i) {
            const int glfw_code = ToGLFWKeyCode(static_cast<e_key_code>(i));
            assert(glfw_code != -1);

            if (glfwGetKey(glfw_window, glfw_code) == GLFW_PRESS) {
                bits |= static_cast<a_keys_down_bits>(1) << i;
            }
        }

        return bits;
    }

    static a_mouse_buttons_down_bits LoadMouseButtonsDown(GLFWwindow* const glfw_window) {
        assert(glfw_window);

        a_mouse_buttons_down_bits bits = 0;

        for (int i = 0; i < eks_mouse_button_code_cnt; ++i) {
            const int glfw_code = ToGLFWMouseButtonCode(static_cast<e_mouse_button_code>(i));
            assert(glfw_code != -1);

            if (glfwGetMouseButton(glfw_window, glfw_code) == GLFW_PRESS) {
                bits |= static_cast<a_mouse_buttons_down_bits>(1) << i;
            }
        }

        return bits;
    }

    static s_vec_2d MousePos(GLFWwindow* const glfw_window) {
        assert(glfw_window);

        double mouse_x_dbl, mouse_y_dbl;
        glfwGetCursorPos(glfw_window, &mouse_x_dbl, &mouse_y_dbl);
        return {static_cast<float>(mouse_x_dbl), static_cast<float>(mouse_y_dbl)};
    }

    static s_input_state LoadInputState(GLFWwindow* const glfw_window) {
        assert(glfw_window);

        return {
            .keys_down = LoadKeysDown(glfw_window),
            .mouse_buttons_down = LoadMouseButtonsDown(glfw_window),
            .mouse_pos = MousePos(glfw_window)
        };
    }

    static void CleanGame(const s_game_cleanup_info& cleanup_info) {
        assert(cleanup_info.IsValid());

        if (cleanup_info.audio_sys) {
            cleanup_info.audio_sys->Clean();
        }

        if (cleanup_info.pers_render_data) {
            cleanup_info.pers_render_data->Clean();
        }

        if (cleanup_info.glfw_window) {
            glfwDestroyWindow(cleanup_info.glfw_window);
        }

        if (cleanup_info.glfw_initialized) {
            glfwTerminate();
        }

        if (cleanup_info.perm_mem_arena) {
            cleanup_info.perm_mem_arena->Clean();
        }
    }

    bool RunGame(const s_game_info& game_info) {
        assert(game_info.IsValid());

        s_game_cleanup_info cleanup_info;

        //
        // Initialisation
        //

        // Set up memory arenas.
        s_mem_arena perm_mem_arena; // The data here exists for the lifetime of the program.

        if (!perm_mem_arena.Init(game_info.perm_mem_arena_size)) {
            LogError("Failed to initialize the permanent memory arena!");
            CleanGame(cleanup_info);
            return false;
        }

        cleanup_info.perm_mem_arena = &perm_mem_arena;

        s_mem_arena temp_mem_arena; // The data here is reset at various points and should only be used as scratch space.

        if (!temp_mem_arena.Init(game_info.temp_mem_arena_size)) {
            LogError("Failed to initialize the temporary memory arena!");
            CleanGame(cleanup_info);
            return false;
        }

        cleanup_info.temp_mem_arena = &temp_mem_arena;

        // Set up GLFW and the window.
        if (!glfwInit()) {
            LogError("Failed to initialize GLFW!");
            CleanGame(cleanup_info);
            return false;
        }

        cleanup_info.glfw_initialized = true;

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, g_gl_version_major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, g_gl_version_minor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, game_info.window_flags & ek_window_flags_resizable);
        glfwWindowHint(GLFW_VISIBLE, false); // We want to show the window later after everything is set up.

        GLFWwindow* const glfw_window = glfwCreateWindow(game_info.window_init_size.x, game_info.window_init_size.y, game_info.window_title, nullptr, nullptr);

        if (!glfw_window) {
            LogError("Failed to create a GLFW window!");
            CleanGame(cleanup_info);
            return false;
        }

        cleanup_info.glfw_window = glfw_window;

        glfwMakeContextCurrent(glfw_window);

        glfwSwapInterval(1); // Enables VSync.

        glfwSetInputMode(glfw_window, GLFW_CURSOR, (game_info.window_flags & ek_window_flags_hide_cursor) ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

        // Set up OpenGL.
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
            LogError("Failed to initialize OpenGL function pointers!");
            CleanGame(cleanup_info);
            return false;
        }

        // Set up rendering.
        graphics::s_pers_render_data pers_render_data;

        if (!pers_render_data.Init(game_info.tex_cnt, game_info.tex_index_to_file_path_mapper, game_info.font_cnt, game_info.font_index_to_info_mapper, perm_mem_arena, temp_mem_arena)) {
            return false;
        }

        cleanup_info.pers_render_data = &pers_render_data;

        graphics::s_surfaces surfs; // Optionally initialised by the developer.

        // Set up audio.
        s_audio_system audio_sys;

        if (!audio_sys.Init(game_info.snd_cnt, game_info.snd_index_to_file_path_mapper, perm_mem_arena)) {
            LogError("Failed to initialize audio system!");
            CleanGame(cleanup_info);
            return false;
        }

        cleanup_info.audio_sys = &audio_sys;

        // Set up the random number generator.
        InitRNG();

        // Store the initial window and input states.
        s_window_state window_state_cache = LoadWindowState(glfw_window);
        s_input_state input_state = LoadInputState(glfw_window);

        // Call the user-defined initialisation function.
        {
            const s_game_init_func_data func_data = {
                .perm_mem_arena = perm_mem_arena,
                .temp_mem_arena = temp_mem_arena,
                .window_state_cache = window_state_cache,
                .input_state = input_state,
                .surfs = surfs,
                .audio_sys = audio_sys
            };

            if (!game_info.init_func(func_data)) {
                CleanGame(cleanup_info);
                return false;
            }
        }

        // Now that everything is initialised, show the window.
        glfwShowWindow(glfw_window);

        //
        // Main Loop
        //
        double frame_time = glfwGetTime();
        double frame_dur_accum = g_targ_tick_dur_secs; // Make sure that we begin with a tick.

        auto draw_instrs = PushList<graphics::a_draw_instr>(game_info.draw_instr_limit, perm_mem_arena);

        if (!draw_instrs.IsInitialized()) {
            LogError("Failed to reserve memory for draw instructions!");
            CleanGame(cleanup_info);
            return false;
        }

        Log("Entering the main loop...");

        while (!glfwWindowShouldClose(glfw_window)) {
            window_state_cache = LoadWindowState(glfw_window);

            const double frame_time_last = frame_time;
            frame_time = glfwGetTime();

            const double frame_dur = frame_time - frame_time_last;
            frame_dur_accum += frame_dur;

            if (frame_dur_accum >= g_targ_tick_dur_secs) {
                const double fps = 1.0 / frame_dur;

                const s_input_state input_state_last = input_state;
                input_state = LoadInputState(glfw_window);

                s_window_state window_state_ideal = window_state_cache; // This can be mutated by the developer to signal a window state change request.

                do {
                    // Execute a tick.
                    temp_mem_arena.offs = 0;

                    const s_game_tick_func_data func_data = {
                        .perm_mem_arena = perm_mem_arena,
                        .temp_mem_arena = temp_mem_arena,
                        .window_state_cache = window_state_cache,
                        .window_state_ideal = window_state_ideal,
                        .input_state = input_state,
                        .input_state_last = input_state_last,
                        .textures = pers_render_data.textures,
                        .fonts = pers_render_data.fonts,
                        .surfs = surfs,
                        .audio_sys = audio_sys,
                        .fps = fps
                    };

                    const e_game_tick_func_result tick_result = game_info.tick_func(func_data);

                    if (tick_result != ek_game_tick_func_result_continue) {
                        return tick_result == ek_game_tick_func_result_success_quit;
                    }

                    frame_dur_accum -= g_targ_tick_dur_secs;
                } while (frame_dur_accum >= g_targ_tick_dur_secs);

                // Execute draw.
                temp_mem_arena.offs = 0;

                {
                    const s_game_append_draw_instrs_func_data func_data = {
                        .window_state_cache = window_state_cache,
                        .textures = pers_render_data.textures,
                        .fonts = pers_render_data.fonts,
                        .fps = fps
                    };

                    draw_instrs.len = 0;

                    if (!game_info.append_draw_instrs_func(draw_instrs, func_data)) {
                        LogError("Failed to load draw instructions!");
                        CleanGame(cleanup_info);
                        return false;
                    }

                    if (!graphics::ExecDrawInstrs(draw_instrs.ToArray(), window_state_cache.size, pers_render_data, surfs, temp_mem_arena)) {
                        LogError("Failed to execute draw instructions!");
                        CleanGame(cleanup_info);
                        return false;
                    }
                }

                glfwSwapBuffers(glfw_window);

                // Process window state change requests.
                assert(window_state_ideal.size.x > 0 && window_state_ideal.size.y > 0);

                if (window_state_ideal.fullscreen && !window_state_cache.fullscreen) {
                    // Switch from windowed to fullscreen.
                    GLFWmonitor* const primary_monitor = glfwGetPrimaryMonitor();
                    const GLFWvidmode* video_mode = glfwGetVideoMode(primary_monitor);
                    glfwSetWindowMonitor(glfw_window, primary_monitor, 0, 0, video_mode->width, video_mode->height, video_mode->refreshRate);
                } else if (!window_state_ideal.fullscreen && window_state_cache.fullscreen) {
                    // Switch from fullscreen to windowed.
                    GLFWmonitor* const primary_monitor = glfwGetPrimaryMonitor();
                    const GLFWvidmode* video_mode = glfwGetVideoMode(primary_monitor);

                    const s_vec_2d_i central_pos = {
                        (video_mode->width - window_state_ideal.size.x) / 2,
                        (video_mode->height - window_state_ideal.size.y) / 2
                    };

                    glfwSetWindowMonitor(glfw_window, nullptr, central_pos.x, central_pos.y, window_state_ideal.size.x, window_state_ideal.size.y, GLFW_DONT_CARE);
                } else if (!window_state_cache.fullscreen && window_state_ideal.size != window_state_cache.size) {
                    glfwSetWindowSize(glfw_window, window_state_ideal.size.x, window_state_ideal.size.y);
                }
            }

            glfwPollEvents();

            const s_vec_2d_i window_size_after_poll_events = WindowSize(glfw_window);

            if (window_size_after_poll_events != s_vec_2d_i() && window_size_after_poll_events != window_state_cache.size) {
                glViewport(0, 0, window_size_after_poll_events.x, window_size_after_poll_events.y);

                if (!surfs.Resize(window_size_after_poll_events)) {
                    LogError("Failed to resize surfaces!");
                    CleanGame(cleanup_info);
                    return false;
                }
            }
        }

        CleanGame(cleanup_info);

        return true;
    }
}
