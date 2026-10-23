// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/DemoTlsTools.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonObject>
#include <QtCore/QProcess>
#include <QtCore/QStandardPaths>
#include <QtCore/QUuid>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslSocket>

#ifdef Q_OS_MACOS
#include <mach-o/dyld.h>
#include <unistd.h>
#endif


namespace imtrestdemo
{


namespace DemoTlsTools
{


const char* const s_singleSampleId = "single";
const char* const s_chainSampleId = "chain";
const char* const s_severalSampleId = "several";
const char* const s_bundleSampleId = "bundle";
const char* const s_ecSampleId = "ec";
const char* const s_wrongHostSampleId = "wrong-host";


namespace
{


const QString s_leafExtensions = QStringLiteral(
			"basicConstraints=CA:FALSE\n"
			"keyUsage=digitalSignature,keyEncipherment\n"
			"extendedKeyUsage=serverAuth\n"
			"subjectAltName=DNS:localhost,IP:127.0.0.1,IP:::1\n");

const QString s_authorityExtensions = QStringLiteral(
			"basicConstraints=critical,CA:TRUE\n"
			"keyUsage=critical,keyCertSign,cRLSign\n");

// Certificates of the servers are valid for a year: the system of macOS does not like longer ones
const int s_leafDays = 365;
const int s_authorityDays = 3650;


QByteArray ReadFile(const QString& path)
{
	QFile file(path);

	return file.open(QIODevice::ReadOnly) ? file.readAll() : QByteArray();
}


bool WriteFile(const QString& path, const QByteArray& data)
{
	QFile file(path);

	return file.open(QIODevice::WriteOnly | QIODevice::Truncate) && file.write(data) == data.size();
}


/**
	Runner of the program openssl: remembers the first failure.
*/
class COpenssl
{
public:
	COpenssl(const QString& program, const QString& workDirectory, QString& errorText)
		:m_program(program), m_workDirectory(workDirectory), m_errorText(errorText)
	{
	}

	bool Run(const QStringList& arguments)
	{
		if (!m_errorText.isEmpty()){
			return false;
		}

		QProcess process;
		process.setWorkingDirectory(m_workDirectory);
		process.setProcessChannelMode(QProcess::MergedChannels);
		process.start(m_program, arguments);

		if (!process.waitForStarted(10000) || !process.waitForFinished(60000) || process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0){
			m_errorText = QStringLiteral("openssl %1 failed: %2").arg(arguments.mid(0, 2).join(' '), QString::fromLocal8Bit(process.readAll()).trimmed());

			return false;
		}

		return true;
	}

	// A key of RSA 2048 in the form that every TLS library reads (PKCS#1 is accepted by all of them)
	bool CreateRsaKey(const QString& path)
	{
		if (!m_errorText.isEmpty()){
			return false;
		}

		return Run({"genrsa", "-traditional", "-out", path, "2048"}) || Retry({"genrsa", "-out", path, "2048"});
	}

	bool CreateEcKey(const QString& path)
	{
		return Run({"ecparam", "-name", "prime256v1", "-genkey", "-noout", "-out", path});
	}

	bool CreateSelfSigned(const QString& keyPath, const QString& certPath, const QString& commonName, const QString& dnsName, bool isLocalAddress = true)
	{
		const QString alternativeNames = isLocalAddress ? "DNS:" + dnsName + ",IP:127.0.0.1,IP:::1" : "DNS:" + dnsName;

		return Run({"req", "-x509", "-new", "-key", keyPath, "-out", certPath, "-days", QString::number(s_leafDays), "-subj", "/CN=" + commonName, "-addext", "basicConstraints=CA:FALSE", "-addext", "subjectAltName=" + alternativeNames});
	}

	bool CreateAuthority(const QString& keyPath, const QString& certPath, const QString& commonName, const QString& issuerCertPath, const QString& issuerKeyPath)
	{
		if (issuerCertPath.isEmpty()){
			return CreateRsaKey(keyPath)
						&& Run({"req", "-x509", "-new", "-key", keyPath, "-out", certPath, "-days", QString::number(s_authorityDays), "-subj", "/CN=" + commonName,
							"-addext", "basicConstraints=critical,CA:TRUE", "-addext", "keyUsage=critical,keyCertSign,cRLSign"});
		}

		return CreateRsaKey(keyPath) && Sign(keyPath, certPath, commonName, issuerCertPath, issuerKeyPath, s_authorityExtensions, s_authorityDays);
	}

	bool CreateLeaf(const QString& keyPath, const QString& certPath, const QString& commonName, const QString& issuerCertPath, const QString& issuerKeyPath)
	{
		return CreateRsaKey(keyPath) && Sign(keyPath, certPath, commonName, issuerCertPath, issuerKeyPath, s_leafExtensions, s_leafDays);
	}

private:
	bool Retry(const QStringList& arguments)
	{
		// The first try has failed and has set the error: the program may not know the option, the second try is without it
		m_errorText.clear();

		return Run(arguments);
	}

	bool Sign(const QString& keyPath, const QString& certPath, const QString& commonName, const QString& issuerCertPath, const QString& issuerKeyPath, const QString& extensions, int days)
	{
		const QString requestPath = certPath + ".csr";
		const QString extensionsPath = certPath + ".cnf";
		if (!WriteFile(extensionsPath, extensions.toUtf8())){
			m_errorText = QStringLiteral("The file %1 cannot be written").arg(extensionsPath);

			return false;
		}

		const bool isDone = Run({"req", "-new", "-key", keyPath, "-out", requestPath, "-subj", "/CN=" + commonName})
					&& Run({"x509", "-req", "-in", requestPath, "-CA", issuerCertPath, "-CAkey", issuerKeyPath, "-CAcreateserial", "-out", certPath, "-days", QString::number(days), "-extfile", extensionsPath});

		QFile::remove(requestPath);
		QFile::remove(extensionsPath);

		return isDone;
	}

private:
	QString m_program;
	QString m_workDirectory;
	QString& m_errorText;
};


struct SSampleInfo
{
	const char* id;
	const char* title;
	const char* description;
	const char* certificateFile;
	QStringList chainFiles;
	const char* keyFile;
	QStringList trustedFiles;
	int certificatesInChain;
};


QList<SSampleInfo> GetSampleInfos()
{
	return {
		{s_singleSampleId, "One certificate",
			"One self-signed certificate. The client must trust this certificate itself.",
			"single/server.pem", {}, "single/server.key", {"single/server.pem"}, 1},
		{s_chainSampleId, "Chain: certificate and one intermediate",
			"The certificate of the server is signed by an intermediate authority, which is signed by the root. The intermediate certificate is a separate file "
			"(a separate parameter of the server). The client trusts only the root and needs the intermediate certificate from the server.",
			"chain/server.pem", {"chain/intermediate.pem"}, "chain/server.key", {"ca/root.pem"}, 2},
		{s_severalSampleId, "Several certificates in separate files",
			"The certificate of the server and two intermediate certificates (root, intermediate 1, intermediate 2, server), every one in its own file. "
			"The client trusts only the root.",
			"several/server.pem", {"several/intermediate2.pem", "several/intermediate1.pem"}, "several/server.key", {"ca/root.pem"}, 3},
		{s_bundleSampleId, "Concatenated certificate (fullchain)",
			"One file with the certificate of the server followed by both intermediate certificates (fullchain.pem). The client trusts only the root.",
			"bundle/fullchain.pem", {}, "bundle/server.key", {"ca/root.pem"}, 3},
		{s_ecSampleId, "Self-signed certificate with an EC key",
			"One self-signed certificate with a key on the curve prime256v1.",
			"ec/server.pem", {}, "ec/server.key", {"ec/server.pem"}, 1},
		{s_wrongHostSampleId, "Certificate for another host",
			"A self-signed certificate for other.example: the client trusts it, but the name does not match the host of the URL, so the client refuses it.",
			"wrong-host/server.pem", {}, "wrong-host/server.key", {"wrong-host/server.pem"}, 1}};
}


} // namespace


void PrepareOpensslBackend(char* argv[])
{
#ifdef Q_OS_MACOS
	const char* const restartedName = "IMTREST_TLS_RESTARTED";
	if (qEnvironmentVariableIsSet(restartedName)){
		return;
	}

	QString libraryDirectory = qEnvironmentVariable("IMTREST_OPENSSL_LIB_DIR");
	if (libraryDirectory.isEmpty()){
		const QString program = FindOpenssl();
		if (program.isEmpty()){
			return;
		}

		// The library is in the directory lib of the installation of the program (Homebrew: Cellar/openssl@3/<version>/bin/openssl)
		QDir directory(QFileInfo(QFileInfo(program).canonicalFilePath()).dir());
		if (!directory.cdUp()){
			return;
		}

		libraryDirectory = directory.filePath(QStringLiteral("lib"));
	}

	if (!QFileInfo::exists(libraryDirectory + QStringLiteral("/libssl.3.dylib"))){
		return;
	}

	const QByteArray current = qgetenv("DYLD_FALLBACK_LIBRARY_PATH");
	QByteArray paths = libraryDirectory.toLocal8Bit();
	if (!current.isEmpty()){
		paths += ':' + current;
	}

	char executable[4096];
	uint32_t size = sizeof(executable);
	if (_NSGetExecutablePath(executable, &size) != 0){
		return;
	}

	qputenv("DYLD_FALLBACK_LIBRARY_PATH", paths);
	qputenv(restartedName, "1");

	// Does not return if the program is started again
	execv(executable, argv);
#else
	Q_UNUSED(argv);
#endif
}


bool UseOpensslBackend(QString& errorText)
{
	const QString backend = QStringLiteral("openssl");

	if (QSslSocket::activeBackend() == backend || (QSslSocket::availableBackends().contains(backend) && QSslSocket::setActiveBackend(backend))){
		return true;
	}

	errorText = QStringLiteral("The OpenSSL backend of Qt TLS is not available (the active one is '%1'): the library of OpenSSL is not found, see "
				"IMTREST_OPENSSL_LIB_DIR. The other backends can change the keychain of the system, so TLS is not used").arg(QSslSocket::activeBackend());

	return false;
}


QString FindOpenssl(const QString& explicitPath)
{
	if (!explicitPath.isEmpty()){
		return QFileInfo(explicitPath).isExecutable() ? explicitPath : QString();
	}

	const QString fromEnvironment = qEnvironmentVariable("IMTREST_OPENSSL");
	if (!fromEnvironment.isEmpty()){
		return QFileInfo(fromEnvironment).isExecutable() ? fromEnvironment : QString();
	}

	return QStandardPaths::findExecutable(QStringLiteral("openssl"));
}


QList<QSslCertificate> ReadCertificates(const QString& path, QString& errorText)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)){
		errorText = QStringLiteral("The file '%1' cannot be read: %2").arg(path, file.errorString());

		return {};
	}

	const QList<QSslCertificate> certificates = QSslCertificate::fromData(file.readAll(), QSsl::Pem);
	if (certificates.isEmpty()){
		errorText = QStringLiteral("The file '%1' has no PEM certificates").arg(path);
	}

	return certificates;
}


bool ReadPrivateKeyAlgorithm(const QString& path, QSsl::KeyAlgorithm& algorithm, QString& errorText)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)){
		errorText = QStringLiteral("The file '%1' cannot be read: %2").arg(path, file.errorString());

		return false;
	}

	const QByteArray data = file.readAll();
	for (QSsl::KeyAlgorithm candidate : {QSsl::Rsa, QSsl::Ec, QSsl::Dsa}){
		const QSslKey key(data, candidate, QSsl::Pem, QSsl::PrivateKey);
		if (!key.isNull()){
			algorithm = candidate;

			return true;
		}
	}

	errorText = QStringLiteral("The file '%1' has no PEM private key (RSA, EC or DSA, without a password)").arg(path);

	return false;
}


bool ValidateServerFiles(const QString& certificateFile, const QStringList& chainFiles, const QString& keyFile, QString& errorText)
{
	if (certificateFile.isEmpty() || keyFile.isEmpty()){
		errorText = QStringLiteral("TLS needs the certificate file and the key file");

		return false;
	}

	if (ReadCertificates(certificateFile, errorText).isEmpty()){
		return false;
	}

	for (const QString& chainFile : chainFiles){
		if (ReadCertificates(chainFile, errorText).isEmpty()){
			return false;
		}
	}

	QSsl::KeyAlgorithm algorithm = QSsl::Rsa;

	return ReadPrivateKeyAlgorithm(keyFile, algorithm, errorText);
}


bool GenerateSamples(const QString& directory, const QString& opensslPath, QString& errorText)
{
	errorText.clear();

	const QString program = FindOpenssl(opensslPath);
	if (program.isEmpty()){
		errorText = QStringLiteral("The program openssl is not found (the argument --openssl, the variable IMTREST_OPENSSL or PATH)");

		return false;
	}

	const QDir root(QDir(directory).absolutePath());
	for (const QString& name : {"ca", "single", "chain", "several", "bundle", "ec", "wrong-host"}){
		if (!root.mkpath(name)){
			errorText = QStringLiteral("The directory '%1' cannot be created").arg(root.filePath(name));

			return false;
		}
	}

	auto path = [&root](const QString& relative){
		return root.filePath(relative);
	};

	COpenssl openssl(program, root.absolutePath(), errorText);

	// The authorities: root, intermediate 1 (signed by the root), intermediate 2 (signed by the intermediate 1)
	openssl.CreateAuthority(path("ca/root.key"), path("ca/root.pem"), "imtrest-demo-root", QString(), QString());
	openssl.CreateAuthority(path("ca/intermediate1.key"), path("ca/intermediate1.pem"), "imtrest-demo-intermediate-1", path("ca/root.pem"), path("ca/root.key"));
	openssl.CreateAuthority(path("ca/intermediate2.key"), path("ca/intermediate2.pem"), "imtrest-demo-intermediate-2", path("ca/intermediate1.pem"), path("ca/intermediate1.key"));

	// One certificate
	openssl.CreateRsaKey(path("single/server.key"));
	openssl.CreateSelfSigned(path("single/server.key"), path("single/server.pem"), "localhost", "localhost");

	// Chain: server <- intermediate 1 <- root
	openssl.CreateLeaf(path("chain/server.key"), path("chain/server.pem"), "localhost", path("ca/intermediate1.pem"), path("ca/intermediate1.key"));

	// Several: server <- intermediate 2 <- intermediate 1 <- root
	openssl.CreateLeaf(path("several/server.key"), path("several/server.pem"), "localhost", path("ca/intermediate2.pem"), path("ca/intermediate2.key"));

	// Bundle: the same leaf as in the other set but in one file
	openssl.CreateLeaf(path("bundle/server.key"), path("bundle/leaf.pem"), "localhost", path("ca/intermediate2.pem"), path("ca/intermediate2.key"));

	openssl.CreateEcKey(path("ec/server.key"));
	openssl.CreateSelfSigned(path("ec/server.key"), path("ec/server.pem"), "localhost", "localhost");

	openssl.CreateRsaKey(path("wrong-host/server.key"));
	openssl.CreateSelfSigned(path("wrong-host/server.key"), path("wrong-host/server.pem"), "other.example", "other.example", false);

	if (!errorText.isEmpty()){
		return false;
	}

	// The files of the chains are the copies of the authorities' ones, the bundle is the concatenation
	const QByteArray intermediate1 = ReadFile(path("ca/intermediate1.pem"));
	const QByteArray intermediate2 = ReadFile(path("ca/intermediate2.pem"));
	const bool isDone = WriteFile(path("chain/intermediate.pem"), intermediate1)
				&& WriteFile(path("several/intermediate1.pem"), intermediate1)
				&& WriteFile(path("several/intermediate2.pem"), intermediate2)
				&& WriteFile(path("bundle/fullchain.pem"), ReadFile(path("bundle/leaf.pem")) + intermediate2 + intermediate1);

	QFile::remove(path("bundle/leaf.pem"));

	if (!isDone || intermediate1.isEmpty() || intermediate2.isEmpty()){
		errorText = QStringLiteral("The files of the chains cannot be written to '%1'").arg(root.absolutePath());

		return false;
	}

	return true;
}


namespace
{


const int s_maxPemSize = 65536;


bool IsPrivateKeyPem(const QByteArray& data)
{
	for (QSsl::KeyAlgorithm candidate : {QSsl::Rsa, QSsl::Ec, QSsl::Dsa}){
		if (!QSslKey(data, candidate, QSsl::Pem, QSsl::PrivateKey).isNull()){
			return true;
		}
	}

	return false;
}


bool CheckCertificatePem(const QString& name, const QString& pem, QString& errorText)
{
	if (pem.isEmpty() || pem.size() > s_maxPemSize){
		errorText = QStringLiteral("%1 must be a PEM text of up to %2 bytes").arg(name).arg(s_maxPemSize);

		return false;
	}

	if (pem.contains(QLatin1String("PRIVATE KEY"))){
		errorText = QStringLiteral("%1 has a private key: the key is sent in its own parameter").arg(name);

		return false;
	}

	if (QSslCertificate::fromData(pem.toUtf8(), QSsl::Pem).isEmpty()){
		errorText = QStringLiteral("%1 has no PEM certificates").arg(name);

		return false;
	}

	return true;
}


} // namespace


bool StorePem(
			const QString& storageDirectory,
			const QString& certificatePem,
			const QStringList& chainPem,
			const QString& keyPem,
			QString& certificateFile,
			QStringList& chainFiles,
			QString& keyFile,
			QString& errorText)
{
	if (storageDirectory.isEmpty()){
		errorText = QStringLiteral("The directory for the certificates of the clients is not set");

		return false;
	}

	if (!CheckCertificatePem(QStringLiteral("The certificate"), certificatePem, errorText)){
		return false;
	}

	if (chainPem.size() > 16){
		errorText = QStringLiteral("There can be up to 16 intermediate certificates");

		return false;
	}

	for (int i = 0; i < chainPem.size(); ++i){
		if (!CheckCertificatePem(QStringLiteral("The intermediate certificate %1").arg(i + 1), chainPem.at(i), errorText)){
			return false;
		}
	}

	if (keyPem.isEmpty() || keyPem.size() > s_maxPemSize || !IsPrivateKeyPem(keyPem.toUtf8())){
		errorText = QStringLiteral("The key must be a PEM private key (RSA, EC or DSA, without a password)");

		return false;
	}

	const QDir storage(QDir::cleanPath(QDir(storageDirectory).absolutePath()));
	const QString directoryName = QUuid::createUuid().toString(QUuid::Id128);
	if (!storage.mkpath(directoryName)){
		errorText = QStringLiteral("The directory '%1' cannot be created").arg(storage.filePath(directoryName));

		return false;
	}

	const QString directory = storage.filePath(directoryName);
	QFile::setPermissions(directory, QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);

	auto write = [&directory](const QString& name, const QString& content, QString& path){
		path = QDir(directory).filePath(name);
		if (!WriteFile(path, content.toUtf8())){
			return false;
		}

		return QFile::setPermissions(path, QFileDevice::ReadOwner | QFileDevice::WriteOwner);
	};

	bool isDone = write(QStringLiteral("server.pem"), certificatePem, certificateFile) && write(QStringLiteral("server.key"), keyPem, keyFile);

	chainFiles.clear();
	for (int i = 0; isDone && i < chainPem.size(); ++i){
		QString path;
		isDone = write(QStringLiteral("intermediate-%1.pem").arg(i + 1), chainPem.at(i), path);
		chainFiles.append(path);
	}

	if (!isDone){
		QDir(directory).removeRecursively();
		errorText = QStringLiteral("The files cannot be written to '%1'").arg(directory);

		return false;
	}

	return true;
}


void RemoveUnusedStoredFiles(const QString& storageDirectory, const QStringList& keptFiles)
{
	if (storageDirectory.isEmpty()){
		return;
	}

	const QDir storage(QDir::cleanPath(QDir(storageDirectory).absolutePath()));

	for (const QFileInfo& entry : storage.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot)){
		bool isUsed = false;
		for (const QString& keptFile : keptFiles){
			isUsed = isUsed || QFileInfo(keptFile).absolutePath() == entry.absoluteFilePath();
		}

		if (!isUsed){
			QDir(entry.absoluteFilePath()).removeRecursively();
		}
	}
}


QJsonArray DescribeSamples(const QString& directory)
{
	QJsonArray retVal;

	const QDir root(QDir(directory).absolutePath());

	for (const SSampleInfo& info : GetSampleInfos()){
		if (!QFileInfo::exists(root.filePath(QString::fromLatin1(info.certificateFile)))){
			continue;
		}

		auto toPaths = [&root](const QStringList& relativePaths){
			QJsonArray paths;
			for (const QString& relativePath : relativePaths){
				paths.append(root.filePath(relativePath));
			}

			return paths;
		};

		QJsonObject sample;
		sample.insert("id", QString::fromLatin1(info.id));
		sample.insert("title", QString::fromLatin1(info.title));
		sample.insert("description", QString::fromLatin1(info.description));
		sample.insert("certificateFile", root.filePath(QString::fromLatin1(info.certificateFile)));
		sample.insert("chainFiles", toPaths(info.chainFiles));
		sample.insert("keyFile", root.filePath(QString::fromLatin1(info.keyFile)));
		sample.insert("trustedFiles", toPaths(info.trustedFiles));

		QJsonArray trustedPem;
		for (const QString& relativePath : info.trustedFiles){
			trustedPem.append(QString::fromUtf8(ReadFile(root.filePath(relativePath))));
		}

		sample.insert("trustedPem", trustedPem);
		sample.insert("certificatesInChain", info.certificatesInChain);
		retVal.append(sample);
	}

	return retVal;
}


} // namespace DemoTlsTools


} // namespace imtrestdemo

