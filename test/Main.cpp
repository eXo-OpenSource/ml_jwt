#include <iostream>
#include <chrono>
#include <jwt-cpp/jwt.h>
#include <cassert>
#include <fstream>

void read_key(const std::string& key_path, std::string& key)
{
	// Load file from filesystem
	std::ifstream in(key_path);
	if (!in.good())
	{
		throw std::runtime_error("could not load key");
	}

	const std::string result{std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>()};
	if (result.empty())
	{
		throw std::runtime_error("key was empty");
	}

	key.assign(result);
}

// POC for https://github.com/Thalhammer/jwt-cpp/issues/36
int main(int argc, char* argv[])
{
	std::string public_key;
	std::string private_key;

	read_key("C:/public.pem", public_key);
	read_key("C:/private.pem", private_key);


	try {
		const auto& now = std::chrono::system_clock::now();
		auto jwt = jwt::create()
			.set_issued_at(now)
			.set_not_before(now);

		const auto token = jwt.sign(jwt::algorithm::rs256{ public_key, private_key });
		std::cout << token << std::endl;

		const auto verifier = jwt::verify()
			.allow_algorithm(jwt::algorithm::rs256{ public_key });
		verifier.verify(jwt::decode(token));
		std::cout << "verified" << std::endl;
	} catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	std::cin.get();
	return 0;
}
