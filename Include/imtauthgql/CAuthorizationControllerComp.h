#pragma once


// ImtCore includes
#include <imtgql/CGqlRepresentationDataControllerCompBase.h>


namespace imtauthgql
{


class CAuthorizationControllerComp: public imtgql::CGqlRepresentationDataControllerCompBase
{
public:
	typedef imtgql::CGqlRepresentationDataControllerCompBase BaseClass;

	I_BEGIN_COMPONENT(CAuthorizationControllerComp);
	I_END_COMPONENT;

	// reimplemented (imtgql::IGqlRepresentationDataController)
	virtual imtbase::CTreeItemModel* CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

private:
	bool LoginMatch(const QString& login) const;
	bool PasswordMatch(const QString& login, const QString& password) const;
	int GetUserId(const QString& verificationLogin) const;
	bool ChangePassword(const QByteArray& userId, const QString& oldPassword, const QString& newPassword);
};


} // namespace imtauthgql


