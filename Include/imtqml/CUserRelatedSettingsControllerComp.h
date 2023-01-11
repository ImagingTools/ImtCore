#pragma once


// ImtCore includes
#include <imtbase/CObjectRepresentationControllerCompBase.h>
#include <imtbase/IObjectCollection.h>
#include <imtgql/CGqlRepresentationDataControllerComp.h>
#include <imtauth/IUserSettings.h>


namespace imtqml
{


class CUserRelatedSettingsControllerComp: public imtbase::CObjectRepresentationControllerCompBase
{
public:
	typedef imtbase::CObjectRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CUserRelatedSettingsControllerComp);
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", true, "UserSettingsCollection");
		I_ASSIGN(m_userSettingsInfoFactCompPtr, "UserSettingsFactory", "Factory used for creation of the new user settings instance", true, "UserSettingsFactory");
		I_ASSIGN(m_userSettingsRepresentationControllerCompPtr, "UserSettingsRepresentationController", "Representation controller for user related settings", true, "UserSettingsRepresentationController");
	I_END_COMPONENT;

protected:
	// reimplemented (IRepresentationController)
	virtual bool GetRepresentationFromDataModel(const istd::IChangeable& dataModel, imtbase::CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr = nullptr) const override;
	virtual bool GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const override;

private:
	I_REF(imtbase::IRepresentationController, m_userSettingsRepresentationControllerCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_FACT(imtauth::IUserSettings, m_userSettingsInfoFactCompPtr);
};


} // namespace imtqml


