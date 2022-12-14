#pragma once


// ACF includes
#include <iqt/ITranslationManager.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtgql/IItemBasedRepresentationDataProvider.h>
#include <imtgql/IGqlModelEditor.h>
#include <imtdb/IDatabaseLoginSettings.h>
#include <imtqml/CObjectRepresentationCompBase.h>
#include <imtauth/IUserSettings.h>


namespace imtqml
{


/**
	Database access settings representation provider.
*/
class CSettingsDatabasePageProviderComp: public imtqml::CObjectRepresentationCompBase
{
public:
	typedef imtqml::CObjectRepresentationCompBase BaseClass;

	I_BEGIN_COMPONENT(CSettingsDatabasePageProviderComp);
		I_ASSIGN(m_userSettingsCollectionCompPtr, "UserSettingsCollection", "User settings collection", true, "UserSettingsCollection");
		I_ASSIGN(m_settingsDataProviderCompPtr, "SettingsDataProvider", "Settings data provider", true, "");
		I_ASSIGN(m_userSettingsInfoFactCompPtr, "UserSettingsFactory", "Factory used for creation of the new user settings instance", true, "UserSettingsFactory");
	I_END_COMPONENT;

	// reimplemented (imtgql::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetRepresentation(
				const QList<imtgql::CGqlObject>& params,
				const QByteArrayList& fields,
				const imtgql::IGqlContext* gqlContext) override;

	// reimplemented (imtgql::IGqlModelEditor)
	virtual bool UpdateModelFromRepresentation(
				const QList<imtgql::CGqlObject>& params,
				imtbase::CTreeItemModel* baseModel,
				const imtgql::IGqlContext* gqlContext) override;

private:
	I_REF(imtbase::IObjectCollection, m_userSettingsCollectionCompPtr);
	I_REF(imtgql::IItemBasedRepresentationDataProvider, m_settingsDataProviderCompPtr);
	I_FACT(imtauth::IUserSettings, m_userSettingsInfoFactCompPtr);
};


} // namespace imtqml


