#include <StormByte/base64.hxx>
#include <StormByte/exception.hxx>

#include <array>
#include <cstdint>

namespace StormByte {

	namespace {
		constexpr char EncodeTable[] =
			"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
			"abcdefghijklmnopqrstuvwxyz"
			"0123456789+/";

		constexpr std::array<std::uint8_t, 256> MakeDecodeTable() {
			std::array<std::uint8_t, 256> table{};
			table.fill(255);

			for (std::uint8_t i = 0; i < 26; ++i) {
				table['A' + i] = i;
				table['a' + i] = 26 + i;
			}
			for (std::uint8_t i = 0; i < 10; ++i) {
				table['0' + i] = 52 + i;
			}
			table['+'] = 62;
			table['/'] = 63;

			return table;
		}

		constexpr auto DecodeTable = MakeDecodeTable();

		std::string EncodeImpl(std::span<const std::byte> input) {
			const std::size_t size = input.size();
			std::string output;
			output.resize(((size + 2) / 3) * 4);

			char* out = output.data();
			std::size_t i = 0;

			while (i + 2 < size) {
				std::uint32_t n = (static_cast<std::uint32_t>(input[i])     << 16) |
								(static_cast<std::uint32_t>(input[i + 1]) <<  8) |
								static_cast<std::uint32_t>(input[i + 2]);

				*out++ = EncodeTable[(n >> 18) & 0x3F];
				*out++ = EncodeTable[(n >> 12) & 0x3F];
				*out++ = EncodeTable[(n >>  6) & 0x3F];
				*out++ = EncodeTable[ n        & 0x3F];
				i += 3;
			}

			if (i < size) {
				std::uint32_t n = static_cast<std::uint32_t>(input[i]) << 16;
				*out++ = EncodeTable[(n >> 18) & 0x3F];

				if (i + 1 < size) {
					n |= static_cast<std::uint32_t>(input[i + 1]) << 8;
					*out++ = EncodeTable[(n >> 12) & 0x3F];
					*out++ = EncodeTable[(n >>  6) & 0x3F];
					*out++ = '=';
				} else {
					*out++ = EncodeTable[(n >> 12) & 0x3F];
					*out++ = '=';
					*out++ = '=';
				}
			}

			return output;
		}
	}

	std::vector<std::byte> Base64Decode(const std::string& input) {
		std::vector<std::byte> output;
		output.reserve((input.size() / 4) * 3);

		std::uint32_t buffer = 0;
		int bits_collected = 0;

		for (char c : input) {
			if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
				continue;

			if (c == '=')
				break;

			std::uint8_t value = DecodeTable[static_cast<unsigned char>(c)];
			if (value == 255) {
				throw Base64Error("Base64", "Invalid character '{}' in input", c);
			}

			buffer = (buffer << 6) | value;
			bits_collected += 6;

			if (bits_collected >= 8) {
				bits_collected -= 8;
				output.push_back(static_cast<std::byte>((buffer >> bits_collected) & 0xFF));
			}
		}

		return output;
	}

	std::string Base64Encode(const std::vector<std::byte>& input) {
		return EncodeImpl(input);
	}

	std::string Base64Encode(std::span<const std::byte> input) {
		return EncodeImpl(input);
	}
}
