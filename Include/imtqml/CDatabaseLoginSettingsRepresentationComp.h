#pragma once


// ImtCore includes
#include <imtqml/CObjectRepresentationCompBase.h>
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtqml
{


/**
*/
class CDatabaseLoginSettingsRepresentationComp: public imtqml::CObjectRepresentationCompBase
{
public:
	typedef imtqml::CObjectRepresentationCompBase BaseClass;

	I_BEGIN_COMPONENT(CDatabaseLoginSettingsRepresentationComp);
		I_ASSIGN(m_databaseSettingsCompPtr, "DatabaseLoginSettings", "Database login settings", false, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IItemBasedRepresentationProvider)
	virtual imtbase::CTreeItemModel* GetRepresentation(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

	// reimplemented (imtgql::IGqlModelEditor)
	virtual bool UpdateModelFromRepresentation(
			const QList<imtgql::CGqlObject>& params,
			imtbase::CTreeItemModel* baseModel,
			const imtgql::IGqlContext* gqlContext) override;

private:
	I_REF(imtdb::IDatabaseLoginSettings, m_databaseSettingsCompPtr);
};


} // namespace imtqml


