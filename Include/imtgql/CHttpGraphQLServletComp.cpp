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

imtrest::ConstResponsePtr CHttpGraphQLServletComp::OnPost(
			const QByteArray& /*commandId*/,
			const imtrest::IRequest::CommandParams& /*commandParams*/,
			const HeadersMap& headers,
			const imtrest::CHttpRequest& request) const
{
	m_lastRequest.ResetData();

	int errorPosition = -1;
	QByteArray requestBody = request.GetBody();
	if (!m_lastRequest.ParseQuery(requestBody, errorPosition)){
		qCritical() << __FILE__ << __LINE__ << QString("Error when parsing request: %1; Error position: %2")
						.arg(qPrintable(request.GetBody()))
						.arg(errorPosition);
	}

	QByteArray gqlCommand = m_lastRequest.GetCommandId();

	QByteArray userId;

	imtgql::IGqlContext* gqlContextPtr = nullptr;
	QByteArray accessToken = headers.value("X-authentication-token");
	if (!accessToken.isEmpty() && m_gqlContextControllerCompPtr.IsValid()){
		QString errorMessage;

		gqlContextPtr = m_gqlContextControllerCompPtr->GetRequestContext(m_lastRequest, accessToken, errorMessage);
		if (gqlContextPtr != nullptr){
			m_lastRequest.SetGqlContext(gqlContextPtr);

			if (gqlContextPtr->GetUserInfo() != nullptr){
				userId = gqlContextPtr->GetUserInfo()->GetId();
			}
		}
		else{
			SendWarningMessage(0, QString("Invalid GraphQL-context for the access token %1. Command-ID: %2.").arg(qPrintable(accessToken)).arg(qPrintable(gqlCommand)), "GraphQL - servlet");

			QByteArray responseData;

			imtbase::CTreeItemModel rootModel;
			imtbase::CTreeItemModel* errorsModelPtr = rootModel.AddTreeModel("errors");
			imtbase::CTreeItemModel* errorItemModelPtr = errorsModelPtr->AddTreeModel(gqlCommand);

			errorItemModelPtr->SetData("message", errorMessage);
			errorItemModelPtr->SetData("type", "Critical");

			{
				iser::CJsonMemWriteArchive archive(responseData);
				rootModel.SerializeModel(archive);
			}

			if (!responseData.isEmpty()){
				return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_OK, responseData, request);
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
			if (requestHandlerPtr->IsRequestSupported(m_lastRequest)){
				istd::TDelPtr<imtbase::CTreeItemModel> sourceItemModelPtr;
				sourceItemModelPtr.SetPtr(requestHandlerPtr->CreateResponse(m_lastRequest, errorMessage));

				if(sourceItemModelPtr.IsValid()){
					imtbase::CTreeItemModel rootModel;
					imtbase::CTreeItemModel* dataItemModel = rootModel.AddTreeModel("data");
					dataItemModel->SetExternTreeModel(gqlCommand, sourceItemModelPtr->GetTreeItemModel("data"));

					imtbase::CTreeItemModel* errorsSourceItemModel = sourceItemModelPtr->GetTreeItemModel("errors");
					if (errorsSourceItemModel != nullptr){
						imtbase::CTreeItemModel* errorsItemModel = rootModel.GetTreeItemModel("errors");
						if (errorsItemModel == nullptr){
							errorsItemModel = rootModel.AddTreeModel("errors");
						}

						if (!errorsSourceItemModel->ContainsKey("type")){
							errorsSourceItemModel->SetData("type", "Warning");
						}

						errorsItemModel->SetExternTreeModel(gqlCommand, errorsSourceItemModel);
					}

					isSuccessful = true;

					iser::CJsonMemWriteArchive archive(responseData);
					if (!rootModel.SerializeModel(archive)){
						isSuccessful = false;
					}
				}
				else{
					if (!errorMessage.isEmpty()){
						imtbase::CTreeItemModel rootModel;
						imtbase::CTreeItemModel* errorsModelPtr = rootModel.AddTreeModel("errors");
						imtbase::CTreeItemModel* errorItemModelPtr = errorsModelPtr->AddTreeModel(gqlCommand);

						errorItemModelPtr->SetData("message", errorMessage);
						errorItemModelPtr->SetData("type", "Warning");

						iser::CJsonMemWriteArchive archive(responseData);
						if (!rootModel.SerializeModel(archive)){
						}
					}

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
		SendErrorMessage(0, QString("Invalid command request: %1").arg(qPrintable(gqlCommand)), "GraphQL - servlet");

		return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_BAD_REQUEST, responseData, request);
	}
	else {
		if (!responseData.isEmpty()){
			return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_OK, responseData, request);
		}
	}

	SendErrorMessage(0, QString("Internal server error for command %1").arg(qPrintable(gqlCommand)), "GraphQL - servlet");

	return GenerateError(imtrest::IProtocolEngine::StatusCode::SC_INTERNAL_SERVER_ERROR,"Request incorrected",request);
}


// reimplemented (imtgql::IGqlRequestProvider)

const IGqlRequest* CHttpGraphQLServletComp::GetGqlRequest() const
{
	return &m_lastRequest;
}


// private methods

imtrest::ConstResponsePtr CHttpGraphQLServletComp::CreateResponse(
			const imtrest::IProtocolEngine::StatusCode& statusCode,
			const QByteArray& payload,
			const imtrest::IRequest& request,
			const QByteArray& contentTypeId) const
{
	return imtrest::ConstResponsePtr(request.GetProtocolEngine().CreateResponse(request, statusCode, payload, contentTypeId));
}


imtrest::ConstResponsePtr CHttpGraphQLServletComp::GenerateError(
			const imtrest::IProtocolEngine::StatusCode& errorCode,
			const QString& /*errorString*/,
			const imtrest::CHttpRequest& request) const
{
	const imtrest::IProtocolEngine& engine = request.GetProtocolEngine();

	int protocolErrorCode = 200;
	QByteArray protocolErrorString;
	engine.GetProtocolStatusCode(errorCode, protocolErrorCode, protocolErrorString);

	QByteArray responseJson;
	return imtrest::ConstResponsePtr(
				engine.CreateResponse(
							request,
							errorCode,
							responseJson,
							QByteArray("application/json;charset=utf-8")));
}


} // namespace imtgql


