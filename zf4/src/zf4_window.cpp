#include <zf4_window.h>

#include <zf4_game.h>

namespace zf4 {
    static e_key_code ConvertGLFWKeyCode(const int key_code) {
        switch (key_code) {
            case GLFW_KEY_SPACE: return ek_key_code_space;

            case GLFW_KEY_0: return ek_key_code_0;
            case GLFW_KEY_1: return ek_key_code_1;
            case GLFW_KEY_2: return ek_key_code_2;
            case GLFW_KEY_3: return ek_key_code_3;
            case GLFW_KEY_4: return ek_key_code_4;
            case GLFW_KEY_5: return ek_key_code_5;
            case GLFW_KEY_6: return ek_key_code_6;
            case GLFW_KEY_7: return ek_key_code_7;
            case GLFW_KEY_8: return ek_key_code_8;
            case GLFW_KEY_9: return ek_key_code_9;

            case GLFW_KEY_A: return ek_key_code_a;
            case GLFW_KEY_B: return ek_key_code_b;
            case GLFW_KEY_C: return ek_key_code_c;
            case GLFW_KEY_D: return ek_key_code_d;
            case GLFW_KEY_E: return ek_key_code_e;
            case GLFW_KEY_F: return ek_key_code_f;
            case GLFW_KEY_G: return ek_key_code_g;
            case GLFW_KEY_H: return ek_key_code_h;
            case GLFW_KEY_I: return ek_key_code_i;
            case GLFW_KEY_J: return ek_key_code_j;
            case GLFW_KEY_K: return ek_key_code_k;
            case GLFW_KEY_L: return ek_key_code_l;
            case GLFW_KEY_M: return ek_key_code_m;
            case GLFW_KEY_N: return ek_key_code_n;
            case GLFW_KEY_O: return ek_key_code_o;
            case GLFW_KEY_P: return ek_key_code_p;
            case GLFW_KEY_Q: return ek_key_code_q;
            case GLFW_KEY_R: return ek_key_code_r;
            case GLFW_KEY_S: return ek_key_code_s;
            case GLFW_KEY_T: return ek_key_code_t;
            case GLFW_KEY_U: return ek_key_code_u;
            case GLFW_KEY_V: return ek_key_code_v;
            case GLFW_KEY_W: return ek_key_code_w;
            case GLFW_KEY_X: return ek_key_code_x;
            case GLFW_KEY_Y: return ek_key_code_y;
            case GLFW_KEY_Z: return ek_key_code_z;

            case GLFW_KEY_ESCAPE: return ek_key_code_escape;
            case GLFW_KEY_ENTER: return ek_key_code_enter;
            case GLFW_KEY_TAB: return ek_key_code_tab;

            case GLFW_KEY_RIGHT: return ek_key_code_right;
            case GLFW_KEY_LEFT: return ek_key_code_left;
            case GLFW_KEY_DOWN: return ek_key_code_down;
            case GLFW_KEY_UP: return ek_key_code_up;

            case GLFW_KEY_F1: return ek_key_code_f1;
            case GLFW_KEY_F2: return ek_key_code_f2;
            case GLFW_KEY_F3: return ek_key_code_f3;
            case GLFW_KEY_F4: return ek_key_code_f4;
            case GLFW_KEY_F5: return ek_key_code_f5;
            case GLFW_KEY_F6: return ek_key_code_f6;
            case GLFW_KEY_F7: return ek_key_code_f7;
            case GLFW_KEY_F8: return ek_key_code_f8;
            case GLFW_KEY_F9: return ek_key_code_f9;
            case GLFW_KEY_F10: return ek_key_code_f10;
            case GLFW_KEY_F11: return ek_key_code_f11;
            case GLFW_KEY_F12: return ek_key_code_f12;

            case GLFW_KEY_LEFT_SHIFT: return ek_key_code_left_shift;
            case GLFW_KEY_LEFT_CONTROL: return ek_key_code_left_control;
            case GLFW_KEY_LEFT_ALT: return ek_key_code_left_alt;
            case GLFW_KEY_RIGHT_SHIFT: return ek_key_code_right_shift;
            case GLFW_KEY_RIGHT_CONTROL: return ek_key_code_right_control;
            case GLFW_KEY_RIGHT_ALT: return ek_key_code_right_alt;

            default: return eks_key_code_null;
        }
    }

    static e_mouse_button_code ConvertGLFWMouseButtonCode(const int button_code) {
        switch (button_code) {
            case GLFW_MOUSE_BUTTON_LEFT: return ek_mouse_button_code_left;
            case GLFW_MOUSE_BUTTON_RIGHT: return ek_mouse_button_code_right;
            case GLFW_MOUSE_BUTTON_MIDDLE: return ek_mouse_button_code_middle;

            default: return ek_mouse_button_code_null;
        }
    }

    static void GLFWKeyCallback(GLFWwindow* const glfwWindow, const int key, const int scancode, const int action, const int mods) {
        const auto window = static_cast<s_window*>(glfwGetWindowUserPointer(glfwWindow));

        const e_key_code key_code = ConvertGLFWKeyCode(key);

        if (key_code) {
            const a_keys_down_bits key_bit = static_cast<a_keys_down_bits>(1) << key_code;

            if (action == GLFW_PRESS) {
                window->input_state.keys_down |= key_bit;
            } else if (action == GLFW_RELEASE) {
                window->input_state.keys_down &= ~key_bit;
            }
        }
    }

    static void GLFWMouseButtonCallback(GLFWwindow* const glfwWindow, const int button, const int action, const int mods) {
        const auto window = static_cast<s_window*>(glfwGetWindowUserPointer(glfwWindow));

        const e_mouse_button_code button_code = ConvertGLFWMouseButtonCode(button);

        if (button_code) {
            const a_mouse_buttons_down_bits button_bit = static_cast<a_mouse_buttons_down_bits>(1) << button_code;

            if (action == GLFW_PRESS) {
                window->input_state.mouse_buttons_down |= button_bit;
            } else if (action == GLFW_RELEASE) {
                window->input_state.mouse_buttons_down &= ~button_bit;
            }
        }
    }

    static void GLFWCursorPosCallback(GLFWwindow* const glfwWindow, const double x, const double y) {
        const auto window = static_cast<s_window*>(glfwGetWindowUserPointer(glfwWindow));
        window->input_state.mouse_pos.x = static_cast<float>(x);
        window->input_state.mouse_pos.y = static_cast<float>(y);
    }

    bool InitWindow(s_window& window, const bool begin_fullscreen, const s_vec_2d_i size_default, const s_vec_2d_i size_min, const char* const title, const e_window_flags flags) {
        assert(IsStructZero(window));
        assert(size_default.x > 0 && size_default.y > 0);
        assert((size_min.x == 0 && size_min.y == 0) || (size_min.x > 0 && size_min.y > 0));
        assert(title);

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, g_gl_version_major);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, g_gl_version_minor);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_RESIZABLE, flags & ek_window_flags_resizable);
        glfwWindowHint(GLFW_VISIBLE, false);

        if (begin_fullscreen) {
            GLFWmonitor* const monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* const vid_mode = glfwGetVideoMode(monitor);
            window.glfw_window = glfwCreateWindow(vid_mode->width, vid_mode->height, title, monitor, nullptr);
        } else {
            window.glfw_window = glfwCreateWindow(size_default.x, size_default.y, title, nullptr, nullptr);
        }

        if (!window.glfw_window) {
            LogError("Failed to create GLFW window!");
            return false;
        }

        window.size_default = size_default;

        window.size_min = size_min;
        glfwSetWindowSizeLimits(window.glfw_window, size_min.x, size_min.y, GLFW_DONT_CARE, GLFW_DONT_CARE);

        glfwMakeContextCurrent(window.glfw_window);

        glfwSwapInterval(1); // Enables VSync.

        glfwSetWindowUserPointer(window.glfw_window, &window);
        glfwSetKeyCallback(window.glfw_window, GLFWKeyCallback);
        glfwSetMouseButtonCallback(window.glfw_window, GLFWMouseButtonCallback);
        glfwSetCursorPosCallback(window.glfw_window, GLFWCursorPosCallback);

        glfwSetInputMode(window.glfw_window, GLFW_CURSOR, (flags & ek_window_flags_hide_cursor) ? GLFW_CURSOR_HIDDEN : GLFW_CURSOR_NORMAL);

        return true;
    }

    void CleanWindow(s_window& window) {
        if (window.glfw_window) {
            glfwDestroyWindow(window.glfw_window);
        }

        ZeroOutStruct(window);
    }

    bool ResizeWindow(const s_window& window, const s_vec_2d_i size, rendering::s_pers_render_data& pers_render_data) {
        if (window.size_min == s_vec_2d_i()) {
            assert(size.x > 0 && size.y > 0);
        } else {
            assert(size.x >= window.size_min.x && size.y >= window.size_min.y);
        }

        glfwSetWindowSize(window.glfw_window, size.x, size.y);
        return ProcWindowResize(window, pers_render_data);
    }

    bool SetFullscreen(s_window& window, const bool fullscreen, rendering::s_pers_render_data& pers_render_data) {
        assert(fullscreen != InFullscreen(window)); // Make sure we are actually changing state.

        GLFWmonitor* const monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* const vid_mode = glfwGetVideoMode(monitor);

        if (fullscreen) {
            glfwSetWindowMonitor(window.glfw_window, monitor, 0, 0, vid_mode->width, vid_mode->height, vid_mode->refreshRate);
        } else {
            // Revert back to default window size, centered position.
            const s_vec_2d_i central_pos = {
                (vid_mode->width - window.size_default.x) / 2.0f,
                (vid_mode->height - window.size_default.y) / 2.0f
            };

            glfwSetWindowMonitor(window.glfw_window, nullptr, central_pos.x, central_pos.y, window.size_default.x, window.size_default.y, GLFW_DONT_CARE);
        }

        return ProcWindowResize(window, pers_render_data);
    }

    bool ProcWindowResize(const s_window& window, rendering::s_pers_render_data& pers_render_data) {
        const s_vec_2d_i size = WindowSize(window);
        glViewport(0, 0, size.x, size.y);
        return rendering::ResizeSurfaces(pers_render_data.surfs, size);
    }
}
