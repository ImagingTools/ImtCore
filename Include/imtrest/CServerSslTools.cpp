// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrest/CServerSslTools.h>


// Qt includes
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslKey>


namespace imtrest
{


// public static methods

void CServerSslTools::ApplyConfiguration(QSslSocket& socket, const QSslConfiguration& configuration)
{
	// The configuration of the socket has the default ciphers, a configuration made from nothing has none
	QSslConfiguration socketConfiguration = socket.sslConfiguration();

	const QList<QSslCertificate> certificateChain = configuration.localCertificateChain();
	if (!certificateChain.isEmpty()){
		socketConfiguration.setLocalCertificateChain(certificateChain);
	}

	if (!configuration.privateKey().isNull()){
		socketConfiguration.setPrivateKey(configuration.privateKey());
	}

	socketConfiguration.setProtocol(configuration.protocol());

	const QList<QSslCertificate> caCertificates = configuration.caCertificates();
	if (!caCertificates.isEmpty()){
		socketConfiguration.setCaCertificates(caCertificates);
	}

	socket.setSslConfiguration(socketConfiguration);
}


} // namespace imtrest


