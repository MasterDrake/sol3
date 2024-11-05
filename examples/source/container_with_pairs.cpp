#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <EASTL/unordered_set.h>
#include <iostream>

int main()
{
	struct hasher
	{
		using argument_type = eastl::pair<eastl::string, eastl::string>;
		using result_type = eastl::size_t;

		result_type operator()(const argument_type& p) const
		{
			return eastl::hash<eastl::string>()(p.first);
		}
	};

	using my_set = eastl::unordered_set<eastl::pair<eastl::string, eastl::string>,hasher>;

	std::cout << "=== containers with eastl::pair<> ===" << std::endl;

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.set_function("f", []()
	{
		return my_set { eastl::pair<eastl::string,eastl::string>(eastl::string("key1"), eastl::string("value1")),
						eastl::pair<eastl::string,eastl::string>(eastl::string("key2"), eastl::string("value2")),
						eastl::pair<eastl::string,eastl::string>(eastl::string("key3"), eastl::string("value3"))};
	});

	lua.safe_script("v = f()");
	lua.safe_script("print('v:', v)");
	lua.safe_script("print('#v:', #v)");
	// note that using my_obj:pairs() is a
	// way around pairs(my_obj) not working in Lua 5.1/LuaJIT:
	// try it!
	lua.safe_script("for k,v1,v2 in v:pairs() do print(k, v1, v2) end");

	std::cout << std::endl;

	return 0;
}
