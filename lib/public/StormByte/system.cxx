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
#include <StormByte/exception.hxx>
#include <StormByte/string.hxx>
#ifdef WINDOWS
#include <windows.h>
#elifdef MACOS
#include <mach-o/dyld.h>
#include <unistd.h>
#else
#include <unistd.h>
#endif
#include <thread>
#include <vector>

namespace StormByte::System {
	std::filesystem::path TempFileName(const std::string& prefix) {
	#ifdef WINDOWS
		wchar_t tempPath[MAX_PATH];
		wchar_t tempFile[MAX_PATH];
		if (GetTempPathW(MAX_PATH, tempPath) == 0) {
			throw SystemError(Component("System"), "Error getting temp path");
		}

		if (GetTempFileNameW(tempPath, String::UTF8Decode(prefix).c_str(), 0, tempFile) == 0) {
			throw SystemError(Component("System"), "Error getting temp file name");
		}

		return String::UTF8Encode(std::wstring(tempFile));
	#else
		std::string temp_filename = "/tmp/" + prefix + "XXXXXX";
		int fd = mkstemp(temp_filename.data());
		if (fd == -1) {
			throw SystemError(Component("System"), "Failed to create temporary file");
		}

		close(fd);
		return temp_filename;
	#endif
	}

	std::filesystem::path CurrentPath() {
		return std::filesystem::current_path();
	}

	std::filesystem::path ExecutablePath() {
	#ifdef WINDOWS
		std::vector<wchar_t> buf(MAX_PATH);
		for (;;) {
			DWORD n = GetModuleFileNameW(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
			if (n == 0) {
				throw SystemError(Component("System"), "Failed to resolve executable path");
			}
			if (n < buf.size()) {
				return std::filesystem::path(String::UTF8Encode(std::wstring(buf.data(), n))).remove_filename();
			}
			buf.resize(buf.size() * 2);
		}
	#elifdef MACOS
		uint32_t size = 0;
		_NSGetExecutablePath(nullptr, &size);
		if (size == 0) {
			throw SystemError(Component("System"), "Failed to resolve executable path");
		}
		std::vector<char> buf(size);
		if (_NSGetExecutablePath(buf.data(), &size) != 0) {
			throw SystemError(Component("System"), "Failed to resolve executable path");
		}
		return std::filesystem::path(buf.data()).remove_filename();
	#else
		std::vector<char> buf(256);
		for (;;) {
			ssize_t count = readlink("/proc/self/exe", buf.data(), buf.size());
			if (count < 0) {
				throw SystemError(Component("System"), "Failed to resolve executable path");
			}
			if (static_cast<size_t>(count) < buf.size()) {
				return std::filesystem::path(std::string(buf.data(), static_cast<size_t>(count))).remove_filename();
			}
			buf.resize(buf.size() * 2);
		}
	#endif
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
