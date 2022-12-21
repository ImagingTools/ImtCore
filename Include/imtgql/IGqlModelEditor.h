#pragma once


// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlObject.h>


namespace imtgql
{


class IGqlModelEditor: virtual public istd::IPolymorphic
{
public:
	virtual bool UpdateModelFromRepresentation(
				const QList<imtgql::CGqlObject>& params,
				imtbase::CTreeItemModel* modelPtr,
				const imtgql::IGqlContext* gqlContext = nullptr) = 0;
};


} // namespace imtgql


