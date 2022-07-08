#include <imtauthgql/CAuthorizationControllerComp.h>

namespace imtauthgql
{


imtbase::CTreeItemModel *CAuthorizationControllerComp::CreateResponse(const imtgql::CGqlRequest &gqlRequest, QString &errorMessage) const
{
	return nullptr;
}


bool CAuthorizationControllerComp::LoginMatch(const QString &login) const
{
	return true;
}


bool CAuthorizationControllerComp::PasswordMatch(const QString &login, const QString &password) const
{
	if (login == "0" && password == "0"){
		return false;
	}
	return true;
}


int CAuthorizationControllerComp::GetUserId(const QString &verificationLogin) const
{
	return 0;
}


bool CAuthorizationControllerComp::ChangePassword(const int &idUser, const QString &oldPassword, const QString &newPassword)
{
	return true;
}




}
