#pragma once


// ImtCore includes
#include <imtqml/CObjectRepresentationCompBase.h>


namespace imtqml
{


class CCompositeObjectRepresentationComp: public CObjectRepresentationCompBase
{
public:
	typedef CObjectRepresentationCompBase BaseClass;

	I_BEGIN_COMPONENT(CCompositeObjectRepresentationComp);
		I_ASSIGN_MULTI_0(m_paramSubElementsCompPtr, "ParamSubElements", "Subelements of the param", false);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IItemBasedRepresentationProvider)
	virtual imtbase::CTreeItemModel* GetRepresentation(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;

	// reimplemented (imtgql::IGqlModelEditor)
	virtual bool UpdateModelFromRepresentation(const QList<imtgql::CGqlObject>& params, imtbase::CTreeItemModel* baseModel, const imtgql::IGqlContext* gqlContext) override;
protected:
	I_MULTIREF(imtgql::IItemBasedRepresentationDataProvider, m_paramSubElementsCompPtr);
};


} // namespace imtqml


