#include "include/library-pch.hpp"
#include "win.hpp"
namespace lighten::debug
{
    // works only if there's no debugger attached to the process
    void RedirectOutputDebugString(std::function<void(std::string_view)> const &);
}