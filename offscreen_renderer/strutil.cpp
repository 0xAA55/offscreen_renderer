#include "strutil.hpp"
#include <cstring>
#include <sstream>
#include <algorithm>

namespace RenderTaskSolver
{
    void LTrimInPlace(std::string& s)
    {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) { return !std::isspace(ch); }));
    }

    void RTrimInPlace(std::string& s)
    {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base(), s.end());
    }

    void TrimInPlace(std::string& s)
    {
        RTrimInPlace(s);
        LTrimInPlace(s);
    }

    std::string LTrim(const std::string& s)
    {
        std::string r = s;
        LTrimInPlace(r);
        return r;
    }

    std::string RTrim(const std::string& s)
    {
        std::string r = s;
        RTrimInPlace(r);
        return r;
    }

    std::string Trim(const std::string& s)
    {
        std::string r = s;
        TrimInPlace(r);
        return r;
    }

    std::vector<std::string> Split(const std::string& s, const std::string& delim)
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

    std::vector<std::string> Split(const std::string& s, const char delim)
    {
        char buf[2] = { delim , 0 };
        return Split(s, buf);
    }

    std::string ToUpper(const std::string& s)
    {
        std::string r = s;
        std::transform(s.begin(), s.end(), r.begin(), std::toupper);
        return r;
    }

    std::string ToLower(const std::string& s)
    {
        std::string r = s;
        std::transform(s.begin(), s.end(), r.begin(), std::tolower);
        return r;
    }

    bool IsSameWord(const std::string& s1, const std::string& s2, const bool CaseSensitive)
    {
        if (CaseSensitive) return !stricmp(s1.c_str(), s2.c_str());
        else return !strcmp(s1.c_str(), s2.c_str());
    }

    bool HasSubString(const std::string& s, const std::string& sub, const bool CaseSensitive)
    {
        if (CaseSensitive) return s.find(sub) != std::string::npos;
        else return ToLower(s).find(ToLower(sub)) != std::string::npos;
    }

    void ReplaceInPlace(std::string& s, const std::string& search, const std::string& replace)
    {
        size_t pos = 0;
        while ((pos = s.find(search, pos)) != std::string::npos)
        {
            s.replace(pos, search.length(), replace);
            pos += replace.length();
        }
    }

    std::string Replace(const std::string& s, const std::string& search, const std::string& replace)
    {
        std::string r = s;
        ReplaceInPlace(r, search, replace);
        return r;
    }

    NotBoolean::NotBoolean(const std::string& what) noexcept :
        std::invalid_argument(what)
    {
    }

    ParseError::ParseError(const std::string& what) noexcept :
        std::runtime_error(what)
    {
    }

    static bool IsTrue(const std::string& s) noexcept
    {
        return
            IsSameWord(s, "true") ||
            IsSameWord(s, "yes") ||
            IsSameWord(s, "on");
    }

    static bool IsFalse(const std::string& s) noexcept
    {
        return
            IsSameWord(s, "false") ||
            IsSameWord(s, "no") ||
            IsSameWord(s, "off");
    }

    static void ThrowNotBoolean(const std::string& s)
    {
        if (s.size())
            throw NotBoolean((std::stringstream() << "Not a boolean value: `" << s << "`").str());
        else
            throw NotBoolean((std::stringstream() << "Not a boolean value.").str());
    }

    bool IsTrueWord(const std::string& s)
    {
        if (IsTrue(s)) return true;
        if (IsFalse(s)) return false;
        ThrowNotBoolean(s);
        return false;
    }

    bool IsFalseWord(const std::string& s)
    {
        if (IsTrue(s)) return false;
        if (IsFalse(s)) return true;
        ThrowNotBoolean(s);
        return false;
    }

    static void CheckNumFields(const size_t ExpectedNumFields, size_t ExactNumFields)
    {
        if (ExpectedNumFields != ExactNumFields)
            throw ParseError((std::stringstream() << "Expected " << ExpectedNumFields << " fields, got " << ExactNumFields << ".").str());
    }

    std::vector<std::string> ParseFields(const std::string& Fields, const std::string& Delim, const bool AllowBlank, const size_t ExpectedNumFields)
    {
        std::vector<std::string> r;

        for (auto& i : Split(Fields, Delim))
        {
            TrimInPlace(i);
            if (AllowBlank || i.size()) r.push_back(i);
        }

        if (ExpectedNumFields) CheckNumFields(ExpectedNumFields, r.size());

        return r;
    }

    std::vector<std::string> ParseFields(const std::string& Fields, const char Delim, const bool AllowBlank, const size_t ExpectedNumFields)
    {
        char buf[2] = { Delim , 0 };
        return ParseFields(Fields, buf, AllowBlank);
    }

    std::set<std::string> ParseUniqueFields(const std::string& Fields, const std::string& Delim, const bool AllowBlank, const size_t ExpectedNumFields)
    {
        std::set<std::string> r;

        for (auto& i : Split(Fields, Delim))
        {
            TrimInPlace(i);
            if (AllowBlank || i.size()) r.insert(i);
        }

        if (ExpectedNumFields) CheckNumFields(ExpectedNumFields, r.size());

        return r;
    }

    std::set<std::string> ParseUniqueFields(const std::string& Fields, const char Delim, const bool AllowBlank, const size_t ExpectedNumFields)
    {
        char buf[2] = { Delim , 0 };
        return ParseUniqueFields(Fields, buf, AllowBlank);
    }

    template<typename NumberType> requires std::is_arithmetic_v<NumberType> || std::is_same_v<NumberType, bool>
    ParseNumberField<NumberType>::ParseNumberField(const std::string& Fields) :
        Parsed()
    {
        auto& r = Parsed;
        std::string i = Trim(Fields);
        if (std::is_integral_v<NumberType>)
        {
            int base = 10;
            int offset = 0;
            size_t length = i.size();
            if (i.starts_with("0x"))
            {
                base = 16;
                offset = 2;
            }
            else if (i.starts_with("0"))
            {
                base = 8;
                offset = 1;
            }
            length -= offset;
            if (i.ends_with("ull"))
                r = static_cast<NumberType>(std::stoull(i.substr(offset, length - 3), nullptr, base));
            else if (i.ends_with("ul"))
                r = static_cast<NumberType>(std::stoul(i.substr(offset, length - 2), nullptr, base));
            else if (i.ends_with("u"))
                r = static_cast<NumberType>(std::stoul(i.substr(offset, length - 1), nullptr, base));
            else if (i.ends_with("ll"))
                r = static_cast<NumberType>(std::stoll(i.substr(offset, length - 2), nullptr, base));
            else if (i.ends_with("l"))
                r = static_cast<NumberType>(std::stol(i.substr(offset, length - 1), nullptr, base));
            else
                r = static_cast<NumberType>(std::stoll(i.substr(offset, length), nullptr, base));
        }
        else if (std::is_floating_point_v<NumberType>)
        {
            r = static_cast<NumberType>(std::stod(i));
        }
        else if (std::is_same_v<NumberType, bool>)
        {
            r = IsTrueWord(i);
        }
    }

    template ParseIntField;
    template ParseUIntField;
    template ParseInt64Field;
    template ParseUInt64Field;
    template ParseFloatField;
    template ParseDoubleField;
    template ParseBooleanField;

    template<typename NumberType> requires std::is_arithmetic_v<NumberType> || std::is_same_v<NumberType, bool>
    ParseNumberFields<NumberType>::ParseNumberFields(const std::string& Fields, const std::string& Delim, const size_t ExpectedNumFields) :
        Parsed()
    {
        auto& r = Parsed;
        for (auto& i : Split(Fields, Delim))
        {
            r.push_back(ParseNumberField<NumberType>(Trim(i)));
        }

        if (ExpectedNumFields) CheckNumFields(ExpectedNumFields, r.size());
    }

    template ParseIntFields;
    template ParseUIntFields;
    template ParseInt64Fields;
    template ParseUInt64Fields;
    template ParseFloatFields;
    template ParseDoubleFields;
    template ParseBooleanFields;

}
