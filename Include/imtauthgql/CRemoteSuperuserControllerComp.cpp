#include <imtauthgql/CRemoteSuperuserControllerComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


// public methods

// reimplemented (ISuperuserController)

bool CRemoteSuperuserControllerComp::SetSuperuserPassword(const QByteArray& password) const
{
	if (!m_applicationInfoCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'ApplicationInfo' was not set", "CRemoteSuperuserControllerComp");
		return false;
	}

	namespace userssdl = sdl::imtauth::Users::V1_0;

	userssdl::UserAddRequestArguments arguments;
	arguments.input.SetId("su");
	arguments.input.SetProductId(m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8());

	sdl::imtauth::Users::V1_0::CUserData userData;
	userData.SetId("su");
	userData.SetUsername("su");
	userData.SetName("superuser");
	userData.SetPassword(qPrintable(password));

	arguments.input.SetItem(userData);

	imtgql::CGqlRequest gqlRequest;
	if (userssdl::CUserAddGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		sdl::imtbase::ImtCollection::V1_0::CAddedNotificationPayload response;
		if (SendModelRequest(gqlRequest, response)){
			return true;
		}
	}

	return false;
}


} // namespace imtauthgql


