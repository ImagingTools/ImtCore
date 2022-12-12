#pragma once


// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/IGqlContext.h>
#include <imtgql/CGqlObject.h>


namespace imtgql
{


/**
	Base interface for representation data controllers
*/
class IGqlModelEditor: virtual public istd::IPolymorphic
{
public:
	virtual bool UpdateModelFromRepresentation(
				const QList<imtgql::CGqlObject>& params,
				imtbase::CTreeItemModel* baseModel,
				const imtgql::IGqlContext* gqlContext = nullptr) = 0;
};


} // namespace imtgql


