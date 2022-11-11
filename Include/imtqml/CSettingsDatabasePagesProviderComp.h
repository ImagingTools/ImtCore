#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtgql/IGqlMutationDataControllerDelegate.h>
#include <imtbase/IObjectCollection.h>
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtqml
{


/**
	Server pages settings provider.
*/
class CSettingsDatabasePagesProviderComp:
			public icomp::CComponentBase,
			public imtbase::IItemBasedRepresentationDataProvider,
			public imtgql::IGqlMutationDataControllerDelegate
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSettingsDatabasePagesProviderComp);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_REGISTER_INTERFACE(imtgql::IGqlMutationDataControllerDelegate);
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", true, "UserSettingsCollection");
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
		I_ASSIGN(m_settingsDataProviderCompPtr, "SettingsDataProvider", "Settings data provider", true, "");
	I_END_COMPONENT;

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(
				const QList<imtgql::CGqlObject>& params,
				const QByteArrayList& fields,
				const imtgql::IGqlContext* gqlContext) override;

	// reimplemented (imtgql::IGqlMutationDataControllerDelegate)
	virtual imtbase::CTreeItemModel* UpdateBaseModelFromRepresentation(
				const QList<imtgql::CGqlObject>& params,
				imtbase::CTreeItemModel* baseModel,
				const imtgql::IGqlContext* gqlContext) override;

private:
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
	I_REF(imtbase::IItemBasedRepresentationDataProvider, m_settingsDataProviderCompPtr);
};


} // namespace imtqml
