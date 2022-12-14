#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>
#include <imtgql/IItemBasedRepresentationDataProvider.h>
#include <imtgql/IGqlModelEditor.h>
#include <imtbase/IObjectCollection.h>
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtguigql
{


class CSettingsControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSettingsControllerComp);
		I_REGISTER_INTERFACE(imtgql::IItemBasedRepresentationDataProvider)
		I_REGISTER_INTERFACE(imtgql::IGqlModelEditor)
		I_ASSIGN(m_settingsDataProviderCompPtr, "SettingsDataProvider", "Settings data provider", true, "");
		I_ASSIGN(m_mutationDataControllerCompPtr, "MutationDataProvider", "Provides data to the controller for saving", true, "");
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
	I_REF(imtgql::IItemBasedRepresentationDataProvider, m_settingsDataProviderCompPtr);
	I_REF(imtgql::IGqlModelEditor, m_mutationDataControllerCompPtr);
};


} // namespace imtguigql


