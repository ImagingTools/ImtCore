// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFuture>
#include <QtCore/QPromise>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtclientgql/IAsyncGqlClient.h>
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtgql/CGqlRequest.h>


namespace imtclientgql
{


/**
	Asynchronous GQL model requests via \c IAsyncGqlClient (\c AsyncApiClient).

	No blocking Wait — callers handle completion through the returned future.
	Does not expose sync \c SendModelRequest; stack with
	\c TClientRequestManagerCompWrap when both paths are needed:

	\code
	TAsyncClientRequestManagerCompWrap<TClientRequestManagerCompWrap<MyHandlerBase>>
	\endcode
*/
template <class Base>
class TAsyncClientRequestManagerCompWrap: public Base
{
public:
	typedef Base BaseClass;

	I_BEGIN_BASE_COMPONENT(TAsyncClientRequestManagerCompWrap);
		I_ASSIGN(m_asyncApiClientCompPtr, "AsyncApiClient", "Asynchronous API Client (IAsyncGqlClient)", true, "AsyncApiClient");
	I_END_COMPONENT;

protected:
	/**
		Non-blocking model request.
		Returns an already finished future if the async client is missing or the request is invalid.
	*/
	QFuture<IAsyncGqlClient::GqlResult> SendModelRequestAsync(const imtgql::IGqlRequest& request) const
	{
		if (!m_asyncApiClientCompPtr.IsValid()){
			return CreateFailedFuture();
		}

		IGqlClient::GqlRequestPtr requestPtr;
		requestPtr.MoveCastedPtr(request.CloneMe());
		if (!requestPtr.IsValid()){
			return CreateFailedFuture(
						IAsyncGqlClient::EC_INVALID_REQUEST,
						QStringLiteral("Request is invalid"));
		}

		CClientRequestModelHelpers::AttachMissingContext(requestPtr);

		return m_asyncApiClientCompPtr->SendRequest(requestPtr);
	}

	bool HasAsyncApiClient() const
	{
		return m_asyncApiClientCompPtr.IsValid();
	}

private:
	static QFuture<IAsyncGqlClient::GqlResult> CreateFailedFuture(
				IAsyncGqlClient::ErrorCategory category = IAsyncGqlClient::EC_INTERNAL,
				const QString& message = QStringLiteral("Async client is not available"))
	{
		QPromise<IAsyncGqlClient::GqlResult> promise;
		promise.start();
		promise.addResult(IAsyncGqlClient::GqlResult{
					IAsyncGqlClient::GqlResponsePtr(),
					category,
					message});
		promise.finish();
		return promise.future();
	}

protected:
	I_REF(imtclientgql::IAsyncGqlClient, m_asyncApiClientCompPtr);
};


typedef TAsyncClientRequestManagerCompWrap<icomp::CComponentBase> CAsyncClientRequestManagerCompBase;


} // namespace imtclientgql
