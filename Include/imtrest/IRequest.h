#pragma once


// ACF includes
#include <imtrest/INetworkObject.h>


namespace imtrest
{


/**
	Common interface for a request object.
*/
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

	typedef QMap<QString, QString> CommandParams;

	virtual RequestState GetState() const = 0;
	virtual QByteArray GetCommandId() const = 0;
	virtual CommandParams GetCommandParams() const = 0;
};


} // namespace imtrest


