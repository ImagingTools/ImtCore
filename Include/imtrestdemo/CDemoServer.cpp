// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoServer.h>


// STL includes
#include <cmath>

// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QMetaObject>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QObject>
#include <QtCore/QRegularExpression>
#include <QtCore/QTimer>
#include <QtCore/QEventLoop>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QSslSocket>
#include <QtNetwork/QTcpServer>

// ACF includes
#include <iattr/TAttribute.h>
#include <icomp/CRegistryElement.h>
#include <ifile/IFileNameParam.h>
#include <iprm/IOptionsList.h>
#include <iprm/IParamsManager.h>
#include <iprm/IParamsSet.h>
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtcom/ISslConfigurationManager.h>
#include <imtrest/IServer.h>
#include <imtrestdemo/CDemoProtocol.h>
#include <imtrestdemo/DemoServletTools.h>
#include <imtrestdemo/DemoTlsTools.h>
#include <imtrestdemo/IDemoSettingsController.h>



namespace imtrestdemo
{


namespace
{


int FindFreePort()
{
	QTcpServer probe;
	if (!probe.listen(QHostAddress::Any, 0)){
		return 0;
	}

	const int port = probe.serverPort();
	probe.close();

	return port;
}


// Time between the answer of the request that changes the settings and the restart, so the answer is delivered
const int s_restartDelayMs = 300;
const int s_adminMaxBodySize = 262144;


/**
	Get the integer value of the setting from JSON, the value must be a whole number inside the range.
*/
bool ReadIntegerSetting(const QString& name, const QJsonValue& value, int minValue, int maxValue, int& result, QString& errorText)
{
	const double number = value.toDouble(-1.0e18);
	if (!value.isDouble() || number != std::floor(number) || number < minValue || number > maxValue){
		errorText = QStringLiteral("The setting '%1' must be a whole number from %2 to %3").arg(name).arg(minValue).arg(maxValue);

		return false;
	}

	result = int(number);

	return true;
}


/**
	Handshake with the server that has just started: a TLS library may not be able to work with the key or the certificate (for example, a key of
	another type, or a key that does not belong to the certificate), and the server would be listening but would not answer anybody, including
	the admin requests that could change the settings back. The certificate is not verified here, it is the job of the clients.
*/
bool CheckTlsHandshake(int port, QString& errorText)
{
	QSslSocket socket;
	socket.setPeerVerifyMode(QSslSocket::VerifyNone);

	QEventLoop loop;
	QTimer timer;
	timer.setSingleShot(true);

	bool isEncrypted = false;
	QObject::connect(&socket, &QSslSocket::encrypted, &loop, [&isEncrypted, &loop](){
		isEncrypted = true;
		loop.quit();
	});
	QObject::connect(&socket, &QAbstractSocket::errorOccurred, &loop, &QEventLoop::quit);
	QObject::connect(&socket, &QAbstractSocket::disconnected, &loop, &QEventLoop::quit);
	QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);

	socket.connectToHostEncrypted(QStringLiteral("127.0.0.1"), quint16(port));
	timer.start(10000);
	loop.exec();

	if (!isEncrypted){
		errorText = QStringLiteral("The TLS handshake with the server fails, the certificate or the key cannot be used: %1").arg(
					socket.errorString().isEmpty() ? QStringLiteral("no answer") : socket.errorString());
	}

	socket.abort();

	return isEncrypted;
}


/**
	Set the value of an exported attribute of the composition (the attribute exists in the composition under this name, see ImtRestDemoServer.acc).
*/
template <class Value>
void SetSettingAttribute(icomp::CRegistryElement& element, const QByteArray& attributeId, const Value& value)
{
	icomp::IRegistryElement::AttributeInfo* infoPtr = element.InsertAttributeInfo(attributeId, iattr::TAttribute<Value>::GetTypeName());
	Q_ASSERT(infoPtr != nullptr);

	infoPtr->attributePtr.SetPtr(new iattr::TAttribute<Value>(value));
}


/**
	Give the parameters of the TLS that cannot be exported attributes: the algorithm of the key (it depends on the file) and the files of
	the intermediate certificates (their number is not known). The certificate and the key files are the attributes of the composition.
*/
bool SetTlsParameters(iprm::IParamsSet& tlsParams, const CDemoServerSettings& settings, QString& errorText)
{
	typedef imtcom::ISslConfigurationManager SslManager;

	iprm::IParamsSet* keyParamsPtr = dynamic_cast<iprm::IParamsSet*>(tlsParams.GetEditableParameter(SslManager::ParamKeys::s_privateKeyParamKey));
	iprm::ISelectionParam* algorithmPtr = (keyParamsPtr != nullptr)
				? dynamic_cast<iprm::ISelectionParam*>(keyParamsPtr->GetEditableParameter(SslManager::ParamKeys::s_keyAlgorithmParamKey))
				: nullptr;
	iprm::IParamsManager* chainPtr = dynamic_cast<iprm::IParamsManager*>(tlsParams.GetEditableParameter(SslManager::ParamKeys::s_localCertChainParamKey));
	if (algorithmPtr == nullptr || algorithmPtr->GetSelectionConstraints() == nullptr || chainPtr == nullptr){
		errorText = QStringLiteral("The composition of the server has no parameters of the key and of the certificate chain of the TLS");

		return false;
	}

	QSsl::KeyAlgorithm keyAlgorithm = QSsl::Rsa;
	QString keyError;
	DemoTlsTools::ReadPrivateKeyAlgorithm(settings.tlsKeyFile, keyAlgorithm, keyError);
	const SslManager::KeyAlgorithm sslKeyAlgorithm = keyAlgorithm == QSsl::Ec ? SslManager::KA_EC : (keyAlgorithm == QSsl::Dsa ? SslManager::KA_DSA : SslManager::KA_RSA);

	const iprm::IOptionsList& algorithms = *algorithmPtr->GetSelectionConstraints();
	bool isAlgorithmSet = false;
	for (int i = 0; i < algorithms.GetOptionsCount() && !isAlgorithmSet; ++i){
		if (algorithms.GetOptionId(i) == SslManager::ToString(sslKeyAlgorithm)){
			isAlgorithmSet = algorithmPtr->SetSelectedOptionIndex(i);
		}
	}

	if (!isAlgorithmSet){
		errorText = QStringLiteral("The algorithm of the key of the TLS cannot be set");

		return false;
	}

	// One set of the parameters for every file of the intermediate certificates, the same as the set of the certificate of the server
	while (chainPtr->GetParamsSetsCount() > 0){
		chainPtr->RemoveParamsSet(0);
	}

	for (const QString& chainFile : settings.tlsChainFiles){
		const int index = chainPtr->InsertParamsSet();
		iprm::IParamsSet* setPtr = (index >= 0) ? chainPtr->GetParamsSet(index) : nullptr;
		ifile::IFileNameParam* filePtr = (setPtr != nullptr)
					? dynamic_cast<ifile::IFileNameParam*>(setPtr->GetEditableParameter(SslManager::ParamKeys::s_filePathParamKey))
					: nullptr;
		if (filePtr == nullptr){
			errorText = QStringLiteral("The composition of the server cannot give a set for a file of the certificate chain");

			return false;
		}

		filePtr->SetPath(chainFile);
	}

	return true;
}


} // namespace


struct CDemoServer::Impl
{
	DemoCompositionFactory factory;
	CDemoServerSettings settings;
	QString errorString;
	int port = 0;
	QString filesDirectory;
	QString uploadDirectory;
	QJsonObject info;

	// The state that is read by the threads of the handlers is protected by the mutex
	mutable QMutex mutex;
	QJsonObject settingsJson;
	QString lastError;
	int restartCount = 0;
	bool isRestartPending = false;
	CDemoServerSettings pendingSettings;
	int generation = 0;

	// The object of the thread of the server, it makes the restart after the delay
	std::unique_ptr<QObject> contextPtr;

	// The settings of the server are the exported attributes of the composition, the composition keeps the pointer to this element
	std::unique_ptr<icomp::CRegistryElement> settingsElementPtr;
	std::unique_ptr<IDemoComposition> compositionPtr;
	imtrest::IServer* tcpServerPtr = nullptr;
};


CDemoServer::CDemoServer(const DemoCompositionFactory& factory)
	:m_implPtr(new Impl)
{
	m_implPtr->factory = factory;
}


CDemoServer::~CDemoServer()
{
	Stop();
}


bool CDemoServer::Start(const CDemoServerSettings& settings)
{
	Stop();

	Impl& impl = *m_implPtr;
	impl.contextPtr.reset(new QObject);

	{
		QMutexLocker locker(&impl.mutex);
		impl.lastError.clear();
		impl.restartCount = 0;
		impl.isRestartPending = false;
	}

	return StartServer(settings);
}


bool CDemoServer::StartServer(const CDemoServerSettings& settings)
{
	StopServer();

	Impl& impl = *m_implPtr;
	impl.settings = settings;
	impl.errorString.clear();

	if (settings.filesDirectory.isEmpty()){
		impl.errorString = QStringLiteral("The directory of the files is not set");

		return false;
	}

	impl.filesDirectory = QDir(settings.filesDirectory).absolutePath();
	impl.uploadDirectory = QDir(impl.filesDirectory).absoluteFilePath(DemoProtocol::s_uploadsDirectoryName);

	if (!QDir().mkpath(impl.uploadDirectory)){
		impl.errorString = QStringLiteral("The directory '%1' cannot be created").arg(impl.uploadDirectory);

		return false;
	}

	if (settings.createSampleFiles && !CreateSampleFiles(impl.filesDirectory)){
		impl.errorString = QStringLiteral("The sample files cannot be created in '%1'").arg(impl.filesDirectory);

		return false;
	}

	if (settings.isTlsEnabled && !DemoTlsTools::UseOpensslBackend(impl.errorString)){
		return false;
	}

	if (settings.isTlsEnabled && !DemoTlsTools::ValidateServerFiles(settings.tlsCertificateFile, settings.tlsChainFiles, settings.tlsKeyFile, impl.errorString)){
		return false;
	}

	impl.port = settings.port > 0 ? settings.port : FindFreePort();
	if (impl.port <= 0){
		impl.errorString = QStringLiteral("There is no free port");

		return false;
	}

	impl.info = QJsonObject();
	impl.info.insert("port", impl.port);
	impl.info.insert("token", settings.token);
	impl.info.insert("threadsLimit", settings.threadsLimit);
	impl.info.insert("requestInactivityTimeoutMs", settings.requestInactivityTimeoutMs);
	impl.info.insert("keepAliveTimeoutMs", settings.keepAliveTimeoutMs);
	impl.info.insert("requestProcessingTimeoutMs", settings.requestProcessingTimeoutMs);
	impl.info.insert("writeHighWatermark", settings.writeHighWatermark);
	impl.info.insert("maxBodySize", settings.maxBodySize);
	impl.info.insert("scheme", settings.isTlsEnabled ? "https" : "http");
	impl.info.insert("tlsEnabled", settings.isTlsEnabled);
	impl.info.insert("echoMaxBodySize", DemoProtocol::s_echoMaxBodySize);

	// The settings replace the values that the composition has by default
	impl.settingsElementPtr.reset(new icomp::CRegistryElement);
	icomp::CRegistryElement& element = *impl.settingsElementPtr;

	SetSettingAttribute(element, "Port", impl.port);
	SetSettingAttribute(element, "Token", settings.token.toUtf8());
	SetSettingAttribute(element, "FilesDirectory", impl.filesDirectory);
	SetSettingAttribute(element, "UploadDirectory", impl.uploadDirectory);
	SetSettingAttribute(element, "ThreadsLimit", settings.threadsLimit);
	SetSettingAttribute(element, "RequestInactivityTimeoutMs", settings.requestInactivityTimeoutMs);
	SetSettingAttribute(element, "KeepAliveTimeoutMs", settings.keepAliveTimeoutMs);
	SetSettingAttribute(element, "RequestProcessingTimeoutMs", settings.requestProcessingTimeoutMs);
	SetSettingAttribute(element, "WriteHighWatermark", settings.writeHighWatermark);
	SetSettingAttribute(element, "MaxBodySize", settings.maxBodySize);
	SetSettingAttribute(element, "InfoJson", QJsonDocument(impl.info).toJson(QJsonDocument::Compact));
	SetSettingAttribute(element, "EnableSsl", settings.isTlsEnabled);
	SetSettingAttribute(element, "TlsCertificateFile", settings.tlsCertificateFile);
	SetSettingAttribute(element, "TlsKeyFile", settings.tlsKeyFile);

	// The messages of the components: the log prints the messages of the category not less than this one, 5 is more than all of them
	SetSettingAttribute(element, "LogMinCategory", settings.isLogEnabled ? 0 : 5);

	impl.compositionPtr = impl.factory(element);

	impl.tcpServerPtr = impl.compositionPtr->GetServer();
	IDemoSettingsControllerBinding* controllerPtr = impl.compositionPtr->GetSettingsController();
	iprm::IParamsSet* tlsParamsPtr = impl.compositionPtr->GetTlsParameters();
	if (impl.tcpServerPtr == nullptr || controllerPtr == nullptr || tlsParamsPtr == nullptr){
		impl.errorString = QStringLiteral("The composition of the server does not give the components that the server needs");
		StopServer();

		return false;
	}

	controllerPtr->SetFunctions(
				[this](){
					return GetSettingsJson();
				},
				[this](const QJsonObject& changes, QString& errorText, QJsonObject& result){
					return ChangeSettings(changes, errorText, result);
				});

	if (settings.isTlsEnabled && !SetTlsParameters(*tlsParamsPtr, settings, impl.errorString)){
		StopServer();

		return false;
	}

	if (!impl.tcpServerPtr->StartServer() || !IsListening()){
		impl.errorString = QStringLiteral("The server cannot listen to the port %1").arg(impl.port);
		StopServer();

		return false;
	}

	if (settings.isTlsEnabled && !CheckTlsHandshake(impl.port, impl.errorString)){
		StopServer();

		return false;
	}

	UpdateSettingsJson();

	return true;
}


void CDemoServer::Stop()
{
	Impl& impl = *m_implPtr;

	// The restart that is not made yet is cancelled
	impl.contextPtr.reset();

	{
		QMutexLocker locker(&impl.mutex);
		impl.isRestartPending = false;
		++impl.generation;
	}

	StopServer();
}


void CDemoServer::StopServer()
{
	Impl& impl = *m_implPtr;

	if (impl.tcpServerPtr != nullptr){
		impl.tcpServerPtr->StopServer();
	}

	// A connection that was accepted just before the stop is registered by a queued call of the server: it is delivered before the
	// connections are closed, otherwise the client of that connection waits for an answer that never comes
	QCoreApplication::processEvents();

	if (impl.compositionPtr != nullptr){
		DemoServletTools::NotifyServerStopping();
		impl.compositionPtr->ShutdownConnections();
	}

	// The connections that are closed are finished by the event loop
	QCoreApplication::processEvents();

	impl.tcpServerPtr = nullptr;
	impl.compositionPtr.reset();
	impl.settingsElementPtr.reset();
}


bool CDemoServer::IsListening() const
{
	return m_implPtr->tcpServerPtr != nullptr
				&& m_implPtr->tcpServerPtr->GetServerStatus() == imtrest::IServer::SS_LISTENING;
}


int CDemoServer::GetPort() const
{
	return m_implPtr->port;
}


QString CDemoServer::GetErrorString() const
{
	return m_implPtr->errorString;
}


QString CDemoServer::GetFilesDirectory() const
{
	return m_implPtr->filesDirectory;
}


QString CDemoServer::GetUploadDirectory() const
{
	return m_implPtr->uploadDirectory;
}


QJsonObject CDemoServer::GetInfo() const
{
	return m_implPtr->info;
}


QJsonObject CDemoServer::GetSettingsJson() const
{
	QMutexLocker locker(&m_implPtr->mutex);

	QJsonObject retVal = m_implPtr->settingsJson;
	retVal.insert("isRestartPending", m_implPtr->isRestartPending);
	retVal.insert("lastError", m_implPtr->lastError);
	retVal.insert("restartCount", m_implPtr->restartCount);

	return retVal;
}


// private methods

void CDemoServer::UpdateSettingsJson()
{
	Impl& impl = *m_implPtr;

	QJsonObject json;
	json.insert("port", impl.port);
	json.insert("filesDirectory", impl.filesDirectory);
	json.insert("token", impl.settings.token);
	json.insert("threadsLimit", impl.settings.threadsLimit);
	json.insert("requestInactivityTimeoutMs", impl.settings.requestInactivityTimeoutMs);
	json.insert("keepAliveTimeoutMs", impl.settings.keepAliveTimeoutMs);
	json.insert("requestProcessingTimeoutMs", impl.settings.requestProcessingTimeoutMs);
	json.insert("writeHighWatermark", impl.settings.writeHighWatermark);
	json.insert("maxBodySize", impl.settings.maxBodySize);
	json.insert("isLogEnabled", impl.settings.isLogEnabled);
	json.insert("scheme", impl.settings.isTlsEnabled ? "https" : "http");
	json.insert("tlsEnabled", impl.settings.isTlsEnabled);
	json.insert("tlsCertificateFile", impl.settings.tlsCertificateFile);
	json.insert("tlsChainFiles", QJsonArray::fromStringList(impl.settings.tlsChainFiles));
	json.insert("tlsKeyFile", impl.settings.tlsKeyFile);
	json.insert("tlsSamplesDirectory", impl.settings.tlsSamplesDirectory);
	json.insert("tlsSamples", impl.settings.tlsSamplesDirectory.isEmpty() ? QJsonArray() : DemoTlsTools::DescribeSamples(impl.settings.tlsSamplesDirectory));

	QMutexLocker locker(&impl.mutex);
	impl.settingsJson = json;
}


bool CDemoServer::ChangeSettings(const QJsonObject& changes, QString& errorText, QJsonObject& result)
{
	Impl& impl = *m_implPtr;

	QMutexLocker locker(&impl.mutex);

	if (impl.contextPtr == nullptr){
		errorText = QStringLiteral("The server is stopping");

		return false;
	}

	// The changes are added to the ones that are accepted already and wait for the restart
	CDemoServerSettings settings = impl.isRestartPending ? impl.pendingSettings : impl.settings;
	if (!impl.isRestartPending){
		settings.port = impl.port;
	}

	const CDemoServerSettings oldSettings = settings;

	// The set of the certificates by the name, or the certificates that the client has sent
	QString presetId;
	QString certificatePem;
	QString keyPem;
	QStringList chainPem;
	bool isPemSent = false;

	for (QJsonObject::const_iterator iter = changes.constBegin(); iter != changes.constEnd(); ++iter){
		const QString name = iter.key();
		const QJsonValue value = iter.value();

		if (name == "port"){
			if (!ReadIntegerSetting(name, value, 1, 65535, settings.port, errorText)){
				return false;
			}
		}
		else if (name == "threadsLimit"){
			if (!ReadIntegerSetting(name, value, 1, 256, settings.threadsLimit, errorText)){
				return false;
			}
		}
		else if (name == "requestInactivityTimeoutMs"){
			if (!ReadIntegerSetting(name, value, 0, 3600000, settings.requestInactivityTimeoutMs, errorText)){
				return false;
			}
		}
		else if (name == "keepAliveTimeoutMs"){
			if (!ReadIntegerSetting(name, value, 0, 3600000, settings.keepAliveTimeoutMs, errorText)){
				return false;
			}
		}
		else if (name == "requestProcessingTimeoutMs"){
			if (!ReadIntegerSetting(name, value, 0, 3600000, settings.requestProcessingTimeoutMs, errorText)){
				return false;
			}
		}
		else if (name == "writeHighWatermark"){
			if (!ReadIntegerSetting(name, value, 1024, 268435456, settings.writeHighWatermark, errorText)){
				return false;
			}
		}
		else if (name == "maxBodySize"){
			if (!ReadIntegerSetting(name, value, 0, 2147483647, settings.maxBodySize, errorText)){
				return false;
			}
		}
		else if (name == "token"){
			const QString token = value.toString();
			static const QRegularExpression tokenExpression("^[A-Za-z0-9._~+/=-]{1,128}$");
			if (!value.isString() || !tokenExpression.match(token).hasMatch()){
				errorText = QStringLiteral("The setting 'token' must be 1-128 characters: letters, digits and . _ ~ + / = -");

				return false;
			}

			settings.token = token;
		}
		else if (name == "tlsEnabled"){
			if (!value.isBool()){
				errorText = QStringLiteral("The setting 'tlsEnabled' must be true or false");

				return false;
			}

			settings.isTlsEnabled = value.toBool();
		}
		else if (name == "tlsCertificateFile" || name == "tlsKeyFile"){
			if (!value.isString() || value.toString().size() > 4096){
				errorText = QStringLiteral("The setting '%1' must be a path of a file").arg(name);

				return false;
			}

			(name == "tlsCertificateFile" ? settings.tlsCertificateFile : settings.tlsKeyFile) = value.toString();
		}
		else if (name == "tlsPreset"){
			if (!value.isString() || value.toString().isEmpty() || value.toString().size() > 64){
				errorText = QStringLiteral("The setting 'tlsPreset' must be the name of a set of the certificates");

				return false;
			}

			presetId = value.toString();
		}
		else if (name == "tlsCertificatePem" || name == "tlsKeyPem"){
			if (!value.isString()){
				errorText = QStringLiteral("The setting '%1' must be a PEM text").arg(name);

				return false;
			}

			isPemSent = true;
			(name == "tlsCertificatePem" ? certificatePem : keyPem) = value.toString();
		}
		else if (name == "tlsChainPem"){
			if (!value.isArray() || value.toArray().size() > 16){
				errorText = QStringLiteral("The setting 'tlsChainPem' must be a list of up to 16 PEM texts");

				return false;
			}

			for (const QJsonValue& item : value.toArray()){
				if (!item.isString()){
					errorText = QStringLiteral("The setting 'tlsChainPem' must be a list of up to 16 PEM texts");

					return false;
				}

				chainPem.append(item.toString());
			}

			isPemSent = true;
		}
		else if (name == "tlsChainFiles"){
			if (!value.isArray() || value.toArray().size() > 16){
				errorText = QStringLiteral("The setting 'tlsChainFiles' must be a list of up to 16 paths of files");

				return false;
			}

			QStringList chainFiles;
			for (const QJsonValue& item : value.toArray()){
				if (!item.isString() || item.toString().isEmpty() || item.toString().size() > 4096){
					errorText = QStringLiteral("The setting 'tlsChainFiles' must be a list of up to 16 paths of files");

					return false;
				}

				chainFiles.append(item.toString());
			}

			settings.tlsChainFiles = chainFiles;
		}
		else if (name == "isLogEnabled"){
			if (!value.isBool()){
				errorText = QStringLiteral("The setting 'isLogEnabled' must be true or false");

				return false;
			}

			settings.isLogEnabled = value.toBool();
		}
		else{
			errorText = QStringLiteral("The setting '%1' is unknown or cannot be changed").arg(name);

			return false;
		}
	}

	QString storedCertificate;
	if (!presetId.isEmpty() || isPemSent){
		if (!presetId.isEmpty() && isPemSent){
			errorText = QStringLiteral("A set of the certificates (tlsPreset) and the certificates themselves cannot be in one request");

			return false;
		}

		if (!presetId.isEmpty()){
			QJsonObject preset;
			for (const QJsonValue& sample : DemoTlsTools::DescribeSamples(settings.tlsSamplesDirectory)){
				if (sample.toObject().value("id").toString() == presetId){
					preset = sample.toObject();
				}
			}

			if (preset.isEmpty()){
				errorText = QStringLiteral("There is no set of the certificates '%1' (the server must be started with --tls-samples and has the sets from tlsSamples)").arg(presetId);

				return false;
			}

			settings.tlsCertificateFile = preset.value("certificateFile").toString();
			settings.tlsKeyFile = preset.value("keyFile").toString();
			settings.tlsChainFiles.clear();
			for (const QJsonValue& chainFile : preset.value("chainFiles").toArray()){
				settings.tlsChainFiles.append(chainFile.toString());
			}
		}
		else{
			// The directory is not in the directory of the files that are given to everybody
			const QString storage = settings.tlsStorageDirectory.isEmpty() ? QDir(impl.filesDirectory).absolutePath() + QStringLiteral(".tls") : settings.tlsStorageDirectory;
			if (QDir::cleanPath(QDir(storage).absolutePath() + '/').startsWith(QDir::cleanPath(QDir(impl.filesDirectory).absolutePath()) + '/')){
				errorText = QStringLiteral("The directory of the keys must not be inside of the directory of the files");

				return false;
			}

			if (certificatePem.isEmpty() || keyPem.isEmpty()){
				errorText = QStringLiteral("The certificate (tlsCertificatePem) and the key (tlsKeyPem) must be sent together");

				return false;
			}

			QString storedKey;
			QStringList storedChain;
			if (!DemoTlsTools::StorePem(storage, certificatePem, chainPem, keyPem, storedCertificate, storedChain, storedKey, errorText)){
				return false;
			}

			settings.tlsCertificateFile = storedCertificate;
			settings.tlsChainFiles = storedChain;
			settings.tlsKeyFile = storedKey;
		}

		// The certificates are for the use: the server encrypts unless the request tells it not to
		if (!changes.contains("tlsEnabled")){
			settings.isTlsEnabled = true;
		}
	}

	const bool isChanged = settings.port != oldSettings.port
				|| settings.threadsLimit != oldSettings.threadsLimit
				|| settings.requestInactivityTimeoutMs != oldSettings.requestInactivityTimeoutMs
				|| settings.keepAliveTimeoutMs != oldSettings.keepAliveTimeoutMs
				|| settings.requestProcessingTimeoutMs != oldSettings.requestProcessingTimeoutMs
				|| settings.writeHighWatermark != oldSettings.writeHighWatermark
				|| settings.maxBodySize != oldSettings.maxBodySize
				|| settings.token != oldSettings.token
				|| settings.isLogEnabled != oldSettings.isLogEnabled
				|| settings.isTlsEnabled != oldSettings.isTlsEnabled
				|| settings.tlsCertificateFile != oldSettings.tlsCertificateFile
				|| settings.tlsChainFiles != oldSettings.tlsChainFiles
				|| settings.tlsKeyFile != oldSettings.tlsKeyFile;

	// A wrong file is refused here, not found by the restart
	if (isChanged && settings.isTlsEnabled && !DemoTlsTools::ValidateServerFiles(settings.tlsCertificateFile, settings.tlsChainFiles, settings.tlsKeyFile, errorText)){
		if (!storedCertificate.isEmpty()){
			QDir(QFileInfo(storedCertificate).absolutePath()).removeRecursively();
		}

		return false;
	}

	result.insert("restartScheduled", isChanged || impl.isRestartPending);
	result.insert("restartDelayMs", s_restartDelayMs);
	result.insert("port", settings.port);
	result.insert("token", settings.token);
	result.insert("scheme", settings.isTlsEnabled ? "https" : "http");

	if (!isChanged){
		return true;
	}

	impl.pendingSettings = settings;
	impl.isRestartPending = true;
	impl.lastError.clear();
	const int generation = ++impl.generation;

	// The restart is made by the thread of the server, the delay is counted there
	QObject* contextPtr = impl.contextPtr.get();
	QMetaObject::invokeMethod(contextPtr, [this, contextPtr, generation](){
		QTimer::singleShot(s_restartDelayMs, contextPtr, [this, generation](){
			Restart(generation);
		});
	}, Qt::QueuedConnection);

	return true;
}


void CDemoServer::Restart(int generation)
{
	Impl& impl = *m_implPtr;

	CDemoServerSettings newSettings;
	{
		QMutexLocker locker(&impl.mutex);
		if (!impl.isRestartPending || generation != impl.generation){
			return;
		}

		newSettings = impl.pendingSettings;
	}

	const int startedGeneration = generation;

	CDemoServerSettings previousSettings = impl.settings;
	previousSettings.port = impl.port;

	QString errorText;
	if (!StartServer(newSettings)){
		errorText = impl.errorString;

		// The server goes back to the settings that worked
		StartServer(previousSettings);
	}

	QMutexLocker locker(&impl.mutex);

	// A request that has come while the server restarted wants its own restart
	if (impl.generation == startedGeneration){
		impl.isRestartPending = false;
	}

	impl.lastError = errorText;
	++impl.restartCount;

	// The files of the certificates that the clients have sent: only the used ones stay
	QStringList keptFiles = {impl.settings.tlsCertificateFile, impl.settings.tlsKeyFile};
	keptFiles += impl.settings.tlsChainFiles;
	if (impl.isRestartPending){
		keptFiles += {impl.pendingSettings.tlsCertificateFile, impl.pendingSettings.tlsKeyFile};
		keptFiles += impl.pendingSettings.tlsChainFiles;
	}

	const QString storage = impl.settings.tlsStorageDirectory.isEmpty() ? QDir(impl.filesDirectory).absolutePath() + QStringLiteral(".tls") : impl.settings.tlsStorageDirectory;
	DemoTlsTools::RemoveUnusedStoredFiles(storage, keptFiles);
}


// static public methods

bool CDemoServer::CreateSampleFiles(const QString& directoryPath)
{
	const QDir directory(directoryPath);

	if (!directory.mkpath(QStringLiteral("private"))){
		return false;
	}

	auto createFile = [&directory](const QString& name, const QByteArray& content){
		const QString path = directory.absoluteFilePath(name);
		if (QFileInfo::exists(path)){
			return true;
		}

		QFile file(path);

		return file.open(QIODevice::WriteOnly) && file.write(content) == content.size();
	};

	return createFile(DemoProtocol::s_helloFileName, DemoProtocol::s_helloFileText)
				&& createFile(DemoProtocol::s_emptyFileName, QByteArray())
				&& createFile(DemoProtocol::s_patternFileName, DemoProtocol::CreatePatternData(0, DemoProtocol::s_patternFileSize))
				&& createFile(DemoProtocol::s_privateFileName, DemoProtocol::s_privateFileText);
}


} // namespace imtrestdemo
