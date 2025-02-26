#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>


int main(int, char*[]) {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua["f"] = [](int a, int b, sol::object c) {
		// sol::object can be anything here: just pass it
		// through
		return eastl::make_tuple(a, b, c);
	};

	eastl::tuple<int, int, int> result = lua["f"](100, 200, 300);
	const eastl::tuple<int, int, int> expected(100, 200, 300);
	SOL_ASSERT(result == expected);

	eastl::tuple<int, int, eastl::string> result2;
	result2 = lua["f"](100, 200, "BARK BARK BARK!");
	const eastl::tuple<int, int, eastl::string> expected2(
	     100, 200, "BARK BARK BARK!");
	SOL_ASSERT(result2 == expected2);

	int a, b;
	eastl::string c;
	sol::tie(a, b, c) = lua["f"](100, 200, "bark");
	SOL_ASSERT(a == 100);
	SOL_ASSERT(b == 200);
	SOL_ASSERT(c == "bark");

	lua.script(R"(
		a, b, c = f(150, 250, "woofbark")
		assert(a == 150)
		assert(b == 250)
		assert(c == "woofbark")
	)");

	return 0;
}
