// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		typedef userssdl::CCheckSuperuserPayload Response;

		Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			return imtauth::ISuperuserProvider::ES_UNKNOWN;
		}

		if (!response.Version_1_0){
			return imtauth::ISuperuserProvider::ES_UNKNOWN;
		}

		if (response.Version_1_0->message){
			errorMessage = *response.Version_1_0->message;
		}

		imtauth::ISuperuserProvider::ExistsStatus retVal = imtauth::ISuperuserProvider::ES_UNKNOWN;
		if (response.Version_1_0->status){
			sdl::imtauth::Users::ExistsStatus status = *response.Version_1_0->status;
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


