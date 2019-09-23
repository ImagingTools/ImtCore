#pragma once


// Qt includes
#include <QtNetwork/QAbstractSocket>

// ACF includes
#include <iser/IVersionInfo.h>


namespace imtrest
{


class IRequest;
class IRequestHandler;
class IResponse;


/**
	Interface for core-level protocol definition.
*/
class IProtocolEngine: virtual public istd::IPolymorphic
{
public:
	/**
		Get type-ID of the used protocol.
	*/
	virtual QByteArray GetProtocolTypeId() const = 0;

	/**
		Get the protocol version.
	*/
	virtual const iser::IVersionInfo* GetProtocolVersion() const = 0;

	/**
		Create request based on the incomming data.
	*/
	virtual IRequest* CreateRequest(
				const QAbstractSocket* socketPtr,
				const IRequestHandler& requestHandler) const = 0;

	/**
		Create reponse for a given request.
		\param data		The response data.
		\param request	Related request.
	*/
	virtual IResponse* CreateResponse(
				const QIODevice* devicePtr,
				const QByteArray& data,
				const IRequest& request,
				int statusCode) const = 0;
};


} // namespace imtrest



