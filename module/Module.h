#pragma once
#include <memory>
#include <unordered_set>

#include "JobManager.h"

struct lua_State;
class ILuaModuleManager;
using GraphId = size_t;

class Module
{
public:
	Module(ILuaModuleManager* manager);
	~Module();

	void Start();
	void Process();

	GraphId LoadGraph(const std::string& path);
	void UnloadGraph(GraphId graphId);

	inline void AddLuaVM(lua_State* luaVM) { _luaStates.insert(luaVM); }
	inline void RemoveLuaVM(lua_State* luaVM) { _luaStates.erase(luaVM); }
	inline bool HasLuaVM(lua_State* luaVM) { return _luaStates.find(luaVM) != _luaStates.end(); }

	//inline JobManager<pathfind::AStarResult>& GetJobManager() { return _jobManager; }

private:
	ILuaModuleManager* _moduleManager;
	//JobManager<pathfind::AStarResult> _jobManager;
	std::unordered_set<lua_State*> _luaStates;
};

extern Module* g_Module;
