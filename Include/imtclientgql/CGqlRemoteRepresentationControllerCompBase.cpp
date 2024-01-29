#include <imtclientgql/CGqlRemoteRepresentationControllerCompBase.h>


// ImtCore includes
#include <imtgql/CGqlResponse.h>


namespace imtclientgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CGqlRemoteRepresentationControllerCompBase::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& /*errorMessage*/) const
{
	//if (!IsRequestSupported(gqlRequest)){
	//	return nullptr;
	//}

	//Response responseHandler;
	//bool retVal = m_apiClientCompPtr->SendRequest(gqlRequest, responseHandler);
	//if (retVal){
	//	return responseHandler.GetResult();
	//}

	return nullptr;
}


// private methods

imtbase::CTreeItemModel* CGqlRemoteRepresentationControllerCompBase::CreateTreeItemModelFromResponse(const imtgql::IGqlResponse& response)
{
	istd::TDelPtr<imtbase::CTreeItemModel> retVal(new imtbase::CTreeItemModel());
	QJsonDocument document = QJsonDocument::fromJson(response.GetResponseData());
	if (document.isObject()){
		QJsonObject dataObject = document.object();

		imtgql::IGqlResponse::GqlRequestPtr requestPtr = response.GetOriginalRequest();
		if (requestPtr.isNull()){
			return nullptr;
		}

		if (dataObject.contains("errors")){
			QJsonValue jsonValue = dataObject.value("errors");
			if (jsonValue.isObject()){
				QJsonObject errorObj = jsonValue.toObject();
				if (errorObj.contains(requestPtr->GetCommandId())){
					QJsonValue bodyValue = errorObj.value(requestPtr->GetCommandId());
					dataObject = QJsonObject();
					dataObject.insert("errors", bodyValue);
					document.setObject(dataObject);
					QByteArray parserData = document.toJson(QJsonDocument::Compact);
					retVal->CreateFromJson(parserData);
				}
			}
		}

		dataObject = document.object().value("data").toObject();
		if (!dataObject.isEmpty()){
			QJsonValue bodyValue = dataObject.value(requestPtr->GetCommandId());
			if (!bodyValue.isNull()){
				dataObject = QJsonObject();
				dataObject.insert("data", bodyValue);
				document.setObject(dataObject);
				QByteArray parserData = document.toJson(QJsonDocument::Compact);
				retVal->CreateFromJson(parserData);
			}
		}

		dataObject = document.object().value("payload").toObject();
		if (!dataObject.isEmpty()){
			document.setObject(dataObject);
			QByteArray parserData = document.toJson(QJsonDocument::Compact);
			retVal->CreateFromJson(parserData);
		}
	}

	return retVal.PopPtr();
}


} // namespace imtclientgql


