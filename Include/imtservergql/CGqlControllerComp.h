#pragma once


// ImtCore includes
#include <imtrest/IRepresentationController.h>
#include <imtgql/IGqlRepresentationController.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtservergql
{


class CGqlControllerComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlControllerComp);
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;
};


} // namespace imtservergql


