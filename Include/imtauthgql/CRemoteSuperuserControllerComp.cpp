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

	userssdl::CreateSuperuserRequestArguments arguments;
	arguments.input.Version_1_0 = userssdl::CCreateSuperuserInput::V1_0();
	arguments.input.Version_1_0->Name = *m_superuserNameAttrPtr;
	arguments.input.Version_1_0->Mail = *m_superuserMailAttrPtr;
	arguments.input.Version_1_0->Password = password;

	imtgql::CGqlRequest gqlRequest;
	if (userssdl::CCreateSuperuserGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		userssdl::CCreateSuperuserPayload::V1_0 response;
		if (SendModelRequest<userssdl::CCreateSuperuserPayload::V1_0, userssdl::CCreateSuperuserPayload>(gqlRequest, response)){
			if (response.Success.has_value()){
				return *response.Success;
			}
		}
	}

	return false;
}


} // namespace imtauthgql


