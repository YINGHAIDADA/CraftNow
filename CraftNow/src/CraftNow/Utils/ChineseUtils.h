#pragma once

#include <windows.h>
#include <winnls.h>
#include <iostream>

namespace CraftNow {

	class ChineseUtils
	{
	public:
		static std::string utf8_to_string(const std::string& str);
		static std::string string_to_utf8(const std::string& str);

		//void show_chinese();
	};

}
