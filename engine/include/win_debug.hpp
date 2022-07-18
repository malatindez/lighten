#include "win.hpp"
#include <malloc.h>
#include <functional>
namespace engine
{
// works only if there's no debugger attached to the process
void RedirectOutputDebugString(std::function<void(std::string_view)> const&);
}