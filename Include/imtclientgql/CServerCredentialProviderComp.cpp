// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CServerCredentialProviderComp.h>


// Qt includes
#include <QtCore/QByteArray>
#include <QtCore/qglobal.h>


namespace imtclientgql
{


// reimplemented (imtclientgql::IServerCredentialProvider)

QByteArray CServerCredentialProviderComp::GetServerToken() const
{
	return m_token;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CServerCredentialProviderComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_token = qgetenv(QByteArray(*m_environmentVariableAttrPtr).constData());
	if (m_token.isEmpty()){
		SendErrorMessage(0, QStringLiteral("The environment variable '%1' is not set: this server has no token, it publishes no permissions and sends no notifications")
					.arg(QString::fromUtf8(*m_environmentVariableAttrPtr)),
					"CServerCredentialProviderComp");
	}
}


} // namespace imtclientgql
