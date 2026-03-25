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

protected:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
};


typedef TClientRequestManagerCompWrap<icomp::CComponentBase> CClientRequestManagerCompBase;


} // namespace imtclientgql


