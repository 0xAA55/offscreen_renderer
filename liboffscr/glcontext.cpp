#include "glcontext.hpp"

namespace GLObjects
{
    Context::Context(Func_GetProcAddress GetProcAddress) :
        Version46(GetProcAddress)
    {
    }
}
