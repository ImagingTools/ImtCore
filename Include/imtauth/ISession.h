#pragma once


// ACF includes
#include <iser/IObject.h>

// ImtCore includes
#include <imtauth/IUserInfo.h>


namespace imtauth
{


/**
	Interface for describing an session.
	\ingroup Session
*/
class ISession:
		virtual public iser::IObject
{
public:
	/**
		Get token of the session.
	*/
	virtual QByteArray GetToken() const = 0;

	/**
		Set token of the session.
	*/
	virtual void SetToken(const QByteArray& token) = 0;

	/**
		Get user-ID of the session.
	*/
	virtual QByteArray GetUserId() const = 0;

	/**
		Set user info of the session.
	*/
	virtual void SetUserId(const QByteArray& userId) = 0;
};


} // namespace imtauth


