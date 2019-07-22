#pragma once
#include <string>

class Crypto
{
public:
	static bool read_key_pair(const std::string& public_key_path, const std::string& private_key_path,
	                          std::string* public_key, std::string* private_key);
};

