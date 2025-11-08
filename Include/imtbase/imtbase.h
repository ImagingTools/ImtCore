#pragma once


// Qt includes
#include <QtCore/QList>



/**
	Basic functionality and interfaces
	This package is system independent.
*/
namespace imtbase
{


/**
	\todo Move to imtrest
*/
static const inline QByteArray s_protocolVersionHeaderId = QByteArrayLiteral("X-Protocol-Version");
static const inline QByteArray s_authenticationTokenHeaderId = QByteArrayLiteral("x-authentication-token");
static const inline QByteArray s_productIdHeaderId = QByteArrayLiteral("productid");


} // namespace imtbase


