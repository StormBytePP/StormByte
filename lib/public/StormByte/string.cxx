#include <StormByte/string.hxx>

#include <cwchar>
#include <cstdlib>
#include <ranges>
#include <sstream>
#include <string>
#include <vector>
#include <queue>
#include <regex>

#ifdef WINDOWS
#include <cwchar>
#include <direct.h> // For _getcwd
#include <windows.h> // For MAX_PATH
#endif

namespace {
	template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
	std::string HumanReadableByteSize(const T& bytes, const std::string& locale) noexcept {
		try {
			constexpr uint64_t KB = 1024;
			constexpr uint64_t MB = KB * 1024;
			constexpr uint64_t GB = MB * 1024;
			constexpr uint64_t TB = GB * 1024;
			constexpr uint64_t PB = TB * 1024;

			uint64_t unsigned_bytes = static_cast<uint64_t>(bytes); // Cast to uint64_t

			double value = static_cast<double>(bytes);
			std::string suffix = "Bytes";

			if (unsigned_bytes >= PB) {
				value /= PB;
				suffix = "PiB";
			} else if (unsigned_bytes >= TB) {
				value /= TB;
				suffix = "TiB";
			} else if (unsigned_bytes >= GB) {
				value /= GB;
				suffix = "GiB";
			} else if (unsigned_bytes >= MB) {
				value /= MB;
				suffix = "MiB";
			} else if (unsigned_bytes >= KB) {
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

			return oss.str() + " " + suffix;

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

	StormByte::Expected<std::pair<int, int>, StormByte::Exception> SplitFraction(const std::string& fraction) {
		std::size_t slashPos = fraction.find('/');
		if (slashPos == std::string::npos) // MSVC needs explicit string namespace
			return StormByte::Unexpected<Exception>("Invalid fraction format: '/' not found.");
	
		const std::string numeratorStr = fraction.substr(0, slashPos);
		const std::string denominatorStr = fraction.substr(slashPos + 1);

		if (!IsNumeric(numeratorStr) || !IsNumeric(denominatorStr))
			return StormByte::Unexpected<Exception>("Invalid fraction format: numerator ({}) and denominator ({}) must be numeric.", numeratorStr, denominatorStr);

		if (denominatorStr == "0")
			return StormByte::Unexpected<Exception>("Invalid fraction format: denominator cannot be zero.");

		return std::make_pair<int, int>(std::stoi(numeratorStr), std::stoi(denominatorStr));
	}

	StormByte::Expected<std::pair<int, int>, StormByte::Exception> SplitFraction(const std::string& fraction, const int& desired_denominator) {
		auto expected_fraction = SplitFraction(fraction);
		if (!expected_fraction)
			return StormByte::Unexpected(expected_fraction.error());

		auto [numerator, denominator] = expected_fraction.value();
		if (denominator == desired_denominator)
			return std::make_pair(numerator, denominator);
		else if (desired_denominator == 0)
			return StormByte::Unexpected<Exception>("Invalid desired denominator: cannot be zero.");
		else {
			const double factor = static_cast<double>(desired_denominator) / static_cast<double>(denominator);
			return std::make_pair(static_cast<int>(numerator * factor), desired_denominator);
		}
	}

	std::string ToLower(const std::string& str) noexcept {
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(), ::tolower);
		return result;
	}

	std::string ToUpper(const std::string& str) noexcept {
		std::string result = str;
		std::transform(result.begin(), result.end(), result.begin(), ::toupper);
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
		if (wstr.empty()) return {};

		std::mbstate_t state = std::mbstate_t();
		const wchar_t* src = wstr.data();
		size_t len;

	#ifdef WINDOWS
		// Use safer alternative on Windows
		errno_t err = wcsrtombs_s(&len, nullptr, 0, &src, 0, &state);
		if (err != 0) {
			throw std::runtime_error("Wide to multibyte conversion failed");
		}
	#else
		// Use standard wcsrtombs for other platforms
		len = std::wcsrtombs(nullptr, &src, 0, &state);
		if (len == static_cast<size_t>(-1)) {
			throw std::runtime_error("Wide to multibyte conversion failed");
		}
	#endif

		std::string result(len, '\0');
		src = wstr.data(); // Reset src pointer

	#ifdef WINDOWS
		// Perform conversion with wcsrtombs_s
		err = wcsrtombs_s(&len, result.data(), result.size() + 1, &src, result.size(), &state);
		if (err != 0) {
			throw std::runtime_error("Wide to multibyte conversion failed");
		}
	#else
		// Perform conversion with wcsrtombs
		std::wcsrtombs(result.data(), &src, len, &state);
	#endif

		return result;
	}

	std::wstring UTF8Decode(const std::string& str) {
		if (str.empty()) return {};

		std::mbstate_t state = std::mbstate_t();
		const char* src = str.data();
		size_t len;

	#ifdef WINDOWS
		// Use safer alternative on Windows
		errno_t err = mbsrtowcs_s(&len, nullptr, 0, &src, 0, &state);
		if (err != 0) {
			throw std::runtime_error("Multibyte to wide conversion failed");
		}
	#else
		// Use standard mbsrtowcs for other platforms
		len = std::mbsrtowcs(nullptr, &src, 0, &state);
		if (len == static_cast<size_t>(-1)) {
			throw std::runtime_error("Multibyte to wide conversion failed");
		}
	#endif

		std::wstring result(len, L'\0');
		src = str.data(); // Reset src pointer

	#ifdef WINDOWS
		// Perform conversion with mbsrtowcs_s
		err = mbsrtowcs_s(&len, result.data(), result.size() + 1, &src, result.size(), &state);
		if (err != 0) {
			throw std::runtime_error("Multibyte to wide conversion failed");
		}
	#else
		// Perform conversion with mbsrtowcs
		std::mbsrtowcs(result.data(), &src, len, &state);
	#endif

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

	template STORMBYTE_PUBLIC std::string HumanReadable<int>(const int&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<unsigned int>(const unsigned int&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<long>(const long&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<unsigned long>(const unsigned long&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<long long>(const long long&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<unsigned long long>(const unsigned long long&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<float>(const float&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<double>(const double&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<long double>(const long double&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<short>(const short&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<unsigned short>(const unsigned short&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<char>(const char&, const Format&, const std::string&) noexcept;
	template STORMBYTE_PUBLIC std::string HumanReadable<unsigned char>(const unsigned char&, const Format&, const std::string&) noexcept;
}