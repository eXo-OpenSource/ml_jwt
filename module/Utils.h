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
				ss << "got invalid table entry, expected key(string) and value(string), but was key(" << lua_typename(lua_vm, lua_type(lua_vm, -2)) << ") and value(" << lua_typename(lua_vm, lua_type(lua_vm, -1)) << ")";
				throw runtime_error(ss.str());
			}

			result[lua_tostring(lua_vm, -2)] = lua_tostring(lua_vm, -1);
			lua_pop(lua_vm, 1);
		}

		return result;
	}

	static inline void format_path(lua_State* lua_vm, const std::string& input_path, std::string& formatted_path)
	{
		char buf[300];
		if (!pModuleManager->GetResourceFilePath(lua_vm, input_path.c_str(), buf, sizeof(buf)))
		{
			throw runtime_error("file not found");
		}

		// Check if path is valid
		const std::string path{ buf };
		if (path.find("..") != std::string::npos)
		{
			throw runtime_error("invalid path");
		}

		formatted_path.assign(path);
	}
};
