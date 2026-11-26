// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFuture>
#include <QtCore/QString>

// ImtCore includes
#include <imtgql/IGqlResponse.h>
#include <imtbase/IUrlParam.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>


namespace imtclientgql
{


/**
	Common interface for an asynchronous GraphQL client.

	Asynchronous counterpart of \c IGqlClient. The request is dispatched
	without blocking the calling thread and the result is delivered through
	the returned \c QFuture.

	The returned \c QFuture can be used to query the state of the request
	(\c QFuture::isFinished / \c QFuture::isCanceled), to cancel it
	(\c QFuture::cancel), or — for "wait-for-response" adapters feeding
	synchronous high-level interfaces (e.g. \c IObjectCollection) —
	to block until the response arrives (\c QFuture::waitForFinished).

	Threading / cancellation contract:
	- \c QFuture::cancel is safe to call from any thread.
	- A finished, non-cancelled future always carries exactly one result:
	  the received response on success, or error details on failure.
*/
class IAsyncGqlClient: virtual public istd::IPolymorphic
{
public:
	typedef istd::TSharedInterfacePtr<imtgql::IGqlRequest> GqlRequestPtr;
	typedef istd::TSharedInterfacePtr<imtgql::IGqlResponse> GqlResponsePtr;

	using ErrorCategory = IGqlClient::ErrorCategory;
	using GqlResult = IGqlClient::GqlResult;
	static constexpr ErrorCategory EC_NONE = IGqlClient::EC_NONE;
	static constexpr ErrorCategory EC_NETWORK = IGqlClient::EC_NETWORK;
	static constexpr ErrorCategory EC_TIMEOUT = IGqlClient::EC_TIMEOUT;
	static constexpr ErrorCategory EC_INVALID_REQUEST = IGqlClient::EC_INVALID_REQUEST;
	static constexpr ErrorCategory EC_INTERNAL = IGqlClient::EC_INTERNAL;
	static constexpr ErrorCategory EC_SERVER_ERROR = IGqlClient::EC_SERVER_ERROR;
	static constexpr ErrorCategory EC_INVALID_RESPONSE = IGqlClient::EC_INVALID_RESPONSE;
	static constexpr ErrorCategory EC_ACCESS_DENIED = IGqlClient::EC_ACCESS_DENIED;

	template<class SdlClass>
	using GqlModelResult = IGqlClient::GqlModelResult<SdlClass>;

	/**
		Dispatch a request to the server asynchronously.

		\param requestPtr	The GraphQL request to send.
		\param urlParamPtr	Optional URL parameters passed to the protocol engine.
		\return A future tracking the in-flight request. On synchronous
		        validation failure the future is returned already finished
		        with the corresponding error result.
	*/
	virtual QFuture<GqlResult> SendRequest(
				GqlRequestPtr requestPtr,
				imtbase::IUrlParam* urlParamPtr = nullptr) const = 0;

	template<class SdlClass>
	QFuture<GqlModelResult<SdlClass>> SendModelRequest(const imtgql::IGqlRequest& request) const
	{
		IAsyncGqlClient::GqlRequestPtr requestPtr;
		requestPtr.MoveCastedPtr(request.CloneMe());
		if (!requestPtr.IsValid()) {
			return QtFuture::makeReadyValueFuture(GqlModelResult<SdlClass>{{}, EC_INVALID_REQUEST, QStringLiteral("Invalid request")});
		}

		CClientRequestModelHelpers::AttachMissingContext(requestPtr);

		const QByteArray commandId = request.GetCommandId();
		return SendRequest(requestPtr)
			.then([commandId](const IAsyncGqlClient::GqlResult& result) {
				return CClientRequestModelHelpers::ParseModelResult<SdlClass>(result, commandId);
			});
	}
};


template<typename T>
using GqlModelResult = typename IAsyncGqlClient::GqlModelResult<T>;


} // namespace imtclientgql
