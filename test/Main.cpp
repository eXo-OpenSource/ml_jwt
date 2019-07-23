#include <iostream>
#include <chrono>
#include <jwt-cpp/jwt.h>
#include <cassert>

int main(int argc, char* argv[])
{
	const auto decoded_jwt = jwt::decode("eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ.XbPfbIHMI6arZ3Y922BhjWgQzWXcXNrz0ogtVhfEd2o");

	auto begin = std::chrono::high_resolution_clock::now();
	std::cout << "Testing JWT verify, wrong secret" << std::endl;
	//for (auto i = 0; i < 1000; i++) {
		try
		{
			const auto verifier = jwt::verify()
				.allow_algorithm(jwt::algorithm::hs256{ "a" });

			verifier.verify(decoded_jwt);

			std::cout << "\tNo Exception thrown." << std::endl;
		} catch (std::exception& e)
		{
			std::cout << "\tException caught: " << e.what() << std::endl;
		}
	//}
	auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin).count(); // / 1000;
	std::cout << "Took: " << time << "ns." << std::endl;

	begin = std::chrono::high_resolution_clock::now();
	std::cout << "Testing JWT verify, correct secret" << std::endl;
	try
	{
		const auto verifier = jwt::verify()
			.allow_algorithm(jwt::algorithm::hs256{ "secret" });

		verifier.verify(decoded_jwt);

		std::cout << "\tNo Exception thrown." << std::endl;
	} catch (std::exception& e)
	{
		std::cout << "\tException caught: " << e.what() << std::endl;
	}
	time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin).count();
	std::cout << "Took: " << time << "ns." << std::endl;

	begin = std::chrono::high_resolution_clock::now();
	std::cout << "Reading JWT claims" << std::endl;
	try
	{
		for(auto& e : decoded_jwt.get_payload_claims()) {
			std::cout << "\t" << e.first << " = " << e.second.to_json() << std::endl;
		}
		
		std::cout << "\tNo Exception thrown." << std::endl;
	} catch (std::exception& e)
	{
		std::cout << "\tException caught: " << e.what() << std::endl;
	}
	time = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - begin).count();
	std::cout << "Took: " << time << "ns." << std::endl;

	std::cin.get();
	return 0;
}
