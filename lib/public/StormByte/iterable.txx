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

// Out-of-line implementation of StormByte::Iterable (and its nested Iterator /
// ConstIterator adapters), included by iterable.hxx.
// See iterable.hxx for documentation of each member.

namespace StormByte {
	// --- Iterable<Container>::Iterator ---

	template <typename Container>
	typename Iterable<Container>::Iterator::reference Iterable<Container>::Iterator::operator*() { return *m_it; }

	template <typename Container>
	typename Iterable<Container>::Iterator::pointer Iterable<Container>::Iterator::operator->() { return m_it.operator->(); }

	template <typename Container>
	typename Iterable<Container>::Iterator& Iterable<Container>::Iterator::operator++() { ++m_it; return *this; }

	template <typename Container>
	typename Iterable<Container>::Iterator Iterable<Container>::Iterator::operator++(int) { Iterator tmp = *this; ++m_it; return tmp; }

	template <typename Container>
	typename Iterable<Container>::Iterator& Iterable<Container>::Iterator::operator--() { --m_it; return *this; }

	template <typename Container>
	typename Iterable<Container>::Iterator Iterable<Container>::Iterator::operator--(int) { Iterator tmp = *this; --m_it; return tmp; }

	template <typename Container>
	typename Iterable<Container>::Iterator& Iterable<Container>::Iterator::operator+=(difference_type n) { m_it += n; return *this; }

	template <typename Container>
	typename Iterable<Container>::Iterator& Iterable<Container>::Iterator::operator-=(difference_type n) { m_it -= n; return *this; }

	template <typename Container>
	typename Iterable<Container>::Iterator Iterable<Container>::Iterator::operator+(difference_type n) const { return Iterator(m_it + n); }

	template <typename Container>
	typename Iterable<Container>::Iterator Iterable<Container>::Iterator::operator-(difference_type n) const { return Iterator(m_it - n); }

	template <typename Container>
	typename Iterable<Container>::Iterator::difference_type Iterable<Container>::Iterator::operator-(const Iterator& other) const { return m_it - other.m_it; }

	template <typename Container>
	bool Iterable<Container>::Iterator::operator==(const Iterator& other) const { return m_it == other.m_it; }

	template <typename Container>
	bool Iterable<Container>::Iterator::operator!=(const Iterator& other) const { return m_it != other.m_it; }

	template <typename Container>
	Iterable<Container>::Iterator::Iterator(typename Container::iterator it): m_it(it) {}

	// --- Iterable<Container>::ConstIterator ---

	template <typename Container>
	typename Iterable<Container>::ConstIterator::reference Iterable<Container>::ConstIterator::operator*() const { return *m_it; }

	template <typename Container>
	typename Iterable<Container>::ConstIterator::pointer Iterable<Container>::ConstIterator::operator->() const { return m_it.operator->(); }

	template <typename Container>
	typename Iterable<Container>::ConstIterator& Iterable<Container>::ConstIterator::operator++() { ++m_it; return *this; }

	template <typename Container>
	typename Iterable<Container>::ConstIterator Iterable<Container>::ConstIterator::operator++(int) { ConstIterator tmp = *this; ++m_it; return tmp; }

	template <typename Container>
	typename Iterable<Container>::ConstIterator& Iterable<Container>::ConstIterator::operator--() { --m_it; return *this; }

	template <typename Container>
	typename Iterable<Container>::ConstIterator Iterable<Container>::ConstIterator::operator--(int) { ConstIterator tmp = *this; --m_it; return tmp; }

	template <typename Container>
	typename Iterable<Container>::ConstIterator& Iterable<Container>::ConstIterator::operator+=(difference_type n) { m_it += n; return *this; }

	template <typename Container>
	typename Iterable<Container>::ConstIterator& Iterable<Container>::ConstIterator::operator-=(difference_type n) { m_it -= n; return *this; }

	template <typename Container>
	typename Iterable<Container>::ConstIterator Iterable<Container>::ConstIterator::operator+(difference_type n) const { return ConstIterator(m_it + n); }

	template <typename Container>
	typename Iterable<Container>::ConstIterator Iterable<Container>::ConstIterator::operator-(difference_type n) const { return ConstIterator(m_it - n); }

	template <typename Container>
	typename Iterable<Container>::ConstIterator::difference_type Iterable<Container>::ConstIterator::operator-(const ConstIterator& other) const { return m_it - other.m_it; }

	template <typename Container>
	bool Iterable<Container>::ConstIterator::operator==(const ConstIterator& other) const { return m_it == other.m_it; }

	template <typename Container>
	bool Iterable<Container>::ConstIterator::operator!=(const ConstIterator& other) const { return m_it != other.m_it; }

	template <typename Container>
	Iterable<Container>::ConstIterator::ConstIterator(typename Container::const_iterator it): m_it(it) {}

	// --- Iterable<Container> ---

	template <typename Container>
	template <typename C>
	Iterable<Container>::Iterable(C&& data)
	requires Type::SameAs<C, Container> && (
		(Type::LvalueReference<C> && Type::CopyConstructible<value_type>) ||
		(!Type::LvalueReference<C> && Type::MoveConstructible<value_type>)
	): m_data(std::forward<C>(data)) {}

	template <typename Container>
	bool Iterable<Container>::operator==(const Iterable& other) const { return m_data == other.m_data; }

	template <typename Container>
	bool Iterable<Container>::operator!=(const Iterable& other) const { return m_data != other.m_data; }

	template <typename Container>
	typename Iterable<Container>::iterator Iterable<Container>::begin() noexcept { return iterator(m_data.begin()); }

	template <typename Container>
	typename Iterable<Container>::const_iterator Iterable<Container>::begin() const noexcept { return const_iterator(m_data.begin()); }

	template <typename Container>
	typename Iterable<Container>::iterator Iterable<Container>::end() noexcept { return iterator(m_data.end()); }

	template <typename Container>
	typename Iterable<Container>::const_iterator Iterable<Container>::end() const noexcept { return const_iterator(m_data.end()); }

	template <typename Container>
	typename Iterable<Container>::const_iterator Iterable<Container>::cbegin() const noexcept { return const_iterator(m_data.cbegin()); }

	template <typename Container>
	typename Iterable<Container>::const_iterator Iterable<Container>::cend() const noexcept { return const_iterator(m_data.cend()); }

	template <typename Container>
	typename Iterable<Container>::reverse_iterator Iterable<Container>::rbegin() noexcept { return reverse_iterator(end()); }

	template <typename Container>
	typename Iterable<Container>::reverse_iterator Iterable<Container>::rend() noexcept { return reverse_iterator(begin()); }

	template <typename Container>
	typename Iterable<Container>::const_reverse_iterator Iterable<Container>::rbegin() const noexcept { return const_reverse_iterator(end()); }

	template <typename Container>
	typename Iterable<Container>::const_reverse_iterator Iterable<Container>::rend() const noexcept { return const_reverse_iterator(begin()); }

	template <typename Container>
	typename Iterable<Container>::const_reverse_iterator Iterable<Container>::crbegin() const noexcept { return const_reverse_iterator(cend()); }

	template <typename Container>
	typename Iterable<Container>::const_reverse_iterator Iterable<Container>::crend() const noexcept { return const_reverse_iterator(cbegin()); }

	template <typename Container>
	typename Iterable<Container>::size_type Iterable<Container>::size() const noexcept { return m_data.size(); }

	template <typename Container>
	bool Iterable<Container>::empty() const noexcept { return m_data.empty(); }

	template <typename Container>
	typename Iterable<Container>::reference Iterable<Container>::operator[](size_type i) {
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

	template <typename Container>
	template<typename K>
	auto Iterable<Container>::operator[](K const& key) -> decltype(auto)
	requires (Type::HasMappedType<Container>) {
		return m_data[static_cast<typename Container::key_type>(key)];
	}

	template <typename Container>
	template<typename K>
	auto Iterable<Container>::operator[](K const& key) const -> decltype(auto)
	requires (Type::HasMappedType<const Container>) {
		auto k = static_cast<typename Container::key_type>(key);
		auto it = m_data.find(k);
		if (it == m_data.cend())
			throw OutOfBoundsError("Key not found in Iterable::operator[]");
		return it->second;
	}

	template <typename Container>
	typename Iterable<Container>::const_reference Iterable<Container>::operator[](size_type i) const {
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

	template <typename Container>
	template <typename T>
	void Iterable<Container>::add(T&& value)
	requires (
		(Type::LvalueReference<T> && Type::CopyConstructible<value_type>) ||
		(!Type::LvalueReference<T> && Type::MoveConstructible<value_type>)
	) {
		if constexpr (Type::HasPushBack<Container>) {
			m_data.push_back(std::forward<T>(value));
		} else if constexpr (Type::HasPushFront<Container>) {
			m_data.push_front(std::forward<T>(value));
		} else if constexpr (Type::HasInsert<Container>) {
			m_data.insert(std::forward<T>(value));
		} else {
			static_assert(Type::HasPushBack<Container> || Type::HasPushFront<Container> || Type::HasInsert<Container>,
				"StormByte::Iterable: container must support push_back, push_front, or insert");
		}
	}

	template <typename Container>
	void Iterable<Container>::add(value_type value)
	requires Type::MoveConstructible<value_type> {
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

	template <typename Container>
	bool Iterable<Container>::has_item(const value_type& value) const {
		for (const auto& item : m_data) {
			if (item == value) {
				return true;
			}
		}
		return false;
	}

	template <typename Container>
	template<typename M>
	bool Iterable<Container>::has_item(M const& value) const
	requires Type::HasMappedType<const Container> && std::convertible_to<M, typename Container::mapped_type> {
		for (const auto& item : m_data) {
			if (item.second == value) return true;
		}
		return false;
	}

	template <typename Container>
	template<typename K>
	bool Iterable<Container>::has_key(const K& key) const
	requires Type::HasMappedType<const Container> {
		auto k = static_cast<typename Container::key_type>(key);
		return m_data.find(k) != m_data.cend();
	}
}
