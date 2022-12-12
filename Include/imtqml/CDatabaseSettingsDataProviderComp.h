#pragma once

// ImtCore includes
#include <imtdb/IDatabaseLoginSettings.h>
#include <imtqml/CParamsDataProviderCompBase.h>

// hairable component omly 41-st
namespace imtqml
{


/**
	Basic implementation for the controller of the application page representation data model.
*/
class CDatabaseSettingsDataProviderComp:
		public imtqml::CParamsDataProviderCompBase
{
public:
	typedef imtqml::CParamsDataProviderCompBase BaseClass;

	I_BEGIN_COMPONENT(CDatabaseSettingsDataProviderComp);
		I_ASSIGN(m_databaseSettingsCompPtr, "DatabaseParameter", "Parameter of database settings", false, "");
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
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseSettingsCompPtr);
};


} // namespace imtqml
