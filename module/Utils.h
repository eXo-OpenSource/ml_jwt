#pragma once
#include <unordered_map>
#include <ostream>
#include <sstream>
#include "ILuaModuleManager.h"

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

	static inline bool format_path(lua_State* lua_vm, const std::string& input_path, std::string& formatted_path)
	{
		char buf[300];
		if (!pModuleManager->GetResourceFilePath(lua_vm, input_path.c_str(), buf, sizeof(buf)))
		{
			return false;
		}

		// Check if path is valid
		const std::string path{ buf };
		if (path.find("..") != std::string::npos)
		{
			return false;
		}

		formatted_path.assign(path);
		return true;
	}
};
