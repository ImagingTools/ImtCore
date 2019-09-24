#pragma once


// ACF includes
#include <imtrest/INetworkObject.h>


namespace imtrest
{


class IRequest: virtual public INetworkObject
{
public:
	enum RequestState
	{
		RS_NON_STARTED,
		RS_START_MESSAGE,
		RS_URL,
		RS_STATUS,
		RS_HEADERS,
		RS_HEADERS_COMPLETE,
		RS_BODY,
		RS_MESSAGE_COMPLETE,
		RS_CHUNK_HEADER,
		RS_CHUNK_COMPLETE
	};

	virtual RequestState GetState() const = 0;
	virtual QByteArray GetRequestId() const = 0;
	virtual bool ParseDeviceData(QIODevice& device) = 0;
};


} // namespace imtrest


