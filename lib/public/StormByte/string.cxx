/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3
 * or later, as published by the Free Software Foundation.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with StormByte. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 */

#include <StormByte/string.hxx>
#include <cstring>
#include <cwchar>
#include <cstdlib>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <regex>
#include <stdexcept>
#include <cstdint>
#ifdef WINDOWS
#include <cwchar>
#include <direct.h> // For _getcwd
#include <windows.h> // For MAX_PATH
#endif
namespace {
	[[noreturn]] void ThrowInvalidUnicode() {
		throw StormByte::UTF8Error("Invalid Unicode input");
	}

	void AppendUTF8(std::string& result, const uint32_t codepoint) {
		if (codepoint <= 0x7F) {
			result.push_back(static_cast<char>(codepoint));
		} else if (codepoint <= 0x7FF) {
			result.push_back(static_cast<char>(0xC0 | (codepoint >> 6)));
			result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
		} else if (codepoint <= 0xFFFF) {
			result.push_back(static_cast<char>(0xE0 | (codepoint >> 12)));
			result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
			result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
		} else if (codepoint <= 0x10FFFF) {
			result.push_back(static_cast<char>(0xF0 | (codepoint >> 18)));
			result.push_back(static_cast<char>(0x80 | ((codepoint >> 12) & 0x3F)));
			result.push_back(static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F)));
			result.push_back(static_cast<char>(0x80 | (codepoint & 0x3F)));
		} else {
			ThrowInvalidUnicode();
		}
	}

	uint32_t DecodeUTF8Codepoint(const std::string& input, std::size_t& index) {
		const auto first = static_cast<unsigned char>(input[index]);
		std::size_t length = 0;
		uint32_t codepoint = 0;
		uint32_t minimum = 0;
		if (first <= 0x7F) {
			++index;
			return first;
		} else if (first >= 0xC2 && first <= 0xDF) {
			length = 2;
			codepoint = first & 0x1F;
			minimum = 0x80;
		} else if (first >= 0xE0 && first <= 0xEF) {
			length = 3;
			codepoint = first & 0x0F;
			minimum = 0x800;
		} else if (first >= 0xF0 && first <= 0xF4) {
			length = 4;
			codepoint = first & 0x07;
			minimum = 0x10000;
		} else {
			ThrowInvalidUnicode();
		}

		if (index + length > input.size()) ThrowInvalidUnicode();
		for (std::size_t offset = 1; offset < length; ++offset) {
			const auto continuation = static_cast<unsigned char>(input[index + offset]);
			if ((continuation & 0xC0) != 0x80) ThrowInvalidUnicode();
			codepoint = (codepoint << 6) | (continuation & 0x3F);
		}

		if (codepoint < minimum || codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF)) {
			ThrowInvalidUnicode();
		}

		index += length;
		return codepoint;
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	std::string HumanReadableByteSize(const T& bytes, const std::string& locale) noexcept {
		try {
			constexpr uint64_t KB = 1024;
			constexpr uint64_t MB = KB * 1024;
			constexpr uint64_t GB = MB * 1024;
			constexpr uint64_t TB = GB * 1024;
			constexpr uint64_t PB = TB * 1024;
			const long double signed_bytes = static_cast<long double>(bytes);
			const bool negative = signed_bytes < 0;
			const long double magnitude = negative ? -signed_bytes : signed_bytes;
			double value = static_cast<double>(magnitude);
			std::string suffix = "Bytes";
			if (magnitude >= PB) {
				value /= PB;
				suffix = "PiB";
			} else if (magnitude >= TB) {
				value /= TB;
				suffix = "TiB";
			} else if (magnitude >= GB) {
				value /= GB;
				suffix = "GiB";
			} else if (magnitude >= MB) {
				value /= MB;
				suffix = "MiB";
			} else if (magnitude >= KB) {
				value /= KB;
				suffix = "KiB";
			}

			std::ostringstream oss;
			try {
				oss.imbue(std::locale(locale));
			} catch (...) {
				oss.imbue(std::locale("C"));
			}

			if (std::fabs(value - std::round(value)) < 0.01) {
				oss << static_cast<int64_t>(std::round(value));
			} else if (value < 0.01) {
				oss << "0";
			} else {
				oss << std::fixed << std::setprecision(2) << value;
			}

			return (negative ? "-" : "") + oss.str() + " " + suffix;
		} catch (...) {
			return std::to_string(bytes) + " Bytes";
		}
	}

	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T> && !std::is_same_v<T, wchar_t>>>
	std::string HumanReadableNumber(const T& number, const std::string& locale) noexcept {
		try {
			std::ostringstream oss;
			try {
				oss.imbue(std::locale(locale));
			} catch (...) {
				oss.imbue(std::locale("C"));
			}

			if constexpr (std::is_integral_v<T>) {
				oss << number;
			} else if constexpr (std::is_floating_point_v<T>) {
				if (std::fmod(number, 1.0) == 0.0) {
					oss << static_cast<int64_t>(number);
				} else {
					oss << std::fixed << std::setprecision(2) << number;
				}
			}

			return oss.str();
		} catch (...) {
			return std::to_string(number);
		}
	}
}

namespace StormByte::String {
	std::queue<std::string> Explode(const std::string& str, const char delimiter) {
		std::queue<std::string> result;
		// Use ranges to split the string by the delimiter and iterate over parts
		for (auto part : std::string_view(str) | std::views::split(delimiter)) {
			// Convert each part into a std::string and push it to the queue
			result.emplace(part.begin(), part.end());
		}

		return result;
	}

	std::vector<std::string> Split(const std::string& str) {
		std::istringstream iss(str);
		std::vector<std::string> result;
		std::string word;
		while (iss >> word) {
			result.push_back(word); // Insert each word into the vector
		}

		return result;
	}

	std::string ToLower(const std::string& str) noexcept {
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
			return static_cast<char>(std::tolower(c));
		});
		return result;
	}

	std::string ToUpper(const std::string& str) noexcept {
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
			return static_cast<char>(std::toupper(c));
		});
		return result;
	}

	template <typename T, typename>
	std::string HumanReadable(const T& number, const Format& format, const std::string& locale) noexcept {
		switch (format) {
		case Format::Raw:
			return std::to_string(number);
		case Format::HumanReadableNumber:
			return HumanReadableNumber(number, locale);
		case Format::HumanReadableBytes:
			return HumanReadableByteSize(number, locale);
		default:
			return std::to_string(number);
		}
	}

	std::string UTF8Encode(const std::wstring& wstr) {
		std::string result;
		result.reserve(wstr.size());
		for (std::size_t index = 0; index < wstr.size(); ++index) {
			uint32_t codepoint = static_cast<uint32_t>(wstr[index]);
			if constexpr (sizeof(wchar_t) == 2) {
				if (codepoint >= 0xD800 && codepoint <= 0xDBFF) {
					if (index + 1 >= wstr.size()) ThrowInvalidUnicode();
					const uint32_t low = static_cast<uint32_t>(wstr[++index]);
					if (low < 0xDC00 || low > 0xDFFF) ThrowInvalidUnicode();
					codepoint = 0x10000 + ((codepoint - 0xD800) << 10) + (low - 0xDC00);
				} else if (codepoint >= 0xDC00 && codepoint <= 0xDFFF) {
					ThrowInvalidUnicode();
				}
			} else if constexpr (sizeof(wchar_t) == 4) {
				if (codepoint > 0x10FFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF)) ThrowInvalidUnicode();
			} else {
				static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4, "Unsupported wchar_t size");
			}

			AppendUTF8(result, codepoint);
		}

		return result;
	}

	std::wstring UTF8Decode(const std::string& str) {
		std::wstring result;
		result.reserve(str.size());
		for (std::size_t index = 0; index < str.size();) {
			const uint32_t codepoint = DecodeUTF8Codepoint(str, index);
			if constexpr (sizeof(wchar_t) == 2) {
				if (codepoint <= 0xFFFF) {
					result.push_back(static_cast<wchar_t>(codepoint));
				} else {
					result.push_back(static_cast<wchar_t>(0xD800 + ((codepoint - 0x10000) >> 10)));
					result.push_back(static_cast<wchar_t>(0xDC00 + ((codepoint - 0x10000) & 0x3FF)));
				}
			} else if constexpr (sizeof(wchar_t) == 4) {
				result.push_back(static_cast<wchar_t>(codepoint));
			} else {
				static_assert(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4, "Unsupported wchar_t size");
			}
		}

		return result;
	}

	std::string SanitizeNewlines(const std::string& str) noexcept {
		std::string result = str;
		return std::regex_replace(str, std::regex("\r\n"), "\n");
	}

	std::string FromByteVector(const std::vector<std::byte>& byte_vector) noexcept {
		auto span = std::span<const std::byte>(byte_vector.data(), byte_vector.size());
		return std::string(reinterpret_cast<const char*>(span.data()), span.size());
	}

	std::vector<std::byte> ToByteVector(const std::string& str) noexcept {
		std::vector<std::byte> byte_vector(str.size());
		std::memcpy(byte_vector.data(), str.data(), str.size());
		return byte_vector;
	}

	std::string RemoveWhitespace(const std::string& str) noexcept {
		std::string out;
		out.reserve(str.size());
		for (char c : str) if (!isspace(static_cast<unsigned char>(c))) out.push_back(c);
		return out;
	}

	bool IsInteger(const std::string& str) noexcept {
		if (str.empty()) return false;
		size_t start = 0;
		if (str[0] == '-' || str[0] == '+') {
			if (str.size() == 1) return false; // Only sign, no digits
			start = 1;
		}

		for (size_t i = start; i < str.size(); ++i) {
			if (!std::isdigit(static_cast<unsigned char>(str[i]))) {
				return false;
			}
		}

		return true;
	}

	// Explicit instantiations for `HumanReadable` (ordered by category).
	// Note: `wchar_t`, `char16_t`, and `char32_t` are excluded because they are not
	// streamed to `std::ostringstream` on many standard library implementations.
	// Boolean
	template STORMBYTE_PUBLIC std::string HumanReadable<bool>(const bool&, const Format&, const std::string&) noexcept;
	// Character types
	template STORMBYTE_PUBLIC std::string HumanReadable<char>(const char&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<signed char>(const signed char&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<unsigned char>(const unsigned char&, const Format&, const std::string&) noexcept;
	// Short
	template STORMBYTE_PUBLIC std::string HumanReadable<short>(const short&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<unsigned short>(const unsigned short&, const Format&, const std::string&) noexcept;
	// Integer
	template STORMBYTE_PUBLIC std::string HumanReadable<int>(const int&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<unsigned int>(const unsigned int&, const Format&, const std::string&) noexcept;
	// Long
	template STORMBYTE_PUBLIC std::string HumanReadable<long>(const long&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<unsigned long>(const unsigned long&, const Format&, const std::string&) noexcept;
	// Long long
	template STORMBYTE_PUBLIC std::string HumanReadable<long long>(const long long&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<unsigned long long>(const unsigned long long&, const Format&, const std::string&) noexcept;
	// Floating point
	template STORMBYTE_PUBLIC std::string HumanReadable<float>(const float&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<double>(const double&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<long double>(const long double&, const Format&, const std::string&) noexcept;
}
