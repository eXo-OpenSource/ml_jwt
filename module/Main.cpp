#include "Common.h"
#include "CFunctions.h"
#include "include/ILuaModuleManager.h"
#include <cstring>
#include "Module.h"
#include <cassert>
#include <jwt-cpp/jwt.h>
#include "Utils.h"

#ifndef WIN32
	#include "luaimports/luaimports.linux.h"
#endif

ILuaModuleManager10* pModuleManager = nullptr;

// Initialization function (module entrypoint)
MTAEXPORT bool InitModule(ILuaModuleManager10* pManager, char* szModuleName, char* szAuthor, float* fVersion)
{
#ifndef WIN32
	ImportLua();
#endif

    pModuleManager = pManager;

	// Set the module info
    const auto module_name	= "JWT Module";
    const auto author		= "StiviK";
	std::memcpy(szModuleName, module_name,	MAX_INFO_LENGTH);
	std::memcpy(szAuthor,     author,		MAX_INFO_LENGTH);
	*fVersion = 1.0f;
	
	// Load module
	g_Module = new Module(pManager);
	g_Module->Start();

    return true;
}

MTAEXPORT void RegisterFunctions(lua_State* lua_vm)
{
	if (!pModuleManager || !lua_vm)
		return;

	// Add lua vm to states list (to check validity)
	g_Module->AddLuaVM(lua_vm);

	// Register functions
	pModuleManager->RegisterFunction(lua_vm, "jwtTest",			&CFunctions::Test);
	pModuleManager->RegisterFunction(lua_vm, "jwtSign",			&CFunctions::sign_jwt_token);
	pModuleManager->RegisterFunction(lua_vm, "jwtVerify",		&CFunctions::verify_jwt_token);
	pModuleManager->RegisterFunction(lua_vm, "jwtGetClaims",	&CFunctions::get_jwt_claims);

	// Register globals
	CFunctions::register_lua_global(lua_vm,	"JWT_ALGORITHM_HS256", reinterpret_cast<void*>(jwt_algorithm_hs256));
	CFunctions::register_lua_global(lua_vm,	"JWT_ALGORITHM_HS384", reinterpret_cast<void*>(jwt_algorithm_hs384));
	CFunctions::register_lua_global(lua_vm,	"JWT_ALGORITHM_HS512", reinterpret_cast<void*>(jwt_algorithm_hs512));
	CFunctions::register_lua_global(lua_vm,	"JWT_ALGORITHM_RS256", reinterpret_cast<void*>(jwt_algorithm_rs256));
	CFunctions::register_lua_global(lua_vm,	"JWT_ALGORITHM_RS384", reinterpret_cast<void*>(jwt_algorithm_rs384));
	CFunctions::register_lua_global(lua_vm,	"JWT_ALGORITHM_RS512", reinterpret_cast<void*>(jwt_algorithm_rs512));
}

MTAEXPORT bool DoPulse()
{
	g_Module->Process();

	return true;
}

MTAEXPORT bool ShutdownModule()
{
	// Unload module
	delete g_Module;

    return true;
}

MTAEXPORT bool ResourceStopping(lua_State* luaVM)
{
	// Invalidate lua vm by removing it from the valid list
	g_Module->RemoveLuaVM(luaVM);

	return true;
}

MTAEXPORT bool ResourceStopped(lua_State* luaVM)
{
	return true;
}
