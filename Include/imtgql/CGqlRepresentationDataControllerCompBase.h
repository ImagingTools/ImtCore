#pragma once


// ImtCore includes
#include <imtgql/IGqlRepresentationController.h>
#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtgql
{


class CGqlRepresentationControllerCompBase: virtual public IGqlRepresentationController, public CGqlRequestHandlerCompBase
{
public:
	typedef CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlRepresentationControllerCompBase);
		I_REGISTER_INTERFACE(IGqlRepresentationController);
	I_END_COMPONENT;

protected:
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
	virtual bool UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const override;

	// reimplemented (imtgql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtgql


