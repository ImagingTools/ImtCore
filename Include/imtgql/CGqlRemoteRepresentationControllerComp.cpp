#include <imtgql/CGqlRemoteRepresentationControllerComp.h>


namespace imtgql
{


// public methods

// reimplemented (imtgql::IGqlClient::ResponseHandler)

CGqlRemoteRepresentationControllerComp::Response::Response()
			: m_replyResultPtr(nullptr)
{

}


imtbase::CTreeItemModel* CGqlRemoteRepresentationControllerComp::Response::GetResult()
{
	return m_replyResultPtr;
}


void CGqlRemoteRepresentationControllerComp::Response::OnReply(const IGqlRequest& request, const QByteArray& replyData)
{
	m_replyResultPtr = new imtbase::CTreeItemModel();
	QJsonDocument document = QJsonDocument::fromJson(replyData);
	if (document.isObject()){
		QJsonObject dataObject = document.object().value("data").toObject();
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


