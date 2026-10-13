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

	enum ErrorCategory
	{
		EC_NONE = 0,
		EC_NETWORK,
		EC_TIMEOUT,
		EC_INVALID_REQUEST,
		EC_INTERNAL,
		/**
			The server handled the request and answered with GraphQL \c errors.
			Only reported by \c SendModelRequest; a raw \c GqlResult carries such an answer as a
			successful response.
		*/
		EC_SERVER_ERROR,
		/**
			The answer is not a JSON object or does not match the requested model.
			Only reported by \c SendModelRequest.
		*/
		EC_INVALID_RESPONSE
	};

	struct GqlResult
	{
		GqlResponsePtr responsePtr;
		ErrorCategory errorCategory = EC_NONE;
		QString errorMessage;

		explicit operator bool() const {
			return errorCategory == EC_NONE && responsePtr.IsValid();
		}
	};

	template<class SdlClass>
	struct GqlModelResult
	{
		SdlClass model;
		ErrorCategory errorCategory = EC_INTERNAL;
		QString errorMessage;

		explicit operator bool() const {
			return errorCategory == EC_NONE;
		}
	};

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
				if (result.errorCategory != IAsyncGqlClient::EC_NONE) {
					return GqlModelResult<SdlClass>{{}, result.errorCategory, result.errorMessage};
				}
				if (!result.responsePtr.IsValid()) {
					return GqlModelResult<SdlClass>{{}, EC_INVALID_RESPONSE, QStringLiteral("Response is missing")};
				}

				QString errorMessage;
				CClientRequestModelHelpers::ParseError parseError = CClientRequestModelHelpers::PE_NONE;
				auto model = CClientRequestModelHelpers::ParseModelResponse<SdlClass>(
					result.responsePtr->GetResponseData(), commandId, errorMessage, &parseError);

				ErrorCategory errorCategory = EC_NONE;
				switch (parseError){
				case CClientRequestModelHelpers::PE_NONE:
					break;
				case CClientRequestModelHelpers::PE_SERVER_ERROR:
					errorCategory = EC_SERVER_ERROR;
					break;
				case CClientRequestModelHelpers::PE_INVALID_RESPONSE:
					errorCategory = EC_INVALID_RESPONSE;
					break;
				}

				return GqlModelResult<SdlClass>{model, errorCategory, errorMessage};
			});
	}
};


template<typename T>
using GqlModelResult = typename IAsyncGqlClient::GqlModelResult<T>;


} // namespace imtclientgql
