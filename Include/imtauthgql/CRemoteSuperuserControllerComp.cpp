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

	namespace userssdl = sdl::V1_0::imtauth;

	userssdl::CreateSuperuserRequestArguments arguments;

	QString superuserName = m_superuserNameAttrPtr.IsValid() ? *m_superuserNameAttrPtr : QStringLiteral("superuser");
	arguments.input.name = superuserName;

	QString superuserMail = m_superuserMailAttrPtr.IsValid() ? *m_superuserMailAttrPtr : QStringLiteral("superuser");
	arguments.input.mail = superuserMail;

	arguments.input.password = password;

	imtgql::CGqlRequest gqlRequest;
	if (userssdl::CCreateSuperuserGqlRequest::SetupGqlRequest(gqlRequest, arguments)){
		typedef userssdl::CCreateSuperuserPayload Response;

		QString errorMessage;
		Response response = SendModelRequest<Response>(gqlRequest, errorMessage);
		if (!errorMessage.isEmpty()){
			return false;
		}

		if (response.success.has_value()){
			return *response.success;
		}
	}

	return false;
}


} // namespace imtauthgql


