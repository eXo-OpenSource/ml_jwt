#include "CFunctions.h"
#include "include/ILuaModuleManager.h"
#include "extra/CLuaArguments.h"

#include "Module.h"
#include <jwt-cpp/jwt.h>
#include <sstream>
#include "UItils.h"
#include <fstream>
#include "Crypto.h"

#ifndef _WIN32
	#include <sys/stat.h>
#endif

int CFunctions::sign_jwt_token(lua_State* lua_vm)
{
	// string jwtSign(table claims, string algorithm, string secret/publicKeyPath, string? privateKey)
	if (lua_type(lua_vm, 1) != LUA_TTABLE || lua_type(lua_vm, 2) != LUA_TSTRING || lua_type(lua_vm, 3) != LUA_TSTRING || 
		(lua_type(lua_vm, 4) != LUA_TNONE && lua_type(lua_vm, 4) != LUA_TSTRING))
	{
		pModuleManager->ErrorPrintf("Bad argument @ jwtSign\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}
	
	const auto claims           = Utils::parse_named_table(lua_vm, 1);
	const auto algorithm        = lua_tostring(lua_vm, 2);
	const auto public_key_path  = lua_tostring(lua_vm, 3);
	const auto private_key_path = lua_tostring(lua_vm, 4);

	std::string public_key = public_key_path, private_key;
	if (lua_type(lua_vm, 4) != LUA_TNONE)
	{
		std::string pub_path;
		if (!Crypto::read_key_pair(public_key_path, private_key_path, &public_key, &private_key))
		{
			pModuleManager->ErrorPrintf("Bad argument @ jwtVerify\n");
			lua_pushboolean(lua_vm, false);
			return 1;
		}
	}

	const auto& now = std::chrono::system_clock::now();
	auto jwt = jwt::create()
		.set_issued_at(now)
		.set_not_before(now);

	// Add claims
	for (const auto& pair : claims)
	{
		jwt.set_payload_claim(pair.first, pair.second);
	}

	lua_pushstring(lua_vm, jwt.sign(jwt::algorithm::hs256(public_key)).c_str());
	return 1;
}

int CFunctions::verify_jwt_token(lua_State* lua_vm)
{
	// bool jwtVerify(string token, string algorithm, string secret/publicKeyPath, string? privateKey)
	if (lua_type(lua_vm, 1) != LUA_TSTRING || lua_type(lua_vm, 2) != LUA_TSTRING || lua_type(lua_vm, 3) != LUA_TSTRING ||
		(lua_type(lua_vm, 4) != LUA_TNONE && lua_type(lua_vm, 4) != LUA_TSTRING))
	{
		pModuleManager->ErrorPrintf("Bad argument @ jwtVerify\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	const auto token            = lua_tostring(lua_vm, 1);
	const auto algorithm        = lua_tostring(lua_vm, 2);
	const auto public_key_path  = lua_tostring(lua_vm, 3);
	const auto private_key_path = lua_tostring(lua_vm, 4);

	std::string public_key = public_key_path, private_key;
	if (lua_type(lua_vm, 4) != LUA_TNONE)
	{
		std::string pub_path;
		if (!Crypto::read_key_pair(public_key_path, private_key_path, &public_key, &private_key))
		{
			pModuleManager->ErrorPrintf("Bad argument @ jwtVerify\n");
			lua_pushboolean(lua_vm, false);
			return 1;
		}
	}

	try {
		const auto decoded_jwt = jwt::decode(token);
		jwt::verify()
			.allow_algorithm(jwt::algorithm::hs256(public_key))
			.verify(decoded_jwt);

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

	const auto token = lua_tostring(lua_vm, 1);

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
