#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtclientgql/IGqlClient.h>
#include <imtbase/CTreeItemModel.h>


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
	virtual bool SendModelRequest(const imtgql::IGqlRequest& request, imtbase::CTreeItemModel& responseModel) const;

private:
	imtbase::CTreeItemModel* CreateTreeItemModel(const imtgql::IGqlRequest& request, const QByteArray& data) const;

protected:
	I_REF(imtclientgql::IGqlClient, m_apiClientCompPtr);
};


template<class Base>
bool TClientRequestManagerCompWrap<Base>::SendModelRequest(const imtgql::IGqlRequest& request, imtbase::CTreeItemModel& responseModel) const
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
		istd::TDelPtr<imtbase::CTreeItemModel> resultModelPtr = CreateTreeItemModel(request, responsePtr->GetResponseData());
		if (!resultModelPtr.IsValid()){
			return false;
		}

		return responseModel.CopyFrom(*resultModelPtr);
	}

	return false;
}


// private methods

template<class Base>
imtbase::CTreeItemModel* TClientRequestManagerCompWrap<Base>::CreateTreeItemModel(const imtgql::IGqlRequest& request, const QByteArray& data) const
{
	qDebug() << "OnReply" << data;

	imtbase::CTreeItemModel* replyResultPtr = nullptr;

	QJsonDocument document = QJsonDocument::fromJson(data);
	if (document.isObject()){
		QJsonObject dataObject = document.object();

		if (dataObject.contains("errors")){
			QJsonValue jsonValue = dataObject.value("errors");
			if (jsonValue.isObject()){
				QJsonObject errorObj = jsonValue.toObject();
				if (errorObj.contains(request.GetCommandId())){
					QJsonValue bodyValue = errorObj.value(request.GetCommandId());
					dataObject = QJsonObject();
					dataObject.insert("errors", bodyValue);
					document.setObject(dataObject);
					QByteArray parserData = document.toJson(QJsonDocument::Compact);

					replyResultPtr = new imtbase::CTreeItemModel();

					bool ok = replyResultPtr->CreateFromJson(parserData);
					if (!ok){
						qDebug() << QString("Error when try create model from json: %1").arg(qPrintable(parserData));
					}
				}
			}
		}

		dataObject = document.object().value("data").toObject();
		if (!dataObject.isEmpty()){
			QJsonValue bodyValue = dataObject.value(request.GetCommandId());
			if (!bodyValue.isNull()){
				dataObject = QJsonObject();
				dataObject.insert("data", bodyValue);
				document.setObject(dataObject);
				QByteArray parserData = document.toJson(QJsonDocument::Compact);

				replyResultPtr = new imtbase::CTreeItemModel();

				replyResultPtr->CreateFromJson(parserData);
			}
		}
	}

	qDebug() << "replyResultPtr" << replyResultPtr->toJSON();

	return replyResultPtr;
}


typedef TClientRequestManagerCompWrap<icomp::CComponentBase> CClientRequestManagerCompBase;


} // namespace imtclientgql


