#pragma once


// ImtCore includes
#include <imtgql/IGqlRepresentationDataController.h>


namespace imtgql
{


class IGqlModelEditor: virtual public imtgql::CGqlRepresentationDataControllerComp
{
public:
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* modelPtr) = 0;
};


} // namespace imtgql


