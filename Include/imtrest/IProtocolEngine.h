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
class IResponder;


/**
	Interface for core-level protocol definition.
*/
class IProtocolEngine: virtual public istd::IPolymorphic
{
public:
	enum StatusCode
	{
		/**
			No status is available.
		*/
		SC_NONE,

		/**
			Operation could be successfull processed.
		*/
		SC_OK,

		/**
			Requested operation is not available or implemented.
		*/
		SC_OPERATION_NOT_AVAILABLE,

		/**
			Requested resource is not available.
		*/
		SC_RESOURCE_NOT_AVAILABLE,

		/**
			Requested resource was not modified since the last call.
			The caller should use the last version of the resource on the client side.
		*/
		SC_NOT_MODIFIED,

		/**
			Operation is not authorized.
		*/
		SC_UNAUTHORIZED,

		/**
			Internal server error.
		*/
		SC_INTERNAL_ERROR
	};

	/**
		Get type-ID of the used protocol.
	*/
	virtual QByteArray GetProtocolTypeId() const = 0;

	/**
		Get the protocol version.
	*/
	virtual const iser::IVersionInfo* GetProtocolVersion() const = 0;

	/**
		Get the protocol-specific code for the engine's status.
	*/
	virtual bool GetProtocolStatusCode(
				int statusCode,
				int& protocolStatusCode,
				QByteArray& statusCodeLiteral) const = 0;

	/**
		Create request based on the incomming data.
		\param socketPtr		Socket instance for reading the incommming data
		\param requestHandler	Instance to process the request after all data has been read.
	*/
	virtual IRequest* CreateRequest(
				QAbstractSocket* socketPtr,
				const IRequestHandler& requestHandler) const = 0;

	/**
		Create reponse for a given request.
		\param data		The response data.
		\param request	Related request.
	*/
	virtual IResponse* CreateResponse(
				const IRequest& request,
				int statusCode,
				const QByteArray& data,
				const QByteArray& dataTypeId) const = 0;

	/**
		Get responder instance using for sending responses to the clients.
	*/
	virtual const IResponder& GetResponder(const IRequest* requestPtr = nullptr) const = 0;
};


} // namespace imtrest



