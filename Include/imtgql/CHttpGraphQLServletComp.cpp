#include <imtgql/CHttpGraphQLServletComp.h>


// ACF includes
#include <iser/CJsonStringWriteArchive.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtrest/IProtocolEngine.h>


namespace imtgql
{


// reimplemented (imtrest::IRequestServlet)

imtrest::IRequestServlet::ConstResponsePtr CHttpGraphQLServletComp::OnPost(
			const QByteArray& commandId,
			const imtrest::IRequest::CommandParams& commandParams,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	imtgql::CGqlRequest gqlRequest;
	int errorPosition = -1;

	if (!gqlRequest.ParseQuery(request.GetBody(), errorPosition)){
		qCritical() << __FILE__ << __LINE__ << QString("Error when parsing request: %1; Error position: %2")
												.arg(qPrintable(request.GetBody()))
												.arg(errorPosition);
	}

	QByteArray representationData;
	bool gqlError = false;

	{
		imtbase::CTreeItemModel rootModel;
		rootModel.AddTreeModel("data");

		QString errorMessage;
		QByteArray gqlCommand = gqlRequest.GetCommandId();

		int dataControllersCount = m_gqlRepresentationDataControllerCompPtr.GetCount();
		for (int index = 0; index < dataControllersCount; index++){
			const imtgql::IGqlRepresentationDataController* representationControllerPtr = m_gqlRepresentationDataControllerCompPtr[index];
			if (representationControllerPtr != nullptr){
				QByteArrayList modelIds = representationControllerPtr->GetModelIds();
				if (modelIds.contains(gqlCommand)){
					imtbase::CTreeItemModel* sourceItemModel = representationControllerPtr->CreateResponse(gqlRequest, errorMessage);
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
					else{
						qCritical() << __FILE__ << __LINE__ << "source itemo model is invalid";
					}
				}
			}
			else{
				qCritical() << __FILE__ << __LINE__ << "Representation controller component could not be resolved";
			}
		}

		iser::CJsonStringWriteArchive archive(representationData);

		bool retVal = rootModel.Serialize(archive);
	}

	if (gqlError == true){
		return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_BAD_REQUEST, representationData, request);
	}
	else if (!representationData.isEmpty()){
		return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_OK, representationData, request);
	}

	return GenerateError(imtrest::IProtocolEngine::StatusCode::SC_INTERNAL_SERVER_ERROR,"Request incorrected",request);
}


// private methods

imtrest::IRequestServlet::ConstResponsePtr CHttpGraphQLServletComp::CreateResponse(
			const imtrest::IProtocolEngine::StatusCode& statusCode,
			const QByteArray& payload,
			const imtrest::IRequest& request,
			const QByteArray& contentTypeId) const
{
	return imtrest::IRequestServlet::ConstResponsePtr(request.GetProtocolEngine().CreateResponse(request, statusCode, payload, contentTypeId));
}


imtrest::IRequestServlet::ConstResponsePtr CHttpGraphQLServletComp::GenerateError(
			const imtrest::IProtocolEngine::StatusCode& errorCode,
			const QString& errorString,
			const imtrest::CHttpRequest& request) const
{
	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	int protocolErrorCode = 200;
	QByteArray protocolErrorString;
	engine.GetProtocolStatusCode(errorCode, protocolErrorCode, protocolErrorString);

	QByteArray responseJson;
	return imtrest::IRequestServlet::ConstResponsePtr(
				engine.CreateResponse(
							request,
							errorCode,
							responseJson,
							QByteArray("application/json;charset=utf-8")));
}


} // namespace imtgql


