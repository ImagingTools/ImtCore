#pragma once


// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlRequest.h>


namespace imtgql
{


class IGqlRepresentationController: virtual public istd::IPolymorphic
{
public:
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(
				const CGqlRequest& gqlRequest,
				QString& errorMessage) const = 0;
	virtual bool UpdateModelFromRepresentation(
				const CGqlRequest& request,
				imtbase::CTreeItemModel* representationPtr) const = 0;
};


} // namespace imtgql


