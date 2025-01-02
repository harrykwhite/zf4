#pragma once

#include <zf4c_mem.h>

namespace zf4 {
    template<SimpleType T>
    class ActivityArray {
    public:
        bool init(MemArena* const memArena, const int len);
        void activate(const int index);
        int activate_first_inactive();
        void deactivate(const int index);

        int get_len() const {
            return m_len;
        }

        bool is_active(const int index) const {
            assert(index >= 0 && index < m_len);
            return is_bit_active(m_activity, index);
        }

        T& operator[](const int index) {
            assert(m_elems);
            assert(index >= 0 && index < m_len);
            assert(is_active(index));
            return m_elems[index];
        }

        const T& operator[](const int index) const {
            assert(m_elems);
            assert(index >= 0 && index < m_len);
            assert(is_active(index));
            return m_elems[index];
        }

    private:
        T* m_elems;
        int m_len;
        Byte* m_activity;
    };

    template<SimpleType T>
    inline bool ActivityArray<T>::init(MemArena* const memArena, const int len) {
        assert(is_zero(this));

        m_elems = memArena->push<T>(len);

        if (!m_elems) {
            return false;
        }

        m_len = len;

        return true;
    }

    template<SimpleType T>
    inline void ActivityArray<T>::activate(const int index) {
        assert(m_elems);
        assert(index >= 0 && index < m_len);
        assert(!is_active(index));

        zero_out(&m_elems[index]);
        activate_bit(m_activity, index);
    }

    template<SimpleType T>
    inline int ActivityArray<T>::activate_first_inactive() {
        const int index = get_first_inactive_bit_index(m_activity, m_len);

        if (index != -1) {
            activate(index);
        }

        return index;
    }

    template<SimpleType T>
    inline void ActivityArray<T>::deactivate(const int index) {
        assert(m_elems);
        assert(index >= 0 && index < m_len);
        assert(is_active(index));

        deactivate_bit(m_activity, index);
    }
}