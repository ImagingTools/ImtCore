#pragma once


// ImtCore includes
#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtgql
{


class CObjectProviderComp: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectProviderComp);
		I_ASSIGN(m_objectCompPtr, "Object", "Object", true, "Object");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CCGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(iser::IObject, m_objectCompPtr);
};


} // namespace imtgql


