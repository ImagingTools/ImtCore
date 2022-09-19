#pragma once


// ACF includes
#include <iser/IObject.h>
#include <idoc/IDocumentMetaInfo.h>

// ImtCore includes
#include <imtauth/IRoleInfoProvider.h>
#include <imtlic/IFeatureInfoProvider.h>
#include <imtlic/IProductLicensingInfo.h>


namespace imtauth
{

/**
	Interface describing a role.
*/
class IRole: virtual public iser::IObject
{
public:
	typedef QSet<QByteArray> FeatureIds;

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
	virtual void SetLocalPermissions(const FeatureIds &permissions) = 0;

	/**
		Get role permissions restrictions.
	*/
	virtual FeatureIds GetProhibitions() const = 0;

	/**
		Set role permissions restrictions.
	*/
	virtual void SetProhibitions(const FeatureIds &prohibitions) = 0;

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
};


} // namespace imtauth


