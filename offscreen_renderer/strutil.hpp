#pragma once
#include <string>
#include <vector>
#include <stdexcept>

namespace RenderTaskSolver
{
	void ltrim(std::string& s);
	void rtrim(std::string& s);
	void trim(std::string& s);
	std::vector<std::string> split(const std::string& s, const char delim);
	std::vector<std::string> split(const std::string& s, const std::string& delim);

	bool ic_equal(const std::string& s1, const std::string& s2);

	class NotBoolean : public std::invalid_argument
	{
	public:
		NotBoolean(const std::string& what) noexcept;
	};

	bool is_true(const std::string& s);
	bool is_false(const std::string& s);
}
