// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtclientgql
{


template <class Base>
class TClientRequestManagerCompWrap: public Base
{
public:
	typedef Base BaseClass;
	
	I_BEGIN_BASE_COMPONENT(TClientRequestManagerCompWrap);
	I_ASSIGN(m_apiClientCompPtr, "ApiClient", "API Client", true, "ApiClient");
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

		// Forwarded requests (e.g. Remote*Controller client wrappers relaying an
		// incoming request to a backend server) otherwise carry no context, so the
		// backend sees an anonymous caller and denies any permission-gated command.
		// Only fills in a missing context - an explicitly attached one (e.g. PAT's
		// own AttachCurrentContext()) is left untouched.
		if (requestPtr->GetRequestContext() == nullptr){
			imtgql::IGqlContext* currentContextPtr = imtgql::CGqlRequestContextManager::GetContext();
			if (currentContextPtr != nullptr){
				imtgql::CGqlRequest* concreteRequestPtr = dynamic_cast<imtgql::CGqlRequest*>(requestPtr.GetPtr());
				if (concreteRequestPtr != nullptr){
					istd::IChangeableUniquePtr clonedContextPtr = currentContextPtr->CloneMe();
					imtgql::IGqlContextUniquePtr castedContextPtr;
					castedContextPtr.MoveCastedPtr(std::move(clonedContextPtr));
					concreteRequestPtr->SetGqlContext(imtgql::IGqlContextSharedPtr::CreateFromUnique(std::move(castedContextPtr)));
				}
			}
		}

		IGqlClient::GqlResponsePtr responsePtr = m_apiClientCompPtr->SendRequest(requestPtr);
		if (!responsePtr.IsValid()){
			errorMessage = QStringLiteral("Response is invalid");
			return SdlClass();
		}

		const QByteArray responseData = responsePtr->GetResponseData();
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

		if (!response.ReadFromJsonObject(object)){
			errorMessage = QStringLiteral("Response parsing error");
		}

		return response;
	}

protected:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
};


typedef TClientRequestManagerCompWrap<icomp::CComponentBase> CClientRequestManagerCompBase;


} // namespace imtclientgql


