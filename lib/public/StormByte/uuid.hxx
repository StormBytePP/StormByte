#pragma once

#include <StormByte/visibility.h>

#include <string>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	/**
	 * @brief Generate a RFC4122-compliant UUID version 4 string.
	 *
	 * See `uuid.cxx` for details and implementation notes about randomness
	 * source selection (OS CSPRNG preferred, fallback to PRNG).
	 *
	 * @return A 36-character lowercase UUID string in the form
	 *         "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx" where `y` is one of
	 *         8, 9, a, or b.
	 */
	STORMBYTE_PUBLIC std::string GenerateUUIDv4() noexcept;
}
