// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtmdbx/CMdbxDatabaseEngineComp.h>


// Qt includes
#include <QtCore/QDebug>


namespace imtmdbx
{


// reimplemented (IMdbxDatabaseEngine)
mdbx::env_managed& CMdbxDatabaseEngineComp::GetEnv()
{
	if (!m_databaseEngine) {
		qWarning() << "CMdbxDatabaseEngineComp::GetEnv: database engine is not initialized";
		static mdbx::env_managed emptyEnv;
		return emptyEnv;
	}

	return m_databaseEngine->GetEnv();
}


void CMdbxDatabaseEngineComp::OnComponentCreated()
{
	if (m_dbPathCompPtr.IsValid()) {
		QString databasePath = m_dbPathCompPtr->GetPath();
		m_databaseEngine = std::make_unique<CMdbxDatabaseEngine>(databasePath);
	}
}


}//namespace imtmdbx
