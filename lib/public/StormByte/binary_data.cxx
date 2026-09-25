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
#include <cstring>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

namespace StormByte {
	struct BinaryData::Storage {
		std::vector<std::byte> bytes;
	};

	namespace {
		std::size_t AsIndex(const StormByte::Size& n) {
			return static_cast<std::size_t>(n.Value());
		}

		StormByte::Size AsSize(std::size_t n) {
			return StormByte::Size{ static_cast<std::uint64_t>(n) };
		}

		std::strong_ordering CompareBytes(std::span<const std::byte> lhs, std::span<const std::byte> rhs) noexcept {
			const std::size_t n = std::min(lhs.size(), rhs.size());
			if (n != 0) {
				const int cmp = std::memcmp(lhs.data(), rhs.data(), n);
				if (cmp < 0)
					return std::strong_ordering::less;
				if (cmp > 0)
					return std::strong_ordering::greater;
			}
			if (lhs.size() < rhs.size())
				return std::strong_ordering::less;
			if (lhs.size() > rhs.size())
				return std::strong_ordering::greater;
			return std::strong_ordering::equal;
		}
	}

	BinaryData::BinaryData() noexcept
		: m_storage(std::make_unique<Storage>()) {}

	BinaryData::BinaryData(const StormByte::Size& count, std::byte value)
		: m_storage(std::make_unique<Storage>()) {
		m_storage->bytes.assign(AsIndex(count), value);
	}

	BinaryData::BinaryData(const StormByte::Size& count)
		: BinaryData(count, std::byte{0}) {}

	BinaryData::BinaryData(std::span<const std::byte> bytes)
		: m_storage(std::make_unique<Storage>()) {
		m_storage->bytes.assign(bytes.begin(), bytes.end());
	}

	BinaryData::BinaryData(const std::byte* bytes, const StormByte::Size& count)
		: m_storage(std::make_unique<Storage>()) {
		if (bytes == nullptr && count.Value() != 0)
			throw OutOfBoundsError(Component("BinaryData"), "null source with non-zero count");
		if (count.Value() == 0)
			return;
		m_storage->bytes.assign(bytes, bytes + AsIndex(count));
	}

	BinaryData::BinaryData(std::initializer_list<std::byte> list)
		: m_storage(std::make_unique<Storage>()) {
		m_storage->bytes.assign(list);
	}

	BinaryData::BinaryData(std::string_view sv)
		: m_storage(std::make_unique<Storage>()) {
		const auto* raw = reinterpret_cast<const std::byte*>(sv.data());
		m_storage->bytes.assign(raw, raw + sv.size());
	}

	BinaryData::BinaryData(const char* s)
		: BinaryData(s ? std::string_view(s) : std::string_view{}) {}

	BinaryData::BinaryData(const std::vector<std::byte>& bytes)
		: m_storage(std::make_unique<Storage>()) {
		m_storage->bytes.assign(bytes.begin(), bytes.end());
	}

	BinaryData::BinaryData(std::vector<std::byte>&& bytes)
		: m_storage(std::make_unique<Storage>()) {
		m_storage->bytes.assign(bytes.begin(), bytes.end());
		bytes.clear();
		bytes.shrink_to_fit();
	}

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
		m_storage->bytes.assign(list);
		return *this;
	}

	bool BinaryData::operator==(const BinaryData& other) const noexcept {
		return m_storage->bytes == other.m_storage->bytes;
	}

	bool BinaryData::operator!=(const BinaryData& other) const noexcept {
		return !(*this == other);
	}

	std::strong_ordering BinaryData::operator<=>(const BinaryData& other) const noexcept {
		return m_storage->bytes <=> other.m_storage->bytes;
	}

	bool BinaryData::operator==(std::span<const std::byte> bytes) const noexcept {
		return (*this <=> bytes) == std::strong_ordering::equal;
	}

	bool BinaryData::operator!=(std::span<const std::byte> bytes) const noexcept {
		return !(*this == bytes);
	}

	std::strong_ordering BinaryData::operator<=>(std::span<const std::byte> bytes) const noexcept {
		return CompareBytes(span(), bytes);
	}

	BinaryData::iterator BinaryData::begin() noexcept {
		return m_storage->bytes.data();
	}

	BinaryData::const_iterator BinaryData::begin() const noexcept {
		return m_storage->bytes.data();
	}

	BinaryData::iterator BinaryData::end() noexcept {
		return m_storage->bytes.data() + m_storage->bytes.size();
	}

	BinaryData::const_iterator BinaryData::end() const noexcept {
		return m_storage->bytes.data() + m_storage->bytes.size();
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

	StormByte::Size BinaryData::size() const noexcept {
		return AsSize(m_storage->bytes.size());
	}

	StormByte::Size BinaryData::max_size() const noexcept {
		return AsSize(m_storage->bytes.max_size());
	}

	StormByte::Size BinaryData::capacity() const noexcept {
		return AsSize(m_storage->bytes.capacity());
	}

	bool BinaryData::empty() const noexcept {
		return m_storage->bytes.empty();
	}

	void BinaryData::reserve(const StormByte::Size& new_cap) {
		m_storage->bytes.reserve(AsIndex(new_cap));
	}

	void BinaryData::resize(const StormByte::Size& new_size) {
		m_storage->bytes.resize(AsIndex(new_size));
	}

	void BinaryData::resize(const StormByte::Size& new_size, std::byte value) {
		m_storage->bytes.resize(AsIndex(new_size), value);
	}

	void BinaryData::shrink_to_fit() {
		m_storage->bytes.shrink_to_fit();
	}

	void BinaryData::clear() noexcept {
		m_storage->bytes.clear();
	}

	std::byte& BinaryData::operator[](const StormByte::Size& index) noexcept {
		return m_storage->bytes[AsIndex(index)];
	}

	const std::byte& BinaryData::operator[](const StormByte::Size& index) const noexcept {
		return m_storage->bytes[AsIndex(index)];
	}

	std::byte& BinaryData::at(const StormByte::Size& index) {
		if (index >= size())
			throw OutOfBoundsError(Component("BinaryData"), "index out of range");
		return m_storage->bytes[AsIndex(index)];
	}

	const std::byte& BinaryData::at(const StormByte::Size& index) const {
		if (index >= size())
			throw OutOfBoundsError(Component("BinaryData"), "index out of range");
		return m_storage->bytes[AsIndex(index)];
	}

	std::byte& BinaryData::front() {
		return m_storage->bytes.front();
	}

	const std::byte& BinaryData::front() const {
		return m_storage->bytes.front();
	}

	std::byte& BinaryData::back() {
		return m_storage->bytes.back();
	}

	const std::byte& BinaryData::back() const {
		return m_storage->bytes.back();
	}

	std::byte* BinaryData::data() noexcept {
		return m_storage->bytes.empty() ? nullptr : m_storage->bytes.data();
	}

	const std::byte* BinaryData::data() const noexcept {
		return m_storage->bytes.empty() ? nullptr : m_storage->bytes.data();
	}

	std::span<std::byte> BinaryData::span() noexcept {
		return std::span<std::byte>(data(), AsIndex(size()));
	}

	std::span<const std::byte> BinaryData::span() const noexcept {
		return std::span<const std::byte>(data(), AsIndex(size()));
	}

	BinaryData::operator std::span<std::byte>() noexcept {
		return span();
	}

	BinaryData::operator std::span<const std::byte>() const noexcept {
		return span();
	}

	BinaryData::operator std::vector<std::byte>() const& {
		return std::vector<std::byte>(m_storage->bytes.begin(), m_storage->bytes.end());
	}

	BinaryData::operator std::vector<std::byte>() && {
		std::vector<std::byte> out(m_storage->bytes.begin(), m_storage->bytes.end());
		clear();
		shrink_to_fit();
		return out;
	}

	void BinaryData::assign(const StormByte::Size& count, std::byte value) {
		m_storage->bytes.assign(AsIndex(count), value);
	}

	void BinaryData::assign(std::span<const std::byte> bytes) {
		m_storage->bytes.assign(bytes.begin(), bytes.end());
	}

	void BinaryData::assign(std::initializer_list<std::byte> list) {
		m_storage->bytes.assign(list);
	}

	void BinaryData::append(std::span<const std::byte> bytes) {
		m_storage->bytes.insert(m_storage->bytes.end(), bytes.begin(), bytes.end());
	}

	void BinaryData::append(const std::byte* bytes, const StormByte::Size& count) {
		if (bytes == nullptr && count.Value() != 0)
			throw OutOfBoundsError(Component("BinaryData"), "null source with non-zero count");
		if (count.Value() == 0)
			return;
		append(std::span<const std::byte>(bytes, AsIndex(count)));
	}

	void BinaryData::append(const BinaryData& other) {
		if (this == &other) {
			const auto copy = other.m_storage->bytes;
			m_storage->bytes.insert(m_storage->bytes.end(), copy.begin(), copy.end());
			return;
		}
		append(other.span());
	}

	void BinaryData::append(BinaryData&& other) {
		if (this == &other)
			return;
		if (empty()) {
			*this = std::move(other);
			return;
		}
		append(other.span());
		other.clear();
		other.shrink_to_fit();
	}

	BinaryData& BinaryData::operator+=(std::span<const std::byte> bytes) {
		append(bytes);
		return *this;
	}

	BinaryData& BinaryData::operator+=(const BinaryData& other) {
		append(other);
		return *this;
	}

	BinaryData& BinaryData::operator+=(BinaryData&& other) {
		append(std::move(other));
		return *this;
	}

	void BinaryData::push_back(std::byte value) {
		m_storage->bytes.push_back(value);
	}

	void BinaryData::pop_back() {
		m_storage->bytes.pop_back();
	}

	BinaryData::iterator BinaryData::insert(const_iterator pos, std::byte value) {
		const auto off = static_cast<std::size_t>(pos - cbegin());
		auto it = m_storage->bytes.insert(m_storage->bytes.begin() + static_cast<std::ptrdiff_t>(off), value);
		return m_storage->bytes.data() + (it - m_storage->bytes.begin());
	}

	BinaryData::iterator BinaryData::insert(const_iterator pos, const StormByte::Size& count, std::byte value) {
		const auto off = static_cast<std::size_t>(pos - cbegin());
		auto it = m_storage->bytes.insert(m_storage->bytes.begin() + static_cast<std::ptrdiff_t>(off), AsIndex(count), value);
		return m_storage->bytes.data() + (it - m_storage->bytes.begin());
	}

	BinaryData::iterator BinaryData::insert(const_iterator pos, std::initializer_list<std::byte> list) {
		return insert(pos, std::span<const std::byte>(list.begin(), list.size()));
	}

	BinaryData::iterator BinaryData::insert(const_iterator pos, std::span<const std::byte> bytes) {
		const auto off = static_cast<std::size_t>(pos - cbegin());
		auto it = m_storage->bytes.insert(m_storage->bytes.begin() + static_cast<std::ptrdiff_t>(off), bytes.begin(), bytes.end());
		return m_storage->bytes.data() + (it - m_storage->bytes.begin());
	}

	BinaryData::iterator BinaryData::erase(const_iterator pos) {
		const auto off = static_cast<std::size_t>(pos - cbegin());
		auto it = m_storage->bytes.erase(m_storage->bytes.begin() + static_cast<std::ptrdiff_t>(off));
		return m_storage->bytes.data() + (it - m_storage->bytes.begin());
	}

	BinaryData::iterator BinaryData::erase(const_iterator first, const_iterator last) {
		const auto off_a = static_cast<std::size_t>(first - cbegin());
		const auto off_b = static_cast<std::size_t>(last - cbegin());
		auto it = m_storage->bytes.erase(
			m_storage->bytes.begin() + static_cast<std::ptrdiff_t>(off_a),
			m_storage->bytes.begin() + static_cast<std::ptrdiff_t>(off_b));
		return m_storage->bytes.data() + (it - m_storage->bytes.begin());
	}

	void BinaryData::swap(BinaryData& other) noexcept {
		m_storage.swap(other.m_storage);
	}

	CString BinaryData::HexDump() const {
		if (empty())
			return CString("");

		std::string out;
		const auto n = AsIndex(size());
		out.reserve((n / 16 + 1) * 80);
		char line[96];
		for (std::size_t offset = 0; offset < n; offset += 16) {
			const std::size_t chunk = std::min<std::size_t>(16, n - offset);
			int used = std::snprintf(line, sizeof(line), "%08zx ", offset);
			if (used < 0)
				used = 0;
			for (std::size_t i = 0; i < 16; ++i) {
				if (i < chunk) {
					used += std::snprintf(line + used, sizeof(line) - static_cast<std::size_t>(used),
						"%02X ", static_cast<unsigned>(m_storage->bytes[offset + i]));
				} else {
					used += std::snprintf(line + used, sizeof(line) - static_cast<std::size_t>(used), "   ");
				}
			}
			used += std::snprintf(line + used, sizeof(line) - static_cast<std::size_t>(used), " ");
			for (std::size_t i = 0; i < chunk; ++i) {
				const auto c = static_cast<unsigned char>(m_storage->bytes[offset + i]);
				line[used++] = (c >= 0x20 && c <= 0x7E) ? static_cast<char>(c) : '.';
			}
			line[used++] = '\n';
			line[used] = '\0';
			out.append(line, static_cast<std::size_t>(used));
		}
		return CString(out.c_str());
	}

	void swap(BinaryData& lhs, BinaryData& rhs) noexcept {
		lhs.swap(rhs);
	}

	bool operator==(std::span<const std::byte> bytes, const BinaryData& data) noexcept {
		return data == bytes;
	}

	bool operator!=(std::span<const std::byte> bytes, const BinaryData& data) noexcept {
		return data != bytes;
	}
}
