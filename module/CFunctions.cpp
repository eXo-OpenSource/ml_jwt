#include "CFunctions.h"
#include "include/ILuaModuleManager.h"

#include "Module.h"
#include <jwt-cpp/jwt.h>
#include <sstream>
#include "UItils.h"
#include "Crypto.h"

#ifndef _WIN32
	#include <sys/stat.h>
#endif

int CFunctions::sign_jwt_token(lua_State* lua_vm)
{
	// bool jwtSign(function(string/boolean) callback, table claims, string algorithm, string secret/public_key_path, string private_key_path)
	if (lua_type(lua_vm, 1) != LUA_TFUNCTION || lua_type(lua_vm, 2) != LUA_TTABLE || lua_type(lua_vm, 3) != LUA_TSTRING || lua_type(lua_vm, 4) != LUA_TSTRING || 
		(lua_type(lua_vm, 5) != LUA_TNONE && lua_type(lua_vm, 5) != LUA_TSTRING))
	{
		pModuleManager->ErrorPrintf("Bad argument @ jwtSign\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	// Save reference of the Lua callback function
	// See: http://lua-users.org/lists/lua-l/2008-12/msg00193.html
	lua_pushvalue(lua_vm, 1);
	const auto func_ref         = luaL_ref(lua_vm, LUA_REGISTRYINDEX);

	// Read other arguments
	const auto claims                   = Utils::parse_named_table(lua_vm, 2);
	const auto algorithm                = lua_tostring(lua_vm, 3);
	const auto public_key_path          = lua_tostring(lua_vm, 4);
	const auto private_key_path         = lua_tostring(lua_vm, 5);
	std::string public_key, private_key = public_key_path;
	
	// Read public- and private key from files
	if (lua_type(lua_vm, 5) != LUA_TNONE)
	{
		if (!Crypto::read_key_pair(public_key_path, public_key, private_key_path, private_key))
		{
			pModuleManager->ErrorPrintf("Bad argument @ jwtSign\n");
			lua_pushboolean(lua_vm, false);
			return 1;
		}
	}

	// Process signing
	g_Module->GetJobManager().PushTask([/* lua_vm, */ claims, algorithm, public_key, private_key]()
	{
		const auto& now = std::chrono::system_clock::now();
		auto jwt = jwt::create()
			.set_issued_at(now)
			.set_not_before(now);

		// Add claims
		for (const auto& pair : claims)
		{
			jwt.set_payload_claim(pair.first, pair.second);
		}

		// sign the token
		std::string token;
		if (std::strcmp(algorithm, "HS256") == 0)
			token = jwt.sign(jwt::algorithm::hs256{ public_key });
		else if (std::strcmp(algorithm, "RS256") == 0)
			token = jwt.sign(jwt::algorithm::rs256{ public_key, private_key });
		else
			//luaL_error(lua_vm, "Error @ jwtSign, invalid algorithm has been passed."); // Todo: find a way to call this without the mta server raising panic
			pModuleManager->ErrorPrintf("Error @ jwtSign, invalid algorithm has been passed.");

		return token;
	}, [lua_vm = lua_getmainstate(lua_vm), func_ref](const std::string& token)
	{
		// Validate LuaVM (use ResourceStart/-Stop to manage valid lua states)
		if (!g_Module->HasLuaVM(lua_vm))
			return;

		// Push stored reference to callback function to the stack
		lua_rawgeti(lua_vm, LUA_REGISTRYINDEX, func_ref);

		// Push token
		if (token.length() > 0) {
			lua_pushstring(lua_vm, token.c_str());
		} else {
			lua_pushboolean(lua_vm, false);
		}

		// Finally, call the function
		const auto err = lua_pcall(lua_vm, 1, 0, 0);
		if (err != 0)
			pModuleManager->ErrorPrintf("%s\n", lua_tostring(lua_vm, -1));
	});

	lua_pushboolean(lua_vm, true);
	return 1;
}

int CFunctions::verify_jwt_token(lua_State* lua_vm)
{
	// bool jwtVerify(string token, string secret/public_key_path, bool is_file_path = false)
	if (lua_type(lua_vm, 1) != LUA_TSTRING || lua_type(lua_vm, 2) != LUA_TSTRING ||
		(lua_type(lua_vm, 3) != LUA_TNONE && lua_type(lua_vm, 3) != LUA_TBOOLEAN))
	{
		pModuleManager->ErrorPrintf("Bad argument @ jwtVerify\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	// Read arguments
	const auto token            = lua_tostring(lua_vm, 1);
	const auto public_key_path  = lua_tostring(lua_vm, 2);
	std::string public_key      = public_key_path;

	// Read public- and private key from files
	const auto is_file_path     = lua_type(lua_vm, 3) != LUA_TNONE && lua_toboolean(lua_vm, 3);
	if (is_file_path)
	{
		if (!Crypto::read_key(public_key_path, public_key))
		{
			pModuleManager->ErrorPrintf("Bad argument @ jwtVerify\n");
			lua_pushboolean(lua_vm, false);
			return 1;
		}
	}

	// Process verification
	try {
		const auto decoded_jwt = jwt::decode(token);
		const auto algorithm = decoded_jwt.get_algorithm().c_str();
		auto verifier = jwt::verify();

		// set verifier algorithm
		if (std::strcmp(algorithm, "HS256") == 0)
			verifier.allow_algorithm(jwt::algorithm::hs256{ public_key });
		else if (std::strcmp(algorithm, "RS256") == 0)
			verifier.allow_algorithm(jwt::algorithm::rs256{ public_key });

		verifier.verify(decoded_jwt);

		lua_pushboolean(lua_vm, true);
		return 1;
	} catch (exception& e)
	{
		std::stringstream ss;
		ss << "Bad Argument @ jwtVerify, " << e.what();
		pModuleManager->ErrorPrintf(ss.str().c_str());

		lua_pushboolean(lua_vm, false);
		return 1;
	}
}

int CFunctions::get_jwt_claims(lua_State* lua_vm)
{
	// table jwtGetClaims(string token)
	if (lua_type(lua_vm, 1) != LUA_TSTRING)
	{
		pModuleManager->ErrorPrintf("Bad argument @ jwtGetClaims\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	// Read arguments
	const auto token = lua_tostring(lua_vm, 1);

	// Process get claims
	try
	{
		const auto decoded_jwt = jwt::decode(token);

		lua_newtable(lua_vm);
			for(auto& e : decoded_jwt.get_payload_claims()) {
				lua_pushstring(lua_vm, e.first.c_str()); // index
				lua_pushstring(lua_vm, e.second.to_json().to_str().c_str()); // value
				lua_settable(lua_vm, -3);
			}

		return 1;
	} catch  (exception& e)
	{
		std::stringstream ss;
		ss << "Bad Argument @ jwtGetClaims, " << e.what();
		pModuleManager->ErrorPrintf(ss.str().c_str());

		lua_pushboolean(lua_vm, false);
		return 1;
	}
}
