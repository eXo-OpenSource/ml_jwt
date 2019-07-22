#pragma once
#include "extra/CLuaArguments.h"
#include <unordered_map>
#include <ostream>
#include <iostream>
#include "CFunctions.h"

class Utils
{
public:
	static inline std::unordered_map<std::string, std::string> parse_named_table(lua_State* lua_vm, const int index) // Can only parse lua tables with string indices, e.g. { ["a"] = 1, ["b"] = "g" }
	{
		unordered_map<std::string, std::string> result;

		lua_pushnil(lua_vm);
		while(lua_next(lua_vm, index) != 0)
		{
			if (lua_type(lua_vm, -1) != LUA_TSTRING || lua_type(lua_vm, -2) != LUA_TSTRING)
			{
				stringstream ss;
				ss << "Bad Argument @ parse_named_table. Got invalid table entry, expected key(string) and value(string), but was key(" << lua_typename(lua_vm, lua_type(lua_vm, -2)) << ") and value(" << lua_typename(lua_vm, lua_type(lua_vm, -1)) << ").";
				luaL_error(lua_vm, ss.str().c_str());
				return {};
			}

			result[lua_tostring(lua_vm, -2)] = lua_tostring(lua_vm, -1);
			lua_pop(lua_vm, 1);
		}

		return result;
	} 
};
