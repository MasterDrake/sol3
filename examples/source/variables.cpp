#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <iostream>

int main() {
	std::cout << "=== variables ===" << std::endl;

	sol::state lua;

	// need the base library for assertions
	lua.open_libraries(sol::lib::base);

	// basic setting of a variable
	// through multiple ways
	lua["x"] = 10;
	lua.set("y", "hello");

	// assert values are as given
	lua.script("assert(x == 10)");
	lua.script("assert(y == 'hello')");


	// basic retrieval of a variable
	// through multiple ways
	int x = lua["x"];
	auto y = lua.get<eastl::string>("y");

	int x2;
	eastl::string y2;
	eastl::tie(x2, y2) = lua.get<int, eastl::string>("x", "y");

	// show the values
	std::cout << x << std::endl;
	std::cout << y << std::endl;
	std::cout << x2 << std::endl;
	std::cout << y2 << std::endl;
	std::cout << std::endl;
}