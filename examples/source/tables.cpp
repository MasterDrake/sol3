#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#include <iostream>

// this example shows how to read data in from a lua table

int main() {
	std::cout << "=== tables ===" << std::endl;

	sol::state lua;
	// table used as an array
	lua.script(R"(table1 = {"hello", "table"})");
	// table with a nested table and the key value syntax
	lua.script(R"(
		table2 = {
		["nestedTable"] = {
			["key1"] = "value1",
			["key2"]= "value2",
		},
		["name"] = "table2",
	}
	)");


	/* Shorter Syntax: */
	// using the values stored in table1
	/*std::cout << (eastl::string)lua["table1"][1] << " "
	     << (eastl::string)lua["table1"][2] << '\n';
	   */
	// some retrieval of values from the nested table
	// the cleaner way of doing things
	// chain off the the get<>() / [] results
	auto t2 = lua.get<sol::table>("table2");
	auto nestedTable = t2.get<sol::table>("nestedTable");
	// Alternatively:
	// sol::table t2 = lua["table2"];
	// sol::table nestedTable = t2["nestedTable"];

	eastl::string x = lua["table2"]["nestedTable"]["key2"];
	std::cout << "nested table: key1 : "
	          << nestedTable.get<eastl::string>("key1")
	          << ", key2: " << x << '\n';
	std::cout << "name of t2: " << t2.get<eastl::string>("name")
	          << '\n';
	eastl::string t2name = t2["name"];
	std::cout << "name of t2: " << t2name << '\n';

	/* Longer Syntax: */
	// using the values stored in table1
	std::cout << lua.get<sol::table>("table1").get<eastl::string>(
	     1) << " "
	          << lua.get<sol::table>("table1").get<eastl::string>(
	                  2)
	          << '\n';

	// some retrieval of values from the nested table
	// the cleaner way of doing things
	std::cout << "nested table: key1 : "
	          << nestedTable.get<eastl::string>("key1")
	          << ", key2: "
	          // yes you can chain the get<>() results
	          << lua.get<sol::table>("table2")
	                  .get<sol::table>("nestedTable")
	                  .get<eastl::string>("key2")
	          << '\n';
	std::cout << "name of t2: " << t2.get<eastl::string>("name")
	          << '\n';

	std::cout << std::endl;

	return 0;
}
