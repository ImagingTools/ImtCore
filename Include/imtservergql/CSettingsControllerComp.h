// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>
#include <imtauth/IUserSettings.h>
#include <imtserverapp/IJsonRepresentationController.h>
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings.h>


namespace imtservergql
{


class CSettingsControllerComp: public sdl::imtbase::Settings::CGraphQlHandlerCompBase
{
public:
	typedef sdl::imtbase::Settings::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSettingsControllerComp);
		I_ASSIGN(m_userSettingsRepresentationControllerCompPtr, "UserSettingsRepresentationController", "Controller for user related data model representation", true, "RepresentationController");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", true, "UserSettingsCollection");
		I_ASSIGN(m_serverInterfaceCompPtr, "ServerConnectionInterface", "Server connection interface", true, "ServerConnectionInterface");
		I_ASSIGN(m_userSettingsInfoFactCompPtr, "UserSettingsFactory", "Factory used for creation of the new user settings instance", true, "UserSettingsFactory");
		I_ASSIGN(m_styleFileNamePrefixAttrPtr, "StyleFileNamePrefix", "Style file name prefix", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::imtbase::Settings::CGraphQlHandlerCompBase)
	virtual sdl::imtbase::Settings::CSetSettingsPayload OnSetSettings(
				const sdl::imtbase::Settings::CSetSettingsGqlRequest& setSettingsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtBaseTypes::CParamsSet OnGetSettings(
				const sdl::imtbase::Settings::CGetSettingsGqlRequest& getSettingsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::Settings::CStyleData OnGetStyleData(
				const sdl::imtbase::Settings::CGetStyleDataGqlRequest& getStyleRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::imtbase::ImtBaseTypes::CUrlParam OnGetWebSocketUrl(
				const sdl::imtbase::Settings::CGetWebSocketUrlGqlRequest& getWebSocketUrlRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	
private:
	imtauth::IUserSettingsSharedPtr GetOrCreateUserSettings(const QByteArray& userId) const;
	bool SetUserSettings(const QByteArray& userId, const imtauth::IUserSettings& userSettings) const;
	
protected:
	I_REF(imtserverapp::IJsonRepresentationController, m_userSettingsRepresentationControllerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_REF(imtcom::IServerConnectionInterface, m_serverInterfaceCompPtr);
	I_ATTR(QString, m_styleFileNamePrefixAttrPtr);
	I_FACT(imtauth::IUserSettings, m_userSettingsInfoFactCompPtr);
};


} // namespace imtservergql


