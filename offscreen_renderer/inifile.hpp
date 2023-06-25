#pragma once
#include <map>
#include <string>
#include <sstream>
#include <stdexcept>

namespace RenderTaskSolver
{
    class IniFileError : public std::runtime_error
    {
    protected:
        size_t LineNo;
    public:
        IniFileError(size_t LineNo, std::string what) noexcept;
    };

    class IniFile
    {
    protected:
        static void RemoveComments(std::string& line);

    public:
        std::map<std::string, std::map<std::string, std::string>> sections;

        IniFile(std::string LoadFrom);
    };
}
