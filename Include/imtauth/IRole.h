// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtauth/IRoleInfoProvider.h>
#include <imtlic/IFeatureInfoProvider.h>


namespace imtauth
{


/**
	Interface describing a role.
*/
class IRole: virtual public iser::IObject
{
public:
	enum MetaInfoTypes
	{
		MIT_ROLE_ID = idoc::IDocumentMetaInfo::MIT_USER + 1,
		MIT_ROLE_NAME,
		MIT_ROLE_DESCRIPTION,
		MIT_PRODUCT_ID
	};
	
	typedef QByteArrayList FeatureIds;

	virtual const IRoleInfoProvider* GetParentRolesProvider() const = 0;

	/**
		Get list of all availiable permissions for this role.
	*/
	virtual const imtlic::IFeatureInfoProvider* GetPermissionProvider() const = 0;

	/**
		Get role ID.
	*/
	virtual QByteArray GetRoleId() const = 0;

	/**
		Set role ID.
	*/
	virtual void SetRoleId(const QByteArray& id) = 0;

	/**
		Get role name.
	*/
	virtual QString GetRoleName() const = 0;

	/**
		Set role name.
	*/
	virtual void SetRoleName(const QString& name) = 0;

	/**
		Get role description.
	*/
	virtual QString GetRoleDescription() const = 0;

	/**
		Set role description.
	*/
	virtual void SetRoleDescription(const QString& description) = 0;

	/**
		Get result permissions of role.
	*/
	virtual FeatureIds GetPermissions() const = 0;

	/**
		Get local permissions of role.
	*/
	virtual FeatureIds GetLocalPermissions() const = 0;

	/**
		Set permissions of role.
	*/
	virtual void SetLocalPermissions(const FeatureIds& permissions) = 0;

	/**
		Get role permissions restrictions.
	*/
	virtual FeatureIds GetProhibitions() const = 0;

	/**
		Set role permissions restrictions.
	*/
	virtual void SetProhibitions(const FeatureIds& prohibitions) = 0;

	/**
		Get product ID.
	*/
	virtual QByteArray GetProductId() const = 0;

	/**
		Set product of role.
	*/
	virtual void SetProductId(const QByteArray& productId) = 0;

	/**
		Get all parent roles.
	*/
	virtual QByteArrayList GetIncludedRoles() const  = 0;

	/**
		Insert parent role.
	*/
	virtual bool IncludeRole(const QByteArray& roleId) = 0;

	/**
		Remove parent role.
	*/
	virtual void ExcludeRole(const QByteArray& roleId) = 0;

	/**
		Checking that the role is default.
	*/
	virtual bool IsDefault() const = 0;

	/**
		Set default flag for role.
	*/
	virtual void SetDefault(bool isDefault) = 0;

	/**
		Checking that the role is guest.
	*/
	virtual bool IsGuest() const = 0;

	/**
		Set guest flag for role.
	*/
	virtual void SetGuest(bool isGuest) = 0;
};


typedef istd::TUniqueInterfacePtr<IRole> IRoleUniquePtr;
typedef istd::TSharedInterfacePtr<IRole> IRoleSharedPtr;


} // namespace imtauth


