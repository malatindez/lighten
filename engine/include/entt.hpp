#pragma once
#include <cassert>
#include <cstdint>
#ifdef _DEBUG
#include "mal-toolkit/mal-toolkit.hpp"
#define ENTT_ASSERT(x, s)                  \
        if (!std::is_constant_evaluated()) \
        {                                  \
                mal_toolkit::Assert(x, s); \
        }                                  \
        else                               \
                assert(x)
#else
#define ENTT_ASSERT(...)
#define ENTT_DISABLE_ASSERT
#define ENTT_NOEXCEPTION
#endif

#define ENTT_SPARSE_PAGE 4096
#define ENTT_PACKED_PAGE 1024
#define ENTT_ID_TYPE std::uint32_t

#include "entt/entt.hpp"