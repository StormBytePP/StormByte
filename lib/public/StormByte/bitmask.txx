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

// Out-of-line implementation of StormByte::Bitmask, included by bitmask.hxx.
// See bitmask.hxx for documentation of each member.

namespace StormByte {
	template<typename Derived, Type::UnsignedEnum E>
	constexpr bool Bitmask<Derived, E>::operator==(const Bitmask& other) const noexcept {
		return m_value == other.m_value;
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr bool Bitmask<Derived, E>::operator!=(const Bitmask& other) const noexcept {
		return m_value != other.m_value;
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr Derived Bitmask<Derived, E>::operator|(const Bitmask& other) const noexcept {
		return Derived(m_value | other.m_value);
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr Derived Bitmask<Derived, E>::operator&(const Bitmask& other) const noexcept {
		return Derived(m_value & other.m_value);
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr Derived Bitmask<Derived, E>::operator^(const Bitmask& other) const noexcept {
		return Derived(m_value ^ other.m_value);
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr Derived Bitmask<Derived, E>::operator~() const noexcept {
		return Derived(~m_value);
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr Bitmask<Derived, E>& Bitmask<Derived, E>::operator|=(const Bitmask& other) noexcept {
		m_value = m_value | other.m_value;
		return *this;
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr Bitmask<Derived, E>& Bitmask<Derived, E>::operator&=(const Bitmask& other) noexcept {
		m_value = m_value & other.m_value;
		return *this;
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr Bitmask<Derived, E>& Bitmask<Derived, E>::operator^=(const Bitmask& other) noexcept {
		m_value = m_value ^ other.m_value;
		return *this;
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr void Bitmask<Derived, E>::Add(E value) noexcept {
		m_value = m_value | value;
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr void Bitmask<Derived, E>::Remove(E value) noexcept {
		m_value = m_value & ~value;
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr E Bitmask<Derived, E>::Value() const noexcept {
		return m_value;
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr bool Bitmask<Derived, E>::Has(E value) const noexcept {
		return (m_value & value) == value;
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr bool Bitmask<Derived, E>::Has(const Bitmask& other) const noexcept {
		return Has(other.m_value);
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr bool Bitmask<Derived, E>::HasAny(E value) const noexcept {
		return (m_value & value) != static_cast<E>(0);
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr bool Bitmask<Derived, E>::HasAny(const Bitmask& other) const noexcept {
		return HasAny(other.m_value);
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr bool Bitmask<Derived, E>::HasNone(E value) const noexcept {
		return (m_value & value) == static_cast<E>(0);
	}

	template<typename Derived, Type::UnsignedEnum E>
	constexpr bool Bitmask<Derived, E>::HasNone(const Bitmask& other) const noexcept {
		return HasNone(other.m_value);
	}
}
