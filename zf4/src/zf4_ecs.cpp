#include <zf4_ecs.h>

#include <zf4_game.h>
#include <zf4_sprites.h>

namespace zf4 {
    bool EntityManager::init(MemArena* const memArena, const int entLimit, const Array<ComponentType>& compTypes) {
        assert(is_zero(this));
        assert(entLimit >= 0);

        if (entLimit > 0) {
            m_entLimit = entLimit;

            //
            // Entities
            //
            m_entPositions = memArena->push<Vec2D>(entLimit);

            if (!m_entPositions) {
                return false;
            }

            m_entCompIndexes = memArena->push<int*>(entLimit);

            if (!m_entCompIndexes) {
                return false;
            }

            for (int i = 0; i < entLimit; ++i) {
                m_entCompIndexes[i] = memArena->push<int>(compTypes.get_len());

                if (!m_entCompIndexes[i]) {
                    return false;
                }

                // WARNING: These are zero!
            }

            m_entTags = memArena->push<int>(entLimit);

            if (!m_entTags) {
                return false;
            }

            m_entFlags = memArena->push<Byte>(entLimit);

            if (!m_entFlags) {
                return false;
            }

            m_entActivity = memArena->push<Byte>(bits_to_bytes(entLimit));

            if (!m_entActivity) {
                return false;
            }

            m_entVersions = memArena->push<int>(entLimit);

            if (!m_entVersions) {
                return false;
            }

            //
            // Components
            //
            m_compArrays = memArena->push<Byte*>(compTypes.get_len());

            if (!m_compArrays) {
                return false;
            }

            m_compActivities = memArena->push<Byte*>(compTypes.get_len());

            if (!m_compActivities) {
                return false;
            }

            for (int i = 0; i < compTypes.get_len(); ++i) {
                m_compArrays[i] = memArena->push<Byte>(compTypes.get(i)->size * entLimit);

                if (!m_compArrays[i]) {
                    return false;
                }

                m_compActivities[i] = memArena->push<Byte>(bits_to_bytes(entLimit));

                if (!m_compActivities[i]) {
                    return false;
                }
            }
        }

        return true;
    }

    bool EntityManager::spawn_ent(EntID* const entID, const Vec2D pos, const Array<ComponentType>& compTypes) {
        assert(is_zero(entID));

        entID->index = get_first_inactive_bit_index(m_entActivity, m_entLimit);

        if (entID->index == -1) {
            return false;
        }

        activate_bit(m_entActivity, entID->index);

        m_entPositions[entID->index] = pos;
        m_entTags[entID->index] = -1;

        for (int i = 0; i < compTypes.get_len(); ++i) {
            m_entCompIndexes[entID->index][i] = -1;
        }

        ++m_entVersions[entID->index];
        entID->version = m_entVersions[entID->index];

        return true;
    }

    void EntityManager::destroy_ent(const EntID entID, const Array<ComponentType>& compTypes) {
        assert(does_ent_exist(entID));

        deactivate_bit(m_entActivity, entID.index);

        // Deactivate the entity's components.
        for (int i = 0; i < compTypes.get_len(); ++i) {
            const int compIndex = m_entCompIndexes[entID.index][i];

            if (compIndex != -1) {
                deactivate_bit(m_compActivities[i], compIndex);
            }
        }
    }

    Byte* EntityManager::get_ent_component(const EntID entID, const int compTypeIndex, const Array<ComponentType>& compTypes) {
        assert(does_ent_exist(entID));
        assert(compTypeIndex >= 0 && compTypeIndex < compTypes.get_len());
        assert(does_ent_have_component(entID, compTypeIndex, compTypes));

        const int compIndex = m_entCompIndexes[entID.index][compTypeIndex];
        return m_compArrays[compTypeIndex] + (compIndex * compTypes.get(compTypeIndex)->size);
    }

    bool EntityManager::add_component_to_ent(const int compTypeIndex, const EntID entID, const Array<ComponentType>& compTypes) {
        assert(does_ent_exist(entID));
        assert(compTypeIndex >= 0 && compTypeIndex < compTypes.get_len());
        assert(!does_ent_have_component(entID, compTypeIndex, compTypes));

        // Find and use the first inactive component of the type.
        const int compIndex = get_first_inactive_bit_index(m_compActivities[compTypeIndex], m_entLimit);

        if (compIndex == -1) {
            return false;
        }

        activate_bit(m_compActivities[compTypeIndex], compIndex);

        m_entCompIndexes[entID.index][compTypeIndex] = compIndex;

        // Clear the component data, and run the defaults loader function if defined.
        Byte* const comp = get_ent_component(entID, compTypeIndex, compTypes);

        zero_out(comp, compTypes.get(compTypeIndex)->size);

        if (compTypes.get(compTypeIndex)->defaultsSetter) {
            compTypes.get(compTypeIndex)->defaultsSetter(comp);
        }

        return true;
    }
}
