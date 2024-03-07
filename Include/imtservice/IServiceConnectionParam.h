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
		QByteArray id;
		QString name;
		QString description;
		QUrl url;

		IncomingConnectionParam(){
			id = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8();
		}

		bool operator==(const IncomingConnectionParam& other) const
		{
			return	(url == other.url) &&
					(description == other.description) &&
					(name == other.name) &&
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


