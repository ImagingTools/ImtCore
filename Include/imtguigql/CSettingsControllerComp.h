#pragma once


// ACF includes
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtbase/IObjectCollection.h>
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtguigql
{


class CSettingsControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSettingsControllerComp);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider)
		I_REGISTER_INTERFACE(imtbase::IObjectCollection)
		I_REGISTER_INTERFACE(iprm::IParamsSet)
		I_REGISTER_INTERFACE(imtdb::IDatabaseLoginSettings)
		I_ASSIGN(m_settingsParamsSetCompPtr, "SettingsParamsSet", "Settings parameters", true, "SettingsParamsSet");
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", true, "UserSettingsCollection");
		I_ASSIGN(m_settingsDataProviderCompPtr, "SettingsDataProvider", "Settings data provider", true, "");
		I_ASSIGN(m_databaseSettingsCompPtr, "DatabaseSettings", "Parameter of database settings", false, "");
	I_END_COMPONENT;

	enum OperationType
	{
		OT_UNKNOWN,
		OT_GET,
		OT_SAVE
	};

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
protected:
	virtual bool GetOperationFromRequest(const imtgql::CGqlRequest& gqlRequest, imtgql::CGqlObject& gqlObject, QString& errorMessage, int& operationType) const;
	virtual imtbase::CTreeItemModel* GetSettings(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			const imtgql::IGqlContext* gqlContext,
			QString& errorMessage) const;
	virtual imtbase::CTreeItemModel* SaveSettings(
			const QList<imtgql::CGqlObject>& inputParams,
			const imtgql::CGqlObject& gqlObject,
			const imtgql::IGqlContext* gqlContext,
			QString& errorMessage) const;

private:
	I_REF(iprm::IParamsSet, m_settingsParamsSetCompPtr);
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_settingsDataProviderCompPtr);
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseSettingsCompPtr);
};


} // namespace imtguigql


