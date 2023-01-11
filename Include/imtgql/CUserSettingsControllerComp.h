#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtbase/IRepresentationController.h>
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
#include <imtauth/IUserSettings.h>


namespace imtgql
{


class CUserSettingsControllerComp: public CGqlRepresentationControllerCompBase
{
public:
	typedef CGqlRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserSettingsControllerComp);
		I_ASSIGN(m_userSettingsRepresentationControllerCompPtr, "UserSettingsRepresentationController", "Controller for user related data model representation", true, "RepresentationController");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", true, "UserSettingsCollection");
		I_ASSIGN(m_userSettingsInfoFactCompPtr, "UserSettingsFactory", "Factory used for creation of the new user settings instance", true, "UserSettingsFactory");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

protected:
	I_REF(imtbase::IRepresentationController, m_userSettingsRepresentationControllerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_FACT(imtauth::IUserSettings, m_userSettingsInfoFactCompPtr);
};


} // namespace imtgql


