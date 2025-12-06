#pragma once

#include <StormByte/exception.hxx>
#include <StormByte/expected.hxx>
#include <StormByte/visibility.h>

#include <algorithm>
#include <cstddef>
#include <cmath>
#include <iomanip>
#include <locale>
#include <queue>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte::String {
	/**
	 * @enum Format
	 * @brief String formatting options.
	 *
	 * Options for formatting strings, such as raw output or human-readable formats.
	 */
	enum class STORMBYTE_PUBLIC Format : unsigned short {
		Raw,                        ///< Raw output.
		HumanReadableNumber,        ///< Human-readable number format.
		HumanReadableBytes          ///< Human-readable bytes format.
	};

	/**
	 * @brief Creates a string with a given level of indentation.
	 * 
	 * This function generates a string consisting of tab characters (`\t`) repeated
	 * according to the specified indentation level.
	 * 
	 * @param level The level of indentation. If `0`, an empty string is returned.
	 * @return A string with the specified indentation.
	 */
	constexpr STORMBYTE_PUBLIC std::string Indent(const int& level) noexcept {
		return level == 0 ? std::string() : std::string(level, '\t');
	}

	/**
	 * @brief Checks if a string is numeric.
	 * 
	 * This function determines whether the given string contains only numeric characters.
	 * 
	 * @param str The string to check.
	 * @return `true` if the string is numeric, `false` otherwise.
	 */
	constexpr STORMBYTE_PUBLIC bool IsNumeric(const std::string& str) noexcept {
		return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
	}

	/**
	 * @brief Converts a string to lowercase.
	 * 
	 * This function converts all characters in the input string to their lowercase equivalents.
	 * 
	 * @param str The string to convert.
	 * @return The lowercase version of the string.
	 */
	STORMBYTE_PUBLIC std::string ToLower(const std::string& str) noexcept;

	/**
	 * @brief Converts a string to uppercase.
	 * 
	 * This function converts all characters in the input string to their uppercase equivalents.
	 * 
	 * @param str The string to convert.
	 * @return The uppercase version of the string.
	 */
	STORMBYTE_PUBLIC std::string ToUpper(const std::string& str) noexcept;

	/**
	 * @brief Splits a string into a queue of substrings based on a delimiter.
	 * 
	 * This function splits the input string into substrings using the specified delimiter
	 * and stores the substrings in a queue.
	 * 
	 * @param str The string to split.
	 * @param delimiter The delimiter character.
	 * @return A queue of substrings.
	 */
	STORMBYTE_PUBLIC std::queue<std::string> Explode(const std::string& str, const char delimiter);

	/**
	 * @brief Splits a string into a vector of substrings delimited by spaces.
	 * 
	 * This function splits the input string into substrings using spaces as the delimiter
	 * and stores the substrings in a vector.
	 * 
	 * @param str The string to split.
	 * @return A vector of substrings.
	 */
	STORMBYTE_PUBLIC std::vector<std::string> Split(const std::string& str);

	/**
	 * @brief Splits a fraction string into a pair of integers.
	 * 
	 * This function parses a fraction string (e.g., "3/4") and returns the numerator
	 * and denominator as a pair of integers.
	 * 
	 * @param fraction The fraction string to split (e.g., "3/4").
	 * @return An `Expected` object containing a pair of integers (numerator and denominator)
	 *         on success, or an `Exception` on failure.
	 */
	STORMBYTE_PUBLIC StormByte::Expected<std::pair<int, int>, Exception> SplitFraction(const std::string& fraction);

	/**
	 * @brief Splits a fraction string into a pair of integers and scales it to a desired denominator.
	 * 
	 * This function parses a fraction string (e.g., "3/4") and scales it to a specified denominator.
	 * If the scaling is not possible, an exception is returned.
	 * 
	 * @param str The fraction string to split (e.g., "3/4").
	 * @param denominator The desired denominator to scale the fraction to.
	 * @return An `Expected` object containing a pair of integers (numerator and denominator)
	 *         on success, or an `Exception` on failure.
	 */
	STORMBYTE_PUBLIC StormByte::Expected<std::pair<int, int>, Exception> SplitFraction(const std::string& str, const int& denominator);

	/**
	 * @brief Converts a number into a human-readable string format.
	 * 
	 * This template function formats a numeric value into a human-readable string based on the specified format.
	 * It supports arithmetic types (e.g., integers, floating-point numbers) and excludes `wchar_t`.
	 * 
	 * @tparam T The numeric type of the input value.
	 * @param number The number to format.
	 * @param format The desired format (e.g., raw, human-readable number, or human-readable bytes).
	 * @param locale The locale to use for formatting (default is "en_US.UTF-8").
	 * @return A human-readable string representation of the number.
	 */
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, wchar_t>>>
	STORMBYTE_PUBLIC std::string HumanReadable(const T& number, const Format& format, const std::string& locale = "en_US.UTF-8") noexcept;

	/**
	 * @brief Converts a wide string to a UTF-8 encoded string.
	 * 
	 * This function converts a wide string (e.g., `std::wstring`) into a UTF-8 encoded string.
	 * 
	 * @param ws The wide string to convert.
	 * @throws std::runtime_error If the conversion fails.
	 * @return The UTF-8 encoded string.
	 */
	STORMBYTE_PUBLIC std::string UTF8Encode(const std::wstring& ws);

	/**
	 * @brief Converts a UTF-8 encoded string to a wide string.
	 * 
	 * This function converts a UTF-8 encoded string (e.g., `std::string`) into a wide string.
	 * 
	 * @param s The UTF-8 encoded string to convert.
	 * @throws std::runtime_error If the conversion fails.
	 * @return The wide string.
	 */
	STORMBYTE_PUBLIC std::wstring UTF8Decode(const std::string& s);

	/**
	 * @brief Sanitizes newlines in a string.
	 * 
	 * This function replaces all newline characters (`\n` or `\r\n`) in the input string with a consistent format.
	 * 
	 * @param str The string to sanitize.
	 * @return The sanitized string with consistent newline formatting.
	 */
	STORMBYTE_PUBLIC std::string SanitizeNewlines(const std::string& str) noexcept;

	/**
	 * @brief Converts a byte vector to a string.
	 * 
	 * This function extracts the contents of a byte vector and converts it into a string.
	 * 
	 * @param byte_vector The byte vector to convert.
	 * @return The string representation of the byte vector's contents.
	 */
	STORMBYTE_PUBLIC std::string FromByteVector(const std::vector<std::byte>& byte_vector) noexcept;

	/**
	 * @brief Converts a string to a byte vector.
	 * 
	 * This function converts a string into a vector of bytes (`std::byte`).
	 * 
	 * @param str The string to convert.
	 * @return A vector of bytes representing the string's contents.
	 */
	STORMBYTE_PUBLIC std::vector<std::byte> ToByteVector(const std::string& str) noexcept;

	/**
	 * @brief Removes all whitespace characters from a string.
	 * 
	 * This function eliminates all whitespace characters from the input string.
	 * 
	 * @param str The string to process.
	 * @return The string with all whitespace characters removed.
	 */
	STORMBYTE_PUBLIC std::string RemoveWhitespace(const std::string& str) noexcept;
}
