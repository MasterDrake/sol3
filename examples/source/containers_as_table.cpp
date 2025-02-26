#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <EASTL/map.h>
#include <iostream>

// nested allows serialization of maps with vectors inside, and
// vice-versa all from a nested structure of Lua tables it has
// less control over which pieces are considered tables in Lua,
// and which ones are considered userdata, but it covers a good
// 90% of cases where someone wants to handle a nested table
void demo(
     sol::nested<eastl::map<eastl::string, eastl::vector<eastl::string>>>
          src) {
	std::cout << "demo, sol::nested<...>" << std::endl;
	const auto& listmap = src.value();
	SOL_ASSERT(listmap.size() == 2);
	for (const auto& kvp : listmap) {
		const eastl::vector<eastl::string>& strings = kvp.second;
		SOL_ASSERT(strings.size() == 3);
		std::cout << "\t" << kvp.first << " = ";
		for (const auto& s : strings) {
			std::cout << "'" << s << "'"
			          << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

// This second demo is equivalent to the first
// Nota bene the signature here
// Every container-type that's meant to be
// a table must be wrapped in `sol::as_table_t`
// it's verbose, so feel free to use typedefs to make it easy on
// you you can mix which parts are considered tables from Lua,
// and which parts are considered other kinds of types, such as
// userdata and the like
void demo_explicit(sol::as_table_t<eastl::map<eastl::string,
          sol::as_table_t<eastl::vector<eastl::string>>>>
          src) {
	std::cout << "demo, explicit sol::as_table_t<...>"
	          << std::endl;
	// Have to access the "source" member variable for
	// as_table_t
	const auto& listmap = src.value();
	SOL_ASSERT(listmap.size() == 2);
	for (const auto& kvp : listmap) {
		// Have to access the internal "source" for the inner
		// as_table_t, as well
		const eastl::vector<eastl::string>& strings
		     = kvp.second.value();
		SOL_ASSERT(strings.size() == 3);
		std::cout << "\t" << kvp.first << " = ";
		for (const auto& s : strings) {
			std::cout << "'" << s << "'"
			          << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}

int main(int, char**) {
	std::cout << "=== containers retrieved from lua tables ==="
	          << std::endl;

	sol::state lua;
	// bind the function
	lua.set_function("f", &demo);
	lua.set_function("g", &demo_explicit);
	// Call it with a table that has string sequences set to
	// distinct keys
	lua.script(R"(
t = { 
	key1 = {'hello', 'there', 'world'},
	key2 = {'bark', 'borf', 'woof'}
}
f(t)
g(t)
	)");

	std::cout << std::endl;

	return 0;
}
