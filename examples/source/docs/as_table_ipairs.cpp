#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

int main(int, char*[]) {

	sol::state lua;
	lua.open_libraries();
	lua.set("my_table", sol::as_table(eastl::vector<int> { 1, 2, 3, 4, 5 }));
	lua.script(
	     "for k, v in ipairs(my_table) do print(k, v) assert(k "
	     "== v) end");

	return 0;
}
