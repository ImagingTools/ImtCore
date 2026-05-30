// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QSemaphore>

// libmdbx includes
#include <mdbx.h++>


namespace imtmdbx
{


/**
 * \brief RAII guard that limits concurrent read transactions via a semaphore.
 *
 * Each instance acquires one slot from the provided QSemaphore before starting
 * a read transaction on the MDBX environment. When the guard is destroyed, the
 * transaction is aborted (if still active) and the semaphore slot is released.
 *
 * This prevents exceeding the configured max_readers limit under concurrent load.
 * If all slots are occupied, construction blocks until a slot becomes available,
 * effectively queuing excess readers instead of crashing with MDBX_READERS_FULL.
 *
 * Usage:
 * \code
 *   QSemaphore readerSemaphore(maxReaders - safetyMargin);
 *   auto guard = CScopedReadTransaction(env, readerSemaphore);
 *   CDocumentTable table("MyTable", guard.Txn());
 *   // ... use table ...
 *   // guard destructor releases the reader slot
 * \endcode
 */
class CScopedReadTransaction
{

public:
	/**
	 * \brief Constructs the guard, acquires a semaphore slot, and starts a read transaction.
	 * \param env The MDBX environment to start the read transaction on.
	 * \param semaphore The semaphore controlling the max concurrent reader count.
	 *
	 * \note This constructor may block if all semaphore slots are occupied.
	 */
	CScopedReadTransaction(mdbx::env_managed& env, QSemaphore& semaphore)
		: m_semaphore(semaphore)
	{
		m_semaphore.acquire();
		m_txn = env.start_read();
	}

	~CScopedReadTransaction()
	{
		// mdbx::txn_managed destructor aborts if not committed, but be explicit
		if (m_txn) {
			m_txn.abort();
		}
		m_semaphore.release();
	}

	// Non-copyable
	CScopedReadTransaction(const CScopedReadTransaction&) = delete;
	CScopedReadTransaction& operator=(const CScopedReadTransaction&) = delete;

	// Moveable
	CScopedReadTransaction(CScopedReadTransaction&& other) noexcept
		: m_semaphore(other.m_semaphore)
		, m_txn(std::move(other.m_txn))
		, m_moved(false)
	{
		other.m_moved = true;
	}

	/**
	 * \brief Returns a reference to the managed read transaction.
	 */
	mdbx::txn_managed& Txn() { return m_txn; }

	/**
	 * \brief Returns a const reference to the managed read transaction.
	 */
	const mdbx::txn_managed& Txn() const { return m_txn; }

private:
	QSemaphore& m_semaphore;
	mdbx::txn_managed m_txn;
	bool m_moved = false;
};


} // namespace imtmdbx
