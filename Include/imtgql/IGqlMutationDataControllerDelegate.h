#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlObject.h>
#include <imtgql/IGqlContext.h>


namespace imtgql
{


/**
	Base interface for representation data controllers
*/
class IGqlMutationDataControllerDelegate: virtual public istd::IPolymorphic
{
public:
	virtual bool UpdateModelFromRepresentation(
				const QList<imtgql::CGqlObject>& params,
				imtbase::CTreeItemModel* baseModel,
				const imtgql::IGqlContext* gqlContext = nullptr) = 0;
};


} // namespace imtgql


