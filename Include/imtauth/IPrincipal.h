// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtauth/IRole.h>
#include <imtauth/IRoleInfoProvider.h>


namespace imtauth
{


class IUserGroupInfoProvider;

/**
	Common interface for security principals (users and user groups).
	\ingroup UserBase
*/
class IPrincipal: virtual public iser::IObject
{
public:
	typedef QList<QByteArray> FeatureIds;
	typedef QList<QByteArray> RoleIds;

	enum MetaInfoTypes
	{
		/**
			ID given as QString.
		*/
		MIT_ID = idoc::IDocumentMetaInfo::MIT_USER + 1,

		/**
			Name given as QString.
		*/
		MIT_NAME,

		/**
			Description given as QString.
		*/
		MIT_DESCRIPTION,

		/**
			Permissions given as QString.
		*/
		MIT_PERMISSIONS,

		/**
			Prohibitions given as QString.
		*/
		MIT_PROHIBITIONS,

		/**
			Products given as QString.
		*/
		MIT_PRODUCTS,

		/**
			Roles given as QString.
		*/
		MIT_ROLES
	};

	/**
		Information about the system (e.g. internal or LDAP) an user or a group belongs to.
	*/
	struct SystemInfo
	{
		QByteArray systemId;
		QString systemName;
		bool enabled = true;

		bool operator == (const SystemInfo& other) const
		{
			return (systemId == other.systemId) && (systemName == other.systemName) && (enabled == other.enabled);
		}

		bool operator != (const SystemInfo& other) const
		{
			return !(*this == other);
		}
	};

	typedef QList<SystemInfo> SystemInfoList;


	/**
		Get list of all availiable permissions.
	*/
	virtual const imtlic::IFeatureInfoProvider* GetPermissionProvider() const = 0;

	/**
		Get list of all availiable roles.
	*/
	virtual const imtauth::IRoleInfoProvider* GetRoleProvider() const = 0;

	/**
		Get id of the user.
	*/
	virtual QByteArray GetId() const = 0;

	/**
		Set id of the user.
	*/
	virtual void SetId(const QByteArray& id) = 0;

	/**
		Get Windows SID of the principal.
	*/
	virtual QByteArray GetSid() const = 0;

	/**
		Set Windows SID of the principal.
	*/
	virtual void SetSid(const QByteArray& sid) = 0;

	/**
		Get name.
	*/
	virtual QString GetName() const = 0;

	/**
		Set name.
	*/
	virtual void SetName(const QString& name) = 0;

	/**
		Get description.
	*/
	virtual QString GetDescription() const = 0;

	/**
		Set description.
	*/
	virtual void SetDescription(const QString& description) = 0;

	/**
		Get permissions.
	*/
	virtual FeatureIds GetPermissions(const QByteArray& productId = QByteArray()) const = 0;

	/**
		Get local permissions.
	*/
	virtual FeatureIds GetLocalPermissions(const QByteArray& productId) const = 0;

	/**
		Set local permissions.
	*/
	virtual void SetLocalPermissions(const QByteArray& productId, const FeatureIds& permissions) = 0;

	/**
		Get restrictions.
	*/
	virtual FeatureIds GetProhibitions() const = 0;

	/**
		Set restrictions.
	*/
	virtual void SetProhibitions(const FeatureIds& prohibitions) = 0;

	/**
		Get all the products in which the user has roles.
	*/
	virtual QByteArrayList GetProducts() const = 0;

	/**
		Remove all roles contained in the product.
	*/
	virtual bool RemoveProduct(const QByteArray& productId) = 0;

	/**
		Get roles.
	*/
	virtual RoleIds GetRoles(const QByteArray& productId) const = 0;

	/**
		Set roles.
	*/
	virtual void SetRoles(const QByteArray& productId, const RoleIds& roles) = 0;

	/**
		Add role to user.
	*/
	virtual void AddRole(const QByteArray& productId, const QByteArray& roleId) = 0;

	/**
		Remove role from user.
	*/
	virtual bool RemoveRole(const QByteArray& productId, const QByteArray& roleId) = 0;

	/**
		Get list of all availiable user groups.
	*/
	virtual const imtauth::IUserGroupInfoProvider* GetUserGroupProvider() const = 0;
};


} // namespace imtauth


