#include "Crypto.h"
#include <fstream>
#include <jwt-cpp/jwt.h>

bool Crypto::read_key(const std::string& key_path, std::string* key)
{
	std::ifstream in(key_path);
	const std::string result{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};

	key->assign(result);
	return true;
}

bool Crypto::read_key_pair(const std::string& public_key_path, const std::string& private_key_path, std::string* public_key, std::string* private_key)
{
	return read_key(public_key_path, public_key) && read_key(private_key_path, private_key);
}
