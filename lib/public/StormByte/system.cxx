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

#include <StormByte/system.hxx>
#include <StormByte/string.hxx>
#ifdef WINDOWS
#include <windows.h>
#elifdef MACOS
#include <mach-o/dyld.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <vector>
#else
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#define MAX_PATH 256
#endif
#include <thread>
namespace StormByte::System {
	std::filesystem::path TempFileName(const std::string& prefix) {
	#ifdef WINDOWS
		wchar_t tempPath[MAX_PATH];
		wchar_t tempFile[MAX_PATH];
		if (GetTempPathW(MAX_PATH, tempPath) == 0) {
			throw std::runtime_error("Error getting temp path");
		}
		if (GetTempFileNameW(tempPath, String::UTF8Decode(prefix).c_str(), 0, tempFile) == 0) {
			throw std::runtime_error("Error getting temp file name");
		}
		return String::UTF8Encode(std::wstring(tempFile));
	#else
		// Linux + macOS (and other UNIX)
		const std::string temp_filename_with_prefix = "/tmp/" + prefix + "XXXXXX";
		char temp_filename[256];
		std::strncpy(temp_filename, temp_filename_with_prefix.c_str(), sizeof(temp_filename));
		temp_filename[sizeof(temp_filename) - 1] = '\0';
		int fd = mkstemp(temp_filename);
		if (fd == -1) {
			throw std::runtime_error("Failed to create temporary file");
		}
		close(fd);
		return std::string(temp_filename);
	#endif
	}
	std::filesystem::path CurrentPath() {
		return std::filesystem::current_path();
	}
	std::filesystem::path ExecutablePath() {
		// Directory that contains the running executable
	#ifdef WINDOWS
		char path[MAX_PATH];
		if (GetModuleFileNameA(nullptr, path, MAX_PATH)) {
			return std::filesystem::path(path).remove_filename();
		}
	#elifdef MACOS
		uint32_t size = 0;
		if (_NSGetExecutablePath(nullptr, &size) == -1 && size > 0) {
			std::vector<char> buf(size);
			if (_NSGetExecutablePath(buf.data(), &size) == 0) {
				return std::filesystem::path(buf.data()).remove_filename();
			}
		}
		char path[1024];
		uint32_t fixed = static_cast<uint32_t>(sizeof(path));
		if (_NSGetExecutablePath(path, &fixed) == 0) {
			return std::filesystem::path(path).remove_filename();
		}
	#else
		char path[MAX_PATH];
		ssize_t count = readlink("/proc/self/exe", path, sizeof(path) - 1);
		if (count != -1) {
			path[count] = '\0';
			return std::filesystem::path(path).remove_filename();
		}
	#endif
		return "NOPATH";
	}
	template <typename Rep, typename Period>
	void Sleep(const std::chrono::duration<Rep, Period>& duration) {
		std::this_thread::sleep_for(duration);
	}
	template STORMBYTE_PUBLIC void Sleep(const std::chrono::milliseconds& duration);
	template STORMBYTE_PUBLIC void Sleep(const std::chrono::seconds& duration);
	template STORMBYTE_PUBLIC void Sleep(const std::chrono::minutes& duration);
	template STORMBYTE_PUBLIC void Sleep(const std::chrono::hours& duration);
}
