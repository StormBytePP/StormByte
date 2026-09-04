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

#include <StormByte/visibility.h>

#include <mutex>
#include <optional>
#include <thread>

/**
 * @namespace StormByte
 * @brief Root namespace of the StormByte suite.
 */
namespace StormByte {
	/**
	 * @class ThreadLock
	 * @brief Mutex with owner-thread reentry.
	 *
	 * The owning thread may call `Lock()` again without blocking.
	 * Another thread blocks in `Lock()` until the owner calls `Unlock()`.
	 * `Unlock()` from a thread that does not own the lock is a no-op.
	 */
	class STORMBYTE_PUBLIC ThreadLock final {
		public:
			/**
			 * @brief Unlocked lock, no owner.
			 */
			ThreadLock() noexcept = default;

			/**
			 * @brief Not copyable.
			 */
			ThreadLock(const ThreadLock&) = delete;

			/**
			 * @brief Not movable (`std::mutex` is not movable on Windows).
			 */
			ThreadLock(ThreadLock&&) = delete;

			/**
			 * @brief Releases internal mutexes.
			 * @note The owner should `Unlock()` before destruction if it still holds the lock.
			 */
			~ThreadLock() noexcept;

			/**
			 * @brief Not copy-assignable.
			 */
			ThreadLock& operator=(const ThreadLock&) = delete;

			/**
			 * @brief Not move-assignable.
			 */
			ThreadLock& operator=(ThreadLock&&) = delete;

			/**
			 * @brief Acquire the lock.
			 *
			 * Returns immediately if this thread already owns it.
			 * Otherwise blocks until ownership is taken.
			 */
			void Lock() noexcept;

			/**
			 * @brief Release ownership.
			 *
			 * No-op if this thread is not the owner.
			 */
			void Unlock() noexcept;

		private:
			std::optional<std::thread::id> m_owner_thread_id;	///< Owner, or empty when free.
			std::mutex m_main_mutex;							///< Blocks non-owners.
			std::mutex m_thread_owner_mutex;					///< Guards `m_owner_thread_id`.
	};
}
