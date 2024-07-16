#pragma once

// Qt includes
#include "QtCore/QString"

// libmdbx includes
#include <mdbx.h++>


namespace imtmdbx
{


class CDatabaseEngine
{

public:
	CDatabaseEngine(const QString& path);
	mdbx::env_managed GetEnv();
	const QString GetDatabasePath();

private:
	QString m_databasePath;
};


}//namespace imtmdbx
