// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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

	QString superuserName = m_superuserNameAttrPtr.IsValid() ? *m_superuserNameAttrPtr : QStringLiteral("superuser");
	arguments.input.Version_1_0->name = superuserName;

	QString superuserMail = m_superuserMailAttrPtr.IsValid() ? *m_superuserMailAttrPtr : QStringLiteral("superuser");
	arguments.input.Version_1_0->mail = superuserMail;

	arguments.input.Version_1_0->password = password;

	imtgql::CGqlRequest gqlRequest;
	if (userssdl::CCreateSuperuserGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		typedef userssdl::CCreateSuperuserPayload Response;

		QString errorMessage;
		Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			return false;
		}

		if (!response.Version_1_0){
			return false;
		}

		if (response.Version_1_0->success.has_value()){
			return *response.Version_1_0->success;
		}
	}

	return false;
}


} // namespace imtauthgql


