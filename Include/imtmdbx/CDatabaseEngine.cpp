#include <imtmdbx/CDatabaseEngine.h>


// Qt includes
#include <QtCore/QFile>


namespace imtmdbx
{


CDatabaseEngine::CDatabaseEngine(const QString& path): m_databasePath(path)
{
}


mdbx::env_managed CDatabaseEngine::GetEnv()
{
	mdbx::env::operate_parameters operateParameters(100,10);
	mdbx::env_managed::create_parameters createParameters;
	mdbx::env_managed env(m_databasePath.toStdString(), createParameters, operateParameters);

	return env;
}


const QString CDatabaseEngine::GetDatabasePath()
{
	return m_databasePath;
}


}//namespace imtmdbx
