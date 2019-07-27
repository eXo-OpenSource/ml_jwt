#include <jwt-cpp/jwt.h>
#include <sstream>

#include "Module.h"
#include "CFunctions.h"
#include "Utils.h"
#include "Crypto.h"
	
#ifndef _WIN32
	#include <sys/stat.h>
#endif

int CFunctions::sign_jwt_token(lua_State* lua_vm)
{
	// bool jwtSign(function(string/boolean) callback, table claims, string algorithm, string secret/private_key_path, bool is_file_path)
	if (lua_type(lua_vm, 1) != LUA_TFUNCTION || lua_type(lua_vm, 2) != LUA_TTABLE || lua_type(lua_vm, 3) != LUA_TLIGHTUSERDATA || lua_type(lua_vm, 4) != LUA_TSTRING ||
		(lua_type(lua_vm, 5) != LUA_TNONE && lua_type(lua_vm, 5) != LUA_TBOOLEAN))
	{
		pModuleManager->ErrorPrintf("Bad argument @ jwtSign\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	try {
		// Save reference of the Lua callback function
		// See: http://lua-users.org/lists/lua-l/2008-12/msg00193.html
		lua_pushvalue(lua_vm, 1);
		const auto func_ref         = luaL_ref(lua_vm, LUA_REGISTRYINDEX);

		// Read other arguments
		const auto claims           = Utils::parse_named_table(lua_vm, 2);
		const auto algorithm        = jwt_algorithm(reinterpret_cast<uint64_t>(lua_touserdata(lua_vm, 3)));
		const auto private_key_path = lua_tostring(lua_vm, 4);
		std::string private_key     = private_key_path;
		
		// Read public- and private key from files
		const auto is_file_path     = lua_type(lua_vm, 5) != LUA_TNONE && lua_toboolean(lua_vm, 5);
		if (is_file_path)
		{
			Crypto::read_key(lua_vm, private_key_path, private_key);
			DEBUG_LOG(private_key.c_str());
		}

		// Process signing
		g_Module->GetJobManager().PushTask([/* lua_vm, */ claims, algorithm, private_key]() -> const std::optional<std::any>
		{
			try {
				const auto& now = std::chrono::system_clock::now();
				auto jwt = jwt::create()
					.set_issued_at(now)
					.set_not_before(now);

				// Add claims
				for (const auto& [id, claim] : claims)
				{
					jwt.set_payload_claim(id, claim);
				}

				switch(algorithm)
				{
				case jwt_algorithm_hs256:
					return jwt.sign(jwt::algorithm::hs256{ private_key });
				case jwt_algorithm_hs384:
					return jwt.sign(jwt::algorithm::hs384{ private_key });
				case jwt_algorithm_hs512:
					return jwt.sign(jwt::algorithm::hs512{ private_key });
				case jwt_algorithm_rs256:
					return jwt.sign(jwt::algorithm::rs256{ std::string(), private_key });
				case jwt_algorithm_rs384:
					return jwt.sign(jwt::algorithm::rs384{ std::string(), private_key });
				case jwt_algorithm_rs512:
					return jwt.sign(jwt::algorithm::rs512{ std::string(), private_key });
				case jwt_algorithm_none:
				default: 
					pModuleManager->ErrorPrintf("Error @ jwtSign, invalid algorithm has been passed.\n");
					return {};
				}
				
			} catch(exception& e)
			{
				std::stringstream ss;
				ss << "Bad Argument @ jwtSign, " << e.what() << ".\n";
				pModuleManager->ErrorPrintf(ss.str().c_str());

				return {};
			}
		}, [lua_vm = lua_getmainstate(lua_vm), func_ref](const std::optional<std::any>& result)
		{
			// Validate LuaVM (use ResourceStart/-Stop to manage valid lua states)
			if (!g_Module->HasLuaVM(lua_vm))
				return;

			// Push stored reference to callback function to the stack
			lua_rawgeti(lua_vm, LUA_REGISTRYINDEX, func_ref);

			// Push token
			try {
				if (result.has_value()) {
					const auto token = std::any_cast<std::string>(result.value()); // might throw bad_any_cast
					lua_pushstring(lua_vm, token.c_str());
				} else
				{
					lua_pushboolean(lua_vm, false);
				}
			} catch (exception&)
			{
				lua_pushboolean(lua_vm, false);
			}

			// Finally, call the function
			const auto err = lua_pcall(lua_vm, 1, 0, 0);
			if (err != 0)
				pModuleManager->ErrorPrintf("%s\n", lua_tostring(lua_vm, -1));
		});

		lua_pushboolean(lua_vm, true);
		return 1;
	} catch (runtime_error& e)
	{
		std::stringstream ss;
		ss << "Bad Argument @ jwtSign, " << e.what() << ".\n";
		pModuleManager->ErrorPrintf(ss.str().c_str());
		lua_pushboolean(lua_vm, false);
		return 1;
	}
}

int CFunctions::verify_jwt_token(lua_State* lua_vm)
{
	// bool jwtVerify(string token, string secret/public_key_path, bool is_file_path = false)
	if (lua_type(lua_vm, 1) != LUA_TFUNCTION || lua_type(lua_vm, 2) != LUA_TSTRING || lua_type(lua_vm, 3) != LUA_TSTRING ||
		(lua_type(lua_vm, 4) != LUA_TNONE && lua_type(lua_vm, 4) != LUA_TBOOLEAN))
	{
		pModuleManager->ErrorPrintf("Bad argument @ jwtVerify\n");
		lua_pushboolean(lua_vm, false);
		return 1;
	}

	try {
		// Save reference of the Lua callback function
		// See: http://lua-users.org/lists/lua-l/2008-12/msg00193.html
		lua_pushvalue(lua_vm, 1);
		const auto func_ref         = luaL_ref(lua_vm, LUA_REGISTRYINDEX);

		// Read other arguments
		const auto token            = lua_tostring(lua_vm, 2);
		const auto public_key_path  = lua_tostring(lua_vm, 3);
		std::string public_key      = public_key_path;

		// Read public- and private key from files
		const auto is_file_path     = lua_type(lua_vm, 4) != LUA_TNONE && lua_toboolean(lua_vm, 4);
		if (is_file_path)
		{
			Crypto::read_key(lua_vm, public_key_path, public_key);
			//DEBUG_LOG(public_key.c_str());
		}

		g_Module->GetJobManager().PushTask([/* lua_vm, */ token, public_key]() -> const std::optional<std::any>
		{
			// Process verification
			try {
				const auto decoded_jwt = jwt::decode(token);
				const auto algorithm = decoded_jwt.get_algorithm().c_str();
				auto verifier = jwt::verify();

				// set verifier algorithm
				if (std::strcmp(algorithm, "HS256") == 0)
					verifier.allow_algorithm(jwt::algorithm::hs256{ public_key });
				if (std::strcmp(algorithm, "HS384") == 0)
					verifier.allow_algorithm(jwt::algorithm::hs384{ public_key });
				if (std::strcmp(algorithm, "HS512") == 0)
					verifier.allow_algorithm(jwt::algorithm::hs512{ public_key });
				if (std::strcmp(algorithm, "RS256") == 0)
					verifier.allow_algorithm(jwt::algorithm::rs256{ public_key });
				if (std::strcmp(algorithm, "RS384") == 0)
					verifier.allow_algorithm(jwt::algorithm::rs384{ public_key });
				if (std::strcmp(algorithm, "RS512") == 0)
					verifier.allow_algorithm(jwt::algorithm::rs512{ public_key });

				verifier.verify(decoded_jwt);

				return true;
			} catch (exception& e)
			{
				std::stringstream ss;
				ss << "Bad Argument @ jwtVerify, " << e.what() << ".\n";
				pModuleManager->ErrorPrintf(ss.str().c_str());

				return false;
			}
		}, [lua_vm = lua_getmainstate(lua_vm), func_ref](const std::optional<std::any>& result)
		{
			// Validate LuaVM (use ResourceStart/-Stop to manage valid lua states)
			if (!g_Module->HasLuaVM(lua_vm))
				return;

			// Push stored reference to callback function to the stack
			lua_rawgeti(lua_vm, LUA_REGISTRYINDEX, func_ref);

			// Push token
			try {
				lua_pushboolean(lua_vm, result.has_value() && std::any_cast<bool>(result.value())); // might throw bad_any_cast
			} catch (exception&)
			{
				lua_pushboolean(lua_vm, false);
			}

			// Finally, call the function
			const auto err = lua_pcall(lua_vm, 1, 0, 0);
			if (err != 0)
				pModuleManager->ErrorPrintf("%s\n", lua_tostring(lua_vm, -1));
		});

		lua_pushboolean(lua_vm, true);
		return 1;
	} catch(runtime_error& e)
	{
		std::stringstream ss;
		ss << "Bad Argument @ jwtVerify, " << e.what() << ".\n";
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
		ss << "Bad Argument @ jwtGetClaims, " << e.what() << ".\n";
		pModuleManager->ErrorPrintf(ss.str().c_str());

		lua_pushboolean(lua_vm, false);
		return 1;
	}
}
