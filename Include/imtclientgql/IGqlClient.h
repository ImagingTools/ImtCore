// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtgql/IGqlResponse.h>
#include <imtbase/IUrlParam.h>


namespace imtclientgql
{


/**
	Common interface for a GraphQL client.
*/
class IGqlClient: virtual public istd::IPolymorphic
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
		/// GraphQL errors in a received response; reported by model request helpers.
		EC_SERVER_ERROR,
		/// A response cannot be parsed as the requested model.
		EC_INVALID_RESPONSE,
		/// The transport reports an explicit access refusal (HTTP 401/403).
		EC_ACCESS_DENIED
	};

	struct GqlResult
	{
		GqlResponsePtr responsePtr;
		ErrorCategory errorCategory = EC_NONE;
		QString errorMessage;

		explicit operator bool() const
		{
			return errorCategory == EC_NONE && responsePtr.IsValid();
		}
	};

	template<class SdlClass>
	struct GqlModelResult
	{
		SdlClass model;
		ErrorCategory errorCategory = EC_INTERNAL;
		QString errorMessage;

		explicit operator bool() const
		{
			return errorCategory == EC_NONE;
		}
	};

	/**
		Send a request and return either a response or its failure category and message.
		A received GraphQL error response is a transport success; model helpers interpret its errors.
	*/
	virtual GqlResult SendRequest(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParamPtr = nullptr) const = 0;

	/**
		Send a request without waiting for the response (fire-and-forget).

		For callers that only care whether the request was handed to the transport, not
		what the server answered - avoids the SendRequest() nested-event-loop wait entirely.
		Default implementation reports "unsupported"; transports capable of a non-blocking
		send should reimplement this instead of falling back to SendRequest().
	*/
	virtual bool SendRequestNoWait(GqlRequestPtr requestPtr, imtbase::IUrlParam* urlParamPtr = nullptr) const = 0;
};


} // namespace imtclientgql
