#pragma once

// ACF includes
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtqml/CObjectRepresentationCompBase.h>


namespace imtqml
{


/**
	Implementation for text input representation.
*/
class CTextRepresentationComp: public imtqml::CObjectRepresentationCompBase
{
public:
	typedef imtqml::CObjectRepresentationCompBase BaseClass;

	I_BEGIN_COMPONENT(CTextRepresentationComp);
		I_ASSIGN(m_nameParamCompPtr, "NameParameter", "Name parameter", false, "");
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
	I_REF(iprm::ITextParam, m_nameParamCompPtr);
};


} // namespace imtqml


