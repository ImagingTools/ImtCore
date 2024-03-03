#pragma once


// Qt includes
#include <QtCore/QUrl>
#include <QtCore/QUuid>

// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtservice/IServiceConnectionInfo.h>


namespace imtservice
{


/**
	Interface for describing an service connection param.
	\ingroup Service
*/
class IServiceConnectionParam: virtual public IServiceConnectionInfo
{
public:
	struct IncomingConnectionParam
	{
		QUrl url;
		QString description;
		QByteArray id;

		IncomingConnectionParam(){
			id = QUuid::createUuid().toByteArray();
		}

		bool operator==(const IncomingConnectionParam& other) const
		{
			return (url == other.url) && (description == other.description);
		}

		bool operator!=(const IncomingConnectionParam& other) const
		{
			return !operator==(other);
		}
	};

	virtual QList<IncomingConnectionParam> GetIncomingConnections() const = 0;
};


} // namespace imtservice


