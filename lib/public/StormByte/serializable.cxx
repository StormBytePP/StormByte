/*
* Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
*
* This file is part of StormByte.
*
* StormByte is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* StormByte is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with StormByte. If not, see <https://www.gnu.org/licenses/>.
*/

#include <StormByte/serializable.hxx>

#include <string>

namespace StormByte {
	namespace {
		void append_utf8(std::string& out, char32_t cp) {
			if (cp > 0x10FFFFu || (cp >= 0xD800u && cp <= 0xDFFFu))
				cp = 0xFFFDu;

			if (cp <= 0x7Fu) {
				out.push_back(static_cast<char>(cp));
			} else if (cp <= 0x7FFu) {
				out.push_back(static_cast<char>(0xC0u | (cp >> 6)));
				out.push_back(static_cast<char>(0x80u | (cp & 0x3Fu)));
			} else if (cp <= 0xFFFFu) {
				out.push_back(static_cast<char>(0xE0u | (cp >> 12)));
				out.push_back(static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu)));
				out.push_back(static_cast<char>(0x80u | (cp & 0x3Fu)));
			} else {
				out.push_back(static_cast<char>(0xF0u | (cp >> 18)));
				out.push_back(static_cast<char>(0x80u | ((cp >> 12) & 0x3Fu)));
				out.push_back(static_cast<char>(0x80u | ((cp >> 6) & 0x3Fu)));
				out.push_back(static_cast<char>(0x80u | (cp & 0x3Fu)));
			}
		}

		template<typename It>
		std::string utf16_units_to_utf8(It first, It last) {
			std::string out;
			while (first != last) {
				const auto unit = static_cast<char16_t>(*first);
				++first;
				char32_t cp = unit;
				if (unit >= 0xD800u && unit <= 0xDBFFu && first != last) {
					const auto low = static_cast<char16_t>(*first);
					if (low >= 0xDC00u && low <= 0xDFFFu) {
						++first;
						cp = 0x10000u + ((static_cast<char32_t>(unit) - 0xD800u) << 10) +
							(static_cast<char32_t>(low) - 0xDC00u);
					} else {
						cp = 0xFFFDu;
					}
				} else if (unit >= 0xDC00u && unit <= 0xDFFFu) {
					cp = 0xFFFDu;
				}
				append_utf8(out, cp);
			}
			return out;
		}

		template<typename It>
		std::string utf32_units_to_utf8(It first, It last) {
			std::string out;
			for (; first != last; ++first)
				append_utf8(out, static_cast<char32_t>(*first));
			return out;
		}

		std::string wstring_to_utf8(const std::wstring& in) {
			if constexpr (sizeof(wchar_t) == 2)
				return utf16_units_to_utf8(in.begin(), in.end());
			else
				return utf32_units_to_utf8(in.begin(), in.end());
		}

		std::string u16_to_utf8(const std::u16string& in) {
			return utf16_units_to_utf8(in.begin(), in.end());
		}

		std::string u32_to_utf8(const std::u32string& in) {
			return utf32_units_to_utf8(in.begin(), in.end());
		}

		bool next_utf8(const std::string& s, std::size_t& i, char32_t& cp) {
			if (i >= s.size())
				return false;

			const auto lead = static_cast<unsigned char>(s[i++]);
			int extra = 0;
			if (lead < 0x80u) {
				cp = lead;
				return true;
			} else if ((lead & 0xE0u) == 0xC0u) {
				cp = lead & 0x1Fu;
				extra = 1;
			} else if ((lead & 0xF0u) == 0xE0u) {
				cp = lead & 0x0Fu;
				extra = 2;
			} else if ((lead & 0xF8u) == 0xF0u) {
				cp = lead & 0x07u;
				extra = 3;
			} else {
				cp = 0xFFFDu;
				return true;
			}

			for (int n = 0; n < extra; ++n) {
				if (i >= s.size()) {
					cp = 0xFFFDu;
					return true;
				}
				const auto cont = static_cast<unsigned char>(s[i]);
				if ((cont & 0xC0u) != 0x80u) {
					cp = 0xFFFDu;
					return true;
				}
				++i;
				cp = (cp << 6) | (cont & 0x3Fu);
			}
			if (cp > 0x10FFFFu || (cp >= 0xD800u && cp <= 0xDFFFu))
				cp = 0xFFFDu;
			return true;
		}

		std::wstring utf8_to_wstring(const std::string& utf8) {
			std::wstring out;
			std::size_t i = 0;
			char32_t cp = 0;
			while (next_utf8(utf8, i, cp)) {
				if constexpr (sizeof(wchar_t) == 2) {
					if (cp <= 0xFFFFu) {
						out.push_back(static_cast<wchar_t>(cp));
					} else {
						cp -= 0x10000u;
						out.push_back(static_cast<wchar_t>(0xD800u + (cp >> 10)));
						out.push_back(static_cast<wchar_t>(0xDC00u + (cp & 0x3FFu)));
					}
				} else {
					out.push_back(static_cast<wchar_t>(cp));
				}
			}
			return out;
		}

		std::u16string utf8_to_u16(const std::string& utf8) {
			std::u16string out;
			std::size_t i = 0;
			char32_t cp = 0;
			while (next_utf8(utf8, i, cp)) {
				if (cp <= 0xFFFFu) {
					out.push_back(static_cast<char16_t>(cp));
				} else {
					cp -= 0x10000u;
					out.push_back(static_cast<char16_t>(0xD800u + (cp >> 10)));
					out.push_back(static_cast<char16_t>(0xDC00u + (cp & 0x3FFu)));
				}
			}
			return out;
		}

		std::u32string utf8_to_u32(const std::string& utf8) {
			std::u32string out;
			std::size_t i = 0;
			char32_t cp = 0;
			while (next_utf8(utf8, i, cp))
				out.push_back(cp);
			return out;
		}

		std::vector<std::byte> write_byte_string(const std::string& payload) {
			const std::uint64_t size = static_cast<std::uint64_t>(payload.size());
			std::vector<std::byte> buffer;
			buffer.reserve(sizeof(std::uint64_t) + payload.size());
			append_vector(buffer, Serializable<std::uint64_t>(size).Serialize());
			const auto* ptr = reinterpret_cast<const std::byte*>(payload.data());
			buffer.insert(buffer.end(), ptr, ptr + payload.size());
			return buffer;
		}

		Expected<std::string, DeserializeError> read_byte_string(std::span<const std::byte> data) {
			if (data.size() < sizeof(std::uint64_t))
				return Unexpected<DeserializeError>("Insufficient data for string size");

			auto expected_size = Serializable<std::uint64_t>::Deserialize(
				data.subspan(0, sizeof(std::uint64_t)));
			if (!expected_size)
				return Unexpected(expected_size.error());

			const std::uint64_t size = expected_size.value();
			const std::size_t offset = sizeof(std::uint64_t);
			if (size > static_cast<std::uint64_t>(data.size() - offset))
				return Unexpected<DeserializeError>("Insufficient data for string content");

			return std::string(reinterpret_cast<const char*>(data.data() + offset),
				static_cast<std::size_t>(size));
		}
	}

	STORMBYTE_PUBLIC
	std::size_t Detail::Codec<std::string>::Size(const std::string& data) noexcept {
		return sizeof(std::uint64_t) + data.size();
	}

	STORMBYTE_PUBLIC
	std::vector<std::byte> Detail::Codec<std::string>::Write(const std::string& data) noexcept {
		return write_byte_string(data);
	}

	STORMBYTE_PUBLIC
	Expected<std::string, DeserializeError> Detail::Codec<std::string>::Read(std::span<const std::byte> data) noexcept {
		return read_byte_string(data);
	}

	STORMBYTE_PUBLIC
	std::size_t Detail::Codec<std::wstring>::Size(const std::wstring& data) noexcept {
		return sizeof(std::uint64_t) + wstring_to_utf8(data).size();
	}

	STORMBYTE_PUBLIC
	std::vector<std::byte> Detail::Codec<std::wstring>::Write(const std::wstring& data) noexcept {
		return write_byte_string(wstring_to_utf8(data));
	}

	STORMBYTE_PUBLIC
	Expected<std::wstring, DeserializeError> Detail::Codec<std::wstring>::Read(std::span<const std::byte> data) noexcept {
		auto payload = read_byte_string(data);
		if (!payload)
			return Unexpected(payload.error());
		return utf8_to_wstring(payload.value());
	}

	STORMBYTE_PUBLIC
	std::size_t Detail::Codec<std::u16string>::Size(const std::u16string& data) noexcept {
		return sizeof(std::uint64_t) + u16_to_utf8(data).size();
	}

	STORMBYTE_PUBLIC
	std::vector<std::byte> Detail::Codec<std::u16string>::Write(const std::u16string& data) noexcept {
		return write_byte_string(u16_to_utf8(data));
	}

	STORMBYTE_PUBLIC
	Expected<std::u16string, DeserializeError> Detail::Codec<std::u16string>::Read(std::span<const std::byte> data) noexcept {
		auto payload = read_byte_string(data);
		if (!payload)
			return Unexpected(payload.error());
		return utf8_to_u16(payload.value());
	}

	STORMBYTE_PUBLIC
	std::size_t Detail::Codec<std::u32string>::Size(const std::u32string& data) noexcept {
		return sizeof(std::uint64_t) + u32_to_utf8(data).size();
	}

	STORMBYTE_PUBLIC
	std::vector<std::byte> Detail::Codec<std::u32string>::Write(const std::u32string& data) noexcept {
		return write_byte_string(u32_to_utf8(data));
	}

	STORMBYTE_PUBLIC
	Expected<std::u32string, DeserializeError> Detail::Codec<std::u32string>::Read(std::span<const std::byte> data) noexcept {
		auto payload = read_byte_string(data);
		if (!payload)
			return Unexpected(payload.error());
		return utf8_to_u32(payload.value());
	}
}
