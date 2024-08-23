#define SOL_ALL_SAFETIES_ON 1
#define SOL_STD_VARIANT 0
#define SOL_NO_EXCEPTIONS 1
#define SOL_ALIGN_MEMORY 0
#include <sol/sol.hpp>

#include <EASTL/vector.h>
#include <iostream>

//void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
//{
//	(void)name;
//	(void)flags;
//	(void)debugFlags;
//	(void)file;
//	(void)line;
//	return new uint8_t[size];
//}
//
//void* __cdecl operator new[](size_t size, size_t alignment, size_t offset, const char* name, int flags, unsigned debugFlags, const char* file, int line)
//{
//	(void)name;
//	(void)alignment;
//	(void)offset;
//	(void)flags;
//	(void)debugFlags;
//	(void)file;
//	(void)line;
//	return new uint8_t[size];
//}

//namespace std
//{
//    std::ostream & operator<<(std::ostream &sout, eastl::string const & p)
//    {
//        return sout << p.c_str();
//    }
//}

int main(int, char**)
{
	std::cout << "=== containers ===" << std::endl;

	sol::state lua;
	lua.open_libraries();

	lua.script(R"(
function f (x)
	print("container has:")
	for k=1,#x do
		v = x[k]
		print("\t", k, v)
	end
	print()
end
	)");

	// Have the function we
	// just defined in Lua
	sol::function f = lua["f"];

	// Set a global variable called
	// "arr" to be a vector of 5 lements
	lua["arr"] = eastl::vector<int> { 2, 4, 6, 8, 10 };

	// Call it, see 5 elements
	// printed out
	f(lua["arr"]);

	// Mess with it in C++
	// Containers are stored as userdata, unless you
	// use `sol::as_table()` and `sol::as_table_t`.
	eastl::vector<int>& reference_to_arr = lua["arr"];
	reference_to_arr.push_back(12);

	// Call it, see *6* elements
	// printed out
	f(lua["arr"]);

	lua.script(R"(
arr:add(28)
	)");

	// Call it, see *7* elements
	// printed out
	f(lua["arr"]);

	lua.script(R"(
arr:clear()
	)");

	// Now it's empty
	f(lua["arr"]);

	std::cout << std::endl;

	return 0;
}