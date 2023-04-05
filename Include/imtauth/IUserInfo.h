#pragma once

// ImtCore includes
#include <imtauth/IUserBaseInfo.h>


namespace imtauth
{


/**
	Interface for describing an user.
	\ingroup User
*/
class IUserInfo:
		virtual public IUserBaseInfo
{
public:

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
		Get username of the user.
	*/
	virtual QByteArray GetUserId() const = 0;

	/**
		Set username of the user.
	*/
	virtual void SetUserId(const QByteArray& userId) = 0;

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
		Check admin user.
	*/
	virtual bool IsAdmin() const = 0;
};


} // namespace imtauth


