#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <EASTL/slist.h>
#include <iosfwd>

#include <iostream>

class automatic {
private:
	eastl::slist<double> data;

public:
	using value_type = decltype(data)::value_type;
	using iterator = decltype(data)::const_iterator;
	using size_type = decltype(data)::size_type;

	// automatically bound to obj( args... ) [ __call ]
	void operator()() {
		data.push_front(
		     static_cast<value_type>(data.size() + 1) / 3.0);
	}

	// automatically used for pairs(obj) [ __pairs ], 5.2+
	iterator begin() const {
		return data.begin();
	}
	iterator end() const {
		return data.end();
	}

	// automatically bound to #obj [ __len ]
	size_type size() const {
		return data.size();
	}

	// automatically bound for obj == obj [ __eq ]
	bool operator==(const automatic& right) const {
		return data == right.data;
	}
	// automatically bound for obj < obj [ __lt ]
	bool operator<(const automatic& right) const {
		return data < right.data;
	}
	// automatically bound for obj <= obj [ __le ]
	bool operator<=(const automatic& right) const {
		return data <= right.data;
	}
	// other comparison operators are based off the above in Lua
	// and cannot be overridden directly
};
namespace std
	{
// automatically bound to tostring(obj) [ __tostring ]
std::ostream& operator<<(
     std::ostream& os, const automatic& right) {
	if (right.size() == 0) {
		os << "{ empty }";
		return os;
	}
	auto b = right.begin();
	auto e = right.end();
	os << "{ " << right.size() << " | ";
	os << *b;
	++b;
	while (b != e) {
		os << ", " << *b;
		++b;
	}
	os << " }";
	return os;
}
}
int main(int, char*[]) {
	std::cout << "=== usertype automatic operators ==="
	          << std::endl;

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<automatic>("automatic");

	lua.script(R"(
obj1 = automatic.new()
obj2 = automatic.new()
obj3 = automatic.new()

print("obj1:", obj1)
print("obj2:", obj2)
print("obj3:", obj2)

print("#obj1:", #obj1)
print("#obj2:", #obj2)
print("#obj3:", #obj3)

obj1() obj1() obj1() obj1() obj1() obj1()
obj2() obj2() obj2()
obj3() obj3() obj3()

print("after modifications using obj() operator")
print("obj1:", obj1)
print("obj2:", obj2)
print("obj3:", obj2)

print("#obj1:", #obj1)
print("#obj2:", #obj2)
print("#obj3:", #obj3)

	)");
#if SOL_LUA_VERSION_I_ > 501
	lua.script(R"(
for k, v in pairs(obj1) do
	assert( (k / 3) == v )
end
)");
#endif

	lua.script(R"(
print("obj1 == obj2:", obj1 == obj2)
print("obj1 <  obj2:", obj1 < obj2)
print("obj1 >= obj2:", obj1 >= obj2)
assert(obj1 ~= obj2)
assert(obj1 > obj2)
assert(obj1 >= obj2)

print("obj2 == obj3:", obj2 == obj3)
print("obj2 >  obj3:", obj2 > obj3)
print("obj2 <= obj3:", obj2 <= obj3)
assert(obj2 == obj3)
assert(obj2 <= obj3)
	)");

	std::cout << std::endl;

	return 0;
}
