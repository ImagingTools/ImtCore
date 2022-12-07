#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtgql/IGqlMutationDataControllerDelegate.h>
#include <imtbase/IObjectCollection.h>
#include <imtdb/IDatabaseLoginSettings.h>
#include <imtqml/CParamsDataProviderCompBase.h>


namespace imtqml
{


/**
	Server pages settings provider.
*/
class CSettingsDatabasePageProviderComp:
		public imtqml::CParamsDataProviderCompBase
{
public:
	typedef imtqml::CParamsDataProviderCompBase BaseClass;

	I_BEGIN_COMPONENT(CSettingsDatabasePageProviderComp);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_REGISTER_INTERFACE(imtgql::IGqlMutationDataControllerDelegate);
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", true, "UserSettingsCollection");
		I_ASSIGN(m_settingsDataProviderCompPtr, "SettingsDataProvider", "Settings data provider", true, "");
	I_END_COMPONENT;

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(
				const QList<imtgql::CGqlObject>& params,
				const QByteArrayList& fields,
				const imtgql::IGqlContext* gqlContext) override;

	// reimplemented (imtgql::IGqlMutationDataControllerDelegate)
	virtual bool UpdateModelFromRepresentation(
				const QList<imtgql::CGqlObject>& params,
				imtbase::CTreeItemModel* baseModel,
				const imtgql::IGqlContext* gqlContext) override;

private:
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_settingsDataProviderCompPtr);
};


} // namespace imtqml
