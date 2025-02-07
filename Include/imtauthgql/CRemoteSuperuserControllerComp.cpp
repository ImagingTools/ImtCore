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

	namespace userssdl = sdl::imtauth::Users;

	userssdl::UserAddRequestArguments arguments;
	arguments.input.Version_1_0 = userssdl::CUserDataInput::V1_0();
	arguments.input.Version_1_0->Id = QByteArray("su");
	arguments.input.Version_1_0->ProductId = QByteArray(m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8());

	sdl::imtauth::Users::CUserData::V1_0 userData;
	userData.Id = QByteArray("su");
	userData.Username = QByteArray("su");
	userData.Name = QString("superuser");
	userData.Password = QString(qPrintable(password));

	arguments.input.Version_1_0->Item = std::make_optional<sdl::imtauth::Users::CUserData::V1_0>(userData);

	imtgql::CGqlRequest gqlRequest;
	if (userssdl::CUserAddGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		sdl::imtbase::ImtCollection::CAddedNotificationPayload::V1_0 response;
		if (SendModelRequest<sdl::imtbase::ImtCollection::CAddedNotificationPayload::V1_0, sdl::imtbase::ImtCollection::CAddedNotificationPayload>(gqlRequest, response)){
			return true;
		}
	}

	return false;
}


} // namespace imtauthgql


