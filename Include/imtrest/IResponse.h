#pragma once


// Qt includes
#include <QtCore/QMap>

// ImtCore includes
#include <imtrest/INetworkObject.h>


namespace imtrest
{


/**
	Common interface for the server's response
*/
class IResponse: virtual public INetworkObject
{
public:
	typedef QMap<QByteArray, QByteArray> Headers;

	virtual int GetStatusCode() const = 0;
	virtual const QByteArray& GetData() const = 0;
	virtual QByteArray GetDataTypeId() const = 0;
	virtual Headers GetHeaders() const = 0;
};


} // namespace imtrest



