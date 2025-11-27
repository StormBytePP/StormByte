#include <StormByte/serializable.hxx>
#include <StormByte/test_handlers.h>

#include <format>
#include <iomanip>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

namespace {
	std::string HexData(const std::vector<std::byte>& buffer) {
		std::ostringstream oss;
		for (size_t i = 0; i < buffer.size(); ++i) {
			if (i > 0 && i % 16 == 0) oss << "\n";
			oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(buffer[i]) << " ";
		}
		return oss.str();
	}

	void AppendVector(std::vector<std::byte>& dest, const std::vector<std::byte>& src) {
		dest.insert(dest.end(), src.begin(), src.end());
	}
}

int github_clang_libstdcpp_bus_error() {
	std::vector<std::byte> buffer;

	{
		std::string str = "Hello World!";
		StormByte::Serializable<std::string> serialization(str);
		AppendVector(buffer, serialization.Serialize());
	}

	std::cout << "Buffer: " << std::endl << HexData(buffer) << std::endl;
	return 0;
}

int github_clang_libstdcpp_bus_error2() {
	std::vector<std::byte> buffer1, buffer2;

	{
		std::string str = "Hello World!";
		StormByte::Serializable<std::string> serialization(str);
		AppendVector(buffer2, serialization.Serialize());
		buffer1 = buffer2;
	}

	std::cout << "Buffer1: " << std::endl << HexData(buffer1) << std::endl;
	return 0;
}

int github_clang_libstdcpp_bus_error3() {
	std::vector<std::byte> buffer1, buffer2;

	{
		std::string str = "Hello World!";
		StormByte::Serializable<std::string> serialization(str);
		buffer2 = std::move(serialization.Serialize());
		AppendVector(buffer1, std::move(buffer2));
	}

	std::cout << "Buffer1: " << std::endl << HexData(buffer1) << std::endl;
	return 0;
}

int main() {
	int result = 0;

	result += github_clang_libstdcpp_bus_error();
	result += github_clang_libstdcpp_bus_error2();
	result += github_clang_libstdcpp_bus_error3();

	if (result == 0) {
		std::cout << "All tests passed!" << std::endl;
	} else {
		std::cout << result << " tests failed." << std::endl;
	}
	return result;
}
