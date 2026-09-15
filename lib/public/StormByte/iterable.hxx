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

#pragma once

#include <StormByte/exception.hxx>
#include <StormByte/type_traits.hxx>

#include <iterator>
#include <utility>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @class Iterable
	 * @brief Wrapper that adds a uniform iteration and mutation API around a standard container.
	 * @tparam Container Underlying container (`std::vector`, `std::map`, `std::deque`, …).
	 *
	 * Iterator adapters, size/empty, subscript (index or key) and `add()` chosen by `Type` concepts
	 * (`push_back`, `push_front` or associative `insert`).
	 *
	 * Insertion and index subscript use one definition with `if constexpr` so MSVC STL and clang-cl
	 * do not instantiate unsupported APIs or emit duplicate symbols under the MSVC ABI.
	 *
	 * Constraints use the template parameter `Container` directly (not `decltype(m_data)`).
	 */
	template <typename Container>
	class Iterable {
		protected:
			Container m_data; ///< Underlying container.

		public:
			using value_type = typename Container::value_type;					///< `Container::value_type`.
			using size_type = typename Container::size_type;					///< `Container::size_type`.
			using difference_type = typename Container::difference_type;		///< `Container::difference_type`.
			using reference = typename Container::reference;					///< `Container::reference`.
			using const_reference = typename Container::const_reference;		///< `Container::const_reference`.
			using pointer = typename Container::pointer;						///< `Container::pointer`.
			using const_pointer = typename Container::const_pointer;			///< `Container::const_pointer`.

			/**
			 * @class Iterator
			 * @brief Mutable iterator adapter over `Container::iterator`.
			 */
			class Iterator {
				friend class Iterable;
				public:
					using iterator_category = typename std::iterator_traits<typename Container::iterator>::iterator_category;	///< Matches the underlying container iterator; not assumed random-access.
					using value_type = typename std::iterator_traits<typename Container::iterator>::value_type;
					using difference_type = typename std::iterator_traits<typename Container::iterator>::difference_type;
					using pointer = typename std::iterator_traits<typename Container::iterator>::pointer;
					using reference = typename std::iterator_traits<typename Container::iterator>::reference;

					/**
					 * @brief Dereference.
					 * @return Reference to the current element.
					 */
					reference operator*();

					/**
					 * @brief Member access.
					 * @return Pointer to the current element.
					 */
					pointer operator->();

					/**
					 * @brief Pre-increment.
					 * @return `*this` after advancing one.
					 */
					Iterator& operator++();

					/**
					 * @brief Post-increment.
					 * @return Copy before advancing.
					 */
					Iterator operator++(int);

					/**
					 * @brief Pre-decrement.
					 * @return `*this` after moving back one.
					 */
					Iterator& operator--();

					/**
					 * @brief Post-decrement.
					 * @return Copy before moving back.
					 */
					Iterator operator--(int);

					/**
					 * @brief Advance in place.
					 * @param n Positions (may be negative).
					 * @return `*this`.
					 */
					Iterator& operator+=(difference_type n);

					/**
					 * @brief Retreat in place.
					 * @param n Positions (may be negative).
					 * @return `*this`.
					 */
					Iterator& operator-=(difference_type n);

					/**
					 * @brief Iterator advanced by @p n.
					 * @param n Positions.
					 */
					Iterator operator+(difference_type n) const;

					/**
					 * @brief Iterator moved back by @p n.
					 * @param n Positions.
					 */
					Iterator operator-(difference_type n) const;

					/**
					 * @brief Distance to @p other.
					 * @param other Iterator to subtract.
					 */
					difference_type operator-(const Iterator& other) const;

					/**
					 * @brief Equality.
					 * @param other Other iterator.
					 */
					bool operator==(const Iterator& other) const;

					/**
					 * @brief Inequality.
					 * @param other Other iterator.
					 */
					bool operator!=(const Iterator& other) const;

				private:
					typename Container::iterator m_it; ///< Wrapped iterator.

					/**
					 * @brief Wraps a container iterator.
					 * @param it `Container::iterator`.
					 */
					Iterator(typename Container::iterator it);
			};

			/**
			 * @class ConstIterator
			 * @brief Const iterator adapter over `Container::const_iterator`.
			 */
			class ConstIterator {
				friend class Iterable;
				public:
					using iterator_category = typename std::iterator_traits<typename Container::const_iterator>::iterator_category;	///< Matches the underlying container iterator; not assumed random-access.
					using value_type = typename std::iterator_traits<typename Container::const_iterator>::value_type;
					using difference_type = typename std::iterator_traits<typename Container::const_iterator>::difference_type;
					using pointer = typename std::iterator_traits<typename Container::const_iterator>::pointer;
					using reference = typename std::iterator_traits<typename Container::const_iterator>::reference;

					/**
					 * @brief Dereference.
					 * @return Const reference to the current element.
					 */
					reference operator*() const;

					/**
					 * @brief Member access.
					 * @return Const pointer to the current element.
					 */
					pointer operator->() const;

					/**
					 * @brief Pre-increment.
					 * @return `*this` after advancing one.
					 */
					ConstIterator& operator++();

					/**
					 * @brief Post-increment.
					 * @return Copy before advancing.
					 */
					ConstIterator operator++(int);

					/**
					 * @brief Pre-decrement.
					 * @return `*this` after moving back one.
					 */
					ConstIterator& operator--();

					/**
					 * @brief Post-decrement.
					 * @return Copy before moving back.
					 */
					ConstIterator operator--(int);

					/**
					 * @brief Advance in place.
					 * @param n Positions (may be negative).
					 * @return `*this`.
					 */
					ConstIterator& operator+=(difference_type n);

					/**
					 * @brief Retreat in place.
					 * @param n Positions (may be negative).
					 * @return `*this`.
					 */
					ConstIterator& operator-=(difference_type n);

					/**
					 * @brief Iterator advanced by @p n.
					 * @param n Positions.
					 */
					ConstIterator operator+(difference_type n) const;

					/**
					 * @brief Iterator moved back by @p n.
					 * @param n Positions.
					 */
					ConstIterator operator-(difference_type n) const;

					/**
					 * @brief Distance to @p other.
					 * @param other Iterator to subtract.
					 */
					difference_type operator-(const ConstIterator& other) const;

					/**
					 * @brief Equality.
					 * @param other Other iterator.
					 */
					bool operator==(const ConstIterator& other) const;

					/**
					 * @brief Inequality.
					 * @param other Other iterator.
					 */
					bool operator!=(const ConstIterator& other) const;

				private:
					typename Container::const_iterator m_it; ///< Wrapped const iterator.

					/**
					 * @brief Wraps a container const iterator.
					 * @param it `Container::const_iterator`.
					 */
					ConstIterator(typename Container::const_iterator it);
			};

			using iterator = Iterator;											///< Mutable adapter.
			using const_iterator = ConstIterator;								///< Const adapter.
			using reverse_iterator = std::reverse_iterator<iterator>;			///< Mutable reverse adapter.
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;	///< Const reverse adapter.

			/**
			 * @brief Empty iterable.
			 */
			Iterable() = default;

			/**
			 * @brief Builds from a container.
			 * @tparam C Deduced container type (lvalue or rvalue of
			 *         @p Container).
			 * @param data Container to take. Lvalues are copied;
			 *        rvalues are moved.
			 *
			 * One function template so clang-cl / MSVC STL do not
			 * instantiate a copy of `vector<unique_ptr<T>>`. An lvalue
			 * requires @ref Type::CopyConstructible on @ref value_type;
			 * an rvalue requires @ref Type::MoveConstructible.
			 * Constrained to @p Container so this constructor does not
			 * hide @c Iterable copy / move.
			 */
			template<typename C>
			explicit Iterable(C&& data)
			requires Type::SameAs<C, Container> && (
				(Type::LvalueReference<C> && Type::CopyConstructible<value_type>) ||
				(!Type::LvalueReference<C> && Type::MoveConstructible<value_type>)
			);

			/**
			 * @brief Copy constructor.
			 * @param other Source iterable.
			 * @throws StormByte::Exception If `Container` is not copy-constructible.
			 *
			 * Always declared so clang-cl / MSVC can export a derived
			 * class. The `vector` copy is behind `if constexpr`; a
			 * `requires = default` is still instantiated under `dllexport`.
			 */
			Iterable(const Iterable& other);

			/**
			 * @brief Move constructor.
			 */
			Iterable(Iterable&&) requires Type::MoveConstructible<Container> = default;

			/**
			 * @brief Destructor.
			 */
			virtual ~Iterable() = default;

			/**
			 * @brief Copy assignment.
			 * @param other Source iterable.
			 * @return `*this`.
			 * @throws StormByte::Exception If `Container` is not copy-assignable.
			 */
			Iterable& operator=(const Iterable& other);

			/**
			 * @brief Move assignment.
			 * @return `*this`.
			 */
			Iterable& operator=(Iterable&&) requires Type::MoveAssignable<Container> = default;

			/**
			 * @brief Equality of the underlying containers.
			 * @param other Other iterable.
			 */
			bool operator==(const Iterable& other) const;

			/**
			 * @brief Inequality of the underlying containers.
			 * @param other Other iterable.
			 */
			bool operator!=(const Iterable& other) const;

			/**
			 * @brief Mutable begin.
			 */
			iterator begin() noexcept;

			/**
			 * @brief Const begin.
			 */
			const_iterator begin() const noexcept;

			/**
			 * @brief Mutable end.
			 */
			iterator end() noexcept;

			/**
			 * @brief Const end.
			 */
			const_iterator end() const noexcept;

			/**
			 * @brief Const begin.
			 */
			const_iterator cbegin() const noexcept;

			/**
			 * @brief Const end.
			 */
			const_iterator cend() const noexcept;

			/**
			 * @brief Mutable reverse begin.
			 */
			reverse_iterator rbegin() noexcept;

			/**
			 * @brief Mutable reverse end.
			 */
			reverse_iterator rend() noexcept;

			/**
			 * @brief Const reverse begin.
			 */
			const_reverse_iterator rbegin() const noexcept;

			/**
			 * @brief Const reverse end.
			 */
			const_reverse_iterator rend() const noexcept;

			/**
			 * @brief Const reverse begin.
			 */
			const_reverse_iterator crbegin() const noexcept;

			/**
			 * @brief Const reverse end.
			 */
			const_reverse_iterator crend() const noexcept;

			/**
			 * @brief Element count.
			 */
			size_type size() const noexcept;

			/**
			 * @brief Whether the container is empty.
			 */
			bool empty() const noexcept;

			/**
			 * @brief Zero-based mutable index access.
			 * @param i Index.
			 * @return Reference to the element at @p i.
			 * @throws OutOfBoundsError If @p i is not less than `size()`.
			 *
			 * Uses `Container::operator[]` when `Type::HasSubscript` holds; otherwise advances iterators.
			 */
			reference operator[](size_type i);

			/**
			 * @brief Mutable key access for associative containers.
			 * @tparam K Convertible to `Container::key_type`.
			 * @param key Key to look up or insert.
			 * @return Mapped value (`Container::operator[]`).
			 */
			template<typename K>
			auto operator[](K const& key) -> decltype(auto)
			requires (Type::HasMappedType<Container>);

			/**
			 * @brief Const key access for associative containers.
			 * @tparam K Convertible to `Container::key_type`.
			 * @param key Key to look up.
			 * @return Const mapped value.
			 * @throws OutOfBoundsError If @p key is missing.
			 */
			template<typename K>
			auto operator[](K const& key) const -> decltype(auto)
			requires (Type::HasMappedType<const Container>);

			/**
			 * @brief Zero-based const index access.
			 * @param i Index.
			 * @return Const reference to the element at @p i.
			 * @throws OutOfBoundsError If @p i is not less than `size()`.
			 */
			const_reference operator[](size_type i) const;

			/**
			 * @brief Inserts @p value via `push_back`, else `push_front`,
			 *        else associative `insert`.
			 * @tparam T Deduced argument type (lvalue or rvalue of
			 *         @ref value_type).
			 * @param value Element to add. Lvalues are copied; rvalues
			 *        are moved.
			 *
			 * One function template so clang-cl / MSVC STL do not
			 * instantiate `push_back(const unique_ptr&)` when the
			 * container holds move-only values. An lvalue requires
			 * @ref Type::CopyConstructible; an rvalue requires
			 * @ref Type::MoveConstructible.
			 */
			template<typename T>
			void add(T&& value)
			requires (
				(Type::LvalueReference<T> && Type::CopyConstructible<value_type>) ||
				(!Type::LvalueReference<T> && Type::MoveConstructible<value_type>)
			);

			/**
			 * @brief Inserts a temporary / braced value.
			 * @param value Element constructed at the call site, then moved.
			 *
			 * Needed because a forwarding reference cannot deduce
			 * from a braced-init-list (`add({"k", v})`).
			 */
			void add(value_type value)
			requires Type::MoveConstructible<value_type>;

			/**
			 * @brief Linear search for an equal element.
			 * @param value Value to find.
			 * @return `true` if found.
			 */
			bool has_item(const value_type& value) const;

			/**
			 * @brief Linear search over mapped values.
			 * @tparam M Comparable to `Container::mapped_type`.
			 * @param value Mapped value to find.
			 * @return `true` if any mapped value equals @p value.
			 */
			template<typename M>
			bool has_item(M const& value) const
			requires Type::HasMappedType<const Container> && std::convertible_to<M, typename Container::mapped_type>;

			/**
			 * @brief Key lookup for associative containers.
			 * @tparam K Convertible to `Container::key_type`.
			 * @param key Key to find.
			 * @return `true` if @p key is present.
			 */
			template<typename K>
			bool has_key(const K& key) const
			requires Type::HasMappedType<const Container>;
	};
}

#include <StormByte/iterable.txx>

