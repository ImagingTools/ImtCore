// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CSettingsControllerComp.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::V1_0::imtbase::CSettingsGqlHandlerCompBase)

sdl::V1_0::imtbase::CSetSettingsPayload CSettingsControllerComp::OnSetSettings(
			const sdl::V1_0::imtbase::CSetSettingsGqlRequest& setSettingsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CSetSettingsPayload response;
	
	sdl::V1_0::imtbase::SetSettingsRequestArguments arguments = setSettingsRequest.GetRequestedArguments();
	if (!arguments.input || !arguments.input->userId.has_value()){
		Q_ASSERT(false);
		return response;
	}
	
	QByteArray userId = *arguments.input->userId;

	if (userId.isEmpty()){
		errorMessage = QString("Unable to set settings. User-ID is empty!");
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}

	if (!arguments.input->settings.has_value()){
		Q_ASSERT(false);
		return response;
	}
	
	QString settings = *arguments.input->settings;
	
	response.ok = false;

	imtauth::IUserSettingsSharedPtr userSettingsPtr = GetOrCreateUserSettings(userId);
	if (!userSettingsPtr.IsValid()){
		errorMessage = QString("Unable to set settings for user '%1'. Error: User settings is invalid");
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();
	if (paramSetPtr == nullptr){
		Q_ASSERT(false);
		errorMessage = QString("Unable to set settings for user '%1'. Error: User settings is invalid");
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	QJsonParseError error;
	QJsonDocument representationDoc = QJsonDocument::fromJson(settings.toUtf8(), &error);
	if (error.error != QJsonParseError::NoError){
		errorMessage = QString("Unable to set settings for user '%1'. Error: Settings json invalid").arg(userId);
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	if (!m_userSettingsRepresentationControllerCompPtr->GetDataModelFromRepresentation(representationDoc.object(), *paramSetPtr)){
		errorMessage = QString("Unable to set settings for user '%1'. Error: Get data model from representation failed").arg(userId);
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	if (!SetUserSettings(userId, *userSettingsPtr)){
		errorMessage = QString("Unable to set settings for user '%1'").arg(userId);
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}

	response.ok = true;
	
	return response;
}


sdl::V1_0::imtbase::CParamsSet CSettingsControllerComp::OnGetSettings(
			const sdl::V1_0::imtbase::CGetSettingsGqlRequest& getSettingsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CParamsSet response;
	
	if (!m_userSettingsCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserSettingsCollection' was not set", "CSettingsControllerComp");
		return response;
	}
	
	if (!m_userSettingsInfoFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserSettingsInfoFact' was not set", "CSettingsControllerComp");
		return response;
	}
	
	if (!m_userSettingsRepresentationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserSettingsRepresentationController' was not set", "CSettingsControllerComp");
		return response;
	}
	
	sdl::V1_0::imtbase::GetSettingsRequestArguments arguments = getSettingsRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	QByteArray userId;
	if (arguments.input->userId){
		userId = *arguments.input->userId;
	}

	if (userId.isEmpty()){
		errorMessage = QString("Unable to get settings. User-ID is empty!");
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}

	QByteArray languageId;
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}
	
	iprm::CParamsSet paramsSet;
	iprm::CIdParam languageParam;
	languageParam.SetId(languageId);
	paramsSet.SetEditableParameter("LanguageParam", &languageParam);
	
	imtauth::IUserSettingsSharedPtr userSettingsPtr = GetOrCreateUserSettings(userId);
	if (!userSettingsPtr.IsValid()){
		errorMessage = QString("Unable to get settings for user '%1'. Error: User settings is invalid").arg(userId);
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();
	Q_ASSERT(paramSetPtr != nullptr);
	if (paramSetPtr == nullptr){
		errorMessage = QString("Unable to create representation for user settings. Error: Params set from user settings is invalid.");
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	QJsonObject representationObject;
	if (!m_userSettingsRepresentationControllerCompPtr->GetRepresentationFromDataModel(*paramSetPtr, representationObject, &paramsSet)){
		errorMessage = QString("Unable to get settings for user '%1'. Error: Get representation failed").arg(userId);
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	if (!response.ReadFromJsonObject(representationObject)){
		errorMessage = QString("Unable to get settings for user '%1'. Error: Read from Json object failed").arg(userId);
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	return response;
}


sdl::V1_0::imtbase::CStyleData CSettingsControllerComp::OnGetStyleData(
			const sdl::V1_0::imtbase::CGetStyleDataGqlRequest& getStyleRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::V1_0::imtbase::CStyleData response;
	
	sdl::V1_0::imtbase::GetStyleDataRequestArguments arguments = getStyleRequest.GetRequestedArguments();
	if (!arguments.input.has_value()){
		Q_ASSERT(false);
		return response;
	}

	QByteArray schemeId;
	if (arguments.input->schemeId){
		schemeId = *arguments.input->schemeId;
	}
	
	if(schemeId.isEmpty()){
		errorMessage = QString("Unable to get style data. Error: Scheme is empty");
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	QString prefix;
	if (m_styleFileNamePrefixAttrPtr.IsValid() && !(*m_styleFileNamePrefixAttrPtr).isEmpty()){
		prefix = *m_styleFileNamePrefixAttrPtr + '_';
	}
	
	QString pathToTheme = prefix + schemeId.toLower() + ".theme";
	QFile resource(":/Style/" + pathToTheme);
	if (!resource.open(QIODevice::ReadOnly)){
		errorMessage = QString("Unable to get style data. Error: '%1'").arg(resource.errorString());
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	response.data = resource.readAll();
	resource.close();
	
	return response;
}


sdl::V1_0::imtbase::CUrlParam CSettingsControllerComp::OnGetWebSocketUrl(
			const sdl::V1_0::imtbase::CGetWebSocketUrlGqlRequest& /*getWebSocketUrlRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_serverInterfaceCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'WebSocketUrlParam' was not set", "CSettingsControllerComp");
		return sdl::V1_0::imtbase::CUrlParam();
	}
	
	sdl::V1_0::imtbase::CUrlParam response;
	QUrl url;
	
	if (m_serverInterfaceCompPtr->GetUrl(imtcom::IServerConnectionInterface::PT_WEBSOCKET, url)){
		response.host = url.host();
		response.port = url.port();
		response.scheme = url.scheme();

		return response;
	}

	errorMessage = "Websocket URL could not be provided";

	return sdl::V1_0::imtbase::CUrlParam();
}


// private methods

imtauth::IUserSettingsSharedPtr CSettingsControllerComp::GetOrCreateUserSettings(const QByteArray& userId) const
{
	if (userId.isEmpty()){
		SendErrorMessage(0, QString("Unable to get or create user settings. User-ID is empty!"), "CSettingsControllerComp");
		return nullptr;
	}

	if (!m_userSettingsCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserSettingsCollection' was not set", "CSettingsControllerComp");
		return nullptr;
	}
	
	if (!m_userSettingsInfoFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'UserSettingsInfoFact' was not set", "CSettingsControllerComp");
		return nullptr;
	}
	
	imtauth::IUserSettingsSharedPtr userSettingsPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
		userSettingsPtr.SetCastedPtr(dataPtr);
	}
	
	if (!userSettingsPtr.IsValid()){
		userSettingsPtr.FromUnique(m_userSettingsInfoFactCompPtr.CreateInstance());
		Q_ASSERT(userSettingsPtr.IsValid());
		if (!userSettingsPtr.IsValid()){
			return nullptr;
		}
		
		userSettingsPtr->SetUserId(userId);
		
		QByteArray retVal = m_userSettingsCollectionCompPtr->InsertNewObject("UserSettings", "", "", userSettingsPtr.GetPtr(), userId);
		if (retVal.isEmpty()){
			return nullptr;
		}
	}
	
	return userSettingsPtr;
}


bool CSettingsControllerComp::SetUserSettings(const QByteArray& userId, const imtauth::IUserSettings& userSettings) const
{
	if (!m_userSettingsCollectionCompPtr.IsValid()){
		return false;
	}
	
	if (!m_userSettingsCollectionCompPtr->SetObjectData(userId, userSettings)){
		return false;
	}
	
	return true;
}


} // namespace imtservergql


