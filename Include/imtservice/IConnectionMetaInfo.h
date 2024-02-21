#pragma once

// Acf includes
#include <istd/IChangeable.h>

// Qt includes
#include <QtCore/QUrl>


namespace imtservice
{


/**
	Interface for describing an service connections info.
	\ingroup Service
*/
class IConnectionMetaInfo: virtual public istd::IChangeable
{
public:
	enum ConnectionType
	{
		CT_INPUT,
		CT_OUTPUT
	};

	virtual ConnectionType GetConnectionType() const = 0;
	virtual QByteArray GetServiceName() const = 0;
};


} // namespace imtservice


