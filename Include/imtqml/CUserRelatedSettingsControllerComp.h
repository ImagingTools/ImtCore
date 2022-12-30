#pragma once


// ImtCore includes
#include <imtbase/IRepresentationController.h>
#include <imtbase/IObjectCollection.h>
#include <imtqml/CObjectRepresentationCompBase.h>
#include <imtauth/IUserSettings.h>


namespace imtqml
{


/**
	Database access settings representation provider.
*/
class CUserRelatedSettingsControllerComp: public imtqml::CObjectRepresentationCompBase
{
public:
	typedef imtqml::CObjectRepresentationCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserRelatedSettingsControllerComp);
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", true, "UserSettingsCollection");
		I_ASSIGN(m_userSettingsInfoFactCompPtr, "UserSettingsFactory", "Factory used for creation of the new user settings instance", true, "UserSettingsFactory");
		I_ASSIGN(m_userSettingsRepresentationControllerCompPtr, "UserSettingsRepresentationController", "Representation controller for user related settings", true, "UserSettingsRepresentationController");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

private:
	I_REF(imtbase::IRepresentationController, m_userSettingsRepresentationControllerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_FACT(imtauth::IUserSettings, m_userSettingsInfoFactCompPtr);
};


} // namespace imtqml


