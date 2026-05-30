// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QSemaphore>

// ImtCore includes
#include <imtmdbx/IMdbxDatabaseEngine.h>
#include <imtmdbx/CScopedReadTransaction.h>


namespace imtmdbx
{


/**
 * \brief Core MDBX database engine managing the environment lifecycle.
 *
 * Wraps an mdbx::env_managed instance and provides access to the environment.
 * Includes a semaphore-based reader count safeguard to prevent MDBX_READERS_FULL
 * errors under concurrent load.
 */
class CMdbxDatabaseEngine: public IMdbxDatabaseEngine
{

public:
	/**
	 * \brief Constructs and opens the MDBX environment at the given path.
	 * \param path Filesystem path to the MDBX database directory.
	 */
	CMdbxDatabaseEngine(const QString& path);

	/**
	 * \brief Returns the database filesystem path.
	 */
	const QString GetDatabasePath() const;

	/**
	 * \brief Creates a scoped read transaction guarded by the internal semaphore.
	 *
	 * This is the preferred way to start read transactions. The returned guard
	 * blocks if the max concurrent reader count is reached, and automatically
	 * releases the reader slot on destruction.
	 *
	 * \return A CScopedReadTransaction RAII guard owning the read transaction.
	 */
	CScopedReadTransaction StartScopedRead();

	// reimplemented (IMdbxDatabaseEngine)
	virtual mdbx::env_managed& GetEnv() override;

private:
	static constexpr unsigned s_maxMaps = 200;
	static constexpr unsigned s_maxReaders = 100;
	/// Safety margin subtracted from max_readers for the semaphore limit.
	static constexpr int s_readerSafetyMargin = 10;

	QString m_databasePath;
	mdbx::env_managed m_env;
	QSemaphore m_readerSemaphore{s_maxReaders - s_readerSafetyMargin};
};


} // namespace imtmdbx
