#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <imtgql/IGqlRequestHandler.h>
//#include <iqtgui/CHierarchicalCommand.h>


namespace imtgql
{


class CDelegatedRequestHandlerComp:
			public icomp::CComponentBase,
			virtual public imtgql::IGqlRequestHandler
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CDelegatedRequestHandlerComp);
		I_REGISTER_INTERFACE(imtgql::IGqlRequestHandler);
		I_ASSIGN(m_slaveRequestHandlerCompPtr, "SlaveRequestHandler", "Slave request handler", false, "SlaveRequestHandler");
	I_END_COMPONENT

	// reimplemented (imtgql::IGqlRequestHandler)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtgql::IGqlRequestHandler, m_slaveRequestHandlerCompPtr);
};


} // namespace imtgql


