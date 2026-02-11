// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QUuid>

// ACF includes
#include <iser/ISerializable.h>


namespace imtservice
{


/**
	Interface for describing a service connection param.
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

	typedef QList<IncomingConnectionParam> IncomingConnectionList;

	virtual QList<IncomingConnectionParam> GetIncomingConnections() const = 0;
};


} // namespace imtservice


