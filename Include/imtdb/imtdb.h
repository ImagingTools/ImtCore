#pragma once


// Qt includes
#include <QtCore/QString>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>


/**
	Interfaces and basic implementations for general database management.
	This package is system independent.
*/
namespace imtdb
{


QString SqlEncode(const QString& sqlQuery);
int GetLastMigration(const QString& migrationFolder, QString& errorMessage);


} // namespace imtdb


