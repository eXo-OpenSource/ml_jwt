#include "Crypto.h"
#include <fstream>
#include <jwt-cpp/jwt.h>
#include "Utils.h"

bool Crypto::read_key(lua_State* lua_vm, const std::string& key_path, std::string& key)
{
	// Get real path
	std::string formatted_path;
	if (!Utils::format_path(lua_vm, key_path, formatted_path))
	{
		return false;
	}

	// Load file from filesystem
	std::ifstream in(formatted_path);
	if (!in.good())
	{
		return false;
	}

	const std::string result{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
	if (result.empty())
	{
		return false;
	}

	key.assign(result);
	return true;
}

bool Crypto::read_key_pair(lua_State* lua_vm, const std::string& public_key_path, std::string& public_key, const std::string& private_key_path, std::string& private_key)
{
	return read_key(lua_vm, public_key_path, public_key) && read_key(lua_vm, private_key_path, private_key);
}
