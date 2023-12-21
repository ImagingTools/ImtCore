#pragma once


// ImtCore includes
#include <imtbase/ICommandPermissionsProvider.h>
#include <imtauth/IPermissionChecker.h>
#include <imtgql/CGqlRequestHandlerCompBase.h>


namespace imtgql
{


class CPermissibleGqlRequestHandlerComp: public imtgql::CGqlRequestHandlerCompBase
{
public:
	typedef imtgql::CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPermissibleGqlRequestHandlerComp);
		I_ASSIGN(m_commandPermissionsCompPtr, "CommandPermissions", "Command Permissions", false, "Command Permissions");
		I_ASSIGN(m_checkPermissionCompPtr, "PermissionChecker", "Checker of the permissions", false, "PermissionChecker");
	I_END_COMPONENT;

	// reimplemented (imtgql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

	// reimplemented (imtgql::CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtbase::ICommandPermissionsProvider, m_commandPermissionsCompPtr);
	I_REF(imtauth::IPermissionChecker, m_checkPermissionCompPtr);
};


} // namespace imtgql


