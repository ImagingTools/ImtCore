// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmdbx/CMdbxDatabaseEngine.h>


namespace imtmdbx
{


// public methods

CMdbxDatabaseEngine::CMdbxDatabaseEngine(const QString& path)
	: m_databasePath(path)
{
	/**
		MDBX environment parameters.
		max_maps - maximum number of named databases (tables) in the environment. Each table requires one map page.
		max_readers - maximum number of concurrent readers (open tables) in the environment.
	*/
	constexpr unsigned max_maps = 200;
	constexpr unsigned max_readers = 100;

	mdbx::env::operate_parameters operateParameters(max_maps, max_readers);
	mdbx::env_managed::create_parameters createParameters;
	// createParameters.geometry.make_dynamic(5 * mdbx::env::geometry::MiB, 500 * mdbx::env::geometry::MiB);

	m_env = mdbx::env_managed(m_databasePath.toStdString(), createParameters, operateParameters);
}


const QString CMdbxDatabaseEngine::GetDatabasePath()
{
	return m_databasePath;
}


// reimplemented (IMdbxDatabaseEngine)
mdbx::env_managed& CMdbxDatabaseEngine::GetEnv()
{
	return m_env;
}


} // namespace imtmdbx
