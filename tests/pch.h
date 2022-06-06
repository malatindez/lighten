#if _MSC_VER && !__INTEL_COMPILER
#pragma warning(push, 0)
#include "gtest/gtest.h"
#pragma warning(pop)
#elif __GNUC__
#include "gtest/gtest.h"
#endif