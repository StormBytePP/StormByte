#include <StormByte/uuid.hxx>

#include <array>
#include <cstdint>
#include <cstring>
#include <cstdio>

#if defined(_WIN32)
#include <windows.h>
#include <bcrypt.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

#include <random>

namespace {
	inline bool FillRandomBytes(uint8_t* out, size_t len) noexcept {
	#if defined(_WIN32)
		NTSTATUS status = BCryptGenRandom(nullptr, out, static_cast<ULONG>(len), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
		return status == 0;
	#else
		int fd = open("/dev/urandom", O_RDONLY | O_CLOEXEC);
		if (fd < 0) return false;
		ssize_t rd = read(fd, out, len);
		close(fd);
		return rd == (ssize_t)len;
	#endif
	}
} // anonymous

namespace StormByte {
	std::string GenerateUUIDv4() noexcept {
		std::array<uint8_t, 16> b{};
		if (!FillRandomBytes(b.data(), b.size())) {
			// Fallback to std::random_device seeded PRNG if OS RNG unavailable
			static thread_local std::mt19937_64 rng((std::random_device())());
			static thread_local std::uniform_int_distribution<uint64_t> dist;
			uint64_t r1 = dist(rng);
			uint64_t r2 = dist(rng);
			std::memcpy(b.data(), &r1, 8);
			std::memcpy(b.data() + 8, &r2, 8);
		}

		// Set RFC4122 version = 4
		b[6] = static_cast<uint8_t>((b[6] & 0x0F) | 0x40);
		// Set RFC4122 variant = 10xxxxxx
		b[8] = static_cast<uint8_t>((b[8] & 0x3F) | 0x80);

		char buf[37];
		std::snprintf(buf, sizeof(buf),
			"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
			b[0], b[1], b[2], b[3],
			b[4], b[5],
			b[6], b[7],
			b[8], b[9],
			b[10], b[11], b[12], b[13], b[14], b[15]
		);

		return std::string(buf);
	}
}
