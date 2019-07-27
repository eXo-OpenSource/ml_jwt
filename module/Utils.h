#pragma once
#include <unordered_map>
#include <ostream>
#include <sstream>

#include "ILuaModuleManager.h"

static const int index_value = -1;
static const int index_key   = -2;

#ifdef _DEBUG
#define DEBUG_LOG( msg )  std::cout << "[ml_jwt] " << __FILE__ << ":" << __LINE__ << ": " << msg << "\n"
#else
#define DEBUG_LOG( msg )
#endif

enum jwt_algorithm : size_t
{
	jwt_algorithm_none  = 0x123456789ABCDEF,
	jwt_algorithm_hs256 = jwt_algorithm_none << 1,
	jwt_algorithm_hs384 = jwt_algorithm_none << 2,
	jwt_algorithm_hs512 = jwt_algorithm_none << 3,
	jwt_algorithm_rs256 = jwt_algorithm_none << 4,
	jwt_algorithm_rs384 = jwt_algorithm_none << 5,
	jwt_algorithm_rs512 = jwt_algorithm_none << 6
};

class Utils
{
public:
	static inline std::unordered_map<std::string, std::string> parse_table(lua_State* lua_vm, const int index)
	{
		unordered_map<std::string, std::string> result;

		for(lua_pushnil(lua_vm); lua_next(lua_vm, index) != 0; lua_pop(lua_vm, 1))
		{
			const auto type_key   = lua_type(lua_vm, index_key);
			const auto type_value = lua_type(lua_vm, index_value);
			if (type_key != LUA_TSTRING && type_key != LUA_TNUMBER)
			{
				stringstream ss;
				ss << "got invalid table key, expected string or number got " << lua_typename(lua_vm, type_key);
				throw runtime_error(ss.str());
			}

			if (type_value != LUA_TSTRING && type_value != LUA_TNUMBER)
			{
				stringstream ss;
				ss << "got invalid table value, expected string or number got " << lua_typename(lua_vm, type_value);
				throw runtime_error(ss.str());
			}

			auto to_string = [lua_vm](const int index) -> const std::string
			{
				const auto type = lua_type(lua_vm, index);
				if (type == LUA_TSTRING)
				{
					return lua_tostring(lua_vm, index);
				}

				if (type == LUA_TNUMBER)
				{
					return std::to_string(lua_tointeger(lua_vm, index));
				}
				return {};
			};

			result[to_string(index_key)] = to_string(index_value);
		}

		return result;
	}

	static inline void get_real_path(lua_State* lua_vm, const std::string& relative_path, std::string& real_path)
	{
		char buf[300];
		if (!pModuleManager->GetResourceFilePath(lua_vm, relative_path.c_str(), buf, sizeof(buf)))
		{
			stringstream ss;
			ss << "invalid path: " << relative_path;
			throw runtime_error(ss.str());
		}

		// Check if path is valid
		const std::string path{ buf };
		if (path.find("..") != std::string::npos)
		{
			stringstream ss;
			ss << "path is illegal: " << relative_path;
			throw runtime_error(ss.str());
		}

		real_path.assign(path);
	}
};
