#pragma once
#include <string>
#include <vector>
#include <stdexcept>
#include <set>
#include <tuple>
#include <type_traits>

namespace RenderTaskSolver
{
	void LTrimInPlace(std::string& s);
	void RTrimInPlace(std::string& s);
	void TrimInPlace(std::string& s);
	std::string LTrim(const std::string& s);
	std::string RTrim(const std::string& s);
	std::string Trim(const std::string& s);
	std::vector<std::string> Split(const std::string& s, const char delim);
	std::vector<std::string> Split(const std::string& s, const std::string& delim);

	std::string ToUpper(const std::string& s);
	std::string ToLower(const std::string& s);

	bool IsSameWord(const std::string& s1, const std::string& s2, const bool CaseSensitive = true);
	bool HasSubString(const std::string& s, const std::string& sub, const bool CaseSensitive = true);
	void ReplaceInPlace(std::string& s, const std::string& search, const std::string& replace);
	std::string Replace(const std::string& s, const std::string& search, const std::string& replace);

	class NotBoolean : public std::invalid_argument
	{
	public:
		NotBoolean(const std::string& what) noexcept;
	};

	bool IsTrueWord(const std::string& s);
	bool IsFalseWord(const std::string& s);

	class ParseError : public std::runtime_error
	{
	public:
		ParseError(const std::string& what) noexcept;
	};

    std::vector<std::string> ParseFields(const std::string& Fields, const std::string& Delim = ",", const bool AllowBlank = false, const size_t ExpectedNumFields = 0);
    std::vector<std::string> ParseFields(const std::string& Fields, const char Delim = ',', const bool AllowBlank = false, const size_t ExpectedNumFields = 0);

	std::set<std::string> ParseUniqueFields(const std::string& Fields, const std::string& Delim = ",", const bool AllowBlank = false, const size_t ExpectedNumFields = 0);
	std::set<std::string> ParseUniqueFields(const std::string& Fields, const char Delim = ',', const bool AllowBlank = false, const size_t ExpectedNumFields = 0);

	template<typename NumberType> requires std::is_arithmetic_v<NumberType> || std::is_same_v<NumberType, bool>
	class ParseNumberField
	{
	protected:
		NumberType Parsed;

	public:
		ParseNumberField(const std::string& Fields);
		inline operator NumberType() const { return Parsed; }
	};

	using ParseIntField = ParseNumberField<int32_t>;
	using ParseUIntField = ParseNumberField<uint32_t>;
	using ParseInt64Field = ParseNumberField<int64_t>;
	using ParseUInt64Field = ParseNumberField<uint64_t>;
	using ParseFloatField = ParseNumberField<float>;
	using ParseDoubleField = ParseNumberField<double>;
	using ParseBooleanField = ParseNumberField<bool>;

	extern template class ParseNumberField<int32_t>;
	extern template class ParseNumberField<uint32_t>;
	extern template class ParseNumberField<int64_t>;
	extern template class ParseNumberField<uint64_t>;
	extern template class ParseNumberField<float>;
	extern template class ParseNumberField<double>;
	extern template class ParseNumberField<bool>;

	template<typename NumberType> requires std::is_arithmetic_v<NumberType> || std::is_same_v<NumberType, bool>
	class ParseNumberFields
	{
	protected:
		std::vector<NumberType> Parsed;

	public:
		ParseNumberFields(const std::string& Fields, const std::string& Delim = ",", const size_t ExpectedNumFields = 0);
		inline std::vector<NumberType> ToVector() const { return Parsed; }
		inline operator std::vector<NumberType>() const { return Parsed; }
		inline NumberType operator [](size_t i) const { return Parsed[i]; }
		inline size_t size() const { return Parsed.size(); }
	};

	using ParseIntFields = ParseNumberFields<int32_t>;
	using ParseUIntFields = ParseNumberFields<uint32_t>;
	using ParseInt64Fields = ParseNumberFields<int64_t>;
	using ParseUInt64Fields = ParseNumberFields<uint64_t>;
	using ParseFloatFields = ParseNumberFields<float>;
	using ParseDoubleFields = ParseNumberFields<double>;
	using ParseBooleanFields = ParseNumberFields<bool>;

	extern template class ParseNumberFields<int32_t>;
	extern template class ParseNumberFields<uint32_t>;
	extern template class ParseNumberFields<int64_t>;
	extern template class ParseNumberFields<uint64_t>;
	extern template class ParseNumberFields<float>;
	extern template class ParseNumberFields<double>;
	extern template class ParseNumberFields<bool>;
}
