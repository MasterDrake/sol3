#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <EASTL/map.h>
#include <iostream>

struct my_thing {
	eastl::map<eastl::string, int> m {
		{ "bark", 20 },
		{ "woof", 60 },
		{ "borf", 30 },
		{ "awoo", 5 },
	};

	my_thing() {
	}
};

struct lua_iterator_state {
	typedef eastl::map<eastl::string, int>::iterator it_t;
	it_t it;
	it_t last;

	lua_iterator_state(my_thing& mt)
	: it(mt.m.begin()), last(mt.m.end()) {
	}
};

int my_next(lua_State* L) {
	// this gets called
	// to start the first iteration, and every
	// iteration there after
	// the state you passed in pairs is argument 1
	// the key value is argument 2
	// we do not care about the key value here
	lua_iterator_state& it_state
	     = sol::stack::get<sol::user<lua_iterator_state>>(L, 1);
	auto& it = it_state.it;
	if (it == it_state.last) {
		return sol::stack::push(L, sol::lua_nil);
	}
	auto itderef = *it;
	// 2 values are returned (pushed onto the stack):
	// the key and the value
	// the state is left alone
	int pushed = sol::stack::push(L, itderef.first);
	pushed += sol::stack::push_reference(L, itderef.second);
	eastl::advance(it, 1);
	return pushed;
}

int my_pairs(lua_State* L) {
	my_thing& mt = sol::stack::get<my_thing>(L, 1);
	lua_iterator_state it_state(mt);
	// pairs expects 3 returns:
	// the "next" function on how to advance,
	// the "table" itself or some state,
	// and an initial key value (can be nil)

	// next function controls iteration
	int pushed = sol::stack::push(L, my_next);
	pushed += sol::stack::push<sol::user<lua_iterator_state>>(
	     L, eastl::move(it_state));
	pushed += sol::stack::push(L, sol::lua_nil);
	return pushed;
}

int main(int, char*[]) {
	std::cout << "===== pairs (using raw Lua C functions) "
	             "(advanced) ====="
	          << std::endl;

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<my_thing>(
	     "my_thing", sol::meta_function::pairs, &my_pairs);

#if SOL_LUA_VERSION_I_ > 501
	lua.safe_script(R"(
local mt = my_thing.new()
for k, v in pairs(mt) do 
	print(k, v) 
end
)");
#endif // Does not work on Lua 5.1 and below

	std::cout << std::endl;

	return 0;
}
