#pragma once
#include <string>
#include <vector>

namespace RenderTaskSolver
{
	void ltrim(std::string& s);
	void rtrim(std::string& s);
	void trim(std::string& s);
	std::vector<std::string> split(std::string s, const char delim);
	std::vector<std::string> split(std::string s, std::string delim);

	bool ic_equal(std::string s1, std::string s2);
}
