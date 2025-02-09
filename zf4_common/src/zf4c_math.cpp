#include <zf4c_math.h>

#include <limits>

namespace zf4 {
    static s_range_f ProjectPts(const s_array<const s_vec_2d> pts, const s_vec_2d edge) {
        s_range_f range = {std::numeric_limits<float>::max(), std::numeric_limits<float>::lowest()};

        for (int i = 0; i < pts.len; ++i) {
            const float dot = Dot(pts[i], edge);

            if (dot < range.min) {
                range.min = dot;
            }

            if (dot > range.max) {
                range.max = dot;
            }
        }

        return range;
    }

    static bool CheckPolySeparation(const s_poly_view& poly, const s_poly_view& other) {
        for (int i = 0; i < poly.pts.len; ++i) {
            const s_vec_2d a = poly.pts[i];
            const s_vec_2d b = poly.pts[(i + 1) % poly.pts.len];

            const s_vec_2d normal = {(b.y - a.y), -(b.x - a.x)};

            const s_range_f poly_a_range = ProjectPts(poly.pts, normal);
            const s_range_f poly_b_range = ProjectPts(other.pts, normal);

            if (poly_a_range.max < poly_b_range.min || poly_b_range.max < poly_a_range.min) {
                return false;
            }
        }

        return true;
    }

    bool DoPolysIntersect(const s_poly_view& poly_a, const s_poly_view& poly_b) {
        return CheckPolySeparation(poly_a, poly_b) && CheckPolySeparation(poly_b, poly_a);
    }

    s_poly PushQuadPoly(const s_vec_2d pos, const s_vec_2d size, const s_vec_2d origin, s_mem_arena& mem_arena) {
        const s_poly poly = {
            .pts = PushArray<s_vec_2d>(4, mem_arena)
        };

        if (!IsStructZero(poly.pts)) {
            poly.pts[0] = pos;
            poly.pts[1] = pos + zf4::s_vec_2d(size.x, 0.0f);
            poly.pts[2] = pos + zf4::s_vec_2d(size.x, size.y);
            poly.pts[3] = pos + zf4::s_vec_2d(0.0f, size.y);
        }

        return poly;
    }

    s_poly PushRotatedQuadPoly(const s_vec_2d pos, const s_vec_2d size, const s_vec_2d origin, const float rot, s_mem_arena& mem_arena) {
        const s_poly poly = {
            .pts = PushArray<s_vec_2d>(4, mem_arena)
        };

        if (!IsStructZero(poly.pts)) {
            const zf4::s_vec_2d left_offs = LenDir(size.x * origin.x, rot + g_pi);
            const zf4::s_vec_2d up_offs = LenDir(size.y * origin.y, rot + (g_pi / 2.0f));
            const zf4::s_vec_2d right_offs = LenDir(size.x * (1.0f - origin.x), rot);
            const zf4::s_vec_2d down_offs = LenDir(size.y * (1.0f - origin.y), rot - (g_pi / 2.0f));

            poly.pts[0] = pos + left_offs + up_offs;
            poly.pts[1] = pos + right_offs + up_offs;
            poly.pts[2] = pos + right_offs + down_offs;
            poly.pts[3] = pos + left_offs + down_offs;
        }

        return poly;
    }

    s_matrix_4x4 GenIdentityMatrix4x4() {
        s_matrix_4x4 mat = {};
        mat.elems[0][0] = 1.0f;
        mat.elems[1][1] = 1.0f;
        mat.elems[2][2] = 1.0f;
        mat.elems[3][3] = 1.0f;
        return mat;
    }

    s_matrix_4x4 GenOrthoMatrix4x4(const float left, const float right, const float bottom, const float top, const float near, const float far) {
        s_matrix_4x4 mat = {};
        mat.elems[0][0] = 2.0f / (right - left);
        mat.elems[1][1] = 2.0f / (top - bottom);
        mat.elems[2][2] = -2.0f / (far - near);
        mat.elems[3][0] = -(right + left) / (right - left);
        mat.elems[3][1] = -(top + bottom) / (top - bottom);
        mat.elems[3][2] = -(far + near) / (far - near);
        mat.elems[3][3] = 1.0f;
        return mat;
    }
}
