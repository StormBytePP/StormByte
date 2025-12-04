#pragma once

#include <StormByte/type_traits.hxx>
#include <StormByte/visibility.h>

#include <span>
#include <vector>
#include <version>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	template<typename T, typename U>
	void append_vector(std::vector<T>& dest, std::span<U> src) noexcept requires Type::ConvertibleTo<T, U> {
		dest.reserve(dest.size() + src.size());
#ifdef __cpp_lib_containers_ranges
		dest.append_range(src);
#else
		dest.insert(dest.end(), src.begin(), src.end());
#endif
	}

	/**
	 * @brief Appends the contents of one vector to another.
	 * @tparam T The type of elements in the vectors.
	 * @param dest The destination vector to which elements will be appended.
	 * @param src The source vector from which elements will be taken.
	 * @note This function reserves enough space in the destination vector to accommodate
	 *       the new elements before appending, to avoid multiple reallocations.
	 *
	 * This function appends all elements from the source vector `src` to the end of the
	 * destination vector `dest`. It supports both lvalue and rvalue source vectors for
	 * efficient appending.
	 */
	template<typename T>
	void append_vector(std::vector<T>& dest, const std::vector<T>& src) noexcept {
		return append_vector(dest, std::span<const T>(src.data(), src.size()));
	}

	/**
	 * @brief Move the contents of one vector to another.
	 * @tparam T The type of elements in the vectors.
	 * @param dest The destination vector to which elements will be appended.
	 * @param src The source vector from which elements will be taken.
	 * @note This function reserves enough space in the destination vector to accommodate
	 *       the new elements before appending, to avoid multiple reallocations.
	 *
	 * This function appends all elements from the source vector `src` to the end of the
	 * destination vector `dest`. It supports both lvalue and rvalue source vectors for
	 * efficient appending.
	 */
	template<typename T>
	void append_vector(std::vector<T>& dest, std::vector<T>&& src) noexcept {
		dest.reserve(dest.size() + src.size());
#ifdef __cpp_lib_containers_ranges
		dest.append_range(std::move(src));
#else
		dest.insert(dest.end(), std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()));
#endif
	}
}