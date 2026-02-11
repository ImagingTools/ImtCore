// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

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
		if (!m_apiClientCompPtr.IsValid()){
			Q_ASSERT(false);
			return SdlClass();
		}

		IGqlClient::GqlRequestPtr requestPtr;
		
		requestPtr.MoveCastedPtr(request.CloneMe());
		if (!requestPtr.IsValid()){
			errorMessage = QString("Request is invalid");

			return SdlClass();
		}

		SdlClass response;
		response.Version_1_0.emplace();

		IGqlClient::GqlRequestPtr gqlRequestPtr(requestPtr);
		IGqlClient::GqlResponsePtr responsePtr = m_apiClientCompPtr->SendRequest(gqlRequestPtr);
		if (!responsePtr.IsValid()){
			errorMessage = QString("Response is invalid");
			return response;
		}

		QByteArray responseData = responsePtr->GetResponseData();
		QJsonDocument document = QJsonDocument::fromJson(responseData);
		if (!document.isObject()){
			errorMessage = QString("Response is invalid");
			return response;
		}

		bool isError = false;
		QJsonObject object = document.object();

		if (object.contains("payload")){
			object = object.value("payload").toObject();
		}

		if (object.contains("errors")){
			object = object.value("errors").toObject();
			isError = true;
		}
		else if (object.contains("data")){
			object = object.value("data").toObject();
		}

		if (object.contains(request.GetCommandId())){
			object = object.value(request.GetCommandId()).toObject();
		}

		if (isError){
			errorMessage = object.value("message").toString();
			return response;
		}

		if (!response.Version_1_0->ReadFromJsonObject(object)){
			errorMessage = QString("Response parsing error");
			return response;
		}

		return response;
	}

protected:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
};


typedef TClientRequestManagerCompWrap<icomp::CComponentBase> CClientRequestManagerCompBase;


} // namespace imtclientgql


