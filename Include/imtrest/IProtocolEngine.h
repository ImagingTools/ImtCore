#pragma once


// ACF includes
#include <iser/IVersionInfo.h>


namespace imtrest
{


class IRequest;
class IResponse;


/**
	Interface for core-level protocol definition.
*/
class IProtocolEngine: virtual public istd::IPolymorphic
{
public:
	enum RequestState
	{
		RS_NON_STARTED,
		RS_MESSAGE_BEGIN,
		RS_URL,
		RS_STATUS,
		RS_HEADERS,
		RS_HEADERS_COMPLETE,
		RS_BODY,
		RS_MESSAGE_COMPLETE,
		RS_CHUNK_HEADER,
		RS_CHUNK_COMPLETE
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
		Get the state of the request data.
	*/
	virtual bool GetRequestState(const QByteArray& data, RequestState& state) const = 0;

	/**
		Create request based on the incomming data.
	*/
	virtual IRequest* CreateRequest(const QByteArray& data) const = 0;

	/**
		Create reponse for a given request.
		\param data		The rseponse data.
		\param request	Related request.
	*/
	virtual IResponse* CreateResponse(
				const QByteArray& data,
				const IRequest& request,
				int statusCode) const = 0;
};


} // namespace imtrest



