#pragma once

#include <StormByte/visibility.h>

#include <mutex>
#include <optional>
#include <thread>

/**
 * @namespace StormByte
 * @brief Main namespace for the StormByte library.
 *
 * The `StormByte` namespace serves as the root for all components and utilities in the StormByte library.
 * It provides foundational classes and tools for building robust, thread-safe, and efficient applications.
 */
namespace StormByte {
	class STORMBYTE_PUBLIC ThreadLock final {
		public:
			/**
			 * @class ThreadLock
			 * @brief A lightweight thread-owned lock with reentrant semantics for the owning thread.
			 *
			 * ThreadLock tracks the owning thread and permits the owner thread to call `Lock()`
			 * multiple times without blocking (reentrant behavior for the owner). If a different
			 * thread calls `Lock()` while another thread owns the lock, that caller will block
			 * until the owning thread calls `Unlock()` and releases ownership.
			 *
			 * This class is intended for simple ownership-based mutual exclusion where the same
			 * thread may need to acquire the lock multiple times. `Unlock()` releases ownership
			 * and makes the lock available to other threads. If a thread that does not own the
			 * lock calls `Unlock()`, the call is a no-op (it does not throw or alter state).
			 */
			constexpr ThreadLock() noexcept = default;

			ThreadLock(const ThreadLock&) = delete;
			// std::mutex is nor movable in Windows.
			ThreadLock(ThreadLock&&) = delete;

			/**
			 * @brief Destroy the ThreadLock and release any held resources.
			 *
			 * The destructor releases internal resources. The owning thread should call
			 * `Unlock()` before destruction if it currently holds the lock.
			 */
			~ThreadLock() noexcept;

			ThreadLock& operator=(const ThreadLock&) = delete;
			ThreadLock& operator=(ThreadLock&&) = delete;

			/**
			 * @brief Acquire the lock.
			 *
			 * If the calling thread already owns the lock, this call returns immediately.
			 * Otherwise the call blocks until the lock becomes available and the calling
			 * thread becomes the owner.
			 */
			void Lock() noexcept;

			/**
			 * @brief Release the lock.
			 *
			 * Releases ownership and unlocks the mutex. After a successful call, other
			 * blocked threads (if any) may acquire the lock. If the calling thread does
			 * not own the lock, `Unlock()` is a no-op and returns immediately.
			 */
			void Unlock() noexcept;

		private:
			std::optional<std::thread::id> m_owner_thread_id;
			std::mutex m_main_mutex, m_thread_owner_mutex;
	};
}