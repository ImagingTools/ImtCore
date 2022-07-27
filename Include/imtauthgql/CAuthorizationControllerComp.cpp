#include <imtauthgql/CAuthorizationControllerComp.h>


namespace imtauthgql
{


imtbase::CTreeItemModel* CAuthorizationControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	return nullptr;
}


bool CAuthorizationControllerComp::LoginMatch(const QString& login) const
{
	return false;
}


bool CAuthorizationControllerComp::PasswordMatch(const QString& login, const QString& password) const
{
	return false;
}


int CAuthorizationControllerComp::GetUserId(const QString& verificationLogin) const
{
	return 0;
}


bool CAuthorizationControllerComp::ChangePassword(const QByteArray& userId, const QString& oldPassword, const QString& newPassword)
{
	return false;
}


} // namespace imtauthgql


