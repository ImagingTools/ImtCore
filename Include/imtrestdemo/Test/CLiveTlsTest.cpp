// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CLiveTlsTest.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QEventLoop>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>
#include <QtCore/QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslCipher>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslSocket>

// ACF includes
#include <itest/CStandardTestExecutor.h>

// ImtCore includes
#include <imtrestdemo/DemoTlsTools.h>


namespace DemoTls = imtrestdemo::DemoTlsTools;


void CLiveTlsTest::CustomizeSettings(imtrestdemo::CDemoServerSettings& settings)
{
	// The server has the sets of the certificates (the presets) and a place for the certificates that a client sends
	if (m_samplesDirectory.isValid() && !DemoTls::FindOpenssl().isEmpty()){
		QString errorText;
		if (DemoTls::GenerateSamples(m_samplesDirectory.path() + "/presets", QString(), errorText)){
			settings.tlsSamplesDirectory = m_samplesDirectory.path() + "/presets";
		}

		settings.tlsStorageDirectory = m_samplesDirectory.path() + "/stored";
	}
}


void CLiveTlsTest::init()
{
	if (!m_isSamplesReady){
		if (DemoTls::FindOpenssl().isEmpty()){
			QSKIP("The program openssl is not found (IMTREST_OPENSSL or PATH)");
		}

		QVERIFY(m_samplesDirectory.isValid());

		m_samples = DemoTls::DescribeSamples(m_samplesDirectory.path() + "/presets");
		QVERIFY(!m_samples.isEmpty());
		m_isSamplesReady = true;
	}

	m_isTls = false;
	m_isVerified = true;
	m_trustedNow.clear();
}


void CLiveTlsTest::cleanup()
{
	if (!m_isSamplesReady || !m_isTls){
		return;
	}

	// The server goes back to http, the certificate is not checked: the test could leave the server with a certificate that the client refuses
	SResult result;
	QJsonObject changes;
	changes.insert("tlsEnabled", false);
	QVERIFY(Send("PUT", "/admin/settings", QJsonDocument(changes).toJson(), true, {}, false, result));
	m_isTls = false;
	QVERIFY(WaitForSettings([](const QJsonObject& settings){
		return !settings.value("tlsEnabled").toBool() && !settings.value("isRestartPending").toBool();
	}));
}


void CLiveTlsTest::SamplesAreMadeTest()
{
	QCOMPARE(m_samples.size(), 6);

	for (const QJsonValue& value : m_samples){
		const QJsonObject sample = value.toObject();
		const QString id = sample.value("id").toString();

		QString errorText;
		const QList<QSslCertificate> certificate = DemoTls::ReadCertificates(sample.value("certificateFile").toString(), errorText);
		QVERIFY2(!certificate.isEmpty(), qPrintable(id + ": " + errorText));

		QSsl::KeyAlgorithm algorithm = QSsl::Rsa;
		QVERIFY2(DemoTls::ReadPrivateKeyAlgorithm(sample.value("keyFile").toString(), algorithm, errorText), qPrintable(id + ": " + errorText));
		QCOMPARE(algorithm, id == "ec" ? QSsl::Ec : QSsl::Rsa);

		// The number of the certificates that the server sends: the file of the certificate and the files of the chain
		int count = certificate.size();
		for (const QJsonValue& chainFile : sample.value("chainFiles").toArray()){
			count += DemoTls::ReadCertificates(chainFile.toString(), errorText).size();
		}

		QCOMPARE(count, sample.value("certificatesInChain").toInt());
		QVERIFY(!ReadTrusted(sample).isEmpty());
	}

	// The concatenated file: the certificate of the server, then the intermediate certificates, every one is signed by the next
	QString errorText;
	const QList<QSslCertificate> bundle = DemoTls::ReadCertificates(FindSample("bundle").value("certificateFile").toString(), errorText);
	QCOMPARE(bundle.size(), 3);
	QCOMPARE(bundle.at(0).issuerInfo(QSslCertificate::CommonName).value(0), QString("imtrest-demo-intermediate-2"));
	QCOMPARE(bundle.at(1).subjectInfo(QSslCertificate::CommonName).value(0), QString("imtrest-demo-intermediate-2"));
	QCOMPARE(bundle.at(1).issuerInfo(QSslCertificate::CommonName).value(0), QString("imtrest-demo-intermediate-1"));
	QCOMPARE(bundle.at(2).subjectInfo(QSslCertificate::CommonName).value(0), QString("imtrest-demo-intermediate-1"));
	QCOMPARE(bundle.at(2).issuerInfo(QSslCertificate::CommonName).value(0), QString("imtrest-demo-root"));

	// The self-signed certificate is its own issuer
	const QSslCertificate single = ReadTrusted(FindSample("single")).value(0);
	QVERIFY(single.isSelfSigned());
	QVERIFY(single.subjectAlternativeNames().values(QSsl::DnsEntry).contains("localhost"));
}


void CLiveTlsTest::ServerFilesAreCheckedTest()
{
	const QJsonObject single = FindSample("single");
	const QJsonObject chain = FindSample("chain");
	const QString certificate = single.value("certificateFile").toString();
	const QString key = single.value("keyFile").toString();

	QString errorText;
	QVERIFY2(DemoTls::ValidateServerFiles(certificate, {}, key, errorText), qPrintable(errorText));
	QVERIFY2(DemoTls::ValidateServerFiles(chain.value("certificateFile").toString(), {chain.value("chainFiles").toArray().at(0).toString()}, chain.value("keyFile").toString(), errorText), qPrintable(errorText));

	// The files that are not what they are called
	QVERIFY(!DemoTls::ValidateServerFiles(QString(), {}, key, errorText));
	QVERIFY(!DemoTls::ValidateServerFiles(certificate, {}, QString(), errorText));
	QVERIFY(!DemoTls::ValidateServerFiles(m_samplesDirectory.path() + "/missing.pem", {}, key, errorText));
	QVERIFY(!DemoTls::ValidateServerFiles(certificate, {}, m_samplesDirectory.path() + "/missing.key", errorText));
	QVERIFY(!DemoTls::ValidateServerFiles(key, {}, key, errorText));
	QVERIFY(!DemoTls::ValidateServerFiles(certificate, {}, certificate, errorText));
	QVERIFY(!DemoTls::ValidateServerFiles(certificate, {key}, key, errorText));
}


void CLiveTlsTest::TlsScenarioTest_data()
{
	QTest::addColumn<QString>("sampleId");
	QTest::addColumn<int>("chainSize");

	QTest::newRow("one certificate") << "single" << 1;
	QTest::newRow("chain with an intermediate certificate") << "chain" << 2;
	QTest::newRow("several certificates in separate files") << "several" << 3;
	QTest::newRow("concatenated certificate") << "bundle" << 3;
	QTest::newRow("EC key") << "ec" << 1;
}


void CLiveTlsTest::TlsScenarioTest()
{
	QFETCH(QString, sampleId);
	QFETCH(int, chainSize);

	const QJsonObject sample = FindSample(sampleId);
	QVERIFY(!sample.isEmpty());

	// Plain http works before
	SResult result;
	QVERIFY(Send("GET", "/echo", QByteArray(), false, {}, true, result));
	QCOMPARE(result.statusCode, 200);

	QVERIFY(EnableTls(sample));

	// The client that trusts the certificates of the set gets the answer, the server has sent the whole chain
	QVERIFY2(Send("GET", "/echo?name=tls", QByteArray(), true, m_trustedNow, true, result), qPrintable(result.errorText));
	QCOMPARE(result.statusCode, 200);
	QCOMPARE(result.chainSize, chainSize);
	QVERIFY2(result.protocol.startsWith("TLS"), qPrintable(result.protocol));
	QCOMPARE(ParseJson(result.body).value("query").toObject().value("name").toString(), QString("tls"));

	// The server tells about it
	QVERIFY(Send("GET", "/info", QByteArray(), true, m_trustedNow, true, result));
	QCOMPARE(ParseJson(result.body).value("scheme").toString(), QString("https"));
	QCOMPARE(ParseJson(result.body).value("tlsEnabled").toBool(), true);

	// All services work over TLS: a file, a body to the server, the token
	QVERIFY(Send("GET", "/files/hello.txt", QByteArray(), true, m_trustedNow, true, result));
	QCOMPARE(result.statusCode, 200);
	QCOMPARE(result.body, QByteArray("Hello, imtrest!\n"));

	QVERIFY(Send("POST", "/echo", "secret body", true, m_trustedNow, true, result));
	QCOMPARE(ParseJson(result.body).value("body").toString(), QString("secret body"));

	QVERIFY(Send("PUT", "/upload/tls-" + sampleId.toUtf8() + ".bin", QByteArray(300000, 'x'), true, m_trustedNow, true, result));
	QCOMPARE(result.statusCode, 201);

	QVERIFY(Send("PUT", "/upload/tls-refused.bin", "1", true, m_trustedNow, true, result, QByteArray("wrong")));
	QCOMPARE(result.statusCode, 401);
}


void CLiveTlsTest::NoTrustIsRefusedTest()
{
	QVERIFY(EnableTls(FindSample("chain")));

	// Nothing is trusted: the certificate of the server is refused, the request is not sent
	SResult result;
	QVERIFY(!Send("GET", "/echo", QByteArray(), true, {}, true, result));
	QVERIFY2(result.sslErrors.contains(QSslError::UnableToGetLocalIssuerCertificate) || result.sslErrors.contains(QSslError::UnableToVerifyFirstCertificate)
				|| result.sslErrors.contains(QSslError::SelfSignedCertificateInChain), qPrintable(result.errorText));

	// The root is trusted but it is not the root of the server: the same
	QVERIFY(!Send("GET", "/echo", QByteArray(), true, ReadTrusted(FindSample("single")), true, result));
	QVERIFY(!result.sslErrors.isEmpty());

	// The right root: it works
	QVERIFY2(Send("GET", "/echo", QByteArray(), true, m_trustedNow, true, result), qPrintable(result.errorText));
	QCOMPARE(result.statusCode, 200);
}


void CLiveTlsTest::WrongHostIsRefusedTest()
{
	QVERIFY(EnableTls(FindSample("wrong-host")));

	// The client trusts the certificate, but the name of the certificate is not the name of the host
	SResult result;
	QVERIFY(!Send("GET", "/echo", QByteArray(), true, m_trustedNow, true, result));
	QVERIFY2(result.sslErrors.contains(QSslError::HostNameMismatch), qPrintable(result.errorText));

	// The address is not in the certificate either
	QVERIFY(!Send("GET", "/echo", QByteArray(), true, m_trustedNow, true, result, QByteArray("demo-token"), QStringLiteral("127.0.0.1")));
	QVERIFY(result.sslErrors.contains(QSslError::HostNameMismatch));
}


void CLiveTlsTest::PlainHttpToTlsPortTest()
{
	QVERIFY(EnableTls(FindSample("single")));

	// A client without TLS gets no answer to its request, the server does not break and works with the client of TLS
	CRawHttpClient client;
	QVERIFY(client.Connect(GetPort()));
	QVERIFY(client.Send(CRawHttpClient::CreateRequest("GET", "/echo")));

	Response response;
	QVERIFY(!client.ReadResponse(response, false, 3000) || response.statusCode != 200);

	SResult result;
	QVERIFY2(Send("GET", "/echo", QByteArray(), true, m_trustedNow, true, result), qPrintable(result.errorText));
	QCOMPARE(result.statusCode, 200);
}


void CLiveTlsTest::SwitchBetweenSamplesTest()
{
	// The sets are changed one by one: the server restarts every time, the client trusts what the set says
	const QList<QPair<QString, int>> order = {{"single", 1}, {"chain", 2}, {"bundle", 3}, {"several", 3}, {"ec", 1}, {"single", 1}};

	int restartCount = GetSettings().value("restartCount").toInt();
	for (const QPair<QString, int>& step : order){
		QVERIFY2(EnableTls(FindSample(step.first)), qPrintable(step.first));

		SResult result;
		QVERIFY2(Send("GET", "/echo", QByteArray(), true, m_trustedNow, true, result), qPrintable(step.first + ": " + result.errorText));
		QCOMPARE(result.chainSize, step.second);

		// A certificate of the set before is not trusted for this one if they differ
		const int count = GetSettings().value("restartCount").toInt();
		QVERIFY(count > restartCount);
		restartCount = count;
	}

	// The same set again does not restart the server
	QVERIFY(EnableTls(FindSample("single")));
	QCOMPARE(GetSettings().value("restartCount").toInt(), restartCount);

	// The certificate of the previous set is not accepted for the next one
	QVERIFY(EnableTls(FindSample("chain")));
	SResult result;
	QVERIFY(!Send("GET", "/echo", QByteArray(), true, ReadTrusted(FindSample("single")), true, result));

	// Back to http: the client without TLS works again
	QJsonObject changes;
	changes.insert("tlsEnabled", false);
	QVERIFY(Send("PUT", "/admin/settings", QJsonDocument(changes).toJson(), true, m_trustedNow, true, result));
	QCOMPARE(result.statusCode, 200);
	m_isTls = false;

	QVERIFY(WaitForSettings([](const QJsonObject& settings){
		return !settings.value("tlsEnabled").toBool() && !settings.value("isRestartPending").toBool();
	}));
	QVERIFY(Send("GET", "/echo", QByteArray(), false, {}, true, result));
	QCOMPARE(result.statusCode, 200);
}


void CLiveTlsTest::BrokenFilesTest()
{
	const QJsonObject single = FindSample("single");
	const QJsonObject chain = FindSample("chain");
	const int restartCount = GetSettings().value("restartCount").toInt();

	// A missing file or a file of another kind: 400 at once, nothing changes
	QJsonObject changes = CreateTlsChanges(single);
	changes.insert("tlsCertificateFile", m_samplesDirectory.path() + "/missing.pem");

	SResult result;
	QVERIFY(Send("PUT", "/admin/settings", QJsonDocument(changes).toJson(), false, {}, true, result));
	QCOMPARE(result.statusCode, 400);

	changes = CreateTlsChanges(single);
	changes.insert("tlsKeyFile", single.value("certificateFile").toString());
	QVERIFY(Send("PUT", "/admin/settings", QJsonDocument(changes).toJson(), false, {}, true, result));
	QCOMPARE(result.statusCode, 400);

	// The files of the earlier sets stay in the settings, so the empty paths are asked directly
	QJsonObject notEnabled;
	notEnabled.insert("tlsEnabled", true);
	notEnabled.insert("tlsCertificateFile", "");
	notEnabled.insert("tlsKeyFile", "");
	QVERIFY(Send("PUT", "/admin/settings", QJsonDocument(notEnabled).toJson(), false, {}, true, result));
	QCOMPARE(result.statusCode, 400);

	QJsonObject wrongType;
	wrongType.insert("tlsChainFiles", "not a list");
	QVERIFY(Send("PUT", "/admin/settings", QJsonDocument(wrongType).toJson(), false, {}, true, result));
	QCOMPARE(result.statusCode, 400);

	QTest::qSleep(700);
	QCOMPARE(GetSettings().value("restartCount").toInt(), restartCount);

	// The files are right but the key does not belong to the certificate: the server cannot work with them, it goes back to http
	changes = CreateTlsChanges(single);
	changes.insert("tlsKeyFile", chain.value("keyFile").toString());
	QVERIFY(Send("PUT", "/admin/settings", QJsonDocument(changes).toJson(), false, {}, true, result));
	QCOMPARE(result.statusCode, 200);

	QJsonObject settings;
	QVERIFY(WaitForSettings([restartCount](const QJsonObject& current){
		return current.value("restartCount").toInt() > restartCount && !current.value("isRestartPending").toBool();
	}, &settings));

	QVERIFY(!settings.value("tlsEnabled").toBool());
	QVERIFY2(!settings.value("lastError").toString().isEmpty(), "The reason must be shown");
	QVERIFY(Send("GET", "/echo", QByteArray(), false, {}, true, result));
	QCOMPARE(result.statusCode, 200);
}


void CLiveTlsTest::LongBodiesOverTlsTest()
{
	QVERIFY(EnableTls(FindSample("bundle")));

	// A big body in both directions: the encryption does not break the streaming
	SResult result;
	QVERIFY2(Send("GET", "/stream/6000000", QByteArray(), true, m_trustedNow, true, result), qPrintable(result.errorText));
	QCOMPARE(result.statusCode, 200);
	QCOMPARE(result.body.size(), qsizetype(6000000));

	QVERIFY2(Send("PUT", "/upload/tls-big.bin", QByteArray(4000000, 'y'), true, m_trustedNow, true, result), qPrintable(result.errorText));
	QCOMPARE(result.statusCode, 201);
	QCOMPARE(ParseJson(result.body).value("size").toDouble(), 4000000.0);
}


void CLiveTlsTest::PresetTest_data()
{
	QTest::addColumn<QString>("sampleId");
	QTest::addColumn<int>("chainSize");

	QTest::newRow("one certificate") << "single" << 1;
	QTest::newRow("chain") << "chain" << 2;
	QTest::newRow("several certificates") << "several" << 3;
	QTest::newRow("concatenated certificate") << "bundle" << 3;
	QTest::newRow("EC key") << "ec" << 1;
}


void CLiveTlsTest::PresetTest()
{
	QFETCH(QString, sampleId);
	QFETCH(int, chainSize);

	// The server shows the sets: a client takes the certificates to trust from the text of the settings, it needs no access to the disk of the server
	QJsonObject preset;
	for (const QJsonValue& value : GetSettings().value("tlsSamples").toArray()){
		if (value.toObject().value("id").toString() == sampleId){
			preset = value.toObject();
		}
	}

	QVERIFY(!preset.isEmpty());
	QVERIFY(!preset.value("description").toString().isEmpty());

	QList<QSslCertificate> trusted;
	for (const QJsonValue& pem : preset.value("trustedPem").toArray()){
		trusted.append(QSslCertificate::fromData(pem.toString().toUtf8(), QSsl::Pem));
	}

	QVERIFY(!trusted.isEmpty());

	// One request with the name of the set
	QJsonObject changes;
	changes.insert("tlsPreset", sampleId);
	QVERIFY(ApplyTls(changes, trusted));

	const QJsonObject settings = GetSettings();
	QCOMPARE(settings.value("tlsCertificateFile").toString(), preset.value("certificateFile").toString());
	QCOMPARE(settings.value("tlsKeyFile").toString(), preset.value("keyFile").toString());
	QCOMPARE(settings.value("tlsChainFiles").toArray(), preset.value("chainFiles").toArray());

	SResult result;
	QVERIFY2(Send("GET", "/echo", QByteArray(), true, m_trustedNow, true, result), qPrintable(result.errorText));
	QCOMPARE(result.chainSize, chainSize);

	// The presets are on the disk of the server, nothing is kept in the storage of the clients
	QCOMPARE(CountStoredDirectories(), 0);
}


void CLiveTlsTest::UnknownPresetTest()
{
	const int restartCount = GetSettings().value("restartCount").toInt();

	for (const QJsonValue& value : {QJsonValue("no-such-set"), QJsonValue(5), QJsonValue(QString()), QJsonValue(QString(100, 'x'))}){
		QJsonObject changes;
		changes.insert("tlsPreset", value);

		SResult result;
		QVERIFY(PutSettings(changes, result));
		QCOMPARE(result.statusCode, 400);
	}

	// A set and the certificates themselves in one request are not allowed
	QJsonObject both;
	both.insert("tlsPreset", "single");
	both.insert("tlsCertificatePem", ReadText(FindSample("single").value("certificateFile").toString()));
	both.insert("tlsKeyPem", ReadText(FindSample("single").value("keyFile").toString()));

	SResult result;
	QVERIFY(PutSettings(both, result));
	QCOMPARE(result.statusCode, 400);

	QTest::qSleep(600);
	QCOMPARE(GetSettings().value("restartCount").toInt(), restartCount);
	QCOMPARE(CountStoredDirectories(), 0);
}


void CLiveTlsTest::UploadPemTest_data()
{
	QTest::addColumn<QString>("sampleId");
	QTest::addColumn<int>("chainSize");

	QTest::newRow("one certificate") << "single" << 1;
	QTest::newRow("chain") << "chain" << 2;
	QTest::newRow("several certificates") << "several" << 3;
	QTest::newRow("concatenated certificate") << "bundle" << 3;
	QTest::newRow("EC key") << "ec" << 1;
}


void CLiveTlsTest::UploadPemTest()
{
	QFETCH(QString, sampleId);
	QFETCH(int, chainSize);

	// The client sends the content of its files, the server keeps it in its own directory and uses it
	const QJsonObject sample = FindSample(sampleId);
	const QString certificatePem = ReadText(sample.value("certificateFile").toString());
	const QString keyPem = ReadText(sample.value("keyFile").toString());
	QVERIFY(!certificatePem.isEmpty() && keyPem.contains("PRIVATE KEY"));

	QJsonArray chainPem;
	for (const QJsonValue& chainFile : sample.value("chainFiles").toArray()){
		chainPem.append(ReadText(chainFile.toString()));
	}

	QJsonObject changes;
	changes.insert("tlsCertificatePem", certificatePem);
	changes.insert("tlsChainPem", chainPem);
	changes.insert("tlsKeyPem", keyPem);
	QVERIFY(ApplyTls(changes, ReadTrusted(sample)));

	SResult result;
	QVERIFY2(Send("GET", "/echo", QByteArray(), true, m_trustedNow, true, result), qPrintable(result.errorText));
	QCOMPARE(result.chainSize, chainSize);

	// The files are in the storage of the server, not in the directory that the server gives to everybody, and only the owner can read the key
	const QJsonObject settings = GetSettings();
	const QString certificateFile = settings.value("tlsCertificateFile").toString();
	const QString keyFile = settings.value("tlsKeyFile").toString();
	QVERIFY2(QFileInfo(certificateFile).absoluteFilePath().startsWith(QDir(GetStorageDirectory()).absolutePath() + "/"), qPrintable(certificateFile));
	QVERIFY(!QFileInfo(certificateFile).absoluteFilePath().startsWith(QDir(GetFilesDirectory()).absolutePath() + "/"));
	QCOMPARE(ReadText(certificateFile), certificatePem);
	QCOMPARE(ReadText(keyFile), keyPem);
	QCOMPARE(QFileInfo(keyFile).permissions() & (QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ReadOther | QFileDevice::WriteOther), QFileDevice::Permissions());
	QCOMPARE(settings.value("tlsChainFiles").toArray().size(), chainPem.size());

	// The key is never in the answer of the server
	QVERIFY(!QJsonDocument(settings).toJson().contains("PRIVATE KEY"));
	QVERIFY(!QJsonDocument(settings).toJson().contains("tlsKeyPem"));

	// The certificates of the files that a client sends are served as the other ones: the same files from the server are not reachable by the paths
	QVERIFY(Send("GET", "/files/../" + QFileInfo(GetStorageDirectory()).fileName() + "/server.key", QByteArray(), true, m_trustedNow, true, result));
	QVERIFY(result.statusCode != 200);
}


void CLiveTlsTest::StoredFilesAreCleanedTest()
{
	// The files that the server uses stay while its settings refer to them (an earlier test could leave its files)
	const QStringList order = {"bundle", "chain", "single", "several"};
	for (const QString& sampleId : order){
		const QJsonObject sample = FindSample(sampleId);

		QJsonArray chainPem;
		for (const QJsonValue& chainFile : sample.value("chainFiles").toArray()){
			chainPem.append(ReadText(chainFile.toString()));
		}

		QJsonObject changes;
		changes.insert("tlsCertificatePem", ReadText(sample.value("certificateFile").toString()));
		changes.insert("tlsChainPem", chainPem);
		changes.insert("tlsKeyPem", ReadText(sample.value("keyFile").toString()));
		QVERIFY(ApplyTls(changes, ReadTrusted(sample)));

		// Only the files that the server uses are kept
		QTest::qSleep(100);
		QCOMPARE(CountStoredDirectories(), 1);
	}

	// A set by the name: the files of the clients are not needed any more
	QJsonObject changes;
	changes.insert("tlsPreset", "single");
	QVERIFY(ApplyTls(changes, ReadTrusted(FindSample("single"))));
	QTest::qSleep(100);
	QCOMPARE(CountStoredDirectories(), 0);
}


void CLiveTlsTest::UploadBrokenPemTest()
{
	const QJsonObject single = FindSample("single");
	const QJsonObject chain = FindSample("chain");
	const QString certificate = ReadText(single.value("certificateFile").toString());
	const QString key = ReadText(single.value("keyFile").toString());
	const int restartCount = GetSettings().value("restartCount").toInt();
	const int storedBefore = CountStoredDirectories();

	auto put = [this](const QJsonObject& changes){
		SResult result;
		const bool isAnswered = PutSettings(changes, result);

		return isAnswered ? result.statusCode : 0;
	};

	QJsonObject changes;

	// The key is not sent, or the certificate is not sent
	changes.insert("tlsCertificatePem", certificate);
	QCOMPARE(put(changes), 400);

	changes = QJsonObject();
	changes.insert("tlsKeyPem", key);
	QCOMPARE(put(changes), 400);

	// The files that are not what they are called
	changes = QJsonObject();
	changes.insert("tlsCertificatePem", key);
	changes.insert("tlsKeyPem", key);
	QCOMPARE(put(changes), 400);

	changes = QJsonObject();
	changes.insert("tlsCertificatePem", certificate);
	changes.insert("tlsKeyPem", certificate);
	QCOMPARE(put(changes), 400);

	changes = QJsonObject();
	changes.insert("tlsCertificatePem", "this is not a certificate");
	changes.insert("tlsKeyPem", key);
	QCOMPARE(put(changes), 400);

	changes = QJsonObject();
	changes.insert("tlsCertificatePem", certificate);
	changes.insert("tlsKeyPem", key);
	changes.insert("tlsChainPem", QJsonArray({"garbage"}));
	QCOMPARE(put(changes), 400);

	changes = QJsonObject();
	changes.insert("tlsCertificatePem", certificate);
	changes.insert("tlsKeyPem", key);
	changes.insert("tlsChainPem", "not a list");
	QCOMPARE(put(changes), 400);

	changes = QJsonObject();
	changes.insert("tlsCertificatePem", 5);
	changes.insert("tlsKeyPem", key);
	QCOMPARE(put(changes), 400);

	// Too big
	changes = QJsonObject();
	changes.insert("tlsCertificatePem", certificate + QString(70000, '\n'));
	changes.insert("tlsKeyPem", key);
	QCOMPARE(put(changes), 400);

	// Nothing is changed, nothing is kept
	QTest::qSleep(600);
	QCOMPARE(GetSettings().value("restartCount").toInt(), restartCount);
	QCOMPARE(CountStoredDirectories(), storedBefore);
	QVERIFY(!GetSettings().value("tlsEnabled").toBool());

	// The files are right but the key is not of this certificate: the server goes back to http, the reason is shown, the files are removed
	changes = QJsonObject();
	changes.insert("tlsCertificatePem", certificate);
	changes.insert("tlsKeyPem", ReadText(chain.value("keyFile").toString()));
	QCOMPARE(put(changes), 200);

	QJsonObject settings;
	QVERIFY(WaitForSettings([restartCount](const QJsonObject& current){
		return current.value("restartCount").toInt() > restartCount && !current.value("isRestartPending").toBool();
	}, &settings));

	QVERIFY(!settings.value("tlsEnabled").toBool());
	QVERIFY(!settings.value("lastError").toString().isEmpty());
	QCOMPARE(CountStoredDirectories(), storedBefore);

	SResult result;
	QVERIFY(Send("GET", "/echo", QByteArray(), false, {}, true, result));
	QCOMPARE(result.statusCode, 200);
}


// private methods

bool CLiveTlsTest::Send(
			const QByteArray& method,
			const QString& path,
			const QByteArray& body,
			bool isTls,
			const QList<QSslCertificate>& trusted,
			bool isVerified,
			SResult& result,
			const QByteArray& token,
			const QString& host) const
{
	result = SResult();

	QNetworkAccessManager manager;
	QNetworkRequest request(QUrl(QStringLiteral("%1://%2:%3%4").arg(isTls ? "https" : "http", host).arg(GetPort()).arg(path)));
	// A poll of the settings that hits the restarting server can be dropped without a notice, it is repeated by the caller
	request.setTransferTimeout(method == "GET" && path == QLatin1String("/admin/settings") ? 3000 : 20000);
	request.setRawHeader("Authorization", "Bearer " + token);
	if (!body.isEmpty()){
		request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
	}

	if (isTls){
		// Only the certificates of the test are trusted
		QSslConfiguration configuration = QSslConfiguration::defaultConfiguration();
		configuration.setCaCertificates(trusted);
		configuration.setPeerVerifyMode(isVerified ? QSslSocket::VerifyPeer : QSslSocket::VerifyNone);
		request.setSslConfiguration(configuration);
	}

	QNetworkReply* replyPtr = manager.sendCustomRequest(request, method, body);

	QObject::connect(replyPtr, &QNetworkReply::sslErrors, replyPtr, [&result, isVerified, replyPtr](const QList<QSslError>& errors){
		for (const QSslError& error : errors){
			result.sslErrors.append(error.error());
			result.errorText += error.errorString() + "; ";
		}

		// An unverified request (the return of the server to http) goes on
		if (!isVerified){
			replyPtr->ignoreSslErrors();
		}
	});

	QEventLoop loop;
	QObject::connect(replyPtr, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	QTimer::singleShot(25000, &loop, &QEventLoop::quit);
	loop.exec();

	if (!replyPtr->isFinished()){
		replyPtr->abort();
	}

	result.statusCode = replyPtr->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	result.body = replyPtr->readAll();

	const QSslConfiguration used = replyPtr->sslConfiguration();
	result.chainSize = used.peerCertificateChain().size();
	if (!used.sessionCipher().isNull()){
		result.protocol = used.sessionCipher().protocolString();
	}

	if (result.statusCode == 0){
		result.errorText += replyPtr->errorString();
	}

	replyPtr->deleteLater();

	return result.statusCode > 0;
}


QJsonObject CLiveTlsTest::GetSettings() const
{
	SResult result;
	if (!Send("GET", "/admin/settings", QByteArray(), m_isTls, m_trustedNow, m_isVerified, result) || result.statusCode != 200){
		return QJsonObject();
	}

	return ParseJson(result.body);
}


bool CLiveTlsTest::PutSettings(const QJsonObject& changes, SResult& result) const
{
	return Send("PUT", "/admin/settings", QJsonDocument(changes).toJson(), m_isTls, m_trustedNow, m_isVerified, result);
}


bool CLiveTlsTest::WaitForSettings(const std::function<bool(const QJsonObject&)>& isReady, QJsonObject* settingsPtr, int timeoutMs) const
{
	QElapsedTimer timer;
	timer.start();

	while (timer.elapsed() < timeoutMs){
		const QJsonObject settings = GetSettings();
		if (!settings.isEmpty() && isReady(settings)){
			if (settingsPtr != nullptr){
				*settingsPtr = settings;
			}

			return true;
		}

		QTest::qSleep(50);
	}

	return false;
}


bool CLiveTlsTest::EnableTls(const QJsonObject& sample)
{
	// The certificate for another host is refused by the client, the settings are read without the check
	return ApplyTls(CreateTlsChanges(sample), ReadTrusted(sample), sample.value("id").toString() != QStringLiteral("wrong-host"));
}


bool CLiveTlsTest::ApplyTls(const QJsonObject& changes, const QList<QSslCertificate>& trusted, bool isVerified)
{
	const QJsonObject before = GetSettings();
	const int restartCount = before.value("restartCount").toInt();
	const bool isChanged = !m_isTls || before.value("tlsCertificateFile").toString() != changes.value("tlsCertificateFile").toString()
				|| before.value("tlsKeyFile").toString() != changes.value("tlsKeyFile").toString() || !changes.contains("tlsCertificateFile");

	SResult result;
	if (!PutSettings(changes, result) || result.statusCode != 200){
		return false;
	}

	// After the restart the client works with the new certificates (the old ones are not accepted any more)
	const QList<QSslCertificate> previousTrusted = m_trustedNow;
	m_trustedNow = trusted;
	m_isVerified = isVerified;
	const bool wasTls = m_isTls;
	m_isTls = true;

	QJsonObject settings;
	const bool isReady = WaitForSettings([restartCount, isChanged](const QJsonObject& current){
		return current.value("tlsEnabled").toBool() && !current.value("isRestartPending").toBool()
					&& (!isChanged || current.value("restartCount").toInt() > restartCount);
	}, &settings);

	if (!isReady){
		m_isTls = wasTls;
		m_trustedNow = previousTrusted;
	}

	return isReady && settings.value("lastError").toString().isEmpty();
}


QString CLiveTlsTest::GetStorageDirectory() const
{
	return m_samplesDirectory.path() + "/stored";
}


int CLiveTlsTest::CountStoredDirectories() const
{
	return QDir(GetStorageDirectory()).entryList(QDir::Dirs | QDir::NoDotAndDotDot).size();
}


QString CLiveTlsTest::ReadText(const QString& path)
{
	QFile file(path);

	return file.open(QIODevice::ReadOnly) ? QString::fromUtf8(file.readAll()) : QString();
}


QJsonObject CLiveTlsTest::FindSample(const QString& id) const
{
	for (const QJsonValue& value : m_samples){
		if (value.toObject().value("id").toString() == id){
			return value.toObject();
		}
	}

	return QJsonObject();
}


QList<QSslCertificate> CLiveTlsTest::ReadTrusted(const QJsonObject& sample)
{
	QList<QSslCertificate> retVal;

	for (const QJsonValue& file : sample.value("trustedFiles").toArray()){
		QString errorText;
		retVal.append(DemoTls::ReadCertificates(file.toString(), errorText));
	}

	return retVal;
}


QJsonObject CLiveTlsTest::CreateTlsChanges(const QJsonObject& sample)
{
	QJsonObject changes;
	changes.insert("tlsEnabled", true);
	changes.insert("tlsCertificateFile", sample.value("certificateFile"));
	changes.insert("tlsChainFiles", sample.value("chainFiles"));
	changes.insert("tlsKeyFile", sample.value("keyFile"));

	return changes;
}


I_ADD_TEST(CLiveTlsTest);

