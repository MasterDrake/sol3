#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <iostream>

struct Shape {
	virtual ~Shape() = default;
};

struct Box : Shape { };

SOL_BASE_CLASSES(Box, Shape);
SOL_DERIVED_CLASSES(Shape, Box);

int main() {
	sol::state lua;
	lua.open_libraries(sol::lib::base);

	lua.new_usertype<Shape>("Shape", sol::no_constructor);

	lua.new_usertype<Box>("Box", sol::factories([&]() {
		auto b = eastl::make_shared<Box>();
		std::cout << "create Box@" << std::hex << b.get()
		          << '\n';
		return b;
	}));

	lua.set_function(
	     "inspect_shape_table", [](const sol::table& args) {
		     eastl::shared_ptr<Shape> defbox = nullptr;
		     // check if there's a field with the name "shape"
		     auto s = args.get<
		          sol::optional<eastl::shared_ptr<Shape>>>(
		          "shape");
		     std::cout << "has   : " << std::boolalpha
		               << s.has_value() << '\n';

		     // get the field named "shape" or use the default
		     // value
		     std::cout << "get_or: " << std::hex
		               << args.get_or<eastl::shared_ptr<Shape>>(
		                           "shape", defbox)
		                       .get()
		               << '\n';

		     // this works but I can't test for existence
		     // beforehand...
		     std::cout
		          << "get   : " << std::hex
		          << args.get<eastl::shared_ptr<Shape>>("shape")
		                  .get()
		          << '\n';
	     });

	sol::protected_function_result result = lua.safe_script(
	     "inspect_shape_table({shape=Box.new()})");
	SOL_ASSERT(result.valid());

	return 0;
}
