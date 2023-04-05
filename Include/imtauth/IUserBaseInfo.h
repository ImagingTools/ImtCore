#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtauth/IRole.h>
#include <imtauth/IRoleInfoProvider.h>


namespace imtauth
{


/**
	Interface for describing.
	\ingroup UserBase
*/
class IUserBaseInfo:
		virtual public iser::IObject
{
public:
	typedef QSet<QByteArray> FeatureIds;
	typedef QSet<QByteArray> RoleIds;


	/**
		Get list of all availiable permissions.
	*/
	virtual const imtlic::IFeatureInfoProvider* GetPermissionProvider() const = 0;

	/**
		Get list of all availiable roles.
	*/
	virtual const imtauth::IRoleInfoProvider* GetRoleProvider() const = 0;

	/**
		Get name.
	*/
	virtual QString GetName() const = 0;

	/**
		Set name.
	*/
	virtual void SetName(const QString& name) = 0;

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
	virtual void SetLocalPermissions(const FeatureIds &permissions) = 0;

	/**
		Get restrictions.
	*/
	virtual FeatureIds GetProhibitions() const = 0;

	/**
		Set restrictions.
	*/
	virtual void SetProhibitions(const FeatureIds &prohibitions) = 0;

	/**
		Get roles.
	*/
	virtual RoleIds GetRoles() const = 0;

	/**
		Set roles.
	*/
	virtual void SetRoles(const RoleIds &roles) = 0;
};


} // namespace imtauth


