// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSslCertificateChainTest.h"


// Qt includes
#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslConfiguration>

// ACF includes
#include <ifile/CFileNameParam.h>
#include <iprm/CNameParam.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtcom/CSslConfigurationManagerComp.h>

#include "SslTestCertificates.h"


namespace
{


typedef imtcom::CSslConfigurationManagerComp Manager;


QSslCertificate CreateLeafCertificate()
{
	return QSslCertificate(SslTestCertificates::s_leafPem, QSsl::Pem);
}


QSslCertificate CreateIntermediateCertificate()
{
	return QSslCertificate(SslTestCertificates::s_intermediatePem, QSsl::Pem);
}


// The leaf certificate and the one that goes after it, as in the file of a full chain
QByteArray CreateBundlePem()
{
	return SslTestCertificates::s_leafPem + SslTestCertificates::s_intermediatePem;
}


/**
	Parameters of a certificate that is read from a file: the same as the parameters of the settings of a server.
*/
struct SFileParams
{
	explicit SFileParams(const QString& path)
	{
		pathParam.SetPath(path);
		paramsSet.SetEditableParameter("FilePath", &pathParam);
	}

	iprm::CParamsSet paramsSet;
	ifile::CFileNameParam pathParam;
};


/**
	Parameters of a certificate that is given as the encoded data.
*/
struct SDataParams
{
	explicit SDataParams(const QByteArray& data)
	{
		dataParam.SetName(QString::fromLatin1(data));
		paramsSet.SetEditableParameter("EncodedData", &dataParam);
	}

	iprm::CParamsSet paramsSet;
	iprm::CNameParam dataParam;
};


bool WriteFile(const QString& path, const QByteArray& data)
{
	QFile file(path);

	return file.open(QIODevice::WriteOnly) && (file.write(data) == data.size());
}


} // namespace


// private slots

void CSslCertificateChainTest::ComposeChainTest()
{
	const QSslCertificate leaf = CreateLeafCertificate();
	const QSslCertificate intermediate = CreateIntermediateCertificate();
	QVERIFY(!leaf.isNull());
	QVERIFY(!intermediate.isNull());

	// The certificate of the server goes first, the intermediate ones follow it in their order
	const QList<QSslCertificate> chain = Manager::ComposeCertificateChain({leaf}, {intermediate});
	QCOMPARE(chain.size(), 2);
	QVERIFY(chain.at(0) == leaf);
	QVERIFY(chain.at(1) == intermediate);

	// A certificate that is already in the chain is not repeated: the file has the whole chain and the parameter adds the same one
	const QList<QSslCertificate> withoutRepeat = Manager::ComposeCertificateChain({leaf, intermediate}, {intermediate, leaf});
	QCOMPARE(withoutRepeat.size(), 2);
	QVERIFY(withoutRepeat.at(0) == leaf);
	QVERIFY(withoutRepeat.at(1) == intermediate);

	// Nothing to add
	QCOMPARE(Manager::ComposeCertificateChain({leaf}, {}).size(), 1);
}


void CSslCertificateChainTest::ComposeChainWithoutServerCertificateTest()
{
	const QSslCertificate leaf = CreateLeafCertificate();
	const QSslCertificate intermediate = CreateIntermediateCertificate();

	// The parameter has the whole chain: its first certificate is the certificate of the server
	const QList<QSslCertificate> chain = Manager::ComposeCertificateChain({}, {leaf, intermediate});
	QCOMPARE(chain.size(), 2);
	QVERIFY(chain.at(0) == leaf);
	QVERIFY(chain.at(1) == intermediate);
}


void CSslCertificateChainTest::ComposeChainOfNothingTest()
{
	QVERIFY(Manager::ComposeCertificateChain({}, {}).isEmpty());
}


void CSslCertificateChainTest::BundleFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath(QStringLiteral("fullchain.pem"));
	QVERIFY(WriteFile(path, CreateBundlePem()));

	SFileParams params(path);

	// All certificates of the file are read, not only the first one
	Manager manager;
	const QList<QSslCertificate> certificates = manager.CreateSslCertificatesFromParams(params.paramsSet);
	QCOMPARE(certificates.size(), 2);
	QVERIFY(certificates.at(0) == CreateLeafCertificate());
	QVERIFY(certificates.at(1) == CreateIntermediateCertificate());

	// The old function gives the first one
	const QSharedPointer<QSslCertificate> firstPtr = manager.CreateSslCertificateFromParams(params.paramsSet);
	QVERIFY(!firstPtr.isNull());
	QVERIFY(*firstPtr == CreateLeafCertificate());
}


void CSslCertificateChainTest::SingleCertificateFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath(QStringLiteral("cert.pem"));
	QVERIFY(WriteFile(path, SslTestCertificates::s_leafPem));

	SFileParams params(path);

	Manager manager;
	const QList<QSslCertificate> certificates = manager.CreateSslCertificatesFromParams(params.paramsSet);
	QCOMPARE(certificates.size(), 1);
	QVERIFY(certificates.at(0) == CreateLeafCertificate());
}


void CSslCertificateChainTest::EncodedDataBundleTest()
{
	SDataParams params(CreateBundlePem());

	Manager manager;
	const QList<QSslCertificate> certificates = manager.CreateSslCertificatesFromParams(params.paramsSet);
	QCOMPARE(certificates.size(), 2);
	QVERIFY(certificates.at(0) == CreateLeafCertificate());
	QVERIFY(certificates.at(1) == CreateIntermediateCertificate());
}


void CSslCertificateChainTest::MissingFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SFileParams params(dir.filePath(QStringLiteral("missing.pem")));

	Manager manager;
	QVERIFY(manager.CreateSslCertificatesFromParams(params.paramsSet).isEmpty());
	QVERIFY(manager.CreateSslCertificateFromParams(params.paramsSet).isNull());
}


void CSslCertificateChainTest::FileWithoutCertificatesTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath(QStringLiteral("text.pem"));
	QVERIFY(WriteFile(path, "this is not a certificate"));

	SFileParams params(path);

	// A file that has no certificates is not a certificate that is null: nothing is read
	Manager manager;
	QVERIFY(manager.CreateSslCertificatesFromParams(params.paramsSet).isEmpty());
	QVERIFY(manager.CreateSslCertificateFromParams(params.paramsSet).isNull());
}


void CSslCertificateChainTest::ConfigurationWithBundleTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath(QStringLiteral("fullchain.pem"));
	QVERIFY(WriteFile(path, CreateBundlePem()));

	SFileParams localCertificate(path);

	iprm::CParamsSet params;
	params.SetEditableParameter(imtcom::ISslConfigurationManager::ParamKeys::s_localCertParamKey, &localCertificate.paramsSet);

	Manager manager;
	QSslConfiguration configuration;
	QVERIFY(manager.CreateSslConfiguration(params, configuration));

	// The client gets the intermediate certificate with the certificate of the server and can verify it
	const QList<QSslCertificate> chain = configuration.localCertificateChain();
	QCOMPARE(chain.size(), 2);
	QVERIFY(chain.at(0) == CreateLeafCertificate());
	QVERIFY(chain.at(1) == CreateIntermediateCertificate());
	QVERIFY(configuration.localCertificate() == CreateLeafCertificate());
}


void CSslCertificateChainTest::ConfigurationWithSingleCertificateTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath(QStringLiteral("cert.pem"));
	QVERIFY(WriteFile(path, SslTestCertificates::s_leafPem));

	SFileParams localCertificate(path);

	iprm::CParamsSet params;
	params.SetEditableParameter(imtcom::ISslConfigurationManager::ParamKeys::s_localCertParamKey, &localCertificate.paramsSet);

	Manager manager;
	QSslConfiguration configuration;
	QVERIFY(manager.CreateSslConfiguration(params, configuration));

	// A certificate without a chain works as before
	QCOMPARE(configuration.localCertificateChain().size(), 1);
	QVERIFY(configuration.localCertificate() == CreateLeafCertificate());
}


void CSslCertificateChainTest::ConfigurationWithoutCertificateTest()
{
	Manager manager;
	QSslConfiguration configuration;

	// No certificate is configured: nothing is set, and it is not a failure of this part
	QVERIFY(manager.CreateSslConfiguration(iprm::CParamsSet(), configuration));
	QVERIFY(configuration.localCertificateChain().isEmpty());
}


void CSslCertificateChainTest::ConfigurationWithMissingFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	SFileParams localCertificate(dir.filePath(QStringLiteral("missing.pem")));

	iprm::CParamsSet params;
	params.SetEditableParameter(imtcom::ISslConfigurationManager::ParamKeys::s_localCertParamKey, &localCertificate.paramsSet);

	// The server must not start with a certificate that is not read (see the start of the server)
	Manager manager;
	QSslConfiguration configuration;
	QVERIFY(!manager.CreateSslConfiguration(params, configuration));
}


void CSslCertificateChainTest::ConfigurationWithInvalidFileTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath(QStringLiteral("text.pem"));
	QVERIFY(WriteFile(path, "this is not a certificate"));

	SFileParams localCertificate(path);

	iprm::CParamsSet params;
	params.SetEditableParameter(imtcom::ISslConfigurationManager::ParamKeys::s_localCertParamKey, &localCertificate.paramsSet);

	// Before, the null certificate was set and the server failed at every handshake
	Manager manager;
	QSslConfiguration configuration;
	QVERIFY(!manager.CreateSslConfiguration(params, configuration));
}


void CSslCertificateChainTest::ConfigurationWithCaBundleTest()
{
	QTemporaryDir dir;
	QVERIFY(dir.isValid());

	const QString path = dir.filePath(QStringLiteral("ca.pem"));
	QVERIFY(WriteFile(path, CreateBundlePem()));

	SFileParams caCertificates(path);

	iprm::CParamsSet params;
	params.SetEditableParameter(imtcom::ISslConfigurationManager::ParamKeys::s_caCertParamKey, &caCertificates.paramsSet);

	Manager manager;
	QSslConfiguration configuration;
	QVERIFY(manager.CreateSslConfiguration(params, configuration));

	// All certificates of the file of the authorities are taken
	QCOMPARE(configuration.caCertificates().size(), 2);
}


void CSslCertificateChainTest::ConfigurationWithoutCaFileTest()
{
	SFileParams caCertificates{QString()};

	iprm::CParamsSet params;
	params.SetEditableParameter(imtcom::ISslConfigurationManager::ParamKeys::s_caCertParamKey, &caCertificates.paramsSet);

	// The CA certificate is optional: an empty path is not a failure
	Manager manager;
	QSslConfiguration configuration;
	QVERIFY(manager.CreateSslConfiguration(params, configuration));
	QVERIFY(configuration.caCertificates().isEmpty());
}


I_ADD_TEST(CSslCertificateChainTest);
