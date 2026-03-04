// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CSettingsControllerComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>


namespace imtservergql
{


// protected methods

// reimplemented (sdl::imtbase::Settings::CGraphQlHandlerCompBase)

sdl::imtbase::Settings::CSetSettingsPayload CSettingsControllerComp::OnSetSettings(
			const sdl::imtbase::Settings::CSetSettingsGqlRequest& setSettingsRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::Settings::CSetSettingsPayload response;
	
	sdl::imtbase::Settings::SetSettingsRequestArguments arguments = setSettingsRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return response;
	}
	
	if (!arguments.input.Version_1_0->userId.has_value()){
		Q_ASSERT(false);
		return response;
	}
	
	QByteArray userId = *arguments.input.Version_1_0->userId;
	
	if (!arguments.input.Version_1_0->settings.has_value()){
		Q_ASSERT(false);
		return response;
	}
	
	QString settings = *arguments.input.Version_1_0->settings;
	
	response.Version_1_0.emplace();
	response.Version_1_0->ok = false;

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
		errorMessage = QString("Unable to set settings for user '%1'. Error: Settings json invalid").arg(qPrintable(userId));
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	if (!m_userSettingsRepresentationControllerCompPtr->GetDataModelFromRepresentation(representationDoc.object(), *paramSetPtr)){
		errorMessage = QString("Unable to set settings for user '%1'. Error: Get data model from representation failed").arg(qPrintable(userId));
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	if (!SetUserSettings(userId, *userSettingsPtr)){
		errorMessage = QString("Unable to set settings for user '%1'").arg(qPrintable(userId));
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}

	response.Version_1_0->ok = true;
	
	return response;
}


sdl::imtbase::ImtBaseTypes::CParamsSet CSettingsControllerComp::OnGetSettings(
			const sdl::imtbase::Settings::CGetSettingsGqlRequest& getSettingsRequest,
			const ::imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	sdl::imtbase::ImtBaseTypes::CParamsSet response;
	
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
	
	sdl::imtbase::Settings::GetSettingsRequestArguments arguments = getSettingsRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return response;
	}
	
	QByteArray userId;
	if (arguments.input.Version_1_0->userId){
		userId = *arguments.input.Version_1_0->userId;
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
		errorMessage = QString("Unable to get settings for user '%1'. Error: User settings is invalid").arg(qPrintable(userId));
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
		errorMessage = QString("Unable to get settings for user '%1'. Error: Get representation failed").arg(qPrintable(userId));
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	response.Version_1_0.emplace();
	
	if (!response.Version_1_0->ReadFromJsonObject(representationObject)){
		errorMessage = QString("Unable to get settings for user '%1'. Error: Read from Json object failed").arg(qPrintable(userId));
		SendErrorMessage(0, errorMessage, "CSettingsControllerComp");
		return response;
	}
	
	return response;
}


sdl::imtbase::Settings::CStyleData CSettingsControllerComp::OnGetStyleData(
			const sdl::imtbase::Settings::CGetStyleDataGqlRequest& getStyleRequest,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	sdl::imtbase::Settings::CStyleData response;
	
	sdl::imtbase::Settings::GetStyleDataRequestArguments arguments = getStyleRequest.GetRequestedArguments();
	if (!arguments.input.Version_1_0.has_value()){
		Q_ASSERT(false);
		return response;
	}
	
	QByteArray schemeId;
	if (arguments.input.Version_1_0->schemeId){
		schemeId = *arguments.input.Version_1_0->schemeId;
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
	
	response.Version_1_0.emplace();
	response.Version_1_0->data = resource.readAll();
	resource.close();
	
	return response;
}


sdl::imtbase::ImtBaseTypes::CUrlParam CSettingsControllerComp::OnGetWebSocketUrl(
			const sdl::imtbase::Settings::CGetWebSocketUrlGqlRequest& /*getWebSocketUrlRequest*/,
			const ::imtgql::CGqlRequest& /*gqlRequest*/,
			QString& errorMessage) const
{
	if (!m_serverInterfaceCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'WebSocketUrlParam' was not set", "CSettingsControllerComp");
		return sdl::imtbase::ImtBaseTypes::CUrlParam();
	}
	
	sdl::imtbase::ImtBaseTypes::CUrlParam response;
	response.Version_1_0.emplace();
	
	QUrl url;
	
	if (m_serverInterfaceCompPtr->GetUrl(imtcom::IServerConnectionInterface::PT_WEBSOCKET, url)){
		response.Version_1_0->host = url.host();
		response.Version_1_0->port = url.port();
		response.Version_1_0->scheme = url.scheme();

		return response;
	}

	errorMessage = "Websocket URL could not be provided";

	return sdl::imtbase::ImtBaseTypes::CUrlParam();
}


// private methods

imtauth::IUserSettingsSharedPtr CSettingsControllerComp::GetOrCreateUserSettings(const QByteArray& userId) const
{
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
	if (!userId.isEmpty()){
		if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
			userSettingsPtr.SetCastedPtr(dataPtr);
		}
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


