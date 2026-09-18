// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtclientgql
{


/**
	Shared GQL model request helpers (parse + context). No client binding —
	used by both sync and async request-manager wraps.
*/
class CClientRequestModelHelpers
{
public:
	template<class SdlClass>
	static SdlClass ParseModelResponse(
				const QByteArray& responseData,
				const QByteArray& commandId,
				QString& errorMessage)
	{
		errorMessage.clear();
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
		else if (payloadValue.isString()){
			const QJsonDocument nested = QJsonDocument::fromJson(payloadValue.toString().toUtf8());
			if (nested.isObject()){
				object = nested.object();
			}
		}

		const QJsonValue errorsValue = object.value(QStringLiteral("errors"));
		const bool hasErrors = errorsValue.isArray() && !errorsValue.toArray().isEmpty();
		if (hasErrors){
			const QJsonValue firstError = errorsValue.toArray().at(0);
			if (firstError.isObject()){
				errorMessage = firstError.toObject().value(QStringLiteral("message")).toString();
			}
			if (errorMessage.isEmpty()){
				errorMessage = QStringLiteral("GraphQL error");
			}
			if (!object.contains(QStringLiteral("data"))){
				return SdlClass();
			}
		}

		const QJsonValue dataValue = object.value(QStringLiteral("data"));
		if (dataValue.isObject()){
			object = dataValue.toObject();
			const QJsonValue commandValue = object.value(QString::fromUtf8(commandId));
			if (commandValue.isObject()){
				object = commandValue.toObject();
			}
		}

		SdlClass response;
		if (!response.OptReadFromJsonObject(object)){
			if (!response.ReadFromJsonObject(object)){
				if (errorMessage.isEmpty()){
					errorMessage = QStringLiteral("Response parsing error");
				}
			}
		}
		return response;
	}

	static void AttachMissingContext(IGqlClient::GqlRequestPtr& requestPtr)
	{
		if (!requestPtr.IsValid() || requestPtr->GetRequestContext() != nullptr){
			return;
		}
		imtgql::IGqlContext* currentContextPtr = imtgql::CGqlRequestContextManager::GetContext();
		if (currentContextPtr == nullptr){
			return;
		}
		imtgql::CGqlRequest* concreteRequestPtr = dynamic_cast<imtgql::CGqlRequest*>(requestPtr.GetPtr());
		if (concreteRequestPtr == nullptr){
			return;
		}
		istd::IChangeableUniquePtr clonedContextPtr = currentContextPtr->CloneMe();
		imtgql::IGqlContextUniquePtr castedContextPtr;
		castedContextPtr.MoveCastedPtr(std::move(clonedContextPtr));
		concreteRequestPtr->SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(castedContextPtr)));
	}
};


/**
	Synchronous GQL model requests via \c IGqlClient (\c ApiClient).
*/
template <class Base>
class TClientRequestManagerCompWrap: public Base
{
public:
	typedef Base BaseClass;

	I_BEGIN_BASE_COMPONENT(TClientRequestManagerCompWrap);
		I_ASSIGN(m_apiClientCompPtr, "ApiClient", "Synchronous API Client (IGqlClient)", true, "ApiClient");
	I_END_COMPONENT;

protected:
	template<class SdlClass>
	SdlClass SendModelRequest(const imtgql::IGqlRequest& request, QString& errorMessage) const
	{
		errorMessage.clear();

		if (!m_apiClientCompPtr.IsValid()){
			Q_ASSERT(false);
			return SdlClass();
		}

		IGqlClient::GqlRequestPtr requestPtr;
		requestPtr.MoveCastedPtr(request.CloneMe());
		if (!requestPtr.IsValid()){
			errorMessage = QStringLiteral("Request is invalid");
			return SdlClass();
		}

		CClientRequestModelHelpers::AttachMissingContext(requestPtr);

		IGqlClient::GqlResponsePtr responsePtr = m_apiClientCompPtr->SendRequest(requestPtr);
		if (!responsePtr.IsValid()){
			errorMessage = QStringLiteral("Response is invalid");
			return SdlClass();
		}

		return CClientRequestModelHelpers::ParseModelResponse<SdlClass>(
					responsePtr->GetResponseData(),
					request.GetCommandId(),
					errorMessage);
	}

	bool HasApiClient() const
	{
		return m_apiClientCompPtr.IsValid();
	}

protected:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
};


typedef TClientRequestManagerCompWrap<icomp::CComponentBase> CClientRequestManagerCompBase;


} // namespace imtclientgql
