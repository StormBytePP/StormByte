#pragma once

#include <StormByte/visibility.h>

#include <cstddef>		// std::byte
#include <span>
#include <string>
#include <vector>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 */
namespace StormByte {
	/**
	 * @brief Decodes a Base64-encoded string into raw bytes.
	 *
	 * The input may contain the standard Base64 alphabet (A-Z, a-z, 0-9, +, /)
	 * and the padding character '='. Whitespace is ignored.
	 *
	 * @param input Base64-encoded string.
	 * @return Decoded binary data as a vector of std::byte.
	 * @throws StormByte::Exception if the input contains invalid characters
	 *         or has incorrect padding.
	 *
	 * @note Accepts both std::string and const char* (via implicit conversion).
	 */
	std::vector<std::byte> STORMBYTE_PUBLIC Base64Decode(const std::string& input);

	/**
	 * @brief Encodes raw binary data into a Base64 string.
	 *
	 * Uses the standard Base64 alphabet and adds the required '=' padding.
	 *
	 * @param input Binary data to encode.
	 * @return Base64-encoded string.
	 */
	std::string STORMBYTE_PUBLIC Base64Encode(const std::vector<std::byte>& input);

	/**
	 * @brief Encodes raw binary data into a Base64 string (span overload).
	 *
	 * This overload avoids unnecessary copies when the data is already
	 * available as a contiguous range (e.g. std::array, C array, etc.).
	 *
	 * @param input Binary data to encode.
	 * @return Base64-encoded string.
	 */
	std::string STORMBYTE_PUBLIC Base64Encode(std::span<const std::byte> input);
}