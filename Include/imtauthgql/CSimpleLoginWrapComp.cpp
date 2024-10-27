#include <imtauthgql/CSimpleLoginWrapComp.h>


// ACF includes
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtqml/CGqlModel.h>
#include <imtauth/CUserInfo.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Authorization.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


// public methods

CSimpleLoginWrapComp::CSimpleLoginWrapComp()
{
}


// reimplemented (iauth::ILogin)

iauth::CUser* CSimpleLoginWrapComp::GetLoggedUser() const
{
	if (!m_loggedUserId.isEmpty() && m_userInfoPtr.IsValid()){
		istd::TDelPtr<iauth::CUser> userPtr(new iauth::CUser);
		userPtr->SetUserName(m_loggedUserId);
		userPtr->SetPassword(m_loggedUserPassword);

		return userPtr.PopPtr();
	}

	return nullptr;
}


bool CSimpleLoginWrapComp::Login(const QString& userName, const QString& password)
{
	if (!m_applicationInfoCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ApplicationInfo' was not set", "CSimpleLoginWrapComp");
		return false;
	}

	namespace authsdl = sdl::imtauth::Authorization::V1_0;

	authsdl::AuthorizationRequestArguments arguments;
	arguments.input.SetLogin(userName);
	arguments.input.SetPassword(password);
	arguments.input.SetProductId(m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8());

	imtgql::CGqlRequest gqlRequest;
	if (authsdl::CAuthorizationGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		authsdl::CAuthorizationPayload response;
		if (!SendModelRequest(gqlRequest, response)){
			return false;
		}

		QByteArray userId = response.GetUserId();
		if (userId.isEmpty()){
			return false;
		}

		RetrieveUserInfo(userId);

		m_loggedUserToken = response.GetToken();
		imtqml::CGqlModel::SetGlobalAccessToken(m_loggedUserToken);

		{
			istd::CChangeNotifier notifier(this);
			Q_UNUSED(notifier);

			m_loggedUserId = response.GetUsername();
		}

		m_loggedUserPassword = password.toUtf8();

		return true;
	}

	return false;
}


bool CSimpleLoginWrapComp::Logout()
{
	if (!m_loggedUserId.isEmpty()){
		istd::CChangeNotifier notifier(this);
		Q_UNUSED(notifier);

		m_loggedUserId.clear();

		return true;
	}

	return false;
}


// reimplemented (iauth::IRightsProvider)

bool CSimpleLoginWrapComp::HasRight(
	const QByteArray& operationId,
	bool /*beQuiet*/) const
{
	if (m_userInfoPtr.IsValid()){
		bool isAdmin = m_userInfoPtr->IsAdmin();
		if (isAdmin){
			return true;
		}
	}

	if (!m_checkPermissionCompPtr.IsValid()){
		return false;
	}

	bool retVal = m_checkPermissionCompPtr->CheckPermission(m_userPermissionIds, QByteArrayList() << operationId);

	return retVal;
}


// reimplemented (imtauth::IAccessTokenProvider)

QByteArray CSimpleLoginWrapComp::GetToken(const QByteArray& /*userId*/) const
{
	return m_loggedUserToken;
}


// private methods

bool CSimpleLoginWrapComp::RetrieveUserInfo(const QByteArray& userObjectId)
{
	if (!m_userInfoFactCompPtr.IsValid()){
		return false;
	}

	if (!m_applicationInfoCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ApplicationInfo' was not set", "CSimpleLoginWrapComp");
		return false;
	}

	namespace userssdl = sdl::imtauth::Users::V1_0;

	QByteArray productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	userssdl::UserItemRequestArguments arguments;
	arguments.input.SetId(userObjectId);
	arguments.input.SetProductId(productId);

	imtgql::CGqlRequest gqlRequest;
	if (userssdl::CUserItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		// TODO: remove !!!
		imtgql::CGqlObject itemObject;
		itemObject.InsertField("id");
		gqlRequest.AddField("item", itemObject);

		userssdl::CUserDataPayload response;
		if (!SendModelRequest(gqlRequest, response)){
			return false;
		}

		userssdl::CUserData userData = response.GetUserData();

		m_userInfoPtr.SetPtr(m_userInfoFactCompPtr.CreateInstance());
		if (!m_userInfoPtr.IsValid()){
			return false;
		}

		m_userInfoPtr->SetId(userData.GetUsername());
		m_userInfoPtr->SetPasswordHash(userData.GetPassword().toUtf8());

		m_userPermissionIds = m_userInfoPtr->GetPermissions(productId);

		return true;
	}

	return false;
}


} // namespace imtauthgql


