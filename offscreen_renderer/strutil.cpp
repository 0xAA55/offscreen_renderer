#include "strutil.hpp"
#include <cstring>

namespace RenderTaskSolver
{
    void ltrim(std::string& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
            }));
    }

    void rtrim(std::string& s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
            }).base(), s.end());
    }

    void trim(std::string& s)
    {
        rtrim(s);
        ltrim(s);
    }

    std::vector<std::string> split(std::string s, std::string delim)
    {
        std::vector<std::string> ret;
        size_t dl = delim.length();
        size_t l = 0;
        size_t i = s.find(delim);
        for (;;)
        {
            ret.push_back(s.substr(l, i - l));
            l = i + dl;
            if (i == std::string::npos) break;
            i = s.find(delim, l);
        }
        return ret;
    }

    std::vector<std::string> split(std::string s, const char delim)
    {
        char buf[2] = { delim , 0 };
        return split(s, buf);
    }

    bool ic_equal(std::string s1, std::string s2)
    {
        return !stricmp(s1.c_str(), s2.c_str());
    }
}
