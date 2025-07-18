#include <imtauthgql/CClientRequestRemoteSuperuserProviderComp.h>


// ImtCore includes
#include <imtgql/CGqlRequest.h>
#include <GeneratedFiles/imtauthsdl/SDL/1.0/CPP/Users.h>


namespace imtauthgql
{


// public methods

// reimplemented (imtauth::ISuperuserProvider)

imtauth::ISuperuserProvider::ExistsStatus CClientRequestRemoteSuperuserProviderComp::SuperuserExists(QString& errorMessage) const
{
	namespace userssdl = sdl::imtauth::Users;

	userssdl::CheckSuperuserExistsRequestArguments arguments;
	arguments.input.Version_1_0 = userssdl::CCheckSuperuserInput::V1_0();

	imtgql::CGqlRequest gqlRequest;
	if (userssdl::CCheckSuperuserExistsGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		userssdl::CCheckSuperuserPayload::V1_0 response;
		if (!SendModelRequest<userssdl::CCheckSuperuserPayload::V1_0, userssdl::CCheckSuperuserPayload>(gqlRequest, response, errorMessage)){
			return imtauth::ISuperuserProvider::ES_UNKNOWN;
		}

		if (response.message){
			errorMessage = *response.message;
		}

		imtauth::ISuperuserProvider::ExistsStatus retVal = imtauth::ISuperuserProvider::ES_UNKNOWN;
		if (response.status){
			sdl::imtauth::Users::ExistsStatus status = *response.status;
			if (status == sdl::imtauth::Users::ExistsStatus::EXISTS){
				retVal = imtauth::ISuperuserProvider::ES_EXISTS;
			}
			else if (status == sdl::imtauth::Users::ExistsStatus::NOT_EXISTS){
				retVal = imtauth::ISuperuserProvider::ES_NOT_EXISTS;
			}
		}

		return retVal;
	}

	return imtauth::ISuperuserProvider::ES_UNKNOWN;
}


QByteArray CClientRequestRemoteSuperuserProviderComp::GetSuperuserId() const
{
	return *m_superuserIdAttrPtr;
}


} // namespace imtauthgql


