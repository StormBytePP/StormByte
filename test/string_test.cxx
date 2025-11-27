#include <StormByte/string.hxx>
#include <StormByte/system.hxx>
#include <StormByte/test_handlers.h>

using namespace StormByte::String;

int test_simple_explode() {
	int result = 0;
	try {
		std::string str = "Hello, World!";
		std::queue<std::string> parts = Explode(str, ',');
		ASSERT_EQUAL("test_simple_explode", 2, parts.size());
		ASSERT_EQUAL("test_simple_explode", "Hello", parts.front());
		parts.pop();
		ASSERT_EQUAL("test_simple_explode", " World!", parts.front());
		parts.pop();
		ASSERT_EQUAL("test_simple_explode", true, parts.empty());
	} catch (const StormByte::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_simple_explode", result);
}

int test_path_explode() {
	int result = 0;
	try {
		std::string str = "path/to/items";
		std::queue<std::string> parts = Explode(str, '/');
		ASSERT_EQUAL("test_path_explode", 3, parts.size());
		ASSERT_EQUAL("test_path_explode", "path", parts.front());
		parts.pop();
		ASSERT_EQUAL("test_path_explode", "to", parts.front());
		parts.pop();
		ASSERT_EQUAL("test_path_explode", "items", parts.front());
		parts.pop();
		ASSERT_EQUAL("test_path_explode", true, parts.empty());
	} catch (const StormByte::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_path_explode", result);
}

int test_explode_one_item() {
	int result = 0;
	try {
		std::string str = "Hello";
		std::queue<std::string> parts = Explode(str, '/');
		ASSERT_EQUAL("test_explode_one_item", 1, parts.size());
		ASSERT_EQUAL("test_explode_one_item", "Hello", parts.front());
		parts.pop();
		ASSERT_EQUAL("test_explode_one_item", true, parts.empty());
	} catch (const StormByte::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_explode_one_item", result);
}

int test_temp_path() {
	int result = 0;
	try {
		const std::filesystem::path path = StormByte::System::TempFileName("something");
		const bool exists = std::filesystem::exists(path);
		std::remove(path.string().c_str());
		ASSERT_TRUE("test_temp_path", exists);
	} catch (const StormByte::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_temp_path", result);
}

int test_human_readable_byte_size() {
    int result = 0;
    const std::string locale = "en_US.UTF-8"; // Can't be a constexpr or gcc complains
    try {
        // Explicitly specify the type of T
        std::string size = HumanReadable<uint64_t>(1024, Format::HumanReadableBytes, locale);
        ASSERT_EQUAL("test_human_readable_byte_size", "1 KiB", size);

        size = HumanReadable<uint64_t>(1024ULL * 1024, Format::HumanReadableBytes, locale);
        ASSERT_EQUAL("test_human_readable_byte_size", "1 MiB", size);

        size = HumanReadable<uint64_t>(1024ULL * 1024 * 1024, Format::HumanReadableBytes, locale);
        ASSERT_EQUAL("test_human_readable_byte_size", "1 GiB", size);

        size = HumanReadable<uint64_t>(1024ULL * 1024 * 1024 * 1024, Format::HumanReadableBytes, locale);
        ASSERT_EQUAL("test_human_readable_byte_size", "1 TiB", size);

        size = HumanReadable<uint64_t>(1024ULL * 1024 * 1024 * 1024 * 1024, Format::HumanReadableBytes, locale);
        ASSERT_EQUAL("test_human_readable_byte_size", "1 PiB", size);

		size = HumanReadable<double>(1027.65, Format::HumanReadableBytes, locale);
		ASSERT_EQUAL("test_human_readable_byte_size", "1 KiB", size);

		size = HumanReadable<double>(1154.65, Format::HumanReadableBytes, locale);
		ASSERT_EQUAL("test_human_readable_byte_size", "1.13 KiB", size);
    } catch (const StormByte::Exception& ex) {
        std::cerr << ex.what() << std::endl;
        result++;
    }
    RETURN_TEST("test_human_readable_byte_size", result);
}

int test_human_readable_number() {
	int result = 0;
	try {
		std::string number = HumanReadable<int>(1024, Format::HumanReadableNumber, "en_US.UTF-8");
		ASSERT_EQUAL("test_human_readable_number", "1,024", number);

		number = HumanReadable<int>(1024 * 1024, Format::HumanReadableNumber, "en_US.UTF-8");
		ASSERT_EQUAL("test_human_readable_number", "1,048,576", number);
	} catch (const StormByte::Exception& ex) {
		std::cerr << ex.what() << std::endl;
		result++;
	}
	RETURN_TEST("test_human_readable_number", result);
}

int test_buffer_to_string() {
	const std::string test_string = "test_buffer_to_string";
	std::vector<std::byte> buffer = StormByte::String::ToByteVector(test_string);
	std::string str = StormByte::String::FromByteVector(buffer);
	ASSERT_EQUAL("test_buffer_to_string", test_string, str);
	RETURN_TEST("test_buffer_to_string", 0);
}

int main() {
    int result = 0;
    try {
		result += test_simple_explode();
		result += test_path_explode();
		result += test_explode_one_item();
		result += test_temp_path();
		result += test_human_readable_byte_size();
		result += test_human_readable_number();
		result += test_buffer_to_string();

    } catch (const StormByte::Exception& ex) {
        std::cerr << ex.what() << std::endl;
        result++;
    }
    if (result == 0) {
        std::cout << "All tests passed!" << std::endl;
    } else {
        std::cout << result << " tests failed." << std::endl;
    }
    return result;
}
