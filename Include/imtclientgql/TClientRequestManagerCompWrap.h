#pragma once


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
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
	bool SendModelRequest(const imtgql::IGqlRequest& request, SdlClass& responseModel) const
	{
		if (!m_apiClientCompPtr.IsValid()){
			return false;
		}

		imtclientgql::IGqlClient::GqlRequestPtr requestPtr(dynamic_cast<imtgql::IGqlRequest*>(request.CloneMe()));
		if (requestPtr.isNull()){
			return false;
		}

		IGqlClient::GqlRequestPtr gqlRequestPtr(requestPtr);

		IGqlClient::GqlResponsePtr responsePtr = m_apiClientCompPtr->SendRequest(gqlRequestPtr);
		if (!responsePtr.isNull()){
			QByteArray responseData = responsePtr->GetResponseData();

			QJsonDocument document = QJsonDocument::fromJson(responseData);
			if (document.isObject()){
				QJsonObject object = document.object();

				if (object.contains("errors")){
					object = object.value("errors").toObject();
				}
				else if (object.contains("data")){
					object = object.value("data").toObject();
				}

				if (object.contains(request.GetCommandId())){
					object = object.value(request.GetCommandId()).toObject();
				}

				qDebug() << "response" << QJsonDocument(object).toJson();

				return SdlClass::ReadFromJsonObject(responseModel, object);
			}
		}

		return false;
	}

	template <typename Request, typename Arguments, typename ResponseData, typename Retval>
	void SendRequest(Arguments arguments, Retval& retVal) const
	{
		imtgql::CGqlRequest gqlRequest;

		bool ok = Request::SetupGqlRequest(gqlRequest, arguments);
		if (ok){
			ResponseData response;
			if (!SendModelRequest(gqlRequest, response)){
				return;
			}

			retVal = response.GetResult();
		}
	}

protected:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
};


typedef TClientRequestManagerCompWrap<icomp::CComponentBase> CClientRequestManagerCompBase;


} // namespace imtclientgql


