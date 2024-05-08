#include "strutil.hpp"
#include <cstring>
#include <sstream>

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

    std::vector<std::string> split(const std::string& s, const std::string& delim)
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

    std::vector<std::string> split(const std::string& s, const char delim)
    {
        char buf[2] = { delim , 0 };
        return split(s, buf);
    }

    bool ic_equal(const std::string& s1, const std::string& s2)
    {
        return !stricmp(s1.c_str(), s2.c_str());
    }

    NotBoolean::NotBoolean(const std::string& what) noexcept :
        std::invalid_argument(what)
    {
    }

    static bool is_true_ne(const std::string& s) noexcept
    {
        return
            ic_equal(s, "true") ||
            ic_equal(s, "yes") ||
            ic_equal(s, "on");
    }

    static bool is_false_ne(const std::string& s) noexcept
    {
        return
            ic_equal(s, "false") ||
            ic_equal(s, "no") ||
            ic_equal(s, "off");
    }

    const void ThrowNotBoolean(const std::string& s)
    {
        if (s.size())
            throw NotBoolean((std::stringstream() << "Not a boolean value: `" << s << "`").str());
        else
            throw NotBoolean((std::stringstream() << "Not a boolean value.").str());
    }

    bool is_true(const std::string& s)
    {
        if (is_true_ne(s)) return true;
        if (is_false_ne(s)) return false;
        ThrowNotBoolean(s);
    }

    bool is_false(const std::string& s)
    {
        if (is_true_ne(s)) return false;
        if (is_false_ne(s)) return true;
        ThrowNotBoolean(s);
    }
}
