#pragma once


// Qt includes
#include <QtCore/QString>


/**
	Interfaces and basic implementations for general database management.
	This package is system independent.
*/
namespace imtdb
{


// statics

static const QString NULL_DATA_LITERAL		= QStringLiteral("NULL");
static const QString DEFAULT_DATA_LITERAL	= QStringLiteral("DEFAULT");


QString SqlEncode(const QString& sqlQuery);


} // namespace imtdb


