// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/TSmartPtr.h>
#include <istd/TInterfacePtr.h>

// ImtCore includes
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

	typedef QMap<QByteArray, QByteArray> CommandParams;

	/**
		Reaction of the server to the head of a request (for HTTP the request line and the headers) before the rest
		of the request is received.
	*/
	enum HeadAction
	{
		/**
			The request is received as usual.
		*/
		HA_NONE,

		/**
			The client waits for the permission to send the rest of the request. The message should be sent to the client
			as it is, the request is still received (for HTTP \c 100 \c Continue).
		*/
		HA_SEND_INTERIM,

		/**
			The request is refused. The message is the final response, it should be sent to the client and the connection
			should be closed. The request should not be handled.
		*/
		HA_REJECT
	};

	virtual RequestState GetState() const = 0;
	virtual QByteArray GetCommandId() const = 0;
	virtual CommandParams GetCommandParams() const = 0;
	virtual QByteArray GetBody() const = 0;
	virtual QByteArray GetRequestId() const = 0;

	/**
		Parse the data that are available in the device.
		Only the data of this request are taken from the device. If the request is complete and the data continue with
		the next request, the next request stays in the device.
		\return \c false if the data are not a valid request.
	*/
	virtual bool ParseDeviceData(QIODevice& device) = 0;

	/**
		Returns \c false if the connection should be closed after the response to the request, for example because
		the client asked for it.
	*/
	virtual bool IsConnectionPersistent() const = 0;

	/**
		Returns \c true if the response to the request should be sent without content, for example the response to \c HEAD.
	*/
	virtual bool IsResponseBodyOmitted() const = 0;

	/**
		Get the reaction to the head of the request, it should be called after each parsing of the data. The action is taken
		once: the next call returns \c HA_NONE for an interim message.
		\param	message	Data to send to the client for \c HA_SEND_INTERIM and \c HA_REJECT, empty otherwise.
	*/
	virtual HeadAction TakeHeadAction(QByteArray& message) = 0;
};


typedef istd::TSmartPtr<const imtrest::IRequest> ConstRequestPtr;
typedef istd::TUniqueInterfacePtr<imtrest::IRequest> IRequestUniquePtr;


} // namespace imtrest


