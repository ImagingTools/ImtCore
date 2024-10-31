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
	namespace userssdl = sdl::imtauth::Users::V1_0;

	userssdl::UserItemRequestArguments arguments;
	arguments.input.SetId(*m_superuserIdAttrPtr);

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input.SetProductId(m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8());
	}

	imtgql::CGqlRequest gqlRequest;
	if (userssdl::CUserItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		userssdl::CUserDataPayload response;
		if (!SendModelRequest(gqlRequest, response)){
			return false;
		}

		return true;
	}

	return false;
}


QByteArray CClientRequestRemoteSuperuserProviderComp::GetSuperuserId() const
{
	return *m_superuserIdAttrPtr;
}


} // namespace imtauthgql


