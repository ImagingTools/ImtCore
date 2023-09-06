#pragma once


// ImtCore includes
#include <imtbase/IRepresentationController.h>
#include <imtgql/IGqlRepresentationController.h>
#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtgql
{


class CGqlControllerComp: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtgql


