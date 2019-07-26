#pragma once
#include "extra/CLuaArguments.h"

class ILuaModuleManager10;
struct lua_State;

extern ILuaModuleManager10* pModuleManager;


class CFunctions
{
public:
	static inline int Test(lua_State* lua_vm)
	{
		lua_pushboolean(lua_vm, true);
		return 1;
	}

	static inline void register_lua_global(lua_State* lua_vm, const char* name, void* value)
	{
		lua_pushlightuserdata(lua_vm, value);
		lua_setglobal(lua_vm, name);
	}


	static int verify_jwt_token(lua_State* lua_vm);
	static int sign_jwt_token(lua_State* lua_vm);

	static int get_jwt_claims(lua_State* lua_vm);

	/*
	static int LoadPathGraph(lua_State* luaVM);
	static int UnloadPathGraph(lua_State* luaVM);
	static int FindShortestPathBetween(lua_State* luaVM);
	static int IsGraphLoaded(lua_State* luaVM);
	static int FindNodeAt(lua_State* luaVM);
	static int GetNodeNeighbors(lua_State* luaVM);
	*/
};
