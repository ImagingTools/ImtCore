#include <imtauthgql/CSimpleLoginWrapComp.h>


// ACF includes
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <imtqml/CGqlModel.h>
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
		m_loggedUser.SetUserName(m_loggedUserId);
		m_loggedUser.SetPassword(m_loggedUserPassword);

		return &m_loggedUser;
	}

	return nullptr;
}


bool CSimpleLoginWrapComp::Login(const QString& userName, const QString& password)
{
	if (!m_applicationInfoCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ApplicationInfo' was not set", "CSimpleLoginWrapComp");
		return false;
	}
	
	namespace authsdl = sdl::imtauth::Authorization;

	QByteArray productId = m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8();
	authsdl::AuthorizationRequestArguments arguments;
	arguments.input.Version_1_0 = authsdl::CAuthorizationInput::V1_0();
	arguments.input.Version_1_0->login = QString(userName);
	arguments.input.Version_1_0->password = QString(password);
	arguments.input.Version_1_0->productId = QByteArray(productId);

	imtgql::CGqlRequest gqlRequest;
	if (authsdl::CAuthorizationGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		authsdl::CAuthorizationPayload response;
		if (!SendModelRequest<authsdl::CAuthorizationPayload, authsdl::CAuthorizationPayload>(gqlRequest, response)){
			return false;
		}
		m_loggedUserPassword = password.toUtf8();

		QByteArray userId;
		if (response.Version_1_0->userId){
			userId = *response.Version_1_0->userId;
		}
		if (userId.isEmpty()){
			return false;
		}

		m_userInfoPtr.SetPtr(m_userInfoFactCompPtr.CreateInstance());
		if (!m_userInfoPtr.IsValid()){
			return false;
		}

		if (response.Version_1_0->username){
			m_userInfoPtr->SetId(*response.Version_1_0->username);
		}
		if (response.Version_1_0->permissions){
			m_userInfoPtr->SetLocalPermissions(productId, response.Version_1_0->permissions->split(';'));
		}

		m_userPermissionIds = m_userInfoPtr->GetPermissions(productId);

		if (response.Version_1_0->token){
			m_loggedUserToken = *response.Version_1_0->token;
		}
		imtqml::CGqlModel::SetGlobalAccessToken(m_loggedUserToken);

		if (response.Version_1_0->username){
			istd::CChangeNotifier notifier(this);
			Q_UNUSED(notifier);

			m_loggedUserId = *response.Version_1_0->username;
		}
		
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


} // namespace imtauthgql


