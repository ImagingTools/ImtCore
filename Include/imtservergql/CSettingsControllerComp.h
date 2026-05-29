// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtcom/IServerConnectionInterface.h>
#include <imtauth/IUserSettings.h>
#include <imtserverapp/IJsonRepresentationController.h>
#include <imtbase/IObjectCollection.h>
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/Settings>


namespace imtservergql
{


class CSettingsControllerComp: public sdl::V1_0::imtbase::CGraphQlHandlerCompBase
{
public:
	typedef sdl::V1_0::imtbase::CGraphQlHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSettingsControllerComp);
		I_ASSIGN(m_userSettingsRepresentationControllerCompPtr, "UserSettingsRepresentationController", "Controller for user related data model representation", true, "RepresentationController");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", true, "UserSettingsCollection");
		I_ASSIGN(m_serverInterfaceCompPtr, "ServerConnectionInterface", "Server connection interface", true, "ServerConnectionInterface");
		I_ASSIGN(m_userSettingsInfoFactCompPtr, "UserSettingsFactory", "Factory used for creation of the new user settings instance", true, "UserSettingsFactory");
		I_ASSIGN(m_styleFileNamePrefixAttrPtr, "StyleFileNamePrefix", "Style file name prefix", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (sdl::V1_0::imtbase::CGraphQlHandlerCompBase)
	virtual sdl::V1_0::imtbase::CSetSettingsPayload OnSetSettings(
				const sdl::V1_0::imtbase::CSetSettingsGqlRequest& setSettingsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CParamsSet OnGetSettings(
				const sdl::V1_0::imtbase::CGetSettingsGqlRequest& getSettingsRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CStyleData OnGetStyleData(
				const sdl::V1_0::imtbase::CGetStyleDataGqlRequest& getStyleRequest,
				const ::imtgql::CGqlRequest& gqlRequest,
				QString& errorMessage) const override;
	virtual sdl::V1_0::imtbase::CUrlParam OnGetWebSocketUrl(
				const sdl::V1_0::imtbase::CGetWebSocketUrlGqlRequest& getWebSocketUrlRequest,
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


