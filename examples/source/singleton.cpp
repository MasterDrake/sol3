#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <iostream>

#include <mutex>

struct SomeLib {
private:
	SomeLib() {
	}

public:
	static eastl::shared_ptr<SomeLib> getInstance();

	int doSomething() const {
		return 20;
	}

	// destructor must be public to work with
	// eastl::shared_ptr and friends
	// if you need it to be private, you must implement
	// a custom deleter with access to the private members
	// (e.g., a deleter struct defined in this class)
	~SomeLib() {
	}
};

eastl::shared_ptr<SomeLib> SomeLib::getInstance() {
	static eastl::weak_ptr<SomeLib> instance;
	static std::mutex m;

	m.lock();
	auto ret = instance.lock();
	if (!ret) {
		ret.reset(new SomeLib());
		instance = ret;
	}
	m.unlock();

	return ret;
}

int main(int, char*[]) {
	std::cout << "=== singleton ===" << std::endl;

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<SomeLib>("SomeLib",
	     "new",
	     sol::no_constructor,
	     "getInstance",
	     &SomeLib::getInstance,
	     "doSomething",
	     &SomeLib::doSomething);

	lua.script(R"(

-- note we use the `.` here, not `:` (there's no self to access)
local sli = SomeLib.getInstance()

-- we use the `:` here because there is something to access
local value = sli:doSomething()

-- check
print('sli:doSomething() returned:', value)
assert(value == 20)
)");

	std::cout << std::endl;
	return 0;
}
