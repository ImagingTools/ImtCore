#include <graphqlserver/CExternGraphQlControllerComp.h>

// ImtCore includes


namespace graphqlserver
{
// public methods
bool CExternGraphQlControllerComp::RegisterGqlHandler(const QByteArray& commandId, graphqlserver::IGqlRequestHandler& gqlRequestHandler, QString& errorMessage)
{
	m_gqlRequestHandlers.insert(commandId, &gqlRequestHandler);

	return true;
}


// protected methods
// reimplemented (imtgql::IGqlRequestHandler)
bool CExternGraphQlControllerComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	QByteArray commandId = gqlRequest.GetCommandId();

	return m_gqlRequestHandlers.contains(commandId);
}


// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CExternGraphQlControllerComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QByteArray commandId = gqlRequest.GetCommandId();
	if (m_gqlRequestHandlers.contains(commandId)){
		// graphqlserver::CGqlRequest externRequest = ConvertRequest(gqlRequest);
		QJsonObject inputParams;
		const imtgql::CGqlObject& gqlParams = gqlRequest.GetParams();
		CreateParams(gqlParams, inputParams);
		const imtgql::CGqlObject& gqlResultKeys = gqlRequest.GetFields();
		graphqlserver::ResultKeys resultKeys;
		CreateResultKeys(gqlResultKeys, resultKeys);
		graphqlserver::RequestType requestType = static_cast<graphqlserver::RequestType>(gqlRequest.GetRequestType());

		QJsonObject jsonData = m_gqlRequestHandlers.value(commandId)->CreateResponse(commandId, requestType, inputParams, resultKeys, errorMessage);
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
		QJsonDocument jsonDocument;
		jsonDocument.setObject(jsonData);
		QByteArray data = jsonDocument.toJson(QJsonDocument::Compact);
		rootModelPtr->CreateFromJson(data);

		return rootModelPtr.PopPtr();
	}

	return nullptr;
}


} // namespace graphqlserver


