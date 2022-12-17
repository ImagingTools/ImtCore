#pragma once


// ACF includes
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtdb/IDatabaseEngine.h>
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>


namespace imtauthgql
{


class CAuthorizationOptionsControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAuthorizationOptionsControllerComp);
		I_ASSIGN(m_userModeCompPtr, "ActiveUserMode", "Active user authorization mode", false, "ActiveUserMode");
		I_ASSIGN(m_userCollectionCompPtr, "UserCollection", "User collection", true, "UserCollection");
		I_ASSIGN(m_databaseEngineCompPtr, "DatabaseEngine", "Database engine for SQL queries", true, "DatabaseEngine");
		I_ASSIGN(m_superuserLoginAttrPtr, "SuperuserLogin", "Login of the system administration", true, "");
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	I_REF(iprm::ISelectionParam, m_userModeCompPtr);
	I_REF(imtbase::IObjectCollection, m_userCollectionCompPtr);
	I_REF(imtdb::IDatabaseEngine, m_databaseEngineCompPtr);
	I_ATTR(QByteArray, m_superuserLoginAttrPtr);
};


} // namespace imtauthgql


