// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CServerSslToolsTest.h"


// Qt includes
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslSocket>

// ImtCore includes
#include <imtrest/CServerSslTools.h>

#include "SslTestCertificates.h"


namespace
{


QSslCertificate CreateLeafCertificate()
{
	return QSslCertificate(SslTestCertificates::s_leafPem, QSsl::Pem);
}


QSslCertificate CreateIntermediateCertificate()
{
	return QSslCertificate(SslTestCertificates::s_intermediatePem, QSsl::Pem);
}


} // namespace


// private slots

void CServerSslToolsTest::CertificateChainTest()
{
	const QSslCertificate leaf = CreateLeafCertificate();
	const QSslCertificate intermediate = CreateIntermediateCertificate();
	QVERIFY(!leaf.isNull());
	QVERIFY(!intermediate.isNull());
	QVERIFY(leaf != intermediate);

	QSslConfiguration configuration;
	configuration.setLocalCertificateChain({leaf, intermediate});

	QSslSocket socket;
	imtrest::CServerSslTools::ApplyConfiguration(socket, configuration);

	// The client gets the intermediate certificate with the certificate of the server and can verify it (the leaf goes first)
	const QList<QSslCertificate> chain = socket.sslConfiguration().localCertificateChain();
	QCOMPARE(chain.size(), 2);
	QVERIFY(chain.at(0) == leaf);
	QVERIFY(chain.at(1) == intermediate);
	QVERIFY(socket.sslConfiguration().localCertificate() == leaf);
}


void CServerSslToolsTest::SingleCertificateTest()
{
	const QSslCertificate leaf = CreateLeafCertificate();
	QVERIFY(!leaf.isNull());

	QSslConfiguration configuration;
	configuration.setLocalCertificate(leaf);

	QSslSocket socket;
	imtrest::CServerSslTools::ApplyConfiguration(socket, configuration);

	// The certificate without a chain works as before
	QCOMPARE(socket.sslConfiguration().localCertificateChain().size(), 1);
	QVERIFY(socket.sslConfiguration().localCertificate() == leaf);
}


void CServerSslToolsTest::ProtocolTest()
{
	QSslConfiguration configuration;
	configuration.setProtocol(QSsl::TlsV1_3OrLater);

	QSslSocket socket;
	imtrest::CServerSslTools::ApplyConfiguration(socket, configuration);

	QCOMPARE(socket.sslConfiguration().protocol(), QSsl::TlsV1_3OrLater);
}


void CServerSslToolsTest::CaCertificatesTest()
{
	const QSslCertificate intermediate = CreateIntermediateCertificate();
	QVERIFY(!intermediate.isNull());

	QSslConfiguration configuration;
	configuration.setCaCertificates({intermediate});

	QSslSocket socket;
	imtrest::CServerSslTools::ApplyConfiguration(socket, configuration);

	const QList<QSslCertificate> caCertificates = socket.sslConfiguration().caCertificates();
	QCOMPARE(caCertificates.size(), 1);
	QVERIFY(caCertificates.at(0) == intermediate);
}


void CServerSslToolsTest::EmptyConfigurationKeepsDefaultsTest()
{
	if (!QSslSocket::supportsSsl()){
		QSKIP("SSL is not supported by this build of Qt");
	}

	QSslSocket socket;
	const QList<QSslCipher> defaultCiphers = socket.sslConfiguration().ciphers();
	QVERIFY(!defaultCiphers.isEmpty());

	// The configuration of the server is made from nothing: its empty ciphers must not replace the ones of the socket,
	// the handshake would have nothing to agree on
	imtrest::CServerSslTools::ApplyConfiguration(socket, QSslConfiguration());

	QCOMPARE(socket.sslConfiguration().ciphers(), defaultCiphers);
}


void CServerSslToolsTest::MissingFieldsAreNotChangedTest()
{
	const QSslCertificate leaf = CreateLeafCertificate();
	QVERIFY(!leaf.isNull());

	// What the socket had before stays if the configuration has no such field
	QSslSocket socket;
	QSslConfiguration socketConfiguration = socket.sslConfiguration();
	socketConfiguration.setLocalCertificate(leaf);
	socket.setSslConfiguration(socketConfiguration);

	imtrest::CServerSslTools::ApplyConfiguration(socket, QSslConfiguration());

	QVERIFY(socket.sslConfiguration().localCertificate() == leaf);
}


I_ADD_TEST(CServerSslToolsTest);
