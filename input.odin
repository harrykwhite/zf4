package zf4

import "vendor:glfw"

Key_Code :: enum {
	Space,
    Num_0, Num_1, Num_2, Num_3, Num_4, Num_5, Num_6, Num_7, Num_8, Num_9,
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	Escape, Enter, Tab,
    Right, Left, Down, Up,
	F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	Left_Shift, Left_Control, Left_Alt,
	Right_Shift, Right_Control, Right_Alt
}

Mouse_Button_Code :: enum {
	Left,
	Right,
	Middle
}

Keys_Down_Bits :: u64
Mouse_Buttons_Down_Bits :: u8

Input_State :: struct {
	keys_down:          Keys_Down_Bits,
	mouse_buttons_down: Mouse_Buttons_Down_Bits,
	mouse_pos:          Vec_2D,
}

load_input_state :: proc(glfw_window: glfw.WindowHandle) -> Input_State {
	assert(glfw_window != nil)

	mouse_x_f64, mouse_y_f64: f64 = glfw.GetCursorPos(glfw_window)

	return Input_State {
		keys_down = load_keys_down(glfw_window),
		mouse_buttons_down = load_mouse_buttons_down(glfw_window),
		mouse_pos = {f32(mouse_x_f64), f32(mouse_y_f64)}
	}
}

is_key_down :: proc(key_code: Key_Code, input_state: Input_State) -> bool {
	key_bit := Keys_Down_Bits(1) << u32(key_code)
	return (input_state.keys_down & key_bit) != 0
}

to_glfw_key_code :: proc(key_code: Key_Code) -> i32 {
	switch key_code {
	    case Key_Code.Space: return glfw.KEY_SPACE

	    case Key_Code.Num_0: return glfw.KEY_0
	    case Key_Code.Num_1: return glfw.KEY_1
	    case Key_Code.Num_2: return glfw.KEY_2
	    case Key_Code.Num_3: return glfw.KEY_3
	    case Key_Code.Num_4: return glfw.KEY_4
	    case Key_Code.Num_5: return glfw.KEY_5
	    case Key_Code.Num_6: return glfw.KEY_6
	    case Key_Code.Num_7: return glfw.KEY_7
	    case Key_Code.Num_8: return glfw.KEY_8
	    case Key_Code.Num_9: return glfw.KEY_9

	    case Key_Code.A: return glfw.KEY_A
	    case Key_Code.B: return glfw.KEY_B
	    case Key_Code.C: return glfw.KEY_C
	    case Key_Code.D: return glfw.KEY_D
	    case Key_Code.E: return glfw.KEY_E
	    case Key_Code.F: return glfw.KEY_F
	    case Key_Code.G: return glfw.KEY_G
	    case Key_Code.H: return glfw.KEY_H
	    case Key_Code.I: return glfw.KEY_I
	    case Key_Code.J: return glfw.KEY_J
	    case Key_Code.K: return glfw.KEY_K
	    case Key_Code.L: return glfw.KEY_L
	    case Key_Code.M: return glfw.KEY_M
	    case Key_Code.N: return glfw.KEY_N
	    case Key_Code.O: return glfw.KEY_O
	    case Key_Code.P: return glfw.KEY_P
	    case Key_Code.Q: return glfw.KEY_Q
	    case Key_Code.R: return glfw.KEY_R
	    case Key_Code.S: return glfw.KEY_S
	    case Key_Code.T: return glfw.KEY_T
	    case Key_Code.U: return glfw.KEY_U
	    case Key_Code.V: return glfw.KEY_V
	    case Key_Code.W: return glfw.KEY_W
	    case Key_Code.X: return glfw.KEY_X
	    case Key_Code.Y: return glfw.KEY_Y
	    case Key_Code.Z: return glfw.KEY_Z

	    case Key_Code.Escape: return glfw.KEY_ESCAPE
	    case Key_Code.Enter: return glfw.KEY_ENTER
	    case Key_Code.Tab: return glfw.KEY_TAB

	    case Key_Code.Right: return glfw.KEY_RIGHT
	    case Key_Code.Left: return glfw.KEY_LEFT
	    case Key_Code.Down: return glfw.KEY_DOWN
	    case Key_Code.Up: return glfw.KEY_UP

	    case Key_Code.F1: return glfw.KEY_F1
	    case Key_Code.F2: return glfw.KEY_F2
	    case Key_Code.F3: return glfw.KEY_F3
	    case Key_Code.F4: return glfw.KEY_F4
	    case Key_Code.F5: return glfw.KEY_F5
	    case Key_Code.F6: return glfw.KEY_F6
	    case Key_Code.F7: return glfw.KEY_F7
	    case Key_Code.F8: return glfw.KEY_F8
	    case Key_Code.F9: return glfw.KEY_F9
	    case Key_Code.F10: return glfw.KEY_F10
	    case Key_Code.F11: return glfw.KEY_F11
	    case Key_Code.F12: return glfw.KEY_F12

	    case Key_Code.Left_Shift: return glfw.KEY_LEFT_SHIFT
	    case Key_Code.Left_Control: return glfw.KEY_LEFT_CONTROL
	    case Key_Code.Left_Alt: return glfw.KEY_LEFT_ALT

	    case Key_Code.Right_Shift: return glfw.KEY_RIGHT_SHIFT
	    case Key_Code.Right_Control: return glfw.KEY_RIGHT_CONTROL
	    case Key_Code.Right_Alt: return glfw.KEY_RIGHT_ALT

	    case: return -1
	}
}

to_glfw_mouse_button_code :: proc(button_code: Mouse_Button_Code) -> i32 {
	switch button_code {
	    case Mouse_Button_Code.Left: return glfw.MOUSE_BUTTON_LEFT
	    case Mouse_Button_Code.Right: return glfw.MOUSE_BUTTON_RIGHT
	    case Mouse_Button_Code.Middle: return glfw.MOUSE_BUTTON_MIDDLE

	    case: return -1
	}
}

load_keys_down :: proc(glfw_window: glfw.WindowHandle) -> Keys_Down_Bits {
	assert(glfw_window != nil)

	bits: Keys_Down_Bits = 0

	for i: u32 = 0; i < len(Key_Code); i += 1 {
		glfw_code := to_glfw_key_code(cast(Key_Code)i)
		assert(glfw_code != -1)

		if glfw.GetKey(glfw_window, glfw_code) == glfw.PRESS {
			bits |= 1 << i
		}
	}

	return bits
}

load_mouse_buttons_down :: proc(glfw_window: glfw.WindowHandle) -> Mouse_Buttons_Down_Bits {
	assert(glfw_window != nil)

	bits: Mouse_Buttons_Down_Bits = 0

	for i: u32 = 0; i < len(Mouse_Button_Code); i += 1 {
		glfw_code := to_glfw_mouse_button_code(cast(Mouse_Button_Code)i)
		assert(glfw_code != -1)

		if glfw.GetMouseButton(glfw_window, glfw_code) == glfw.PRESS {
			bits |= 1 << i
		}
	}

	return bits
}
