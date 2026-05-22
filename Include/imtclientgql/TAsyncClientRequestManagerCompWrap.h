// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QEventLoop>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtclientgql/IAsyncGqlClient.h>
#include <imtclientgql/IAsyncGqlResponseHandler.h>
#include <imtclientgql/IAsyncGqlRequestToken.h>


namespace imtclientgql
{


template <class Base>
class TAsyncClientRequestManagerCompWrap: public Base
{
public:
	typedef Base BaseClass;
	
	I_BEGIN_BASE_COMPONENT(TAsyncClientRequestManagerCompWrap);
	I_ASSIGN(m_asyncApiClientCompPtr, "ApiClient", "Async API Client", true, "ApiClient");
	I_END_COMPONENT;
	
protected:
	template<class SdlClass>
	SdlClass SendModelRequest(const imtgql::IGqlRequest& request, QString& errorMessage) const
	{
		errorMessage.clear();

		if (!m_asyncApiClientCompPtr.IsValid()){
			Q_ASSERT(false);
			return SdlClass();
		}

		IAsyncGqlClient::GqlRequestPtr requestPtr;
		requestPtr.MoveCastedPtr(request.CloneMe());
		if (!requestPtr.IsValid()){
			errorMessage = QStringLiteral("Request is invalid");
			return SdlClass();
		}

		// Use a blocking adapter: send async and wait for the response
		class ResponseHandler: public IAsyncGqlResponseHandler
		{
		public:
			ResponseHandler(QEventLoop& loop): m_loop(loop), m_succeeded(false) {}

			virtual void OnResponseReceived(GqlResponsePtr responsePtr) override
			{
				m_responsePtr = responsePtr;
				m_succeeded = true;
				m_loop.quit();
			}

			virtual void OnError(ErrorCategory /*category*/, const QString& message) override
			{
				m_errorMessage = message;
				m_succeeded = false;
				m_loop.quit();
			}

			QEventLoop& m_loop;
			GqlResponsePtr m_responsePtr;
			QString m_errorMessage;
			bool m_succeeded;
		};

		QEventLoop eventLoop;
		ResponseHandler handler(eventLoop);

		IAsyncGqlRequestTokenPtr tokenPtr = m_asyncApiClientCompPtr->SendRequest(requestPtr, &handler);
		if (!tokenPtr.IsValid()){
			errorMessage = QStringLiteral("Failed to send async request");
			return SdlClass();
		}

		// If the token is already in a terminal state, handler was invoked synchronously
		if (tokenPtr->GetState() == IAsyncGqlRequestToken::RS_PENDING){
			eventLoop.exec();
		}

		if (!handler.m_succeeded){
			errorMessage = handler.m_errorMessage.isEmpty()
				? QStringLiteral("Async request failed")
				: handler.m_errorMessage;
			return SdlClass();
		}

		if (!handler.m_responsePtr.IsValid()){
			errorMessage = QStringLiteral("Response is invalid");
			return SdlClass();
		}

		const QByteArray responseData = handler.m_responsePtr->GetResponseData();
		const QJsonDocument document = QJsonDocument::fromJson(responseData);
		if (!document.isObject()){
			errorMessage = QStringLiteral("Response is invalid");
			return SdlClass();
		}

		QJsonObject object = document.object();

		const QJsonValue payloadValue = object.value(QStringLiteral("payload"));
		if (payloadValue.isObject()){
			object = payloadValue.toObject();
		}

		const QJsonValue errorsValue = object.value(QStringLiteral("errors"));
		if (errorsValue.isArray()){
			const QJsonArray errorArr = errorsValue.toArray();
			if (!errorArr.isEmpty()){
				const QJsonValue firstError = errorArr.at(0);
				if (firstError.isObject()){
					errorMessage = firstError.toObject().value(QStringLiteral("message")).toString();
				}
			}

			SdlClass response;
			response.Version_1_0.emplace();
			return response;
		}

		const QJsonValue dataValue = object.value(QStringLiteral("data"));
		if (dataValue.isObject()){
			object = dataValue.toObject();

			const QString commandId = request.GetCommandId();
			const QJsonValue commandValue = object.value(commandId);
			if (commandValue.isObject()){
				object = commandValue.toObject();
			}
		}

		SdlClass response;
		response.Version_1_0.emplace();

		if (!response.Version_1_0->ReadFromJsonObject(object)){
			errorMessage = QStringLiteral("Response parsing error");
		}

		return response;
	}

	/**
		Non-blocking variant — dispatches the request and returns the token immediately.
		The caller is responsible for managing the handler lifetime and processing
		the response asynchronously via the handler callbacks.
	*/
	IAsyncGqlRequestTokenPtr SendModelRequestAsync(
				const imtgql::IGqlRequest& request,
				IAsyncGqlResponseHandler* handlerPtr) const
	{
		if (!m_asyncApiClientCompPtr.IsValid()){
			Q_ASSERT(false);
			return IAsyncGqlRequestTokenPtr();
		}

		IAsyncGqlClient::GqlRequestPtr requestPtr;
		requestPtr.MoveCastedPtr(request.CloneMe());
		if (!requestPtr.IsValid()){
			return IAsyncGqlRequestTokenPtr();
		}

		return m_asyncApiClientCompPtr->SendRequest(requestPtr, handlerPtr);
	}

protected:
	I_REF(imtclientgql::IAsyncGqlClient, m_asyncApiClientCompPtr);
};


typedef TAsyncClientRequestManagerCompWrap<icomp::CComponentBase> CAsyncClientRequestManagerCompBase;


} // namespace imtclientgql


