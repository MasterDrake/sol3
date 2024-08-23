#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>


#include <EASTL/string.h>
#include <EASTL/set.h>

int main() {
	sol::state lua;

	lua.set_function("f", []() {
		eastl::set<eastl::string> results {
			"arf", "bark", "woof"
		};
		return sol::as_returns(eastl::move(results));
	});

	lua.script("v1, v2, v3 = f()");

	eastl::string v1 = lua["v1"];
	eastl::string v2 = lua["v2"];
	eastl::string v3 = lua["v3"];

	SOL_ASSERT(v1 == "arf");
	SOL_ASSERT(v2 == "bark");
	SOL_ASSERT(v3 == "woof");

	return 0;
}
