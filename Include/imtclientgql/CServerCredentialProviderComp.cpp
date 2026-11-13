// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtclientgql/CServerCredentialProviderComp.h>


// Qt includes
#include <QtCore/QHash>
#include <QtCore/QMutex>


namespace imtclientgql
{


namespace
{


QMutex s_tokensMutex;
QHash<QByteArray, QByteArray> s_tokensOfVariables;


} // namespace


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

	const QByteArray variableName = GetServerCredentialVariableName(*m_serverProductIdAttrPtr);
	// Read once for the process and removed: the processes this server starts must not inherit the token
	{
		QMutexLocker locker(&s_tokensMutex);
		if (!s_tokensOfVariables.contains(variableName)){
			s_tokensOfVariables.insert(variableName, qgetenv(variableName.constData()));
			qunsetenv(variableName.constData());
		}

		m_token = s_tokensOfVariables.value(variableName);
	}
	if (m_token.isEmpty()){
		SendErrorMessage(0, QStringLiteral("The environment variable '%1' is not set: this server has no token for %2. It works only while %2 takes servers without tokens, and publishes no permissions and sends no notifications there")
					.arg(QString::fromUtf8(variableName), QString::fromUtf8(*m_serverProductIdAttrPtr)),
					"CServerCredentialProviderComp");
	}
}


} // namespace imtclientgql
