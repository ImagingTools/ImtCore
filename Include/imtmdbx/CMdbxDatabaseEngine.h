#pragma once

// Qt includes
#include "QtCore/QString"

// ImtCore includes
#include <imtmdbx/IMdbxDatabaseEngine.h>


namespace imtmdbx
{


class CMdbxDatabaseEngine: public IMdbxDatabaseEngine
{

public:
	CMdbxDatabaseEngine(const QString& path);
	const QString GetDatabasePath();

	// reimplemented (IMdbxDatabaseEngine)
	virtual mdbx::env_managed& GetEnv() override;

private:
	QString m_databasePath;
	mdbx::env_managed m_env;
};


}//namespace imtmdbx
