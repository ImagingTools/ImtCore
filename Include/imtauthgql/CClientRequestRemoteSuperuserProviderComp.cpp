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
	Q_ASSERT(m_superuserIdAttrPtr.IsValid());
	Q_ASSERT(m_applicationInfoCompPtr.IsValid());

	namespace userssdl = sdl::imtauth::Users;

	userssdl::V1_0::UserItemRequestArguments arguments;
	arguments.input.Id = QByteArray(*m_superuserIdAttrPtr);

	if (m_applicationInfoCompPtr.IsValid()){
		arguments.input.ProductId = QByteArray(m_applicationInfoCompPtr->GetApplicationAttribute(ibase::IApplicationInfo::AA_APPLICATION_ID).toUtf8());
	}

	imtgql::CGqlRequest gqlRequest;
	if (userssdl::V1_0::CUserItemGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		userssdl::CUserDataPayload::V1_0 response;
		if (!SendModelRequest<userssdl::CUserDataPayload::V1_0, userssdl::CUserDataPayload>(gqlRequest, response)){
			return false;
		}

		if (response.UserData.has_value()){
			Q_ASSERT_X((*response.UserData).Id.has_value(), "CClientRequestRemoteSuperuserProviderComp::SuperuserExists", "User-ID was requested and should be returned by server");
			if ((*response.UserData).Id.has_value()){
				return (*m_superuserIdAttrPtr == *(*response.UserData).Id);
			}
		}
	}

	return false;
}


QByteArray CClientRequestRemoteSuperuserProviderComp::GetSuperuserId() const
{
	return *m_superuserIdAttrPtr;
}


} // namespace imtauthgql


