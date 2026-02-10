// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ImtCore includes
#include <imtauth/IRoleManager.h>
#include <imtauthgql/CClientRequestManagerCompBase.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Roles.h>


namespace imtauthgql
{


class CClientRequestRoleManagerComp:
			public imtauthgql::CClientRequestManagerCompBase,
			virtual public imtauth::IRoleManager
{
public:
	typedef imtauthgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CClientRequestRoleManagerComp)
		I_REGISTER_INTERFACE(imtauth::IRoleManager);
		I_ASSIGN(m_roleFactoryCompPtr, "RoleFactory", "Role factory", true, "RoleFactory");
	I_END_COMPONENT;

	// reimplemented (imtauth::IRoleManager)
	virtual QByteArrayList GetRoleIds() const override;
	virtual imtauth::IRoleUniquePtr GetRole(const QByteArray& roleId) const override;
	virtual QByteArray CreateRole(
				const QByteArray& productId,
				const QString& roleName,
				const QString& roleDescription = QString(),
				const QByteArrayList& permissions = QByteArrayList()) override;
	virtual bool RemoveRole(const QByteArray& roleId) override;
	virtual QByteArrayList GetRolePermissions(const QByteArray& roleId) const override;
	virtual bool AddPermissionsToRole(const QByteArray& roleId, const QByteArrayList& permissions) override;
	virtual bool RemovePermissionsFromRole(const QByteArray& roleId, const QByteArrayList& permissions) override;

private:
	bool GetRoleDataSdl(const QByteArray& roleId, sdl::imtauth::Roles::CRoleData::V1_0& roleData) const;
	bool SetRoleDataSdl(const QByteArray& roleId, const sdl::imtauth::Roles::CRoleData::V1_0& roleData) const;

private:
	I_FACT(imtauth::IRole, m_roleFactoryCompPtr);
};


} // namespace imtauthgql


