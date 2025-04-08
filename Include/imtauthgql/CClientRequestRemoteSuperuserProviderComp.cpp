#include <imtauthgql/CClientRequestRemoteSuperuserProviderComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::ISuperuserProvider)

bool CClientRequestRemoteSuperuserProviderComp::SuperuserExists(QString& /*errorMessage*/) const
{
	namespace userssdl = sdl::imtauth::Users;
	
	userssdl::CheckSuperuserExistsRequestArguments arguments;
	arguments.input.Version_1_0 = userssdl::CCheckSuperuserInput::V1_0();
	
	imtgql::CGqlRequest gqlRequest;
	if (userssdl::CCheckSuperuserExistsGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		userssdl::CCheckSuperuserPayload::V1_0 response;
		if (!SendModelRequest<userssdl::CCheckSuperuserPayload::V1_0, userssdl::CCheckSuperuserPayload>(gqlRequest, response)){
			return false;
		}
		
		if (response.exists.has_value()){
			return *response.exists;
		}
	}
	
	return false;
}


QByteArray CClientRequestRemoteSuperuserProviderComp::GetSuperuserId() const
{
	return *m_superuserIdAttrPtr;
}


} // namespace imtauthgql


