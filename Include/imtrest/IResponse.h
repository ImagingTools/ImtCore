#pragma once


// Qt includes
#include <QtCore/QMap>

// ACF includes
#include <istd/TSmartPtr.h>

// ImtCore includes
#include <imtrest/INetworkObject.h>


namespace imtrest
{


/**
	Common interface for the server's response.
	Each response consists of following sections:
	* Status code.
	* List of headers given as key-value pair in textual form.
	* Reponse data, if provided.
*/
class IResponse: virtual public INetworkObject
{
public:
	typedef QMap<QByteArray, QByteArray> Headers;

	/**
		Get the status code of the response.
		\sa IProtocolEngine::StatusCode
	*/
	virtual int GetStatusCode() const = 0;

	/**
		Get the data of the response.
	*/
	virtual const QByteArray& GetData() const = 0;

	/**
		Get the type-ID of the response data.
	*/
	virtual QByteArray GetDataTypeId() const = 0;

	/**
		Get headers of the response object.
	*/
	virtual Headers GetHeaders() const = 0;
};

typedef istd::TSmartPtr<const imtrest::IResponse> ConstResponsePtr;

} // namespace imtrest

Q_DECLARE_METATYPE(imtrest::ConstResponsePtr)


