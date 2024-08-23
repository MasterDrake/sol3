#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

int main() {
	sol::state lua;

	lua.script("function f (a, b, c) return a, b, c end");

	eastl::tuple<int, int, int> result;
	result = lua["f"](1, 2, 3);
	SOL_ASSERT(result == eastl::make_tuple(1, 2, 3));
	int a, b;
	eastl::string c;
	// NOTE: sol::tie, NOT eastl::tie
	// (ESS OH ELL prefix, not ESS TEE DEE prefix)
	sol::tie(a, b, c) = lua["f"](1, 2, "bark");
	SOL_ASSERT(a == 1);
	SOL_ASSERT(b == 2);
	SOL_ASSERT(c == "bark");

	return 0;
}
