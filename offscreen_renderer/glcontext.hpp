#pragma once

#include <memory>
#include <unordered_map>
#include <type_traits>

#include "glcore.hpp"

namespace GLObjects
{
    using namespace GL;

    class Context : public Version46
    {
    protected:

    public:
        Context() = delete;
        Context(Func_GetProcAddress GetProcAddress);
    };
}