#pragma once
#include <string>

#include "CFunctions.h"

class Crypto
{
public:
	static void read_key(lua_State* lua_vm, const std::string& key_path, std::string& key);
	static void read_key_pair(lua_State* lua_vm, const std::string& public_key_path, std::string& public_key, 
		const std::string& private_key_path, std::string& private_key);
};

