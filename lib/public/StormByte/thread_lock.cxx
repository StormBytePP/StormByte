#include <StormByte/thread_lock.hxx>

using namespace StormByte;

ThreadLock::~ThreadLock() noexcept = default;

void ThreadLock::Lock() noexcept {
	const auto this_id = std::this_thread::get_id();

	// Fast path: if current thread already owns the lock, return immediately.
	{
		std::unique_lock owner_lock(m_thread_owner_mutex);
		if (m_owner_thread_id && *m_owner_thread_id == this_id) {
			return;
		}
	}

	// Acquire the main mutex (blocks other threads). After acquiring, set ownership.
	m_main_mutex.lock();
	{
		std::unique_lock owner_lock(m_thread_owner_mutex);
		m_owner_thread_id = this_id;
	}
}

void ThreadLock::Unlock() noexcept {
	const auto this_id = std::this_thread::get_id();

	std::unique_lock owner_lock(m_thread_owner_mutex);
	if (!m_owner_thread_id || *m_owner_thread_id != this_id) {
		// Not the owner: per design Unlock() is a no-op for non-owning threads.
		// Return without changing state.
		return;
	}

	// Always fully release ownership and unlock the main mutex.
	m_owner_thread_id = std::nullopt;
	owner_lock.unlock();
	m_main_mutex.unlock();
}