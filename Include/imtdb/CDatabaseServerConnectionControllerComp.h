#pragma once


// ImtCore includes
#include <imtdb/IDatabaseServerConnectionChecker.h>
#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtdb
{


class CDatabaseServerConnectionControllerComp: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CDatabaseServerConnectionControllerComp);
		I_ASSIGN(m_dbServerConnectionCheckerCompPtr, "DatabaseServerConnectionChecker", "Database server connection status provider", true, "DatabaseServerConnectionChecker");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationControllerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtdb::IDatabaseServerConnectionChecker, m_dbServerConnectionCheckerCompPtr);
};


} // namespace imtdb


