#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <iostream>
#include <EASTL/numeric.h>

class number_storage {
private:
	eastl::vector<int> data;

public:
	number_storage(int i) {
		data.push_back(i);
	}

	int accumulate() const {
		return eastl::accumulate(data.begin(), data.end(), 0);
	}

public:
	using value_type = decltype(data)::value_type;
	using iterator = decltype(data)::iterator;
	using size_type = decltype(data)::size_type;
	iterator begin() {
		return iterator(data.begin());
	}
	iterator end() {
		return iterator(data.end());
	}
	size_type size() const noexcept {
		return data.size();
	}
	size_type max_size() const noexcept {
		return SIZE_MAX;
	}
	void push_back(int value) {
		data.push_back(value);
	}
	bool empty() const noexcept {
		return data.empty();
	}
};

namespace sol {
	template <>
	struct is_container<number_storage> : eastl::false_type { };
} // namespace sol

int main(int, char*[]) {
	std::cout << "=== container as container ===" << std::endl;

	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<number_storage>("number_storage",
	     sol::constructors<number_storage(int)>(),
	     "accumulate",
	     &number_storage::accumulate,
	     "iterable",
	     [](number_storage& ns) {
		     return sol::as_container(
		          ns); // treat like a container, despite
		               // is_container specialization
	     });

	lua.script(R"(
ns = number_storage.new(23)
print("accumulate before:", ns:accumulate())

-- reference original usertype like a container
ns_container = ns:iterable()
ns_container:add(24)
ns_container:add(25)

-- now print to show effect
print("accumulate after :", ns:accumulate())
	)");

	number_storage& ns = lua["ns"];
	number_storage& ns_container = lua["ns_container"];
	SOL_ASSERT(&ns == &ns_container);
	SOL_ASSERT(ns.size() == 3);

	std::cout << std::endl;

	return 0;
}
