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
        IniFileError(size_t LineNo, const std::string& what) noexcept;
    };

    class IniFile
    {
    protected:
        static void RemoveComments(std::string& line);

    public:
        using SectionType = std::map<std::string, std::string>;
        using IniFileType = std::map<std::string, SectionType>;

        IniFileType sections;

        IniFile(const std::string& LoadFrom);
    };
}
