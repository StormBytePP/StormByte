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

#include <StormByte/binary_data.hxx>

#include <algorithm>
#include <utility>
#include <vector>

using namespace StormByte;

class BinaryData::Storage {
	public:
		std::vector<std::byte> bytes;
};

BinaryData::BinaryData() noexcept
	: m_storage(std::make_unique<Storage>()) {}

BinaryData::BinaryData(const Size& count, std::byte value)
	: m_storage(std::make_unique<Storage>()) {
	m_storage->bytes.assign(static_cast<std::size_t>(count), value);
}

BinaryData::BinaryData(const Size& count)
	: BinaryData(count, std::byte{0}) {}

BinaryData::BinaryData(std::span<const std::byte> bytes)
	: BinaryData(bytes.data(), Size{bytes.size()}) {}

BinaryData::BinaryData(const std::byte* bytes, const Size& count)
	: m_storage(std::make_unique<Storage>()) {
	if (count > Size{0} && bytes != nullptr)
		m_storage->bytes.assign(bytes, bytes + static_cast<std::size_t>(count));
}

BinaryData::BinaryData(std::initializer_list<std::byte> list)
	: m_storage(std::make_unique<Storage>()) {
	m_storage->bytes.assign(list);
}

BinaryData::BinaryData(std::string_view sv)
	: m_storage(std::make_unique<Storage>()) {
	m_storage->bytes.reserve(sv.size());
	for (char c : sv)
		m_storage->bytes.push_back(static_cast<std::byte>(c));
}

BinaryData::BinaryData(const char* s)
	: BinaryData(s ? std::string_view(s) : std::string_view{}) {}

BinaryData::BinaryData(const BinaryData& other)
	: m_storage(std::make_unique<Storage>()) {
	m_storage->bytes = other.m_storage->bytes;
}

BinaryData::BinaryData(BinaryData&& other) noexcept
	: m_storage(std::move(other.m_storage)) {
	other.m_storage = std::make_unique<Storage>();
}

BinaryData::~BinaryData() noexcept = default;

BinaryData& BinaryData::operator=(const BinaryData& other) {
	if (this != &other)
		m_storage->bytes = other.m_storage->bytes;
	return *this;
}

BinaryData& BinaryData::operator=(BinaryData&& other) noexcept {
	if (this != &other) {
		m_storage = std::move(other.m_storage);
		other.m_storage = std::make_unique<Storage>();
	}
	return *this;
}

BinaryData& BinaryData::operator=(std::initializer_list<std::byte> list) {
	assign(list);
	return *this;
}

bool BinaryData::operator==(const BinaryData& other) const noexcept {
	return m_storage->bytes == other.m_storage->bytes;
}

bool BinaryData::operator!=(const BinaryData& other) const noexcept {
	return !(*this == other);
}

std::strong_ordering BinaryData::operator<=>(const BinaryData& other) const noexcept {
	return std::lexicographical_compare_three_way(
		m_storage->bytes.begin(), m_storage->bytes.end(),
		other.m_storage->bytes.begin(), other.m_storage->bytes.end());
}

BinaryData::iterator BinaryData::begin() noexcept {
	return data();
}

BinaryData::const_iterator BinaryData::begin() const noexcept {
	return data();
}

BinaryData::iterator BinaryData::end() noexcept {
	return data() + static_cast<std::size_t>(size());
}

BinaryData::const_iterator BinaryData::end() const noexcept {
	return data() + static_cast<std::size_t>(size());
}

BinaryData::const_iterator BinaryData::cbegin() const noexcept {
	return begin();
}

BinaryData::const_iterator BinaryData::cend() const noexcept {
	return end();
}

BinaryData::reverse_iterator BinaryData::rbegin() noexcept {
	return reverse_iterator(end());
}

BinaryData::reverse_iterator BinaryData::rend() noexcept {
	return reverse_iterator(begin());
}

BinaryData::const_reverse_iterator BinaryData::rbegin() const noexcept {
	return const_reverse_iterator(end());
}

BinaryData::const_reverse_iterator BinaryData::rend() const noexcept {
	return const_reverse_iterator(begin());
}

BinaryData::const_reverse_iterator BinaryData::crbegin() const noexcept {
	return rbegin();
}

BinaryData::const_reverse_iterator BinaryData::crend() const noexcept {
	return rend();
}

Size BinaryData::size() const noexcept {
	return Size{m_storage->bytes.size()};
}

Size BinaryData::max_size() const noexcept {
	return Size{m_storage->bytes.max_size()};
}

Size BinaryData::capacity() const noexcept {
	return Size{m_storage->bytes.capacity()};
}

bool BinaryData::empty() const noexcept {
	return m_storage->bytes.empty();
}

void BinaryData::reserve(const Size& new_cap) {
	m_storage->bytes.reserve(static_cast<std::size_t>(new_cap));
}

void BinaryData::resize(const Size& new_size) {
	m_storage->bytes.resize(static_cast<std::size_t>(new_size));
}

void BinaryData::resize(const Size& new_size, std::byte value) {
	m_storage->bytes.resize(static_cast<std::size_t>(new_size), value);
}

void BinaryData::shrink_to_fit() {
	m_storage->bytes.shrink_to_fit();
}

void BinaryData::clear() noexcept {
	m_storage->bytes.clear();
}

std::byte& BinaryData::operator[](const Size& index) noexcept {
	return m_storage->bytes[static_cast<std::size_t>(index)];
}

const std::byte& BinaryData::operator[](const Size& index) const noexcept {
	return m_storage->bytes[static_cast<std::size_t>(index)];
}

std::byte& BinaryData::at(const Size& index) {
	if (index >= size())
		throw OutOfBoundsError(Component("BinaryData"), "index out of range");
	return m_storage->bytes[static_cast<std::size_t>(index)];
}

const std::byte& BinaryData::at(const Size& index) const {
	if (index >= size())
		throw OutOfBoundsError(Component("BinaryData"), "index out of range");
	return m_storage->bytes[static_cast<std::size_t>(index)];
}

std::byte& BinaryData::front() noexcept {
	return m_storage->bytes.front();
}

const std::byte& BinaryData::front() const noexcept {
	return m_storage->bytes.front();
}

std::byte& BinaryData::back() noexcept {
	return m_storage->bytes.back();
}

const std::byte& BinaryData::back() const noexcept {
	return m_storage->bytes.back();
}

std::byte* BinaryData::data() noexcept {
	return m_storage->bytes.empty() ? nullptr : m_storage->bytes.data();
}

const std::byte* BinaryData::data() const noexcept {
	return m_storage->bytes.empty() ? nullptr : m_storage->bytes.data();
}

std::span<std::byte> BinaryData::span() noexcept {
	return std::span<std::byte>(data(), static_cast<std::size_t>(size()));
}

std::span<const std::byte> BinaryData::span() const noexcept {
	return std::span<const std::byte>(data(), static_cast<std::size_t>(size()));
}

BinaryData::operator std::span<std::byte>() noexcept {
	return span();
}

BinaryData::operator std::span<const std::byte>() const noexcept {
	return span();
}

void BinaryData::push_back(std::byte value) {
	m_storage->bytes.push_back(value);
}

void BinaryData::pop_back() {
	m_storage->bytes.pop_back();
}

BinaryData::iterator BinaryData::insert(const_iterator pos, std::byte value) {
	return insert(pos, Size{1}, value);
}

BinaryData::iterator BinaryData::insert(const_iterator pos, const Size& count, std::byte value) {
	const Size index = offset_of(pos);
	if (count == Size{0})
		return begin() + static_cast<std::ptrdiff_t>(index);
	m_storage->bytes.insert(m_storage->bytes.begin() + static_cast<std::ptrdiff_t>(index),
		static_cast<std::size_t>(count), value);
	return begin() + static_cast<std::ptrdiff_t>(index);
}

BinaryData::iterator BinaryData::insert(const_iterator pos, std::initializer_list<std::byte> list) {
	return insert(pos, std::span<const std::byte>(list.begin(), list.size()));
}

BinaryData::iterator BinaryData::insert(const_iterator pos, std::span<const std::byte> bytes) {
	return insert_at(offset_of(pos), bytes.data(), Size{bytes.size()});
}

BinaryData::iterator BinaryData::erase(const_iterator pos) {
	const Size index = offset_of(pos);
	m_storage->bytes.erase(m_storage->bytes.begin() + static_cast<std::ptrdiff_t>(index));
	return begin() + static_cast<std::ptrdiff_t>(index);
}

BinaryData::iterator BinaryData::erase(const_iterator first, const_iterator last) {
	const Size index = offset_of(first);
	const Size last_index = offset_of(last);
	m_storage->bytes.erase(
		m_storage->bytes.begin() + static_cast<std::ptrdiff_t>(index),
		m_storage->bytes.begin() + static_cast<std::ptrdiff_t>(last_index));
	return begin() + static_cast<std::ptrdiff_t>(index);
}

void BinaryData::assign(const Size& count, std::byte value) {
	m_storage->bytes.assign(static_cast<std::size_t>(count), value);
}

void BinaryData::assign(std::initializer_list<std::byte> list) {
	m_storage->bytes.assign(list);
}

void BinaryData::assign(std::span<const std::byte> bytes) {
	clear();
	append(bytes);
}

void BinaryData::append(std::span<const std::byte> bytes) {
	append(bytes.data(), Size{bytes.size()});
}

void BinaryData::append(const std::byte* bytes, const Size& count) {
	if (count == Size{0} || bytes == nullptr)
		return;
	m_storage->bytes.insert(m_storage->bytes.end(), bytes, bytes + static_cast<std::size_t>(count));
}

void BinaryData::append(const BinaryData& other) {
	append(other.span());
}

void BinaryData::append(BinaryData&& other) {
	if (this == &other)
		return;
	if (empty()) {
		m_storage.swap(other.m_storage);
		other.m_storage = std::make_unique<Storage>();
		return;
	}
	append(other.span());
	other.clear();
}

void BinaryData::swap(BinaryData& other) noexcept {
	m_storage.swap(other.m_storage);
}

Size BinaryData::offset_of(const_iterator pos) const noexcept {
	if (data() == nullptr)
		return Size{0};
	const auto distance = pos - data();
	if (distance <= 0)
		return Size{0};
	const auto cap = static_cast<std::ptrdiff_t>(size());
	if (distance >= cap)
		return size();
	return Size{static_cast<std::size_t>(distance)};
}

BinaryData::iterator BinaryData::insert_at(const Size& index, const std::byte* bytes, const Size& count) {
	if (count == Size{0} || bytes == nullptr)
		return begin() + static_cast<std::ptrdiff_t>(index);
	m_storage->bytes.insert(m_storage->bytes.begin() + static_cast<std::ptrdiff_t>(index),
		bytes, bytes + static_cast<std::size_t>(count));
	return begin() + static_cast<std::ptrdiff_t>(index);
}

namespace StormByte {
	void swap(BinaryData& lhs, BinaryData& rhs) noexcept {
		lhs.swap(rhs);
	}
}
