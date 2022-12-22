#include <imtgql/CGqlRepresentationControllerComp.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerCompBase)

imtbase::CHierarchicalItemModelPtr CGqlRepresentationControllerComp::CreateInternalResponse(const CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1){
		return imtbase::CHierarchicalItemModelPtr();
	}

	Response responseHandler;
	bool retVal = m_apiClientCompPtr->SendRequest(gqlRequest, responseHandler);
	if (retVal){
		return responseHandler.GetResult();
	}
	return imtbase::CHierarchicalItemModelPtr();
}


// public methods

imtbase::CHierarchicalItemModelPtr CGqlRepresentationControllerComp::Response::GetResult()
{
	return m_replyResultPtr;
}


// reimplemented (imtgql::IGqlClient::ResponseHandler)

void CGqlRepresentationControllerComp::Response::OnReply(const IGqlRequest& request, const QByteArray& replyData)
{
	m_replyResultPtr.SetPtr(new imtbase::CTreeItemModel());
	QJsonDocument document = QJsonDocument::fromJson(replyData);
	if (document.isObject()){
		QJsonObject dataObject = document.object().value("data").toObject();
		if (!dataObject.isEmpty()){
			QJsonObject bodyObject = dataObject.value(request.GetCommandId()).toObject();
			if (!bodyObject.isEmpty()){
				dataObject = QJsonObject();
				dataObject.insert("data", bodyObject);
				document.setObject(dataObject);
				QByteArray parserData = document.toJson(QJsonDocument::Compact);
				m_replyResultPtr->CreateFromJson(parserData);
			}
		}
	}
}


} // namespace imtgql


