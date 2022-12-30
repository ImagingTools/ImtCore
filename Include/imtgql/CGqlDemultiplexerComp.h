#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationControllerCompBase.h>


namespace imtgql
{


class CGqlDemultiplexerComp: public imtgql::CGqlRepresentationControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CGqlDemultiplexerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlRequestHandler);
		I_ASSIGN_MULTI_0(m_gqlRequestHandlersCompPtr, "GqlRequestHandlers", "Demultiplexer for GraphQL request", true)
	I_END_COMPONENT;

	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;

protected:
	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_MULTIREF(imtgql::IGqlRequestHandler, m_gqlRequestHandlersCompPtr);
};


} // namespace imtgql


