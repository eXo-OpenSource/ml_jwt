#pragma once
#include <string>

class Crypto
{
public:
	static bool read_key(const std::string& key_path, std::string* key);
	static bool read_key_pair(const std::string& public_key_path, const std::string& private_key_path,
	                          std::string* public_key, std::string* private_key);

	template<typename T>
	static T get_algorithm(const std::string& algorithm, const std::string& public_key, const std::string& private_key);
};

