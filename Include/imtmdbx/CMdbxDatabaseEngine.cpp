// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmdbx/CMdbxDatabaseEngine.h>


// Qt includes
#include <QtCore/QFile>


namespace imtmdbx
{


CMdbxDatabaseEngine::CMdbxDatabaseEngine(const QString& path): m_databasePath(path)
{
	mdbx::env::operate_parameters operateParameters(100,10);
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


}//namespace imtmdbx
