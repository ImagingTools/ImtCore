// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmdbx/CMdbxDatabaseEngine.h>


namespace imtmdbx
{


// public methods

CMdbxDatabaseEngine::CMdbxDatabaseEngine(const QString& path)
	: m_databasePath(path)
{
	mdbx::env::operate_parameters operateParameters(s_maxMaps, s_maxReaders);
	mdbx::env_managed::create_parameters createParameters;

	m_env = mdbx::env_managed(m_databasePath.toStdString(), createParameters, operateParameters);
}


const QString CMdbxDatabaseEngine::GetDatabasePath() const
{
	return m_databasePath;
}


CScopedReadTransaction CMdbxDatabaseEngine::StartScopedRead()
{
	return CScopedReadTransaction(m_env, m_readerSemaphore);
}


// reimplemented (IMdbxDatabaseEngine)
mdbx::env_managed& CMdbxDatabaseEngine::GetEnv()
{
	return m_env;
}


} // namespace imtmdbx
