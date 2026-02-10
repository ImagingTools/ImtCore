// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtserverapp/ICommandPermissionsProvider.h>
#include <imtauth/IPermissionChecker.h>
#include <imtservergql/CGqlRequestHandlerCompBase.h>


namespace imtservergql
{


class CPermissibleGqlRequestHandlerComp: public CGqlRequestHandlerCompBase
{
public:
	typedef CGqlRequestHandlerCompBase BaseClass;

	I_BEGIN_COMPONENT(CPermissibleGqlRequestHandlerComp);
		I_ASSIGN(m_commandPermissionsCompPtr, "CommandPermissions", "Command Permissions", false, "Command Permissions");
		I_ASSIGN(m_checkPermissionCompPtr, "PermissionChecker", "Checker of the permissions", false, "PermissionChecker");
	I_END_COMPONENT;

	// reimplemented (CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	virtual bool CheckPermissions(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const;

	// reimplemented (CGqlRequestHandlerCompBase)
	virtual imtbase::CTreeItemModel* CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	I_REF(imtserverapp::ICommandPermissionsProvider, m_commandPermissionsCompPtr);
	I_REF(imtauth::IPermissionChecker, m_checkPermissionCompPtr);
};


} // namespace imtservergql


