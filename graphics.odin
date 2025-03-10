package zf4

import rt "base:runtime"
import "core:fmt"
import "core:mem"
import gl "vendor:OpenGL"
import stbi "vendor:stb/image"

TEXTURE_CHANNEL_CNT :: 4

TEXTURED_QUAD_SHADER_PROG_VERT_CNT :: 13

BATCH_SLOT_CNT :: 2048
BATCH_SLOT_VERT_CNT :: TEXTURED_QUAD_SHADER_PROG_VERT_CNT * 4
BATCH_SLOT_VERTS_SIZE :: BATCH_SLOT_VERT_CNT * BATCH_SLOT_CNT
BATCH_SLOT_ELEM_CNT :: 6

WHITE :: Vec_4D{1.0, 1.0, 1.0, 1.0}
BLACK :: Vec_4D{0.0, 0.0, 0.0, 1.0}
RED :: Vec_4D{1.0, 0.0, 0.0, 1.0}
GREEN :: Vec_4D{0.0, 1.0, 0.0, 1.0}
BLUE :: Vec_4D{0.0, 0.0, 1.0, 1.0}

Textures :: struct {
	gl_ids: []u32,
	sizes:  []Size_2D,
	cnt:    i32,
}

Texture_Index_To_File_Path :: proc(index: i32) -> cstring

Textured_Quad_Shader_Prog :: struct {
	gl_id:                u32,
	proj_uniform_loc:     i32,
	view_uniform_loc:     i32,
	textures_uniform_loc: i32,
}

Pers_Render_Data :: struct {
	textures:                  Textures,
	textured_quad_shader_prog: Textured_Quad_Shader_Prog,
	batch_gl_ids:              Batch_GL_IDs,
}

Batch_GL_IDs :: struct {
	vert_array_gl_id: u32,
	vert_buf_gl_id:   u32,
	elem_buf_gl_id:   u32,
}

Draw_Phase_State :: struct {
	batch_slots_used_cnt: i32,
	batch_slot_verts:     [BATCH_SLOT_CNT][BATCH_SLOT_VERT_CNT]f32,
	batch_tex_gl_id:      u32,
	view_mat:             Matrix_4x4,
}

CreateShaderFromSrc :: proc(src: cstring, frag: bool) -> u32 {
	shader_type: u32

	if frag {
		shader_type = gl.FRAGMENT_SHADER
	} else {
		shader_type = gl.VERTEX_SHADER
	}

	gl_id := gl.CreateShader(shader_type)

	src := src
	gl.ShaderSource(gl_id, 1, &src, nil)
	gl.CompileShader(gl_id)

	compile_success: i32
	gl.GetShaderiv(gl_id, gl.COMPILE_STATUS, &compile_success)

	if compile_success == 0 {
		gl.DeleteShader(gl_id)
		return 0
	}

	return gl_id
}

CreateShaderProgFromSrcs :: proc(vert_shader_src, frag_shader_src: cstring) -> u32 {
	vert_shader_gl_id := CreateShaderFromSrc(vert_shader_src, false)

	if vert_shader_gl_id == 0 {
		return 0
	}

	frag_shader_gl_id := CreateShaderFromSrc(frag_shader_src, true)

	if frag_shader_gl_id == 0 {
		gl.DeleteShader(vert_shader_gl_id)
		return 0
	}

	prog_gl_id := gl.CreateProgram()
	gl.AttachShader(prog_gl_id, vert_shader_gl_id)
	gl.AttachShader(prog_gl_id, frag_shader_gl_id)
	gl.LinkProgram(prog_gl_id)

	gl.DeleteShader(vert_shader_gl_id)
	gl.DeleteShader(frag_shader_gl_id)

	return prog_gl_id
}

LoadTexturedQuadShaderProg :: proc() -> Textured_Quad_Shader_Prog {
	vert_shader_src: cstring = `#version 430 core
layout (location = 0) in vec2 a_vert;
layout (location = 1) in vec2 a_pos;
layout (location = 2) in vec2 a_size;
layout (location = 3) in float a_rot;
layout (location = 4) in vec2 a_tex_coord;
layout (location = 5) in vec4 a_blend;

out vec2 v_tex_coord;
out vec4 v_blend;

uniform mat4 u_view;
uniform mat4 u_proj;

void main() {
    float rot_cos = cos(a_rot);
    float rot_sin = -sin(a_rot);

    mat4 model = mat4(
        vec4(a_size.x * rot_cos, a_size.x * rot_sin, 0.0, 0.0),
        vec4(a_size.y * -rot_sin, a_size.y * rot_cos, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(a_pos.x, a_pos.y, 0.0, 1.0)
    );

    gl_Position = u_proj * u_view * model * vec4(a_vert, 0.0, 1.0);
    v_tex_coord = a_tex_coord;
    v_blend = a_blend;
}`


	frag_shader_src: cstring = `#version 430 core
in vec2 v_tex_coord;
in vec4 v_blend;

out vec4 o_frag_color;

uniform sampler2D u_tex;

void main() {
    vec4 tex_color = texture(u_tex, v_tex_coord);
    o_frag_color = tex_color * v_blend;
}`


	prog: Textured_Quad_Shader_Prog
	prog.gl_id = CreateShaderProgFromSrcs(vert_shader_src, frag_shader_src)
	assert(prog.gl_id != 0)

	prog.proj_uniform_loc = gl.GetUniformLocation(prog.gl_id, "u_proj")
	prog.view_uniform_loc = gl.GetUniformLocation(prog.gl_id, "u_view")
	prog.textures_uniform_loc = gl.GetUniformLocation(prog.gl_id, "u_textures")

	return prog
}

init_textures :: proc(
	textures: ^Textures,
	allocator: mem.Allocator,
	tex_cnt: i32,
	tex_index_to_file_path: Texture_Index_To_File_Path,
) -> bool {
	assert(textures != nil)
	assert(tex_cnt > 0)
	assert(tex_index_to_file_path != nil)

	textures.cnt = tex_cnt

	textures.gl_ids = make([]u32, tex_cnt, allocator)

	if textures.gl_ids == nil {
		return false
	}

	textures.sizes = make([]Size_2D, tex_cnt, allocator)

	if textures.sizes == nil {
		return false
	}

	gl.GenTextures(tex_cnt, &textures.gl_ids[0])

	for i: i32 = 0; i < tex_cnt; i += 1 {
		fp := tex_index_to_file_path(i)
		assert(fp != nil)

		width, height, channel_cnt: i32
		px_data := stbi.load(fp, &width, &height, &channel_cnt, TEXTURE_CHANNEL_CNT)

		if px_data == nil {
			fmt.printf("Failed to load texture with file path \"%s\"!\n", fp)
			gl.DeleteTextures(tex_cnt, &textures.gl_ids[0])
			return false
		}

		textures.sizes[i] = {u32(width), u32(height)}

		gl.BindTexture(gl.TEXTURE_2D, textures.gl_ids[i])
		gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST)
		gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST)
		gl.TexImage2D(
			gl.TEXTURE_2D,
			0,
			gl.RGBA,
			width,
			height,
			0,
			gl.RGBA,
			gl.UNSIGNED_BYTE,
			px_data,
		)

		stbi.image_free(px_data)
	}

	return true
}

gen_pers_render_data :: proc(
	allocator: mem.Allocator,
	tex_cnt: i32,
	tex_index_to_file_path: Texture_Index_To_File_Path,
) -> (
	Pers_Render_Data,
	bool,
) {
	render_data: Pers_Render_Data

	if !init_textures(&render_data.textures, allocator, tex_cnt, tex_index_to_file_path) {
		return {}, false
	}

	render_data.textured_quad_shader_prog = LoadTexturedQuadShaderProg()

	render_data.batch_gl_ids = gen_batch()

	return render_data, true
}

clean_pers_render_data :: proc(render_data: ^Pers_Render_Data) {
	gl.DeleteVertexArrays(1, &render_data.batch_gl_ids.vert_array_gl_id)
	gl.DeleteBuffers(1, &render_data.batch_gl_ids.vert_buf_gl_id)
	gl.DeleteBuffers(1, &render_data.batch_gl_ids.elem_buf_gl_id)

	gl.DeleteProgram(render_data.textured_quad_shader_prog.gl_id)

	rt.mem_zero(render_data, size_of(render_data^))
}

gen_batch :: proc() -> Batch_GL_IDs {
	gl_ids: Batch_GL_IDs

	// Generate vertex array.
	gl.GenVertexArrays(1, &gl_ids.vert_array_gl_id)
	gl.BindVertexArray(gl_ids.vert_array_gl_id)

	// Generate vertex buffer.
	gl.GenBuffers(1, &gl_ids.vert_buf_gl_id)
	gl.BindBuffer(gl.ARRAY_BUFFER, gl_ids.vert_buf_gl_id)
	gl.BufferData(gl.ARRAY_BUFFER, BATCH_SLOT_VERTS_SIZE * BATCH_SLOT_CNT, nil, gl.DYNAMIC_DRAW)

	// Generate element buffer.
	gl.GenBuffers(1, &gl_ids.elem_buf_gl_id)
	gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, gl_ids.elem_buf_gl_id)

	indices: [BATCH_SLOT_ELEM_CNT * BATCH_SLOT_CNT]u16

	for i := 0; i < BATCH_SLOT_CNT; i += 1 {
		indices[(i * 6) + 0] = u16((i * 4) + 0)
		indices[(i * 6) + 1] = u16((i * 4) + 1)
		indices[(i * 6) + 2] = u16((i * 4) + 2)
		indices[(i * 6) + 3] = u16((i * 4) + 2)
		indices[(i * 6) + 4] = u16((i * 4) + 3)
		indices[(i * 6) + 5] = u16((i * 4) + 0)
	}

	gl.BufferData(gl.ELEMENT_ARRAY_BUFFER, size_of(indices), &indices, gl.STATIC_DRAW)

	// Set vertex attribute pointers.
	verts_stride: i32 = size_of(f32) * TEXTURED_QUAD_SHADER_PROG_VERT_CNT

	gl.VertexAttribPointer(0, 2, gl.FLOAT, gl.FALSE, verts_stride, size_of(f32) * 0)
	gl.EnableVertexAttribArray(0)

	gl.VertexAttribPointer(1, 2, gl.FLOAT, gl.FALSE, verts_stride, size_of(f32) * 2)
	gl.EnableVertexAttribArray(1)

	gl.VertexAttribPointer(2, 2, gl.FLOAT, gl.FALSE, verts_stride, size_of(f32) * 4)
	gl.EnableVertexAttribArray(2)

	gl.VertexAttribPointer(3, 1, gl.FLOAT, gl.FALSE, verts_stride, size_of(f32) * 6)
	gl.EnableVertexAttribArray(3)

	gl.VertexAttribPointer(4, 2, gl.FLOAT, gl.FALSE, verts_stride, size_of(f32) * 7)
	gl.EnableVertexAttribArray(4)

	gl.VertexAttribPointer(5, 4, gl.FLOAT, gl.FALSE, verts_stride, size_of(f32) * 9)
	gl.EnableVertexAttribArray(5)

	return gl_ids
}

begin_draw_phase :: proc(allocator: mem.Allocator) -> ^Draw_Phase_State {
	phase_state := new(Draw_Phase_State, allocator)
	init_iden_matrix_4x4(&phase_state.view_mat)
	return phase_state
}

draw_clear :: proc(col: Vec_4D) {
	gl.ClearColor(col.x, col.y, col.z, col.w)
	gl.Clear(gl.COLOR_BUFFER_BIT)
}

draw :: proc(
	tex_gl_id: u32,
	tex_coords: Rect_Edges,
	pos: Vec_2D,
	size: Vec_2D,
	draw_phase_state: ^Draw_Phase_State,
	pers_render_data: ^Pers_Render_Data,
	origin := Vec_2D{},
	rot: f32 = 0.0,
	blend := WHITE,
) {
	if draw_phase_state.batch_slots_used_cnt == 0 {
		draw_phase_state.batch_tex_gl_id = tex_gl_id
	} else if draw_phase_state.batch_slots_used_cnt == BATCH_SLOT_CNT ||
	   tex_gl_id != draw_phase_state.batch_tex_gl_id {
		flush(draw_phase_state, pers_render_data)
		draw(
			tex_gl_id,
			tex_coords,
			pos,
			size,
			draw_phase_state,
			pers_render_data,
			origin,
			rot,
			blend,
		)
		return
	}

	// Submit the vertex data to the batch.
	slot_index := draw_phase_state.batch_slots_used_cnt
	slot_verts := &draw_phase_state.batch_slot_verts[slot_index]

	slot_verts[0] = 0.0 - origin.x
	slot_verts[1] = 0.0 - origin.y
	slot_verts[2] = pos.x
	slot_verts[3] = pos.y
	slot_verts[4] = size.x
	slot_verts[5] = size.y
	slot_verts[6] = rot
	slot_verts[7] = tex_coords.left
	slot_verts[8] = tex_coords.top
	slot_verts[9] = blend.x
	slot_verts[10] = blend.y
	slot_verts[11] = blend.z
	slot_verts[12] = blend.w

	slot_verts[13] = 1.0 - origin.x
	slot_verts[14] = 0.0 - origin.y
	slot_verts[15] = pos.x
	slot_verts[16] = pos.y
	slot_verts[17] = size.x
	slot_verts[18] = size.y
	slot_verts[19] = rot
	slot_verts[20] = tex_coords.right
	slot_verts[21] = tex_coords.top
	slot_verts[22] = blend.x
	slot_verts[23] = blend.y
	slot_verts[24] = blend.z
	slot_verts[25] = blend.w

	slot_verts[26] = 1.0 - origin.x
	slot_verts[27] = 1.0 - origin.y
	slot_verts[28] = pos.x
	slot_verts[29] = pos.y
	slot_verts[30] = size.x
	slot_verts[31] = size.y
	slot_verts[32] = rot
	slot_verts[33] = tex_coords.right
	slot_verts[34] = tex_coords.bottom
	slot_verts[35] = blend.x
	slot_verts[36] = blend.y
	slot_verts[37] = blend.z
	slot_verts[38] = blend.w

	slot_verts[39] = 0.0 - origin.x
	slot_verts[40] = 1.0 - origin.y
	slot_verts[41] = pos.x
	slot_verts[42] = pos.y
	slot_verts[43] = size.x
	slot_verts[44] = size.y
	slot_verts[45] = rot
	slot_verts[46] = tex_coords.left
	slot_verts[47] = tex_coords.bottom
	slot_verts[48] = blend.x
	slot_verts[49] = blend.y
	slot_verts[50] = blend.z
	slot_verts[51] = blend.w

	draw_phase_state.batch_slots_used_cnt += 1
}

draw_texture :: proc(
	tex_index: i32,
	src_rect: Rect_I,
	pos: Vec_2D,
	draw_phase_state: ^Draw_Phase_State,
	pers_render_data: ^Pers_Render_Data,
	origin := Vec_2D{0.5, 0.5},
	scale := Vec_2D{1.0, 1.0},
	rot: f32 = 0.0,
	blend := WHITE,
) {
	assert(tex_index >= 0 && tex_index < pers_render_data.textures.cnt)

	tex_size := pers_render_data.textures.sizes[tex_index]
	tex_coords := calc_texture_coords(src_rect, tex_size)
	draw(
		pers_render_data.textures.gl_ids[tex_index],
		tex_coords,
		pos,
		{f32(src_rect.width) * scale.x, f32(src_rect.height) * scale.y},
		draw_phase_state,
		pers_render_data,
		origin,
		rot,
		blend,
	)
}

flush :: proc(draw_phase_state: ^Draw_Phase_State, pers_render_data: ^Pers_Render_Data) {
	if draw_phase_state.batch_slots_used_cnt == 0 {
		return
	}

	// Write the batch vertex data to the GPU.
	gl.BindVertexArray(pers_render_data.batch_gl_ids.vert_array_gl_id)
	gl.BindBuffer(gl.ARRAY_BUFFER, pers_render_data.batch_gl_ids.vert_buf_gl_id)

	write_size := BATCH_SLOT_VERTS_SIZE * draw_phase_state.batch_slots_used_cnt
	gl.BufferSubData(gl.ARRAY_BUFFER, 0, int(write_size), &draw_phase_state.batch_slot_verts[0][0])

	// Draw the batch.
	prog := &pers_render_data.textured_quad_shader_prog

	gl.UseProgram(prog.gl_id)

	proj_mat: Matrix_4x4
	init_ortho_matrix_4x4(&proj_mat, 0.0, 1280.0, 720.0, 0.0, -1.0, 1.0)

	gl.UniformMatrix4fv(prog.proj_uniform_loc, 1, false, &proj_mat.elems[0][0])
	gl.UniformMatrix4fv(prog.view_uniform_loc, 1, false, &draw_phase_state.view_mat.elems[0][0])

	gl.BindTexture(gl.TEXTURE_2D, draw_phase_state.batch_tex_gl_id)

	gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, pers_render_data.batch_gl_ids.elem_buf_gl_id)
	gl.DrawElements(
		gl.TRIANGLES,
		BATCH_SLOT_ELEM_CNT * draw_phase_state.batch_slots_used_cnt,
		gl.UNSIGNED_SHORT,
		nil,
	)

	// Reset batch state.
	draw_phase_state.batch_slots_used_cnt = 0
	draw_phase_state.batch_tex_gl_id = 0
}

calc_texture_coords :: proc(src_rect: Rect_I, tex_size: Size_2D) -> Rect_Edges {
	return {
		f32(src_rect.x) / f32(tex_size.x),
		f32(src_rect.y) / f32(tex_size.y),
		f32(calc_rect_i_right(src_rect)) / f32(tex_size.x),
		f32(calc_rect_i_bottom(src_rect)) / f32(tex_size.y),
	}
}

