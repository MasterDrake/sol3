#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <iostream>

struct Doge {
	int tailwag = 50;

	Doge() {
	}

	Doge(int wags) : tailwag(wags) {
	}

	~Doge() {
		std::cout << "Dog at " << this
		          << " is being destroyed..." << std::endl;
	}
};

int main(int, char*[]) {
	std::cout << "=== userdata ===" << std::endl;

	sol::state lua;

	Doge dog { 30 };

	// fresh one put into Lua
	lua["dog"] = Doge {};
	// Copy into lua: destroyed by Lua VM during garbage
	// collection
	lua["dog_copy"] = dog;
	// OR: move semantics - will call move constructor if
	// present instead Again, owned by Lua
	lua["dog_move"] = eastl::move(dog);
	lua["dog_unique_ptr"] = eastl::make_unique<Doge>(25);
	lua["dog_shared_ptr"] = eastl::make_shared<Doge>(31);

	// Identical to above
	Doge dog2 { 30 };
	lua.set("dog2", Doge {});
	lua.set("dog2_copy", dog2);
	lua.set("dog2_move", eastl::move(dog2));
	lua.set("dog2_unique_ptr",
	     eastl::unique_ptr<Doge>(new Doge(25)));
	lua.set("dog2_shared_ptr",
	     eastl::shared_ptr<Doge>(new Doge(31)));

	// Note all of them can be retrieved the same way:
	Doge& lua_dog = lua["dog"];
	Doge& lua_dog_copy = lua["dog_copy"];
	Doge& lua_dog_move = lua["dog_move"];
	Doge& lua_dog_unique_ptr = lua["dog_unique_ptr"];
	Doge& lua_dog_shared_ptr = lua["dog_shared_ptr"];
	SOL_ASSERT(lua_dog.tailwag == 50);
	SOL_ASSERT(lua_dog_copy.tailwag == 30);
	SOL_ASSERT(lua_dog_move.tailwag == 30);
	SOL_ASSERT(lua_dog_unique_ptr.tailwag == 25);
	SOL_ASSERT(lua_dog_shared_ptr.tailwag == 31);

	// lua will treat these types as opaque, and you will be
	// able to pass them around to C++ functions and Lua
	// functions alike

	// Use a C++ reference to handle memory directly
	// otherwise take by value, without '&'
	lua["f"] = [](Doge& dog) {
		std::cout << "dog wags its tail " << dog.tailwag
		          << " times!" << std::endl;
	};

	// if you bind a function using a pointer,
	// you can handle when `nil` is passed
	lua["handling_f"] = [](Doge* dog) {
		if (dog == nullptr) {
			std::cout << "dog was nil!" << std::endl;
			return;
		}
		std::cout << "dog wags its tail " << dog->tailwag
		          << " times!" << std::endl;
	};

	lua.script(R"(
		f(dog)
		f(dog_copy)
		f(dog_move)
		f(dog_unique_ptr)
		f(dog_shared_ptr)

		-- C++ arguments that are pointers can handle nil
		handling_f(dog)
		handling_f(dog_copy)
		handling_f(dog_move)
		handling_f(dog_unique_ptr)
		handling_f(dog_shared_ptr)
		handling_f(nil)

		-- never do this
		-- f(nil)
	)");

	std::cout << std::endl;

	return 0;
}
