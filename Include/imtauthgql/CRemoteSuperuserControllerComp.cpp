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
	arguments.input.Id.reset(new QByteArray("su"));
	arguments.input.ProductId.reset(new QByteArray(m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8()));

	sdl::imtauth::Users::CUserData::V1_0 userData;
	userData.Id.reset(new QByteArray("su"));
	userData.Username.reset(new QByteArray("su"));
	userData.Name.reset(new QString("superuser"));
	userData.Password.reset(new QString(qPrintable(password)));

	arguments.input.Item = std::make_unique<sdl::imtauth::Users::CUserData::V1_0>(userData);

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


