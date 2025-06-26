#pragma once


// Qt includes
#include <QtCore/QUrl>
#include <QtCore/QUuid>


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtcom/CServerConnectionInterfaceParam.h>


namespace imtservice
{


/**
	Interface for describing an service connection param.
	\ingroup Service
*/
class IServiceConnectionParam: virtual public iser::ISerializable
{
public:
	struct IncomingConnectionParam
	{
		QByteArray id;
		QString host = "localhost";
		QString description;
		int wsPort = -1;
		int httpPort = -1;

		IncomingConnectionParam()
		{
			id = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
		}

		bool operator==(const IncomingConnectionParam& other) const
		{
			return	(host == other.host) &&
					(description == other.description) &&
					(wsPort == other.wsPort) &&
					(httpPort == other.httpPort) &&
					(id == other.id);
		}

		bool operator!=(const IncomingConnectionParam& other) const
		{
			return !operator==(other);
		}
	};

	typedef QList<IServiceConnectionParam::IncomingConnectionParam> IncomingConnectionList;

	virtual QList<IncomingConnectionParam> GetIncomingConnections() const = 0;
};


} // namespace imtservice


