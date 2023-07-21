#include <imtgql/CHttpGraphQLServletComp.h>


// ACF includes
#include <iser/CJsonMemWriteArchive.h>
#include <iprm/TParamsPtr.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtauth/IUserSettings.h>
#include <imtgql/CGqlRequest.h>
#include <imtgql/IGqlContext.h>
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

	QByteArray requestBody = request.GetBody();
	if (!gqlRequest.ParseQuery(requestBody, errorPosition)){
		qCritical() << __FILE__ << __LINE__ << QString("Error when parsing request: %1; Error position: %2")
						.arg(qPrintable(request.GetBody()))
						.arg(errorPosition);
	}

	QByteArray gqlCommand = gqlRequest.GetCommandId();

	QByteArray userId;

	imtgql::IGqlContext* gqlContextPtr = nullptr;
	QByteArray accessToken = headers.value("X-authentication-token");
	if (!accessToken.isEmpty() && m_gqlContextControllerCompPtr.IsValid()){
		gqlContextPtr = m_gqlContextControllerCompPtr->GetRequestContext(gqlRequest, accessToken);

		if (gqlContextPtr != nullptr){
			gqlRequest.SetGqlContext(gqlContextPtr);

			if (gqlContextPtr->GetUserInfo() != nullptr){
				userId = gqlContextPtr->GetUserInfo()->GetId();
			}
		}
	}

	QByteArray languageId;
	QByteArray designSchemeId;

	if (m_userSettingsCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
			imtauth::IUserSettings* userSettingsPtr = dynamic_cast<imtauth::IUserSettings*>(dataPtr.GetPtr());
			if (userSettingsPtr != nullptr){
				iprm::IParamsSet* paramsSetPtr = userSettingsPtr->GetSettings();
				if (paramsSetPtr != nullptr){
					iprm::TParamsPtr<iprm::ISelectionParam> languageParamPtr(paramsSetPtr, "Language");
					if (languageParamPtr.IsValid()){
						const iprm::IOptionsList* optionListPtr = languageParamPtr->GetSelectionConstraints();
						if (optionListPtr != nullptr){
							int index = languageParamPtr->GetSelectedOptionIndex();
							if (index >= 0){
								languageId = optionListPtr->GetOptionId(index);
							}
						}
					}

					iprm::TParamsPtr<iprm::ISelectionParam> designParamPtr(paramsSetPtr, "DesignSchema");
					if (designParamPtr.IsValid()){
						const iprm::IOptionsList* optionListPtr = designParamPtr->GetSelectionConstraints();
						if (optionListPtr != nullptr){
							int index = designParamPtr->GetSelectedOptionIndex();
							if (index >= 0){
								designSchemeId = optionListPtr->GetOptionId(index);
							}
						}
					}
				}
			}
		}
	}

	if (gqlContextPtr != nullptr){
		gqlContextPtr->SetLanguageId(languageId);
		gqlContextPtr->SetDesignScheme(designSchemeId);
	}

	QByteArray responseData;
	bool isSuccessful = false;
	QString errorMessage;

	int dataControllersCount = m_gqlRequestHandlerCompPtr.GetCount();
	for (int index = 0; index < dataControllersCount; index++){
		const imtgql::IGqlRequestHandler* requestHandlerPtr = m_gqlRequestHandlerCompPtr[index];
		if (requestHandlerPtr != nullptr){
			if (requestHandlerPtr->IsRequestSupported(gqlRequest)){
				imtbase::CTreeItemModel* sourceItemModel = requestHandlerPtr->CreateResponse(gqlRequest, errorMessage);
				if(sourceItemModel != nullptr){
					imtbase::CTreeItemModel rootModel;
					imtbase::CTreeItemModel* dataItemModel = rootModel.AddTreeModel("data");
					dataItemModel->SetExternTreeModel(gqlCommand, sourceItemModel->GetTreeItemModel("data"));

					imtbase::CTreeItemModel* errorsSourceItemModel = sourceItemModel->GetTreeItemModel("errors");
					if (errorsSourceItemModel != nullptr){
						imtbase::CTreeItemModel* errorsItemModel = rootModel.GetTreeItemModel("errors");
						if (errorsItemModel == nullptr){
							errorsItemModel = rootModel.AddTreeModel("errors");
						}

						errorsItemModel->SetExternTreeModel(gqlCommand, errorsSourceItemModel);
					}

					isSuccessful = true;

					iser::CJsonMemWriteArchive archive(responseData);
					if (!rootModel.Serialize(archive)){
						isSuccessful = false;
					}
				}
				else{
					QString servletErrorMessage = QString("Can not create response for command: '%1'. Info:'%2'").arg(qPrintable(gqlCommand)).arg(errorMessage);
					SendErrorMessage(0, servletErrorMessage, "GraphQL - servlet");
				}
				break;
			}
		}
		else{
			qCritical() << __FILE__ << __LINE__ << "Representation controller component could not be resolved";
		}
	}

	if (!isSuccessful){
		return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_BAD_REQUEST, responseData, request);
	}
	else {
		if (!responseData.isEmpty()){
			return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_OK, responseData, request);
		}
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


