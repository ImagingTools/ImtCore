#include <imtgql/CHttpGraphQLServletComp.h>


// ACF includes
#include <iser/CJsonStringWriteArchive.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtgql/CGqlContext.h>
#include <imtrest/IProtocolEngine.h>
#include <imtauth/ISession.h>
#include <imtauth/IUserInfo.h>
#include <imtauth/IUserSettings.h>


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

	QByteArray accessToken = headers.value("X-authentication-token");

	if (m_sessionCollectionCompPtr.IsValid() && !accessToken.isEmpty()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_sessionCollectionCompPtr->GetObjectData(accessToken, dataPtr)){
			const imtauth::ISession* sessionInfoPtr = dynamic_cast<const imtauth::ISession*>(dataPtr.GetPtr());
			if (sessionInfoPtr != nullptr){
				imtgql::CGqlContext* gqlContextPtr = new imtgql::CGqlContext();

				QByteArray userId = sessionInfoPtr->GetUserId();

				if (m_userCollectionCompPtr.IsValid()){
					imtbase::IObjectCollection::DataPtr userDataPtr;
					if (m_userCollectionCompPtr->GetObjectData(userId, userDataPtr)){
						const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());
						if (userInfoPtr != nullptr){
							gqlContextPtr->SetUserInfo(dynamic_cast<const imtauth::IUserInfo*>(userInfoPtr->CloneMe()));
						}
					}
				}

				if (m_settingsCollectionCompPtr.IsValid()){
					imtbase::IObjectCollection::DataPtr settingsDataPtr;
					if (m_settingsCollectionCompPtr->GetObjectData(userId, settingsDataPtr)){
						const imtauth::IUserSettings* userSettingsPtr = dynamic_cast<const imtauth::IUserSettings*>(settingsDataPtr.GetPtr());
						if (userSettingsPtr != nullptr){
							iprm::IParamsSet* settingsPtr = userSettingsPtr->GetSettings();
							if (settingsPtr != nullptr){
								const iprm::ISelectionParam* languageParamPtr = dynamic_cast<const iprm::ISelectionParam*>(settingsPtr->GetParameter("Language"));
								if (languageParamPtr != nullptr){
									const iprm::IOptionsList* optionList = languageParamPtr->GetSelectionConstraints();
									int index = languageParamPtr->GetSelectedOptionIndex();

									QByteArray languageId = optionList->GetOptionId(index);

									gqlContextPtr->SetLanguageId(languageId);
								}
							}
						}
					}
				}

				gqlContextPtr->SetToken(accessToken);
				gqlRequest.SetGqlContext(gqlContextPtr);
			}
		}
	}

	QByteArray representationData;
	bool isSuccessful = false;
	QString errorMessage;
	QByteArray gqlCommand = gqlRequest.GetCommandId();

	int dataControllersCount = m_gqlRepresentationDataControllerCompPtr.GetCount();
	for (int index = 0; index < dataControllersCount; index++){
		const imtgql::IGqlRepresentationDataController* representationControllerPtr = m_gqlRepresentationDataControllerCompPtr[index];
		if (representationControllerPtr != nullptr){
			QByteArrayList modelIds = representationControllerPtr->GetModelIds();
			if (modelIds.contains(gqlCommand)){
				imtbase::CHierarchicalItemModelPtr sourceItemModel = representationControllerPtr->CreateResponse(gqlRequest, errorMessage);
				if(sourceItemModel.IsValid()){
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

					iser::CJsonStringWriteArchive archive(representationData);
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
		return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_BAD_REQUEST, representationData, request);
	}
	else {
		if (!representationData.isEmpty()){
			return CreateResponse(imtrest::IProtocolEngine::StatusCode::SC_OK, representationData, request);
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


