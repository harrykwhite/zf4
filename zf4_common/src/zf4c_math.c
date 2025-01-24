#include <zf4c_math.h>

#include <assert.h>
#include <zf4c_mem.h>

bool DoesRectIntersectWithOtherRects(const s_rect* const rect, const s_rect* const other_rects, const int other_rect_cnt) {
    assert(rect);
    assert(other_rects);
    assert(other_rect_cnt > 0);

    for (int i = 0; i < other_rect_cnt; ++i) {
        if (DoRectsIntersect(rect, &other_rects[i])) {
            return true;
        }
    }

    return false;
}

void InitIdentityMatrix4x4(s_matrix_4x4* const mat) {
    assert(mat);
    assert(IsClear(mat, sizeof(*mat)));

    mat->elems[0][0] = 1.0f;
    mat->elems[1][1] = 1.0f;
    mat->elems[2][2] = 1.0f;
    mat->elems[3][3] = 1.0f;
}

void InitOrthoMatrix4x4(s_matrix_4x4* const mat, const float left, const float right, const float bottom, const float top, const float near, const float far) {
    assert(mat);
    assert(IsClear(mat, sizeof(*mat)));

    mat->elems[0][0] = 2.0f / (right - left);
    mat->elems[1][1] = 2.0f / (top - bottom);
    mat->elems[2][2] = -2.0f / (far - near);
    mat->elems[3][0] = -(right + left) / (right - left);
    mat->elems[3][1] = -(top + bottom) / (top - bottom);
    mat->elems[3][2] = -(far + near) / (far - near);
    mat->elems[3][3] = 1.0f;
}
