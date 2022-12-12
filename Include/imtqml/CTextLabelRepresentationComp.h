#pragma once


// ImtCore includes
#include <imtqml/CObjectRepresentationCompBase.h>


namespace imtqml
{


/**
	Param set to representation model adapter for text label component.
*/
class CTextLabelRepresentationComp: public imtqml::CObjectRepresentationCompBase
{
public:
	typedef imtqml::CObjectRepresentationCompBase BaseClass;

	I_BEGIN_COMPONENT(CTextLabelRepresentationComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::IItemBasedRepresentationProvider)
	virtual imtbase::CTreeItemModel* GetRepresentation(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext) override;
};


} // namespace imtqml


