/*
 * Copyright (C) 2024-2026 David C. Manuelda (StormBytePP)
 *
 * This file is part of StormByte.
 *
 * StormByte original source is dual-licensed:
 *
 * 1. GNU Lesser General Public License v3.0 (or later)
 *    You may redistribute and/or modify this file under the terms of the
 *    GNU Lesser General Public License as published by the Free Software
 *    Foundation, either version 3 of the License, or (at your option)
 *    any later version.
 *
 * 2. Commercial license
 *    Alternatively, this file may be used under the terms of a commercial
 *    license agreement with the copyright holder
 *    (David C. Manuelda <StormByte@gmail.com>).
 *
 * Both licenses apply only to original StormByte source in this repository.
 * They do not cover other StormByte modules or any third-party material
 * shipped with this repository (including everything under thirdparty/),
 * which remains under its own license.
 *
 * Neither license grants any patent rights. Any patent licenses required
 * to use this software or third-party components must be obtained separately
 * from the patent holders.
 *
 * StormByte is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * version 3 along with StormByte. If not, see
 * <https://www.gnu.org/licenses/lgpl-3.0.html>.
 *
 * SPDX-License-Identifier: LGPL-3.0-or-later OR LicenseRef-StormByte-Commercial
 */

#pragma once

#include <StormByte/type_traits.hxx>
#include <StormByte/visibility.h>

#include <cstddef>
#include <memory>
#include <utility>

/**
 * @file StormByte/safe_pointers.hxx
 * @brief DLL-safe shared and unique owners. Storage is on Base's heap.
 *
 * @ref StormByte::Heap is not a public header. `Allocate` / `Free` are
 * exported from the StormByte library and defined in a private translation
 * unit that is not installed.
 */

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	template<class T>
	class Shared;

	template<class T>
	class Unique;

	/**
	 * @namespace StormByte::Heap
	 * @brief Allocate and free raw blocks on Base's heap.
	 *
	 * Declarations match the private `heap.cxx` definitions. The private
	 * header is not installed; public templates need these names here.
	 */
	namespace Heap {
		/**
		 * @brief Allocate @p bytes on Base's heap.
		 * @param bytes Block size in octets. Zero is forwarded to `operator new`.
		 * @return Address of the block.
		 * @throws std::bad_alloc When the allocator cannot satisfy the request.
		 */
		STORMBYTE_PUBLIC void* Allocate(std::size_t bytes);

		/**
		 * @brief Release a block obtained from @ref Allocate.
		 * @param pointer Block address, or a null pointer.
		 */
		STORMBYTE_PUBLIC void Free(void* pointer) noexcept;

		/**
		 * @struct ObjectDeleter
		 * @brief Destroy @p T and return its block to @ref Free.
		 */
		struct ObjectDeleter {
			/**
			 * @brief Destroy @p pointer and free its block on Base's heap.
			 * @tparam U Pointee type.
			 * @param pointer Object constructed with placement `new` on @ref Allocate.
			 */
			template<class U>
			void operator()(U* pointer) const noexcept {
				pointer->~U();
				Free(pointer);
			}
		};

		/**
		 * @struct Allocator
		 * @brief `std::shared_ptr` allocator that uses @ref Allocate / @ref Free.
		 * @tparam T Control-block value type.
		 */
		template<typename T>
		struct Allocator {
			using value_type = T;	///< Allocator value type.

			/**
			 * @brief Default constructor.
			 */
			constexpr Allocator() noexcept = default;

			/**
			 * @brief Rebind constructor.
			 * @tparam U Other allocator value type.
			 */
			template<typename U>
			constexpr Allocator(const Allocator<U>&) noexcept {}

			/**
			 * @brief Allocate @p count objects of @p T.
			 * @param count Object count.
			 * @return Storage for @p count objects.
			 */
			[[nodiscard]] T* allocate(std::size_t count) {
				return static_cast<T*>(Allocate(count * sizeof(T)));
			}

			/**
			 * @brief Release storage obtained from @ref allocate.
			 * @param pointer Storage to release.
			 */
			void deallocate(T* pointer, std::size_t) noexcept {
				Free(pointer);
			}

			/**
			 * @brief All @ref Allocator instances compare equal.
			 * @tparam U Other allocator value type.
			 * @return Always `true`.
			 */
			template<typename U>
			constexpr bool operator==(const Allocator<U>&) const noexcept {
				return true;
			}
		};

		/**
		 * @brief Construct @p T on Base's heap and wrap it in @ref Shared.
		 * @tparam T Object type.
		 * @tparam Args Constructor argument types.
		 * @param args Forwarded to @p T.
		 * @return Owning @ref Shared.
		 */
		template<class T, class... Args>
		Shared<T> MakeShared(Args&&... args);

		/**
		 * @brief Construct @p T on Base's heap and wrap it in @ref Unique.
		 * @tparam T Object type.
		 * @tparam Args Constructor argument types.
		 * @param args Forwarded to @p T.
		 * @return Owning @ref Unique.
		 */
		template<class T, class... Args>
		Unique<T> MakeUnique(Args&&... args);
	}

	/**
	 * @class Shared
	 * @brief Shared owner of a @p T allocated on Base's heap.
	 * @tparam T Pointee type.
	 *
	 * Construct the exact type with @ref Heap::MakeShared. Construct a derived
	 * type with @ref Shared::MakePointer. Converts implicitly to
	 * `std::shared_ptr<T>` (same control block). There is no conversion from
	 * `std::shared_ptr`.
	 */
	template<class T>
	class Shared {
		public:
			using element_type = T;	///< Pointee type.

			/**
			 * @brief Empty owner.
			 */
			Shared() noexcept = default;

			/**
			 * @brief Empty owner from `nullptr`.
			 */
			Shared(std::nullptr_t) noexcept {}

			/**
			 * @brief Share ownership with @p other.
			 * @tparam U Pointee convertible to @p T.
			 * @param other Other owner.
			 */
			template<class U>
			requires Type::SameAs<U, T> || Type::DerivedFrom<U, T>
			Shared(const Shared<U>& other) noexcept: m_ptr(other.m_ptr) {}

			/**
			 * @brief Take ownership from @p other.
			 * @tparam U Pointee convertible to @p T.
			 * @param other Other owner.
			 */
			template<class U>
			requires Type::SameAs<U, T> || Type::DerivedFrom<U, T>
			Shared(Shared<U>&& other) noexcept: m_ptr(std::move(other.m_ptr)) {}

			/**
			 * @brief Copy constructor.
			 */
			Shared(const Shared&) noexcept = default;

			/**
			 * @brief Move constructor.
			 */
			Shared(Shared&&) noexcept = default;

			/**
			 * @brief Copy assignment.
			 * @return @c *this.
			 */
			Shared& operator=(const Shared&) noexcept = default;

			/**
			 * @brief Move assignment.
			 * @return @c *this.
			 */
			Shared& operator=(Shared&&) noexcept = default;

			/**
			 * @brief Destructor.
			 */
			~Shared() = default;

			/**
			 * @brief Raw pointer, or null.
			 * @return Pointee.
			 */
			T* get() const noexcept {
				return m_ptr.get();
			}

			/**
			 * @brief Dereference.
			 * @return Pointee.
			 */
			T& operator*() const {
				return *m_ptr;
			}

			/**
			 * @brief Member access.
			 * @return Pointee.
			 */
			T* operator->() const noexcept {
				return m_ptr.get();
			}

			/**
			 * @brief Whether this owner holds an object.
			 * @return @c true when non-empty.
			 */
			explicit operator bool() const noexcept {
				return static_cast<bool>(m_ptr);
			}

			/**
			 * @brief Same control block as `std::shared_ptr`. Deleter stays Base.
			 * @return `std::shared_ptr<T>` that still frees on Base's heap.
			 */
			operator std::shared_ptr<T>() const noexcept {
				return m_ptr;
			}

			/**
			 * @brief Number of @ref Shared sharing this object.
			 * @return Use count.
			 */
			long use_count() const noexcept {
				return m_ptr.use_count();
			}

			/**
			 * @brief Drop this owner's reference.
			 */
			void reset() noexcept {
				m_ptr.reset();
			}

			/**
			 * @brief Equality with null.
			 * @param lhs Owner.
			 * @return Whether @p lhs is empty.
			 */
			friend bool operator==(const Shared& lhs, std::nullptr_t) noexcept {
				return !lhs.m_ptr;
			}

			/**
			 * @brief Inequality with null.
			 * @param lhs Owner.
			 * @return Whether @p lhs is non-empty.
			 */
			friend bool operator!=(const Shared& lhs, std::nullptr_t) noexcept {
				return static_cast<bool>(lhs.m_ptr);
			}

			/**
			 * @brief Equality with null.
			 * @param rhs Owner.
			 * @return Whether @p rhs is empty.
			 */
			friend bool operator==(std::nullptr_t, const Shared& rhs) noexcept {
				return !rhs.m_ptr;
			}

			/**
			 * @brief Inequality with null.
			 * @param rhs Owner.
			 * @return Whether @p rhs is non-empty.
			 */
			friend bool operator!=(std::nullptr_t, const Shared& rhs) noexcept {
				return static_cast<bool>(rhs.m_ptr);
			}

			/**
			 * @brief Allocate @p Target on Base's heap and own it as @p T.
			 * @tparam Target Concrete type (`T` or derived from @p T).
			 * @tparam Args Constructor argument types.
			 * @param args Forwarded to `Target`.
			 * @return Owner of the `Target` object. The deleter still destroys `Target`.
			 */
			template<class Target, class... Args>
			requires Type::SameAs<Target, T> || Type::DerivedFrom<Target, T>
			static Shared<T> MakePointer(Args&&... args) {
				return Heap::MakeShared<Target>(std::forward<Args>(args)...);
			}

		private:
			/**
			 * @brief Tag for @ref Heap::MakeShared.
			 */
			struct Adopt {
				explicit constexpr Adopt() noexcept = default;
			};

			/**
			 * @brief Take a block already constructed on Base's heap.
			 * @param object Placement-new address from @ref Heap::Allocate.
			 */
			explicit Shared(Adopt, T* object): m_ptr(object, Heap::ObjectDeleter{}, Heap::Allocator<T>{}) {}

			template<class U>
			friend class Shared;

			template<class U, class... Args>
			friend Shared<U> Heap::MakeShared(Args&&...);

			std::shared_ptr<T> m_ptr;	///< Control block and object on Base's heap
	};

	/**
	 * @class Unique
	 * @brief Unique owner of a @p T allocated on Base's heap.
	 * @tparam T Pointee type.
	 *
	 * Construct the exact type with @ref Heap::MakeUnique. Construct a derived
	 * type with @ref Unique::MakePointer. Converts implicitly to
	 * `std::unique_ptr<T, Heap::ObjectDeleter>` on move. There is no conversion
	 * from or to `std::unique_ptr<T>`. A derived object needs a virtual
	 * destructor: the unique deleter calls the static type.
	 */
	template<class T>
	class Unique {
		public:
			using element_type = T;	///< Pointee type.

			/**
			 * @brief Empty owner.
			 */
			Unique() noexcept = default;

			/**
			 * @brief Empty owner from `nullptr`.
			 */
			Unique(std::nullptr_t) noexcept {}

			/**
			 * @brief Take ownership from @p other.
			 * @tparam U Pointee convertible to @p T.
			 * @param other Other owner.
			 */
			template<class U>
			requires Type::SameAs<U, T> || Type::DerivedFrom<U, T>
			Unique(Unique<U>&& other) noexcept: m_ptr(std::move(other.m_ptr)) {}

			/**
			 * @brief Copy constructor. Deleted.
			 */
			Unique(const Unique&) = delete;

			/**
			 * @brief Move constructor.
			 */
			Unique(Unique&&) noexcept = default;

			/**
			 * @brief Copy assignment. Deleted.
			 * @return @c *this.
			 */
			Unique& operator=(const Unique&) = delete;

			/**
			 * @brief Move assignment.
			 * @return @c *this.
			 */
			Unique& operator=(Unique&&) noexcept = default;

			/**
			 * @brief Destructor.
			 */
			~Unique() = default;

			/**
			 * @brief Raw pointer, or null.
			 * @return Pointee.
			 */
			T* get() const noexcept {
				return m_ptr.get();
			}

			/**
			 * @brief Dereference.
			 * @return Pointee.
			 */
			T& operator*() const {
				return *m_ptr;
			}

			/**
			 * @brief Member access.
			 * @return Pointee.
			 */
			T* operator->() const noexcept {
				return m_ptr.get();
			}

			/**
			 * @brief Whether this owner holds an object.
			 * @return @c true when non-empty.
			 */
			explicit operator bool() const noexcept {
				return static_cast<bool>(m_ptr);
			}

			/**
			 * @brief Release as `std::unique_ptr` with Base's deleter.
			 * @return `std::unique_ptr<T, Heap::ObjectDeleter>`.
			 */
			operator std::unique_ptr<T, Heap::ObjectDeleter>() && noexcept {
				return std::move(m_ptr);
			}

			/**
			 * @brief Destroy the object and drop this owner.
			 */
			void reset() noexcept {
				m_ptr.reset();
			}

			/**
			 * @brief Equality with null.
			 * @param lhs Owner.
			 * @return Whether @p lhs is empty.
			 */
			friend bool operator==(const Unique& lhs, std::nullptr_t) noexcept {
				return !lhs.m_ptr;
			}

			/**
			 * @brief Inequality with null.
			 * @param lhs Owner.
			 * @return Whether @p lhs is non-empty.
			 */
			friend bool operator!=(const Unique& lhs, std::nullptr_t) noexcept {
				return static_cast<bool>(lhs.m_ptr);
			}

			/**
			 * @brief Equality with null.
			 * @param rhs Owner.
			 * @return Whether @p rhs is empty.
			 */
			friend bool operator==(std::nullptr_t, const Unique& rhs) noexcept {
				return !rhs.m_ptr;
			}

			/**
			 * @brief Inequality with null.
			 * @param rhs Owner.
			 * @return Whether @p rhs is non-empty.
			 */
			friend bool operator!=(std::nullptr_t, const Unique& rhs) noexcept {
				return static_cast<bool>(rhs.m_ptr);
			}

			/**
			 * @brief Allocate @p Target on Base's heap and own it as @p T.
			 * @tparam Target Concrete type (`T` or derived from @p T).
			 * @tparam Args Constructor argument types.
			 * @param args Forwarded to `Target`.
			 * @return Owner of the `Target` object.
			 * @note `Target` needs a virtual destructor. The deleter calls `~T`.
			 */
			template<class Target, class... Args>
			requires Type::SameAs<Target, T> || Type::DerivedFrom<Target, T>
			static Unique<T> MakePointer(Args&&... args) {
				return Heap::MakeUnique<Target>(std::forward<Args>(args)...);
			}

		private:
			/**
			 * @brief Tag for @ref Heap::MakeUnique.
			 */
			struct Adopt {
				explicit constexpr Adopt() noexcept = default;
			};

			/**
			 * @brief Take a block already constructed on Base's heap.
			 * @param object Placement-new address from @ref Heap::Allocate.
			 */
			explicit Unique(Adopt, T* object): m_ptr(object) {}

			template<class U>
			friend class Unique;

			template<class U, class... Args>
			friend Unique<U> Heap::MakeUnique(Args&&...);

			std::unique_ptr<T, Heap::ObjectDeleter> m_ptr;	///< Object on Base's heap
	};
}

#include <StormByte/safe_pointers.txx>
