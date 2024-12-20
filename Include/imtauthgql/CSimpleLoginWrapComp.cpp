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
	authsdl::V1_0::AuthorizationRequestArguments arguments;
	arguments.input.Login.reset(new QString(userName));
	arguments.input.Password.reset(new QString(password));
	arguments.input.ProductId.reset(new QByteArray(productId));

	imtgql::CGqlRequest gqlRequest;
	if (authsdl::V1_0::CAuthorizationGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		authsdl::CAuthorizationPayload::V1_0 response;
		if (!SendModelRequest<authsdl::CAuthorizationPayload::V1_0, authsdl::CAuthorizationPayload>(gqlRequest, response)){
			return false;
		}

		QByteArray userId;
		if (response.UserId){
			userId = *response.UserId;
		}
		if (userId.isEmpty()){
			return false;
		}

		m_userInfoPtr.SetPtr(m_userInfoFactCompPtr.CreateInstance());
		if (!m_userInfoPtr.IsValid()){
			return false;
		}

		if (response.Username){
			m_userInfoPtr->SetId(*response.Username);
		}
		if (response.Permissions){
			m_userInfoPtr->SetLocalPermissions(productId, response.Permissions->split(';'));
		}

		m_userPermissionIds = m_userInfoPtr->GetPermissions(productId);

		if (response.Token){
			m_loggedUserToken = *response.Token;
		}
		imtqml::CGqlModel::SetGlobalAccessToken(m_loggedUserToken);

		if (response.Username){
			istd::CChangeNotifier notifier(this);
			Q_UNUSED(notifier);

			m_loggedUserId = *response.Username;
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


} // namespace imtauthgql


