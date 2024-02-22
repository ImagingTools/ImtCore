#pragma once


// Qt includes
#include <QtCore/QUrl>

// ACF includes
#include <istd/IChangeable.h>


namespace imtservice
{


/**
	Interface for describing an service connections info.
	\ingroup Service
*/
class IServiceConnectionParam: virtual public istd::IChangeable
{
public:
	struct IncomingConnectionParam
	{
		QUrl url;
		QString description;

		bool operator==(const IncomingConnectionParam& other) const
		{
			return (url == other.url) && (description == other.description);
		}

		bool operator!=(const IncomingConnectionParam& other) const
		{
			return !operator==(other);
		}
	};

	enum ConnectionType
	{
		CT_INPUT,
		CT_OUTPUT
	};

	I_DECLARE_ENUM(ConnectionType, CT_INPUT, CT_OUTPUT);

	virtual ConnectionType GetConnectionType() const = 0;
	virtual QByteArray GetServiceName() const = 0;
	virtual QList<IncomingConnectionParam> GetIncomingConnections() const = 0;
};


} // namespace imtservice


