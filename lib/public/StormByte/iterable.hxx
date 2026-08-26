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

#pragma once

#include <StormByte/exception.hxx>
#include <StormByte/type_traits.hxx>

#include <iterator>
#include <utility>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte library.
 */
namespace StormByte {
	/**
	 * @class Iterable
	 * @brief Generic wrapper that adds a uniform iteration and mutation API around a standard container.
	 * @tparam Container Underlying container type (e.g. `std::vector`, `std::map`, `std::deque`).
	 *
	 * Provides iterator adapters, size/empty queries, subscript access (index or key, depending on
	 * @p Container) and `add()` selected by @ref Type concepts so the correct insertion
	 * API (`push_back`, `push_front` or associative `insert`) is used on every standard library.
	 *
	 * Insertion uses a single `add` with `if constexpr` (not constrained overloads) so MSVC STL
	 * and clang-cl do not instantiate bodies that call APIs the container does not provide.
	 *
	 * Constraints use the template parameter @p Container directly (not `decltype(m_data)`) so
	 * concept checks remain stable under the MSVC STL and clang-cl.
	 */
	template <typename Container>
	class Iterable {
		protected:
			Container m_data; ///< Underlying container storage.

		public:
			using value_type = typename Container::value_type;
			using size_type = typename Container::size_type;
			using difference_type = typename Container::difference_type;
			using reference = typename Container::reference;
			using const_reference = typename Container::const_reference;
			using pointer = typename Container::pointer;
			using const_pointer = typename Container::const_pointer;

			/**
			 * @class Iterator
			 * @brief Mutable random-access style iterator adapter for @ref Iterable.
			 *
			 * Wraps `Container::iterator` and exposes the usual increment, decrement and
			 * arithmetic operators. Equality is defined in terms of the underlying iterator.
			 */
			class Iterator {
				friend class Iterable;
				public:
					using iterator_category = std::random_access_iterator_tag;
					using value_type = typename Container::value_type;
					using difference_type = typename Container::difference_type;
					using pointer = typename Container::pointer;
					using reference = typename Container::reference;

					/**
					 * @brief Dereferences the iterator.
					 * @return Reference to the current element.
					 */
					reference operator*() { return *m_it; }

					/**
					 * @brief Member access through the iterator.
					 * @return Pointer to the current element.
					 */
					pointer operator->() { return m_it.operator->(); }

					/**
					 * @brief Pre-increment.
					 * @return Reference to this iterator after advancing one position.
					 */
					Iterator& operator++() { ++m_it; return *this; }

					/**
					 * @brief Post-increment.
					 * @return Copy of the iterator before advancing.
					 */
					Iterator operator++(int) { Iterator tmp = *this; ++m_it; return tmp; }

					/**
					 * @brief Pre-decrement.
					 * @return Reference to this iterator after moving one position back.
					 */
					Iterator& operator--() { --m_it; return *this; }

					/**
					 * @brief Post-decrement.
					 * @return Copy of the iterator before moving back.
					 */
					Iterator operator--(int) { Iterator tmp = *this; --m_it; return tmp; }

					/**
					 * @brief Advances this iterator by @p n positions.
					 * @param n Number of positions to advance (may be negative).
					 * @return Reference to this iterator.
					 */
					Iterator& operator+=(difference_type n) { m_it += n; return *this; }

					/**
					 * @brief Moves this iterator back by @p n positions.
					 * @param n Number of positions to retreat (may be negative).
					 * @return Reference to this iterator.
					 */
					Iterator& operator-=(difference_type n) { m_it -= n; return *this; }

					/**
					 * @brief Returns a new iterator advanced by @p n positions.
					 * @param n Number of positions to advance.
					 * @return New iterator at the computed position.
					 */
					Iterator operator+(difference_type n) const { return Iterator(m_it + n); }

					/**
					 * @brief Returns a new iterator moved back by @p n positions.
					 * @param n Number of positions to retreat.
					 * @return New iterator at the computed position.
					 */
					Iterator operator-(difference_type n) const { return Iterator(m_it - n); }

					/**
					 * @brief Distance between this iterator and @p other.
					 * @param other Iterator to subtract.
					 * @return Number of positions from @p other to this iterator.
					 */
					difference_type operator-(const Iterator& other) const { return m_it - other.m_it; }

					/**
					 * @brief Equality comparison.
					 * @param other Iterator to compare with.
					 * @return `true` if both refer to the same element.
					 */
					bool operator==(const Iterator& other) const { return m_it == other.m_it; }

					/**
					 * @brief Inequality comparison.
					 * @param other Iterator to compare with.
					 * @return `true` if the iterators refer to different elements.
					 */
					bool operator!=(const Iterator& other) const { return m_it != other.m_it; }

				private:
					typename Container::iterator m_it; ///< Underlying container iterator.

					/**
					 * @brief Constructs an iterator from a container iterator.
					 * @param it Underlying `Container::iterator`.
					 */
					Iterator(typename Container::iterator it): m_it(it) {}
			};

			/**
			 * @class ConstIterator
			 * @brief Const random-access style iterator adapter for @ref Iterable.
			 *
			 * Wraps `Container::const_iterator`. Same interface as @ref Iterator but yields
			 * const references and pointers.
			 */
			class ConstIterator {
				friend class Iterable;
				public:
					using iterator_category = std::random_access_iterator_tag;
					using value_type = typename Container::value_type;
					using difference_type = typename Container::difference_type;
					using pointer = typename Container::const_pointer;
					using reference = typename Container::const_reference;

					/**
					 * @brief Dereferences the iterator.
					 * @return Const reference to the current element.
					 */
					reference operator*() const { return *m_it; }

					/**
					 * @brief Member access through the iterator.
					 * @return Const pointer to the current element.
					 */
					pointer operator->() const { return m_it.operator->(); }

					/**
					 * @brief Pre-increment.
					 * @return Reference to this iterator after advancing one position.
					 */
					ConstIterator& operator++() { ++m_it; return *this; }

					/**
					 * @brief Post-increment.
					 * @return Copy of the iterator before advancing.
					 */
					ConstIterator operator++(int) { ConstIterator tmp = *this; ++m_it; return tmp; }

					/**
					 * @brief Pre-decrement.
					 * @return Reference to this iterator after moving one position back.
					 */
					ConstIterator& operator--() { --m_it; return *this; }

					/**
					 * @brief Post-decrement.
					 * @return Copy of the iterator before moving back.
					 */
					ConstIterator operator--(int) { ConstIterator tmp = *this; --m_it; return tmp; }

					/**
					 * @brief Advances this iterator by @p n positions.
					 * @param n Number of positions to advance (may be negative).
					 * @return Reference to this iterator.
					 */
					ConstIterator& operator+=(difference_type n) { m_it += n; return *this; }

					/**
					 * @brief Moves this iterator back by @p n positions.
					 * @param n Number of positions to retreat (may be negative).
					 * @return Reference to this iterator.
					 */
					ConstIterator& operator-=(difference_type n) { m_it -= n; return *this; }

					/**
					 * @brief Returns a new iterator advanced by @p n positions.
					 * @param n Number of positions to advance.
					 * @return New const iterator at the computed position.
					 */
					ConstIterator operator+(difference_type n) const { return ConstIterator(m_it + n); }

					/**
					 * @brief Returns a new iterator moved back by @p n positions.
					 * @param n Number of positions to retreat.
					 * @return New const iterator at the computed position.
					 */
					ConstIterator operator-(difference_type n) const { return ConstIterator(m_it - n); }

					/**
					 * @brief Distance between this iterator and @p other.
					 * @param other Const iterator to subtract.
					 * @return Number of positions from @p other to this iterator.
					 */
					difference_type operator-(const ConstIterator& other) const { return m_it - other.m_it; }

					/**
					 * @brief Equality comparison.
					 * @param other Const iterator to compare with.
					 * @return `true` if both refer to the same element.
					 */
					bool operator==(const ConstIterator& other) const { return m_it == other.m_it; }

					/**
					 * @brief Inequality comparison.
					 * @param other Const iterator to compare with.
					 * @return `true` if the iterators refer to different elements.
					 */
					bool operator!=(const ConstIterator& other) const { return m_it != other.m_it; }

				private:
					typename Container::const_iterator m_it; ///< Underlying container const iterator.

					/**
					 * @brief Constructs a const iterator from a container const iterator.
					 * @param it Underlying `Container::const_iterator`.
					 */
					ConstIterator(typename Container::const_iterator it): m_it(it) {}
			};

			using iterator = Iterator;
			using const_iterator = ConstIterator;
			using reverse_iterator = std::reverse_iterator<iterator>;
			using const_reverse_iterator = std::reverse_iterator<const_iterator>;

			/**
			 * @brief Default-constructs an empty iterable.
			 */
			Iterable() = default;

			/**
			 * @brief Constructs from a copy of an existing container.
			 * @param data Container to copy.
			 */
			explicit Iterable(const Container& data): m_data(data) {}

			/**
			 * @brief Constructs by moving an existing container.
			 * @param data Container to move from.
			 */
			explicit Iterable(Container&& data): m_data(std::move(data)) {}

			/**
			 * @brief Copy constructor.
			 */
			Iterable(const Iterable&) = default;

			/**
			 * @brief Move constructor.
			 */
			Iterable(Iterable&&) = default;

			/**
			 * @brief Virtual destructor.
			 */
			virtual ~Iterable() = default;

			/**
			 * @brief Copy assignment.
			 * @return Reference to this object.
			 */
			Iterable& operator=(const Iterable&) = default;

			/**
			 * @brief Move assignment.
			 * @return Reference to this object.
			 */
			Iterable& operator=(Iterable&&) = default;

			/**
			 * @brief Equality comparison of the underlying containers.
			 * @param other Iterable to compare with.
			 * @return `true` if the underlying containers compare equal.
			 */
			bool operator==(const Iterable& other) const { return m_data == other.m_data; }

			/**
			 * @brief Inequality comparison of the underlying containers.
			 * @param other Iterable to compare with.
			 * @return `true` if the underlying containers are not equal.
			 */
			bool operator!=(const Iterable& other) const { return m_data != other.m_data; }

			/**
			 * @brief Mutable begin iterator.
			 * @return Iterator to the first element.
			 */
			iterator begin() noexcept { return iterator(m_data.begin()); }

			/**
			 * @brief Const begin iterator.
			 * @return Const iterator to the first element.
			 */
			const_iterator begin() const noexcept { return const_iterator(m_data.begin()); }

			/**
			 * @brief Mutable end iterator.
			 * @return Iterator past the last element.
			 */
			iterator end() noexcept { return iterator(m_data.end()); }

			/**
			 * @brief Const end iterator.
			 * @return Const iterator past the last element.
			 */
			const_iterator end() const noexcept { return const_iterator(m_data.end()); }

			/**
			 * @brief Const begin iterator.
			 * @return Const iterator to the first element.
			 */
			const_iterator cbegin() const noexcept { return const_iterator(m_data.cbegin()); }

			/**
			 * @brief Const end iterator.
			 * @return Const iterator past the last element.
			 */
			const_iterator cend() const noexcept { return const_iterator(m_data.cend()); }

			/**
			 * @brief Mutable reverse begin iterator.
			 * @return Reverse iterator to the last element.
			 */
			reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

			/**
			 * @brief Mutable reverse end iterator.
			 * @return Reverse iterator before the first element.
			 */
			reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

			/**
			 * @brief Const reverse begin iterator.
			 * @return Const reverse iterator to the last element.
			 */
			const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

			/**
			 * @brief Const reverse end iterator.
			 * @return Const reverse iterator before the first element.
			 */
			const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

			/**
			 * @brief Const reverse begin iterator.
			 * @return Const reverse iterator to the last element.
			 */
			const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

			/**
			 * @brief Const reverse end iterator.
			 * @return Const reverse iterator before the first element.
			 */
			const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

			/**
			 * @brief Number of elements in the underlying container.
			 * @return Element count.
			 */
			size_type size() const noexcept { return m_data.size(); }

			/**
			 * @brief Whether the underlying container holds no elements.
			 * @return `true` if empty.
			 */
			bool empty() const noexcept { return m_data.empty(); }

			/**
			 * @brief Index access when the container supports `operator[]`.
			 * @param i Zero-based index.
			 * @return Reference to the element at @p i.
			 * @throws OutOfBoundsError If @p i is not less than `size()`.
			 */
			reference operator[](size_type i)
			requires Type::HasSubscript<Container, size_type> {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				return m_data[i];
			}

			/**
			 * @brief Index access by advancing iterators when `operator[]` is unavailable.
			 * @param i Zero-based index.
			 * @return Reference to the element at @p i.
			 * @throws OutOfBoundsError If @p i is not less than `size()`.
			 */
			reference operator[](size_type i)
			requires (!Type::HasSubscript<Container, size_type>) {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				auto it = m_data.begin();
				std::advance(it, i);
				return *it;
			}

			/**
			 * @brief Index access for non-subscript, non-associative containers (deduced return).
			 * @param i Zero-based index.
			 * @return Element at @p i.
			 * @throws OutOfBoundsError If @p i is not less than `size()`.
			 */
			auto operator[](size_type i) -> decltype(auto)
			requires (!Type::HasSubscript<Container, size_type> && !Type::HasMappedType<Container>) {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				auto it = m_data.begin();
				std::advance(it, i);
				return *it;
			}

			/**
			 * @brief Key-based mutable access for associative containers.
			 * @tparam K Key type convertible to `Container::key_type`.
			 * @param key Key to look up or insert.
			 * @return Reference to the mapped value (`Container::operator[]`).
			 */
			template<typename K>
			auto operator[](K const& key) -> decltype(auto)
			requires (Type::HasMappedType<Container>) {
				return m_data[static_cast<typename Container::key_type>(key)];
			}

			/**
			 * @brief Key-based const access for associative containers.
			 * @tparam K Key type convertible to `Container::key_type`.
			 * @param key Key to look up.
			 * @return Const reference to the mapped value.
			 * @throws OutOfBoundsError If @p key is not present.
			 */
			template<typename K>
			auto operator[](K const& key) const -> decltype(auto)
			requires (Type::HasMappedType<const Container>) {
				auto k = static_cast<typename Container::key_type>(key);
				auto it = m_data.find(k);
				if (it == m_data.cend())
					throw OutOfBoundsError("Key not found in Iterable::operator[]");
				return it->second;
			}

			/**
			 * @brief Const index access when the container supports `operator[]`.
			 * @param i Zero-based index.
			 * @return Const reference to the element at @p i.
			 * @throws OutOfBoundsError If @p i is not less than `size()`.
			 */
			const_reference operator[](size_type i) const
			requires Type::HasSubscript<const Container, size_type> {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				return m_data[i];
			}

			/**
			 * @brief Const index access by advancing iterators when `operator[]` is unavailable.
			 * @param i Zero-based index.
			 * @return Const reference to the element at @p i.
			 * @throws OutOfBoundsError If @p i is not less than `size()`.
			 */
			const_reference operator[](size_type i) const
			requires (!Type::HasSubscript<const Container, size_type>) {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				auto it = m_data.cbegin();
				std::advance(it, i);
				return *it;
			}

			/**
			 * @brief Const index access for non-subscript, non-associative containers (deduced return).
			 * @param i Zero-based index.
			 * @return Element at @p i.
			 * @throws OutOfBoundsError If @p i is not less than `size()`.
			 */
			auto operator[](size_type i) const -> decltype(auto)
			requires (!Type::HasSubscript<const Container, size_type> && !Type::HasMappedType<const Container>) {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				auto it = m_data.cbegin();
				std::advance(it, i);
				return *it;
			}

			/**
			 * @brief Appends a copy of @p value using the best available container API.
			 * @param value Element to add.
			 *
			 * Prefers `push_back`, then `push_front`, then associative `insert`.
			 * Implemented with `if constexpr` so clang-cl / MSVC STL never instantiate
			 * insertion paths the container does not support.
			 */
			void add(const value_type& value) {
				if constexpr (Type::HasPushBack<Container>) {
					m_data.push_back(value);
				} else if constexpr (Type::HasPushFront<Container>) {
					m_data.push_front(value);
				} else if constexpr (Type::HasInsert<Container>) {
					m_data.insert(value);
				} else {
					static_assert(Type::HasPushBack<Container> || Type::HasPushFront<Container> || Type::HasInsert<Container>,
						"StormByte::Iterable: container must support push_back, push_front, or insert");
				}
			}

			/**
			 * @brief Appends @p value by move using the best available container API.
			 * @param value Element to add.
			 *
			 * Same selection rules as the const overload; uses moves into the container.
			 */
			void add(value_type&& value) {
				if constexpr (Type::HasPushBack<Container>) {
					m_data.push_back(std::move(value));
				} else if constexpr (Type::HasPushFront<Container>) {
					m_data.push_front(std::move(value));
				} else if constexpr (Type::HasInsert<Container>) {
					m_data.insert(std::move(value));
				} else {
					static_assert(Type::HasPushBack<Container> || Type::HasPushFront<Container> || Type::HasInsert<Container>,
						"StormByte::Iterable: container must support push_back, push_front, or insert");
				}
			}

			/**
			 * @brief Linear search for an element equal to @p value.
			 * @param value Value to look for.
			 * @return `true` if an equal element exists.
			 */
			bool has_item(const value_type& value) const {
				for (const auto& item : m_data) {
					if (item == value) {
						return true;
					}
				}
				return false;
			}

			/**
			 * @brief Linear search over mapped values of an associative container.
			 * @tparam M Type comparable to `Container::mapped_type`.
			 * @param value Mapped value to look for.
			 * @return `true` if any element has a mapped value equal to @p value.
			 */
			template<typename M>
			bool has_item(M const& value) const
			requires Type::HasMappedType<const Container> && std::convertible_to<M, typename Container::mapped_type> {
				for (const auto& item : m_data) {
					if (item.second == value) return true;
				}
				return false;
			}

			/**
			 * @brief Key lookup for associative containers.
			 * @tparam K Key type convertible to `Container::key_type`.
			 * @param key Key to look for.
			 * @return `true` if @p key is present.
			 */
			template<typename K>
			bool has_key(const K& key) const
			requires Type::HasMappedType<const Container> {
				auto k = static_cast<typename Container::key_type>(key);
				return m_data.find(k) != m_data.cend();
			}
	};
}
