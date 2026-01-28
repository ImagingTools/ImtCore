#pragma once

// Qt includes
#include <QtCore/QMap>


/**
	Namespace containing the implementation of HTTP-based communication
	This package is system independent.
*/
namespace imthttp
{
typedef QMap<QByteArray,QByteArray> QueryParams;

} // namespace imthttp


