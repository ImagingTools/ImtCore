#pragma once


// ACF includes
#include <istd/IChangeable.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlObject.h>


namespace imtgql
{


/**
	Base interface for representation data controllers
*/
class IGqlMutationDataControllerDelegate: virtual public istd::IPolymorphic
{
public:
	virtual imtbase::CTreeItemModel* UpdateBaseModelFromRepresentation(
				const QList<imtgql::CGqlObject>& params,
				imtbase::CTreeItemModel* baseModel) = 0;
};


} // namespace imtgql


