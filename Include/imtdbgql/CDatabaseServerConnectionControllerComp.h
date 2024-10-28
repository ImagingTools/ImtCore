#pragma once


// ImtCore includes
#include <imtdb/IDatabaseServerConnectionChecker.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtdbgql
{


class CDatabaseServerConnectionControllerComp: public imtservergql::CGqlRequestHandlerCompBase
{
public:
	typedef imtservergql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDatabaseServerConnectionControllerComp);
		I_ASSIGN(m_dbServerConnectionCheckerCompPtr, "DatabaseServerConnectionChecker", "Database server connection status provider", true, "DatabaseServerConnectionChecker");
	I_END_COMPONENT;

protected:
	// reimplemented (imtservergql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtdb::IDatabaseServerConnectionChecker, m_dbServerConnectionCheckerCompPtr);
};


} // namespace imtdbgql


