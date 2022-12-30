#include <imtgql/CGqlRemoteRepresentationControllerComp.h>


namespace imtgql
{


// public methods

// reimplemented (imtgql::IGqlClient::ResponseHandler)

imtbase::CHierarchicalItemModelPtr CGqlRemoteRepresentationControllerComp::Response::GetResult()
{
	return m_replyResultPtr;
}


void CGqlRemoteRepresentationControllerComp::Response::OnReply(const IGqlRequest& request, const QByteArray& replyData)
{
	m_replyResultPtr.SetPtr(new imtbase::CTreeItemModel());
	QJsonDocument document = QJsonDocument::fromJson(replyData);
	if (document.isObject()){
		QJsonValue jsonValue = document.object().value("data");
		QJsonObject dataObject = document.object().value("data").toObject();
		document.setObject(dataObject);
		QByteArray parserData = document.toJson(QJsonDocument::Compact);
		m_replyResultPtr->CreateFromJson(replyData);
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
		return responseHandler.GetResult().GetPtr();
	}

	return nullptr;
}


} // namespace imtgql


