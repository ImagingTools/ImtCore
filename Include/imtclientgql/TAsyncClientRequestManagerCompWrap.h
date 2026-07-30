// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QFuture>
#include <QtCore/QPromise>

// stdlib
#include <functional>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtclientgql/IAsyncGqlClient.h>
#include <imtclientgql/IAsyncGqlResponseHandler.h>
#include <imtclientgql/IGqlClient.h>
#include <imtclientgql/TClientRequestManagerCompWrap.h>
#include <imtgql/CGqlRequest.h>


namespace imtclientgql
{


/**
	Asynchronous GQL model requests via \c IAsyncGqlClient (\c AsyncApiClient).

	No blocking Wait — callers supply a completion callback.
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
		\param callback invoked once with (parsedSdl, errorMessage).
		Returns an already finished future if the async client is missing or the request is invalid.
	*/
	template<class SdlClass>
	QFuture<IAsyncGqlClient::GqlResponsePtr> SendModelRequestAsync(
				const imtgql::IGqlRequest& request,
				std::function<void(SdlClass, QString)> callback) const
	{
		if (!m_asyncApiClientCompPtr.IsValid() || !callback){
			return CreateFailedFuture();
		}

		IGqlClient::GqlRequestPtr requestPtr;
		requestPtr.MoveCastedPtr(request.CloneMe());
		if (!requestPtr.IsValid()){
			callback(SdlClass(), QStringLiteral("Request is invalid"));
			return CreateFailedFuture();
		}

		CClientRequestModelHelpers::AttachMissingContext(requestPtr);

		const QByteArray commandId = request.GetCommandId();

		class CHandler: virtual public IAsyncGqlResponseHandler
		{
		public:
			CHandler(std::function<void(SdlClass, QString)> cb, const QByteArray& commandId):
				m_callback(std::move(cb)),
				m_commandId(commandId)
			{
			}

			virtual void OnResponseReceived(GqlResponsePtr responsePtr) override
			{
				QString error;
				SdlClass payload;
				if (responsePtr.IsValid()){
					payload = CClientRequestModelHelpers::ParseModelResponse<SdlClass>(
								responsePtr->GetResponseData(),
								m_commandId,
								error);
				}
				else{
					error = QStringLiteral("Response is invalid");
				}
				m_callback(payload, error);
			}

			virtual void OnError(ErrorCategory /*category*/, const QString& message) override
			{
				m_callback(SdlClass(), message.isEmpty() ? QStringLiteral("Request failed") : message);
			}

		private:
			std::function<void(SdlClass, QString)> m_callback;
			QByteArray m_commandId;
		};

		// Self-deleting owner so fire-and-forget callers need not keep the handler alive.
		struct HandlerOwner: virtual public IAsyncGqlResponseHandler
		{
			explicit HandlerOwner(CHandler* h):
				m_handler(h)
			{
			}
			~HandlerOwner()
			{
				delete m_handler;
			}
			virtual void OnResponseReceived(GqlResponsePtr r) override
			{
				m_handler->OnResponseReceived(r);
				delete this;
			}
			virtual void OnError(ErrorCategory c, const QString& m) override
			{
				m_handler->OnError(c, m);
				delete this;
			}
			CHandler* m_handler = nullptr;
		};

		auto* handlerPtr = new CHandler(std::move(callback), commandId);
		auto* ownerPtr = new HandlerOwner(handlerPtr);
		return m_asyncApiClientCompPtr->SendRequest(requestPtr, ownerPtr, nullptr);
	}

	bool HasAsyncApiClient() const
	{
		return m_asyncApiClientCompPtr.IsValid();
	}

private:
	static QFuture<IAsyncGqlClient::GqlResponsePtr> CreateFailedFuture()
	{
		QPromise<IAsyncGqlClient::GqlResponsePtr> promise;
		promise.start();
		promise.addResult(IAsyncGqlClient::GqlResponsePtr());
		promise.finish();
		return promise.future();
	}

protected:
	I_REF(imtclientgql::IAsyncGqlClient, m_asyncApiClientCompPtr);
};


typedef TAsyncClientRequestManagerCompWrap<icomp::CComponentBase> CAsyncClientRequestManagerCompBase;


} // namespace imtclientgql
