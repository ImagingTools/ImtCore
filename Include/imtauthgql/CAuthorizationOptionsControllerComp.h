#pragma once


// ImtCore includes
#include <imtbase/IObjectCollection.h>
#include <imtdb/IDatabaseEngine.h>
#include <imtgql/CGqlRepresentationDataControllerComp.h>


namespace imtauthgql
{


class CAuthorizationOptionsControllerComp: public imtgql::CGqlRepresentationDataControllerComp
{
public:
	typedef imtgql::CGqlRepresentationDataControllerComp BaseClass;

	I_BEGIN_COMPONENT(CAuthorizationOptionsControllerComp);
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for SQL queries", true, "DatabaseEngine");
		I_ASSIGN(m_superuserLoginAttrPtr, "SuperuserLogin", "Login of the system administration", true, "");
	I_END_COMPONENT;

protected:
	// reimplemented (imtgql::CGqlRepresentationDataControllerComp)
	virtual imtbase::CTreeItemModel* CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_ATTR(QByteArray, m_superuserLoginAttrPtr);
};


} // namespace imtauthgql


