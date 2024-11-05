#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp> // or #include "sol.hpp", whichever suits your needs
//
//void* __cdecl operator new[](size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line)
//{
//	(void)name;
//	(void)flags;
//	(void)debugFlags;
//	(void)file;
//	(void)line;
//	return new uint8_t[size];
//}

int main(int argc, char* argv[]) {
	// silence unused warnings
	(void)argc;
	(void)argv;

	sol::state lua;
	lua.open_libraries(sol::lib::base);
	
	lua.script("print('bark bark bark!')");

	return 0;
}
