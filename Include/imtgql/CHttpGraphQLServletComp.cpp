#include "CHttpGraphQLServletComp.h"

// Acf includes
#include <iser/CJsonStringWriteArchive.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtrest/IProtocolEngine.h>


namespace imtgql
{


// reimplemented (ilog::CLoggerComponentBase)
void CHttpGraphQLServletComp::OnComponentCreated()
{
	ilog::CLoggerComponentBase::OnComponentCreated();
}

void CHttpGraphQLServletComp::OnComponentDestroyed()
{
	ilog::CLoggerComponentBase::OnComponentDestroyed();
}

// reimplemented (imtrest::IRequestServlet)

imtrest::IRequestServlet::ConstResponsePtr CHttpGraphQLServletComp::OnPost(const QByteArray& commandId, const imtrest::IRequest::CommandParams& commandParams, const HeadersMap& headers, const imtrest::CHttpRequest& request) const
{
	using statusCode = imtrest::IProtocolEngine::StatusCode;

	imtgql::CGqlRequest gqlRequest;
	int errorPosition = -1;

	gqlRequest.ParseQuery(request.GetBody(), errorPosition);
	QByteArray representationData;
	bool gqlError = false;

	{
		imtbase::CTreeItemModel rootModel;
		rootModel.AddTreeModel("data");

		QString errorMessage;
		QByteArray gqlCommand = gqlRequest.GetCommandId();
		int dataControllersCount = m_gqlRepresentationDataControllerCompPtr.GetCount();
		for (int index = 0; index < dataControllersCount; index++){
			QByteArrayList modelIds = m_gqlRepresentationDataControllerCompPtr[index]->GetModelIds();
			if (modelIds.contains(gqlCommand)){
				imtbase::CTreeItemModel* sourceItemModel = m_gqlRepresentationDataControllerCompPtr[index]->CreateResponse(gqlRequest, errorMessage);
				if(sourceItemModel != nullptr){
					imtbase::CTreeItemModel* dataItemModel = rootModel.GetTreeItemModel("data");
					imtbase::CTreeItemModel* errorsSourceItemModel = sourceItemModel->GetTreeItemModel("errors");
					dataItemModel->SetExternTreeModel(gqlCommand, sourceItemModel->GetTreeItemModel("data"));
					if (errorsSourceItemModel != nullptr){
						imtbase::CTreeItemModel* errorsItemModel = rootModel.GetTreeItemModel("errors");
						if (errorsItemModel == nullptr){
							errorsItemModel = rootModel.AddTreeModel("errors");
						}
						errorsItemModel->SetExternTreeModel(gqlCommand, errorsSourceItemModel);
						gqlError = true;
					}
				}
				else {
					qCritical() << __FILE__ << __LINE__ << "{CRITICAL!!!} sourceItemModel IS NULL!!!";
				}
			}

		}
		iser::CJsonStringWriteArchive archive(representationData);

		bool retVal = rootModel.Serialize(archive);
	}

	if (gqlError == true){
		return CreateResponse(statusCode::SC_BAD_REQUEST, representationData, request);
	}
	else if (!representationData.isEmpty()){
		return CreateResponse(statusCode::SC_OK, representationData, request);
	}

	return GenerateError(statusCode::SC_INTERNAL_SERVER_ERROR,"Request incorrected",request);
}


imtrest::IRequestServlet::ConstResponsePtr CHttpGraphQLServletComp::CreateResponse(const imtrest::IProtocolEngine::StatusCode &statusCode, const QByteArray &payload, const imtrest::IRequest &request, const QByteArray &contentType) const
{
	return imtrest::IRequestServlet::ConstResponsePtr(
				request.GetProtocolEngine().CreateResponse(
				request,
				statusCode,
				payload,
				contentType));
}

imtrest::IRequestServlet::ConstResponsePtr CHttpGraphQLServletComp::GenerateError(const imtrest::IProtocolEngine::StatusCode &errorCode, const QString &errorString, const imtrest::CHttpRequest &request) const
{
	int protocolErrorCode = 200;
	QByteArray protocolErrorString;
	request.GetProtocolEngine().GetProtocolStatusCode(errorCode, protocolErrorCode, protocolErrorString);

	QByteArray responseJson;

	return imtrest::IRequestServlet::ConstResponsePtr(request.GetProtocolEngine().CreateResponse(
														request,
														errorCode,
														responseJson,
														QByteArray("application/json;charset=utf-8")));

}



} // namespace imtgql

