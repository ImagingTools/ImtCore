#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtauth/IRole.h>
#include <imtauth/IRoleInfoProvider.h>
#include <imtauth/IUserGroupInfoProvider.h>


namespace imtauth
{


/**
	Interface for describing.
	\ingroup UserBase
*/
class IUserBaseInfo: virtual public iser::IObject
{
public:
	typedef QList<QByteArray> FeatureIds;
	typedef QList<QByteArray> RoleIds;

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
	virtual FeatureIds GetPermissions() const = 0;

	/**
		Get local permissions.
	*/
	virtual FeatureIds GetLocalPermissions() const = 0;

	/**
		Set local permissions.
	*/
	virtual void SetLocalPermissions(const FeatureIds& permissions) = 0;

	/**
		Get restrictions.
	*/
	virtual FeatureIds GetProhibitions() const = 0;

	/**
		Set restrictions.
	*/
	virtual void SetProhibitions(const FeatureIds& prohibitions) = 0;

	/**
		Get roles.
	*/
	virtual RoleIds GetRoles() const = 0;

	/**
		Set roles.
	*/
	virtual void SetRoles(const RoleIds& roles) = 0;

	/**
		Add role to user.
	*/
	virtual void AddRole(const QByteArray& roleId) = 0;

	/**
		Remove role from user.
	*/
	virtual bool RemoveRole(const QByteArray& userId) = 0;

	/**
		Get list of all availiable user groups.
	*/
	virtual const imtauth::IUserGroupInfoProvider* GetUserGroupProvider() const = 0;
};


} // namespace imtauth


