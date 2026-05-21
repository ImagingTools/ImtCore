// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QString>

// ImtCore includes
#include <imtgql/IGqlResponse.h>


namespace imtclientgql
{


/**
	Callback sink for the async GraphQL client (\c IAsyncGqlClient).

	Instances are owned by the caller and must remain alive at least until the
	related \c IAsyncGqlRequestToken transitions out of the \c S_PENDING state.

	The handler is invoked exactly once per request — either through
	\c OnResponseReceived (success) or through \c OnError (any failure,
	including a cancellation triggered via \c IAsyncGqlRequestToken::Cancel).

	By default the handler is invoked on the network/event-loop thread of the
	client component that produced the token. Adapters that need to dispatch
	to another thread are responsible for marshalling.
*/
class IAsyncGqlResponseHandler: virtual public istd::IPolymorphic
{
public:
	typedef istd::TSharedInterfacePtr<imtgql::IGqlResponse> GqlResponsePtr;

	/**
		Error category reported through \c OnError.
	*/
	enum ErrorCategory
	{
		EC_NONE = 0,			//!< Reserved, never reported through \c OnError.
		EC_NETWORK,				//!< Network / transport failure (e.g. \c QNetworkReply error).
		EC_TIMEOUT,				//!< Request did not complete within the configured timeout.
		EC_CANCELLED,			//!< Request was cancelled via \c IAsyncGqlRequestToken::Cancel.
		EC_INVALID_REQUEST,		//!< Request could not be sent (invalid request, unsupported type, ...).
		EC_INTERNAL				//!< Internal error (e.g. protocol engine missing / failed to create network request).
	};

	/**
		Invoked when a response for the request has been received successfully.
		\param responsePtr The parsed GraphQL response. Always valid.
	*/
	virtual void OnResponseReceived(GqlResponsePtr responsePtr) = 0;

	/**
		Invoked when the request failed, timed out or was cancelled.
		\param category Error category describing the reason of the failure.
		\param message Human-readable error message (may be empty).
	*/
	virtual void OnError(ErrorCategory category, const QString& message) = 0;
};


} // namespace imtclientgql

