#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iauth/ILogin.h>

// ImtCore includes
#include <imtgql/TClientRequestManagerCompWrap.h>
#include <imtauth/IAccessTokenProvider.h>


namespace imtqml
{


class CSimpleLoginWrapComp:
			public imtgql::CClientRequestManagerCompBase,
			public iauth::ILogin,
			public imtauth::IAccessTokenProvider
{
public:
	typedef imtgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSimpleLoginWrapComp);
		I_REGISTER_INTERFACE(ILogin);
		I_REGISTER_INTERFACE(IAccessTokenProvider);
	I_END_COMPONENT;

	CSimpleLoginWrapComp();

	// reimplemented (iauth::ILogin)
	virtual iauth::CUser* GetLoggedUser() const override;
	virtual bool Login(const QString& userName, const QString& password) override;
	virtual bool Logout() override;

	// reimplemented (imtauth::IAccessTokenProvider)
	virtual QByteArray GetToken(const QByteArray& userId) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	QByteArray m_loggedUserId;
	QByteArray m_loggedUserToken;
};


} // namespace imtqml


