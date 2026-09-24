#include <StormByte/size.hxx>

#include <array>
#include <cstdint>
#include <cstdio>

using namespace StormByte;

namespace {
	CString FormatByteSize(std::uint64_t bytes) noexcept {
		static constexpr std::array<const char*, 7> units{
			"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB"
		};
		std::uint64_t value = bytes;
		std::size_t unit = 0;
		std::uint64_t remainder = 0;
		while (value >= 1024 && unit + 1 < units.size()) {
			remainder = value % 1024;
			value /= 1024;
			++unit;
		}

		char buffer[32];
		if (unit == 0 || remainder == 0)
			std::snprintf(buffer, sizeof(buffer), "%llu %s",
				static_cast<unsigned long long>(value), units[unit]);
		else {
			const unsigned tenths = static_cast<unsigned>((remainder * 10 + 512) / 1024);
			if (tenths == 0)
				std::snprintf(buffer, sizeof(buffer), "%llu %s",
					static_cast<unsigned long long>(value), units[unit]);
			else if (tenths == 10)
				std::snprintf(buffer, sizeof(buffer), "%llu %s",
					static_cast<unsigned long long>(value + 1), units[unit]);
			else
				std::snprintf(buffer, sizeof(buffer), "%llu.%u %s",
					static_cast<unsigned long long>(value), tenths, units[unit]);
		}
		return CString(buffer);
	}
}

Size::operator CString() const noexcept {
	return FormatByteSize(m_value);
}
