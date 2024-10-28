#pragma once


// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtservergql
{


class CObjectProviderComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CObjectProviderComp);
		I_ASSIGN(m_objectCompPtr, "Object", "Object", true, "Object");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CCGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(iser::IObject, m_objectCompPtr);
};


} // namespace imtservergql


