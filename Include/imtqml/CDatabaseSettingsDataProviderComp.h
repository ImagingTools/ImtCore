#pragma once


// ACF includes
#include <imod/TModelWrap.h>
#include <iqt/ITranslationManager.h>
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtbase/IItemBasedRepresentationDataProvider.h>
#include <imtbase/IItemBasedRepresentationDataController.h>
#include <imtdb/IDatabaseLoginSettings.h>
#include <imtgql/IGqlMutationDataControllerDelegate.h>
#include <imtqml/CUserOptionsProviderCompBase.h>


namespace imtqml
{


/**
	Basic implementation for the controller of the application page representation data model.
*/
class CDatabaseSettingsDataProviderComp:
			public icomp::CComponentBase,
			public imtqml::CUserOptionsProviderCompBase,
			public imtbase::IItemBasedRepresentationDataProvider,
			public imtgql::IGqlMutationDataControllerDelegate
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDatabaseSettingsDataProviderComp);
		I_REGISTER_INTERFACE(imtbase::IItemBasedRepresentationDataProvider);
		I_REGISTER_INTERFACE(imtgql::IGqlMutationDataControllerDelegate);
		I_ASSIGN(m_paramIdAttrPtr, "ParamId", "ID of the database", true, "");
		I_ASSIGN(m_paramNameAttrPtr, "ParamName", "Name of the database", false, "");
		I_ASSIGN(m_paramComponentTypeAttrPtr, "ComponentType", "Type of component\n0 - UNKNOWN\n1 - TEXT\n2 - INTEGER\n3 - COMBOBOX", false, 0);
		I_ASSIGN(m_databaseSettingsCompPtr, "Parameter", "Parameter of database settings", false, "");
		I_ASSIGN_MULTI_0(m_mutationDataDelegateCompPtr, "m_mutationDataDelegateCompPtr", "Mutation data delegate", false);
		I_ASSIGN(m_translationManagerCompPtr, "TranslationManager", "Translation manager", false, "TranslationManager");
	I_END_COMPONENT;

	// reimplemented (imtbase::IItemBasedRepresentationProvider)
	virtual QByteArray GetModelId() const override;
	virtual imtbase::CTreeItemModel* GetTreeItemModel(
				const QList<imtgql::CGqlObject>& params,
				const QByteArrayList& fields) override;

	// reimplemented (imtgql::IGqlMutationDataControllerDelegate)
	virtual imtbase::CTreeItemModel* UpdateBaseModelFromRepresentation(
				const QList<imtgql::CGqlObject>& params,
				imtbase::CTreeItemModel* baseModel) override;
private:
	I_ATTR(QByteArray, m_paramIdAttrPtr);
	I_TEXTATTR(m_paramNameAttrPtr);
	I_ATTR(int, m_paramComponentTypeAttrPtr);
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseSettingsCompPtr);
	I_MULTIREF(imtgql::IGqlMutationDataControllerDelegate, m_mutationDataDelegateCompPtr);
	I_REF(iqt::ITranslationManager, m_translationManagerCompPtr);
};


} // namespace imtqml


