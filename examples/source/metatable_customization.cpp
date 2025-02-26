#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <iostream>

struct thing {
	int member_variable = 5;

	double member_function() const {
		return member_variable / 2.0;
	}
};

#define TEMPLATE_AUTO(x) decltype(x), x

// cheap storage: in reality, you'd need to find a
// better way of handling this.
// or not! It's up to you.
static eastl::unordered_map<sol::string_view, sol::object>
     thing_function_associations;
static eastl::unordered_map<sol::string_view, sol::object>
     thing_variable_associations;

void register_thing_type(sol::state& lua) {
	thing_variable_associations.emplace_hint(
	     thing_variable_associations.cend(),
	     "member_variable",
	     sol::object(lua.lua_state(),
	          sol::in_place,
	          &sol::c_call<TEMPLATE_AUTO(
	               &thing::member_variable)>));
	thing_function_associations.emplace_hint(
	     thing_function_associations.cend(),
	     "member_function",
	     sol::object(lua.lua_state(),
	          sol::in_place,
	          &sol::c_call<TEMPLATE_AUTO(
	               &thing::member_function)>));

	struct call_handler {
		static int lookup_function(lua_State* L) {
			sol::stack_object source(L, 1);
			sol::stack_object key(L, 2);
			if (!source.is<thing>()) {
				return luaL_error(L,
				     "given an incorrect object for this "
				     "call");
			}
			sol::optional<sol::string_view> maybe_svkey
			     = key.as<sol::optional<sol::string_view>>();
			if (maybe_svkey) {
				{
					// functions are different from
					// variables functions, when obtain with
					// the syntax obj.f, obj.f(), and
					// obj:f() must return the function
					// itself so we just push it realy into
					// our target
					auto it
					     = thing_function_associations.find(
					          *maybe_svkey);
					if (it
					     != thing_function_associations
					             .cend()) {
						return it->second.push(L);
					}
				}
				{
					// variables are different than funtions
					// when someone does `obj.a`, they
					// expect this __index call (this lookup
					// function) to return to them the value
					// itself they're seeing so we call out
					// lua_CFunction that we serialized
					// earlier
					auto it
					     = thing_variable_associations.find(
					          *maybe_svkey);
					if (it
					     != thing_variable_associations
					             .cend()) {
						// note that calls generated by
						// sol2 for member variables expect
						// the stack ordering to be 2(, 3,
						// ..., n) -- value(s) 1 -- source
						// so we destroy the key on the
						// stack
						sol::stack::remove(L, 2, 1);
						lua_CFunction cf
						     = it->second
						            .as<lua_CFunction>();
						return cf(L);
					}
				}
			}
			return sol::stack::push(L, sol::lua_nil);
		}

		static int insertion_function(lua_State* L) {
			sol::stack_object source(L, 1);
			sol::stack_object key(L, 2);
			sol::stack_object value(L, 3);
			if (!source.is<thing>()) {
				return luaL_error(L,
				     "given an incorrect object for this "
				     "call");
			}
			// write to member variables, etc. etc...
			sol::optional<sol::string_view> maybe_svkey
			     = key.as<sol::optional<sol::string_view>>();
			if (maybe_svkey) {
				{
					// variables are different than funtions
					// when someone does `obj.a`, they
					// expect this __index call (this lookup
					// function) to return to them the value
					// itself they're seeing so we call out
					// lua_CFunction that we serialized
					// earlier
					auto it
					     = thing_variable_associations.find(
					          *maybe_svkey);
					if (it
					     != thing_variable_associations
					             .cend()) {
						// note that calls generated by
						// sol2 for member variables expect
						// the stack ordering to be 2(, 3,
						// ..., n) -- value(s) 1 -- source
						// so we remove the key value
						sol::stack::remove(L, 2, 1);
						lua_CFunction cf
						     = it->second
						            .as<lua_CFunction>();
						return cf(L);
					}
					else {
						// write to member variable, maybe
						// override function if your class
						// allows for it?
						(void)value;
					}
				}
				// exercise for reader:
				// how do you override functions on the
				// metatable with proper syntax, but error
				// when the type is an "instance" object?
			}
			return 0;
		}
	};

	lua.new_usertype<thing>("thing");
	sol::table metatable = lua["thing"];

	metatable[sol::meta_method::index]
	     = &call_handler::lookup_function;
	metatable[sol::meta_method::new_index]
	     = &call_handler::insertion_function;
}

void unregister_thing_type(sol::state&) {
	thing_function_associations.clear();
	thing_variable_associations.clear();
}

int main() {

	std::cout << "=== metatable with custom-built (static) "
	             "handling ==="
	          << std::endl;


	sol::state lua;
	lua.open_libraries(sol::lib::base);

	// register custom type + storage
	register_thing_type(lua);

	lua.script(R"(t = thing.new() 
		print(t.member_variable)
		print(t:member_function())
		t.member_variable = 24
		print(t.member_variable)
		print(t:member_function())
	)");

	// clear storage
	unregister_thing_type(lua);

	std::cout << std::endl;

	return 0;
}
