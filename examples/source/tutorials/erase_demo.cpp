#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <iostream>
void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
{
(void)name;
	(void)flags;
	(void)debugFlags;
	(void)file;
	(void)line;
	return new uint8_t[size];
}
int main() {

	sol::state lua;
	lua["bark"] = 50;
	sol::optional<int> x = lua["bark"];
	// x will have a value
	if (x) {
		std::cout << "x has no value, as expected"
		          << std::endl;
	}
	else {
		return -1;
	}

	lua["bark"] = sol::lua_nil;
	sol::optional<int> y = lua["bark"];
	// y will not have a value
	if (y) {
		return -1;
	}
	else {
		std::cout << "y has no value, as expected"
		          << std::endl;
	}

	return 0;
}
