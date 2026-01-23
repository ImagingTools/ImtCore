#pragma once


// ACF includes
#include <istd/TSmartPtr.h>
#include <istd/TInterfacePtr.h>

// ImtCore includes
#include <imthttp/INetworkObject.h>


namespace imthttp
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

	typedef QMap<QByteArray, QByteArray> CommandParams;

	virtual RequestState GetState() const = 0;
	virtual QByteArray GetCommandId() const = 0;
	virtual CommandParams GetCommandParams() const = 0;
	virtual QByteArray GetBody() const = 0;
	virtual QByteArray GetRequestId() const = 0;
	virtual bool ParseDeviceData(QIODevice& device) = 0;
};


typedef istd::TSmartPtr<const imthttp::IRequest> ConstRequestPtr;
typedef istd::TUniqueInterfacePtr<imthttp::IRequest> IRequestUniquePtr;


} // namespace imthttp


