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

/**
 * @def WINDOWS
 * @brief Defined on Win32/Win64 and Cygwin.
 */
/**
 * @def MACOS
 * @brief Defined on Apple platforms (`__APPLE__` && `__MACH__`).
 */
/**
 * @def LINUX
 * @brief Defined on Linux.
 */
/**
 * @def UNIX
 * @brief Defined on macOS, Linux, and other Unix.
 */
/**
 * @def BIT64
 * @brief Defined when pointers are 64-bit.
 */
/**
 * @def BIT32
 * @brief Defined when pointers are 32-bit.
 */
/**
 * @def CLANG
 * @brief Defined when compiling with Clang, including clang-cl.
 */
/**
 * @def GCC
 * @brief Defined when compiling with GCC and not Clang.
 */
/**
 * @def MSVC
 * @brief Defined when compiling with MSVC only (`_MSC_VER` and not `__clang__`).
 */

#if defined(_WIN32) || defined(__CYGWIN__)
	#define WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
	#define MACOS
	#define UNIX
#elif defined(__linux__)
	#define LINUX
	#define UNIX
#elif defined(__unix__) || defined(__unix)
	#define UNIX
#else
	#error "Unsupported operating system"
#endif

#if defined(_WIN64) || defined(__x86_64__) || defined(__amd64__) \
	|| defined(__aarch64__) || defined(_M_X64) || defined(_M_ARM64) \
	|| (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 8)
	#define BIT64
#elif defined(_WIN32) || defined(__i386__) || defined(__i386) \
	|| defined(_M_IX86) || defined(__arm__) || defined(_M_ARM) \
	|| (defined(__SIZEOF_POINTER__) && __SIZEOF_POINTER__ == 4)
	#define BIT32
#else
	#error "Unsupported architecture (expected 32-bit or 64-bit)"
#endif

#if defined(__clang__)
	#define CLANG
#elif defined(_MSC_VER)
	#define MSVC
#elif defined(__GNUC__) || defined(__GNUG__)
	#define GCC
#endif
