#pragma once

// Qt includes
#include <QtCore/QString>

/**
	Interfaces and basic implementations for general database management.
	This package is system independent.
*/
namespace imtdb
{


QString SqlEncode(const QString& sqlQuery)
{
	QString retVal = sqlQuery;
	return retVal.replace("'", "''").replace(";", "\b");
}


} // namespace imtdb


