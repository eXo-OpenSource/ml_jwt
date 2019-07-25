#include <fstream>
#include <jwt-cpp/jwt.h>

#include "Crypto.h"
#include "Utils.h"

void Crypto::read_key(lua_State* lua_vm, const std::string& key_path, std::string& key)
{
	// Get real path
	std::string real_path;
	Utils::get_real_path(lua_vm, key_path, real_path);

	// Load file from filesystem
	std::ifstream in(real_path);
	if (!in.good())
	{
		throw runtime_error("could not load key");
	}

	const std::string result{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
	if (result.empty())
	{
		throw runtime_error("key was empty");
	}

	key.assign(result);
}

void Crypto::read_key_pair(lua_State* lua_vm, const std::string& public_key_path, std::string& public_key, const std::string& private_key_path, std::string& private_key)
{
	read_key(lua_vm, public_key_path, public_key);
	read_key(lua_vm, private_key_path, private_key);
}
