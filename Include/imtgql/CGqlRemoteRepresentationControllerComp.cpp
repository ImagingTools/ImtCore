#include <imtgql/CGqlRemoteRepresentationControllerComp.h>


namespace imtgql
{


// public methods

CGqlRemoteRepresentationControllerComp::Response::Response()
	:m_replyResultPtr(nullptr)
{
}


// reimplemented (imtgql::IGqlClient::ResponseHandler)

imtbase::CTreeItemModel* CGqlRemoteRepresentationControllerComp::Response::GetResult()
{
	return m_replyResultPtr;
}


void CGqlRemoteRepresentationControllerComp::Response::OnReply(const IGqlRequest& request, const QByteArray& replyData)
{
	m_replyResultPtr = new imtbase::CTreeItemModel();
	QJsonDocument document = QJsonDocument::fromJson(replyData);
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
					m_replyResultPtr->CreateFromJson(parserData);
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
				m_replyResultPtr->CreateFromJson(parserData);
			}
		}
	}
}


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CGqlRemoteRepresentationControllerComp::CreateInternalResponse(const CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!IsRequestSupported(gqlRequest)){
		return nullptr;
	}

	Response responseHandler;
	bool retVal = m_apiClientCompPtr->SendRequest(gqlRequest, responseHandler);
	if (retVal){
		return responseHandler.GetResult();
	}

	return nullptr;
}


} // namespace imtgql


