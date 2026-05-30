// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once

// libmdbx includes
#include <mdbx.h++>


namespace imtmdbx
{


/**
 * \brief Interface for accessing the MDBX database environment.
 *
 * Provides access to the underlying mdbx::env_managed instance,
 * which is required for starting transactions and opening tables.
 */
class IMdbxDatabaseEngine
{

public:
	virtual ~IMdbxDatabaseEngine() = default;

	/**
	 * \brief Returns a reference to the managed MDBX environment.
	 * \warning Callers should prefer StartScopedRead() (on CMdbxDatabaseEngine)
	 *          over direct env.start_read() to avoid exceeding the reader limit.
	 */
	virtual mdbx::env_managed& GetEnv() = 0;
};


}//namespace imtmdbx
