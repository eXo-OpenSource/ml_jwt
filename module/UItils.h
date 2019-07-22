#pragma once
#include "extra/CLuaArguments.h"
#include <unordered_map>
#include <ostream>
#include <iostream>

struct lua_State;

class Utils
{
public:
	static inline std::unordered_map<std::string, std::string> parse_named_table(lua_State* lua_vm, const int index) // Can only parse lua tables with string indices, e.g. { ["a"] = 1, ["b"] = "g" }
	{
		unordered_map<std::string, std::string> result;

		lua_pushnil(lua_vm);
		while(lua_next(lua_vm, index) != 0)
		{
			result[lua_tostring(lua_vm, -1)] = lua_tostring(lua_vm, -2);
			lua_pop(lua_vm, 1);
		}

		return result;
	} 
};
