#include <imtmdbx/CMdbxDatabaseEngineComp.h>


// Qt includes
#include <QtCore/QFile>


namespace imtmdbx
{


// reimplemented (IMdbxDatabaseEngine)
mdbx::env_managed& CMdbxDatabaseEngineComp::GetEnv()
{
	static mdbx::env_managed emptyEnv;
	if (!m_databaseEngine.IsValid()){
		return emptyEnv;
	}

	return m_databaseEngine->GetEnv();
}


void CMdbxDatabaseEngineComp::OnComponentCreated()
{
	if (m_dbPathCompPtr.IsValid()){
		QByteArray databasePath = m_dbPathCompPtr->GetPath().toUtf8();
		m_databaseEngine.SetPtr(new CMdbxDatabaseEngine(databasePath));
	}
}


}//namespace imtmdbx
