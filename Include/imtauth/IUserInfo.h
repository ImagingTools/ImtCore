#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtauth/IRole.h>
#include <imtauth/IRoleInfoProvider.h>


namespace imtauth
{


/**
	Interface for describing an user.
	\ingroup User
*/
class IUserInfo:
		virtual public iser::IObject
{
public:
	typedef QSet<QByteArray> FeatureIds;
	typedef QSet<QByteArray> RoleIds;

	/**
		Supported action types.
	*/
	enum ActionType
	{
		/**
			A login action.
		*/
		AT_LOGIN,

		/**
			A logout action.
		*/
		AT_LOGOUT,

		/**
			A password change action.
		*/
		AT_PASSWORD_CHANGE,

		/**
			A name change action.
		*/
		AT_NAME_CHANGE
	};

	enum MetaInfoTypes
	{
		/**
			Username given as QString.
		*/
		MIT_USERNAME,

		/**
			Name given as QString.
		*/
		MIT_NAME,

		/**
			User description given as QString.
		*/
		MIT_DESCRIPTION,

		/**
			Contact email given as QString.
		*/
		MIT_EMAIL
	};

	/**
		Get list of all availiable permissions for this user.
	*/
	virtual const imtlic::IFeatureInfoProvider* GetPermissionProvider() const = 0;

	/**
		Get list of all availiable roles for this user.
	*/
	virtual const imtauth::IRoleInfoProvider* GetRoleProvider() const = 0;

	/**
		Get username of the user.
	*/
	virtual QByteArray GetUsername() const = 0;

	/**
		Set username of the user.
	*/
	virtual void SetUsername(const QByteArray& username) = 0;

	/**
		Get name of the user.
	*/
	virtual QString GetName() const = 0;

	/**
		Set name of the user.
	*/
	virtual void SetName(const QString& name) = 0;

	/**
		Get password hash of the user.
	*/
	virtual QByteArray GetPasswordHash() const = 0;

	/**
		Set password hash of the user.
	*/
	virtual void SetPasswordHash(const QByteArray& passwordHash) = 0;

	/**
		Get e-mail of the user.
	*/
	virtual QString GetMail() const = 0;

	/**
		Set e-mail of the user.
	*/
	virtual void SetMail(const QString& mail) = 0;

	/**
		Get permissions of user.
	*/
	virtual FeatureIds GetPermissions() const = 0;

	/**
		Get local permissions of user.
	*/
	virtual FeatureIds GetLocalPermissions() const = 0;

	/**
		Set local permissions of user.
	*/
	virtual void SetLocalPermissions(const FeatureIds &permissions) = 0;

	/**
		Get user restrictions.
	*/
	virtual FeatureIds GetProhibitions() const = 0;

	/**
		Set user restrictions.
	*/
	virtual void SetProhibitions(const FeatureIds &prohibitions) = 0;

	/**
		Get user roles.
	*/
	virtual RoleIds GetRoles() const = 0;

	/**
		Set user roles.
	*/
	virtual void SetRoles(const RoleIds &roles) = 0;
};


} // namespace imtauth


