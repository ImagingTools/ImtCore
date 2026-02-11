// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtserverapp/IRepresentationController.h>
#include <imtauth/IUserSettings.h>
#include <imtservergql/CGqlRepresentationDataControllerCompBase.h>


namespace imtauthgql
{


class CUserSettingsControllerComp: public imtservergql::CGqlRepresentationControllerCompBase
{
public:
	typedef imtservergql::CGqlRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserSettingsControllerComp);
		I_ASSIGN(m_userSettingsRepresentationControllerCompPtr, "UserSettingsRepresentationController", "Controller for user related data model representation", true, "RepresentationController");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", true, "UserSettingsCollection");
		I_ASSIGN(m_userSettingsInfoFactCompPtr, "UserSettingsFactory", "Factory used for creation of the new user settings instance", true, "UserSettingsFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

protected:
	I_REF(imtserverapp::IRepresentationController, m_userSettingsRepresentationControllerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_FACT(imtauth::IUserSettings, m_userSettingsInfoFactCompPtr);
};


} // namespace imtauthgql


