#pragma once


// ImtCore includes
#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtgql
{


class CGqlRepresentationControllerCompBase: public CGqlRequestHandlerCompBase
{
public:
	typedef CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_BASE_COMPONENT(CGqlRepresentationControllerCompBase);
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const = 0;

	// reimplemented (imtgql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtgql


