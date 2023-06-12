#include "runtime/function/framework/object/object_id_allocator.h"

#include "core/base/macro.h"

namespace LunarYue
{
    std::atomic<ObjectID> ObjectIDAllocator::m_next_id {0};

    ObjectID ObjectIDAllocator::alloc()
    {
        std::atomic<ObjectID> new_object_ret = m_next_id.load();
        ++m_next_id;
        if (m_next_id >= k_invalid_object_id)
        {
            LOG_FATAL("gobject id overflow");
        }

        return new_object_ret;
    }

} // namespace LunarYue
