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
					using iterator_category = std::random_access_iterator_tag;	///< Category advertised to algorithms.
					using value_type = typename Container::value_type;			///< Element type.
					using difference_type = typename Container::difference_type;	///< Distance type.
					using pointer = typename Container::pointer;				///< Mutable pointer.
					using reference = typename Container::reference;			///< Mutable reference.

					/**
					 * @brief Dereference.
					 * @return Reference to the current element.
					 */
					reference operator*() { return *m_it; }

					/**
					 * @brief Member access.
					 * @return Pointer to the current element.
					 */
					pointer operator->() { return m_it.operator->(); }

					/**
					 * @brief Pre-increment.
					 * @return `*this` after advancing one.
					 */
					Iterator& operator++() { ++m_it; return *this; }

					/**
					 * @brief Post-increment.
					 * @return Copy before advancing.
					 */
					Iterator operator++(int) { Iterator tmp = *this; ++m_it; return tmp; }

					/**
					 * @brief Pre-decrement.
					 * @return `*this` after moving back one.
					 */
					Iterator& operator--() { --m_it; return *this; }

					/**
					 * @brief Post-decrement.
					 * @return Copy before moving back.
					 */
					Iterator operator--(int) { Iterator tmp = *this; --m_it; return tmp; }

					/**
					 * @brief Advance in place.
					 * @param n Positions (may be negative).
					 * @return `*this`.
					 */
					Iterator& operator+=(difference_type n) { m_it += n; return *this; }

					/**
					 * @brief Retreat in place.
					 * @param n Positions (may be negative).
					 * @return `*this`.
					 */
					Iterator& operator-=(difference_type n) { m_it -= n; return *this; }

					/**
					 * @brief Iterator advanced by @p n.
					 * @param n Positions.
					 */
					Iterator operator+(difference_type n) const { return Iterator(m_it + n); }

					/**
					 * @brief Iterator moved back by @p n.
					 * @param n Positions.
					 */
					Iterator operator-(difference_type n) const { return Iterator(m_it - n); }

					/**
					 * @brief Distance to @p other.
					 * @param other Iterator to subtract.
					 */
					difference_type operator-(const Iterator& other) const { return m_it - other.m_it; }

					/**
					 * @brief Equality.
					 * @param other Other iterator.
					 */
					bool operator==(const Iterator& other) const { return m_it == other.m_it; }

					/**
					 * @brief Inequality.
					 * @param other Other iterator.
					 */
					bool operator!=(const Iterator& other) const { return m_it != other.m_it; }

				private:
					typename Container::iterator m_it; ///< Wrapped iterator.

					/**
					 * @brief Wraps a container iterator.
					 * @param it `Container::iterator`.
					 */
					Iterator(typename Container::iterator it): m_it(it) {}
			};

			/**
			 * @class ConstIterator
			 * @brief Const iterator adapter over `Container::const_iterator`.
			 */
			class ConstIterator {
				friend class Iterable;
				public:
					using iterator_category = std::random_access_iterator_tag;	///< Category advertised to algorithms.
					using value_type = typename Container::value_type;			///< Element type.
					using difference_type = typename Container::difference_type;	///< Distance type.
					using pointer = typename Container::const_pointer;			///< Const pointer.
					using reference = typename Container::const_reference;		///< Const reference.

					/**
					 * @brief Dereference.
					 * @return Const reference to the current element.
					 */
					reference operator*() const { return *m_it; }

					/**
					 * @brief Member access.
					 * @return Const pointer to the current element.
					 */
					pointer operator->() const { return m_it.operator->(); }

					/**
					 * @brief Pre-increment.
					 * @return `*this` after advancing one.
					 */
					ConstIterator& operator++() { ++m_it; return *this; }

					/**
					 * @brief Post-increment.
					 * @return Copy before advancing.
					 */
					ConstIterator operator++(int) { ConstIterator tmp = *this; ++m_it; return tmp; }

					/**
					 * @brief Pre-decrement.
					 * @return `*this` after moving back one.
					 */
					ConstIterator& operator--() { --m_it; return *this; }

					/**
					 * @brief Post-decrement.
					 * @return Copy before moving back.
					 */
					ConstIterator operator--(int) { ConstIterator tmp = *this; --m_it; return tmp; }

					/**
					 * @brief Advance in place.
					 * @param n Positions (may be negative).
					 * @return `*this`.
					 */
					ConstIterator& operator+=(difference_type n) { m_it += n; return *this; }

					/**
					 * @brief Retreat in place.
					 * @param n Positions (may be negative).
					 * @return `*this`.
					 */
					ConstIterator& operator-=(difference_type n) { m_it -= n; return *this; }

					/**
					 * @brief Iterator advanced by @p n.
					 * @param n Positions.
					 */
					ConstIterator operator+(difference_type n) const { return ConstIterator(m_it + n); }

					/**
					 * @brief Iterator moved back by @p n.
					 * @param n Positions.
					 */
					ConstIterator operator-(difference_type n) const { return ConstIterator(m_it - n); }

					/**
					 * @brief Distance to @p other.
					 * @param other Iterator to subtract.
					 */
					difference_type operator-(const ConstIterator& other) const { return m_it - other.m_it; }

					/**
					 * @brief Equality.
					 * @param other Other iterator.
					 */
					bool operator==(const ConstIterator& other) const { return m_it == other.m_it; }

					/**
					 * @brief Inequality.
					 * @param other Other iterator.
					 */
					bool operator!=(const ConstIterator& other) const { return m_it != other.m_it; }

				private:
					typename Container::const_iterator m_it; ///< Wrapped const iterator.

					/**
					 * @brief Wraps a container const iterator.
					 * @param it `Container::const_iterator`.
					 */
					ConstIterator(typename Container::const_iterator it): m_it(it) {}
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
			 * @brief Copy from a container.
			 * @param data Container to copy.
			 */
			explicit Iterable(const Container& data): m_data(data) {}

			/**
			 * @brief Move from a container.
			 * @param data Container to move.
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
			 * @brief Destructor.
			 */
			virtual ~Iterable() = default;

			/**
			 * @brief Copy assignment.
			 * @return `*this`.
			 */
			Iterable& operator=(const Iterable&) = default;

			/**
			 * @brief Move assignment.
			 * @return `*this`.
			 */
			Iterable& operator=(Iterable&&) = default;

			/**
			 * @brief Equality of the underlying containers.
			 * @param other Other iterable.
			 */
			bool operator==(const Iterable& other) const { return m_data == other.m_data; }

			/**
			 * @brief Inequality of the underlying containers.
			 * @param other Other iterable.
			 */
			bool operator!=(const Iterable& other) const { return m_data != other.m_data; }

			/**
			 * @brief Mutable begin.
			 */
			iterator begin() noexcept { return iterator(m_data.begin()); }

			/**
			 * @brief Const begin.
			 */
			const_iterator begin() const noexcept { return const_iterator(m_data.begin()); }

			/**
			 * @brief Mutable end.
			 */
			iterator end() noexcept { return iterator(m_data.end()); }

			/**
			 * @brief Const end.
			 */
			const_iterator end() const noexcept { return const_iterator(m_data.end()); }

			/**
			 * @brief Const begin.
			 */
			const_iterator cbegin() const noexcept { return const_iterator(m_data.cbegin()); }

			/**
			 * @brief Const end.
			 */
			const_iterator cend() const noexcept { return const_iterator(m_data.cend()); }

			/**
			 * @brief Mutable reverse begin.
			 */
			reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }

			/**
			 * @brief Mutable reverse end.
			 */
			reverse_iterator rend() noexcept { return reverse_iterator(begin()); }

			/**
			 * @brief Const reverse begin.
			 */
			const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }

			/**
			 * @brief Const reverse end.
			 */
			const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

			/**
			 * @brief Const reverse begin.
			 */
			const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(cend()); }

			/**
			 * @brief Const reverse end.
			 */
			const_reverse_iterator crend() const noexcept { return const_reverse_iterator(cbegin()); }

			/**
			 * @brief Element count.
			 */
			size_type size() const noexcept { return m_data.size(); }

			/**
			 * @brief Whether the container is empty.
			 */
			bool empty() const noexcept { return m_data.empty(); }

			/**
			 * @brief Zero-based mutable index access.
			 * @param i Index.
			 * @return Reference to the element at @p i.
			 * @throws OutOfBoundsError If @p i is not less than `size()`.
			 *
			 * Uses `Container::operator[]` when `Type::HasSubscript` holds; otherwise advances iterators.
			 */
			reference operator[](size_type i) {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				if constexpr (Type::HasSubscript<Container, size_type>) {
					return m_data[i];
				} else {
					auto it = m_data.begin();
					std::advance(it, static_cast<difference_type>(i));
					return *it;
				}
			}

			/**
			 * @brief Mutable key access for associative containers.
			 * @tparam K Convertible to `Container::key_type`.
			 * @param key Key to look up or insert.
			 * @return Mapped value (`Container::operator[]`).
			 */
			template<typename K>
			auto operator[](K const& key) -> decltype(auto)
			requires (Type::HasMappedType<Container>) {
				return m_data[static_cast<typename Container::key_type>(key)];
			}

			/**
			 * @brief Const key access for associative containers.
			 * @tparam K Convertible to `Container::key_type`.
			 * @param key Key to look up.
			 * @return Const mapped value.
			 * @throws OutOfBoundsError If @p key is missing.
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
			 * @brief Zero-based const index access.
			 * @param i Index.
			 * @return Const reference to the element at @p i.
			 * @throws OutOfBoundsError If @p i is not less than `size()`.
			 */
			const_reference operator[](size_type i) const {
				if (i >= m_data.size())
					throw OutOfBoundsError("Index {} out of bounds in Iterable::operator[]", i);
				if constexpr (Type::HasSubscript<const Container, size_type>) {
					return m_data[i];
				} else {
					auto it = m_data.cbegin();
					std::advance(it, static_cast<difference_type>(i));
					return *it;
				}
			}

			/**
			 * @brief Inserts a copy via `push_back`, else `push_front`, else associative `insert`.
			 * @param value Element to add.
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
			 * @brief Inserts by move via `push_back`, else `push_front`, else associative `insert`.
			 * @param value Element to add.
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
			 * @brief Linear search for an equal element.
			 * @param value Value to find.
			 * @return `true` if found.
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
			 * @brief Linear search over mapped values.
			 * @tparam M Comparable to `Container::mapped_type`.
			 * @param value Mapped value to find.
			 * @return `true` if any mapped value equals @p value.
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
			 * @tparam K Convertible to `Container::key_type`.
			 * @param key Key to find.
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
