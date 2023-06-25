#include <fstream>
#include <sstream>

#include "inifile.hpp"
#include "strutil.hpp"

namespace RenderTaskSolver
{

    IniFileError::IniFileError(size_t LineNo, std::string what) noexcept :
        LineNo(LineNo),
        std::runtime_error(what)
    {
    }

    void IniFile::RemoveComments(std::string& line)
    {
        size_t sc = line.find('#');
        if (sc != std::string::npos) line.erase(line.begin() + sc, line.end());
    }

    IniFile::IniFile(std::string LoadFrom)
    {
        std::ifstream ifs(LoadFrom);
        if (ifs.fail())
        {
            std::stringstream sserr;
            sserr << "Could not open `" << LoadFrom << "` for read.";
            throw IniFileError(0, sserr.str());
        }
        std::string line;
        std::map<std::string, std::string>* lastSect = &sections["<global>"];
        size_t LineNo = 0;
        while (std::getline(ifs, line))
        {
            LineNo++;
            RemoveComments(line);
            trim(line);
            if (line.length() == 0) continue;
            if (line.front() == '[' && line.back() == ']')
            {
                auto sectname = line.substr(1, line.size() - 2);
                lastSect = &sections[sectname];
            }
            else
            {
                std::string key, value;
                size_t ass = line.find('=');
                if (ass == std::string::npos)
                {
                    throw IniFileError(LineNo, std::string("Unknown line ") + line);
                }
                key = line.substr(0, ass);
                value = line.substr(ass + 1, std::string::npos);
                trim(key);
                trim(value);
                auto& sect = *lastSect;
                sect[key] = value;
            }
        }
    }
}
