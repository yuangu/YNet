#pragma once 
#include <string.h>
#include <string>
#include <algorithm>
#include <cctype>
#include <functional>

namespace YStrHelper
{
	template <typename T>
	bool isEqual(T str1, T str2)
	{
		std::string str_1 = str1;
		std::string str_2 = str2;
		return str1 == str2;
	}


	template <typename T1, typename T2>
	bool isEqual(T1 str1, T2 str2)
	{
		std::string str_1 = str1;
		std::string str_2 = str2;
		return str1 == str2;
	}

	// trim from start 
	inline std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// trim from end 
	inline std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	 inline std::string &trim(std::string &s) {
		return ltrim(rtrim(s));
	}


};