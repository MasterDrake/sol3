#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <iostream>

struct holy {
private:
	holy() : data() {
	}
	holy(int value) : data(value) {
	}
	~holy() {
	}

public:
	struct deleter {
		void operator()(holy* p) const {
			destroy(*p);
		}
	};

	const int data;

	static eastl::unique_ptr<holy, deleter> create() {
		std::cout << "creating 'holy' unique_ptr directly and "
		             "letting sol/Lua handle it"
		          << std::endl;
		return eastl::unique_ptr<holy, deleter>(new holy(50));
	}

	static void initialize(holy& uninitialized_memory) {
		std::cout << "initializing 'holy' userdata at "
		          << static_cast<void*>(&uninitialized_memory)
		          << std::endl;
		// receive uninitialized memory from Lua:
		// properly set it by calling a constructor
		// on it
		// "placement new"
		new (&uninitialized_memory) holy();
	}

	static void destroy(holy& memory_from_lua) {
		std::cout << "destroying 'holy' userdata at "
		          << static_cast<void*>(&memory_from_lua)
		          << std::endl;
		memory_from_lua.~holy();
	}
};

int main() {
	std::cout << "=== usertype_initializers ===" << std::endl;
	{ // additional scope to make usertype destroy earlier
		sol::state lua;
		lua.open_libraries();

		lua.new_usertype<holy>("holy",
		     "new",
		     sol::initializers(&holy::initialize),
		     "create",
		     sol::factories(&holy::create),
		     sol::meta_function::garbage_collect,
		     sol::destructor(&holy::destroy),
		     "data",
		     &holy::data);

		lua.script(R"(
h1 = holy.create()
h2 = holy.new()
print('h1.data is ' .. h1.data)
print('h2.data is ' .. h2.data)
)");
		holy& h1 = lua["h1"];
		holy& h2 = lua["h2"];
		SOL_ASSERT(h1.data == 50);
		SOL_ASSERT(h2.data == 0);
	}
	std::cout << std::endl;
}
