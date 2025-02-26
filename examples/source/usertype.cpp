#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <iostream>
#include <cmath>

struct foo {
private:
	eastl::string name;

public:
	foo(eastl::string name) : name(eastl::string(name)) {
	}

	void print() {
		std::cout << name << '\n';
	}

	int test(int x) {
		return static_cast<int>(name.length()) + x;
	}
};

struct vector {
private:
	float x = 0;
	float y = 0;

public:
	vector() = default;
	vector(float x) : x(x) {
	}
	vector(float x, float y) : x(x), y(y) {
	}

	bool is_unit() const {
		return (x * x + y * y) == 1.f;
	}
};

struct variables {
	bool low_gravity = false;
	int boost_level = 0;
};

int main() {
	std::cout << "=== usertype ===" << std::endl;

	sol::state lua;
	lua.open_libraries(sol::lib::base, sol::lib::math);

	// the simplest way to create a class is through
	// sol::state::new_userdata
	// the first template is the class type
	// the rest are the constructor parameters
	// using new_userdata you can only have one constructor


	// you must make sure that the name of the function
	// goes before the member function pointer
	lua.new_usertype<foo>("foo",
	     sol::constructors<foo(eastl::string)>(),
	     "print",
	     &foo::print,
	     "test",
	     &foo::test);

	// making the class from lua is simple
	// same with calling member functions
	lua.script(
	     "x = foo.new('test')\n"
	     "x:print()\n"
	     "y = x:test(10)");

	auto y = lua.get<int>("y");
	std::cout << y << std::endl; // show 14

	// if you want a class to have more than one constructor
	// the way to do so is through set_userdata and creating
	// a userdata yourself with constructor types

	{
		// Notice the brace: this means we're in a new scope

		// first, define the different types of constructors
		// notice here that the return type
		// on the function-type doesn't exactly matter,
		// which allows you to use a shorter class name/void
		// if necessary
		sol::constructors<vector(),
		     vector(float),
		     void(float, float)>
		     ctor;
		// then you must register it
		sol::usertype<vector> utype
		     = lua.new_usertype<vector>("vector", ctor);

		// add to it as much as you like
		utype["is_unit"] = &vector::is_unit;
		// You can throw away the usertype after
		// you set it: you do NOT
		// have to keep it around
		// cleanup happens automagically!
	}

	// calling it is the same as
	// working with userdata is C++
	lua.script(
	     "v = vector.new()\n"
	     "v = vector.new(12)\n"
	     "v = vector.new(10, 10)\n"
	     "assert(not v:is_unit())\n");

	// You can even have C++-like member-variable-access
	// just pass is public member variables in the same style as
	// functions
	lua.new_usertype<variables>("variables",
	     "low_gravity",
	     &variables::low_gravity,
	     "boost_level",
	     &variables::boost_level);

	// making the class from lua is simple
	// same with calling member functions/variables
	lua.script(
	     "local vars = variables.new()\n"
	     "assert(not vars.low_gravity)\n"
	     "vars.low_gravity = true\n"
	     "local x = vars.low_gravity\n"
	     "assert(x)");

	std::cout << std::endl;

	return 0;
}
