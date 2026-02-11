// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtservergql
{


class CGqlDemultiplexerComp: public ilog::CLoggerComponentBase, virtual public imtgql::IGqlRequestHandler
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CGqlDemultiplexerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlRequestHandler);
		I_ASSIGN_MULTI_0(m_gqlRequestHandlersCompPtr, "GqlRequestHandlers", "Demultiplexer for GraphQL request", false)
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_MULTIREF(imtgql::IGqlRequestHandler, m_gqlRequestHandlersCompPtr);
};


} // namespace imtservergql


