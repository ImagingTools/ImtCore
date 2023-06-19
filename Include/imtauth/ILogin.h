#pragma once


// ImtCore includes
#include <imtauth/ILoginStatusProvider>


namespace imtauth
{


/*
	Interface supporting login operation.
*/
class ILogin: virtual public imtauth::ILoginStatusProvider
{
public:
	/**
		Data model change notification flags.
	*/
	enum ChangeFlags
	{
		CF_LOGIN = 0x36e7b62,
		CF_LOGOUT
	};

	// TODO: Replace by GetLoginInfo (sa ILoginInfo after refactoring)
	virtual QByteArray GetLoggedUserId() const = 0;

	/**
		Log in user.
		\return	true, if user is accepted.
	*/
	virtual bool Login(const QByteArray& userId, const QString& password) = 0;

	/**
		Log in user.
		\return	true, if logout successful.
	*/
	virtual bool Logout() = 0;
};


} // namespace imtauth


