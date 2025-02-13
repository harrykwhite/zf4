#include <zf4c_mem.h>

namespace zf4 {
    const s_static_array<const int, 256> g_first_active_bit_indexes = {
        -1, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
        4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
    };

    const s_static_array<const int, 256> g_first_inactive_bit_indexes = {
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 7,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 6,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 5,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 4,
        0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0, 8
    };

    const s_static_array<const int, 256> g_active_bit_cnts = {
        0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
        4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
    };

    int ActiveBitCnt(const s_array<const a_byte> bytes, const int bit_cnt) {
        assert(bit_cnt > 0);
        assert(BytesToBits(bytes.len) >= bit_cnt);

        int cnt = 0;

        for (int i = 0; i < BitsToBytes(bit_cnt); ++i) {
            cnt += g_active_bit_cnts[bytes[i]];
        }

        return cnt;
    }

    int InactiveBitCnt(const s_array<const a_byte> bytes, const int bit_cnt) {
        assert(bit_cnt > 0);
        assert(BytesToBits(bytes.len) >= bit_cnt);
        return bit_cnt - ActiveBitCnt(bytes, bit_cnt);
    }

    int IndexOfFirstActiveBit(const s_array<const a_byte> bytes, const int bit_cnt) {
        assert(bit_cnt > 0);
        assert(BytesToBits(bytes.len) >= bit_cnt);

        for (int i = 0; i < BitsToBytes(bit_cnt); ++i) {
            if (bytes[i]) {
                return (i * 8) + g_first_active_bit_indexes[bytes[i]];
            }
        }

        return -1;
    }

    int IndexOfFirstInactiveBit(const s_array<const a_byte> bytes, const int bit_cnt) {
        assert(bit_cnt > 0);
        assert(BytesToBits(bytes.len) >= bit_cnt);

        for (int i = 0; i < BitsToBytes(bit_cnt); ++i) {
            if (bytes[i] != 0xFF) {
                return (i * 8) + g_first_inactive_bit_indexes[bytes[i]];
            }
        }

        return -1;
    }

    bool AreAllBitsActive(const s_array<const a_byte> bytes, const int bit_cnt) {
        assert(bit_cnt > 0);
        assert(BytesToBits(bytes.len) >= bit_cnt);

        for (int i = 0; i < BitsToBytes(bit_cnt); ++i) {
            if (bytes[i] != 0xFF) {
                return false;
            }
        }

        return true;
    }

    bool AreAllBitsInactive(const s_array<const a_byte> bytes, const int bit_cnt) {
        assert(bit_cnt > 0);
        assert(BytesToBits(bytes.len) >= bit_cnt);

        for (int i = 0; i < BitsToBytes(bit_cnt); ++i) {
            if (bytes[i]) {
                return false;
            }
        }

        return true;
    }

    bool InitMemArena(s_mem_arena& arena, const int size) {
        assert(IsStructZero(arena));
        assert(size > 0);

        arena.buf = calloc(1, size);

        if (!arena.buf) {
            return false;
        }

        arena.size = size;

        return true;
    }

    void CleanMemArena(s_mem_arena& arena) {
        if (arena.buf) {
            free(arena.buf);
        }

        ZeroOutStruct(arena);
    }

    void EmptyMemArena(s_mem_arena& arena) {
        if (arena.offs > 0) {
            memset(arena.buf, 0, arena.offs);
        }

        arena.offs = 0;
    }

    void RewindMemArena(s_mem_arena& arena, const int offs) {
        assert(offs >= 0 && offs < arena.size);

        const int clear_size = arena.size - offs;
        memset(static_cast<char*>(arena.buf) + offs, 0, clear_size);
    }

    void* Push(const int size, const int alignment, s_mem_arena& mem_arena) {
        const int offs_aligned = AlignForward(mem_arena.offs, alignment);
        const int offs_next = offs_aligned + size;

        if (offs_next > mem_arena.size) {
            assert(false && "Failed to push to memory arena due to insufficient space!");
            return nullptr;
        }

        mem_arena.offs = offs_next;

        return (char*)mem_arena.buf + offs_aligned;
    }
}
