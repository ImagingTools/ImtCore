#pragma once


// ACF includes
#include <icomp/CComponentBase.h>
#include <iauth/ILogin.h>
#include <iauth/IRightsProvider.h>

// ImtCore includes
#include <imtgql/TClientRequestManagerCompWrap.h>
#include <imtbase/CTreeItemModel.h>
#include <imtauth/IAccessTokenProvider.h>
#include <imtauth/IPermissionChecker.h>
#include <imtauth/ISuperuserProvider.h>
#include <imtauth/CUserInfo.h>


namespace imtauthgql
{


class CSimpleLoginWrapComp:
			public imtgql::CClientRequestManagerCompBase,
			public iauth::ILogin,
			public iauth::IRightsProvider,
			public imtauth::IAccessTokenProvider
{
public:
	typedef imtgql::CClientRequestManagerCompBase BaseClass;

	I_BEGIN_COMPONENT(CSimpleLoginWrapComp);
		I_REGISTER_INTERFACE(ILogin);
		I_REGISTER_INTERFACE(IRightsProvider);
		I_REGISTER_INTERFACE(IAccessTokenProvider);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_userInfoFactCompPtr, "UserFactory", "Factory used for creation of the new user", true, "UserFactory");
		I_ASSIGN(m_superuserProviderCompPtr, "SuperuserProvider", "Superuser provider", false, "SuperuserProvider");
		I_ASSIGN(m_checkPermissionCompPtr, "PermissionChecker", "Checker of the permissions", false, "PermissionChecker");
		I_ASSIGN(m_productIdAttrPtr, "ProductId", "Product ID", true, "");
	I_END_COMPONENT;

	CSimpleLoginWrapComp();

	// reimplemented (iauth::ILogin)
	virtual iauth::CUser* GetLoggedUser() const override;
	virtual bool Login(const QString& userName, const QString& password) override;
	virtual bool Logout() override;

	// reimplemented (iauth::IRightsProvider)
	virtual bool HasRight(
				const QByteArray& operationId,
				bool beQuiet = false) const override;

	// reimplemented (imtauth::IAccessTokenProvider)
	virtual QByteArray GetToken(const QByteArray& userId) const override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	bool UserInfoUpdate(const QByteArray& userObjectId);
	void CheckSuperuser();

private:
	QByteArray m_loggedUserId;
	QByteArray m_loggedUserToken;
	QByteArrayList m_userPermissionIds;
	istd::TSmartPtr<imtauth::IUserInfo> m_userInfoPtr;

protected:
	I_FACT(imtauth::IUserInfo, m_userInfoFactCompPtr);
	I_REF(imtauth::IPermissionChecker, m_checkPermissionCompPtr);
	I_REF(imtauth::ISuperuserProvider, m_superuserProviderCompPtr);
	I_ATTR(QByteArray, m_productIdAttrPtr);
};


} // namespace imtauthgql


