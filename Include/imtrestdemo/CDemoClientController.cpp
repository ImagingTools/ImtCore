// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtrestdemo/CDemoClientController.h>


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QFileInfo>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QRegularExpression>
#include <QtCore/QUrlQuery>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QSslCipher>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslSocket>

// ImtCore includes
#include <imtrestdemo/CDemoProtocol.h>
#include <imtrestdemo/CPatternDevice.h>


namespace imtrestdemo
{


namespace
{


QString QuoteForShell(const QString& text)
{
	QString quoted = text;
	quoted.replace('\'', QStringLiteral("'\\''"));

	return '\'' + quoted + '\'';
}


bool HasHeader(const SDemoExchange::Headers& headers, const QByteArray& name)
{
	for (const QPair<QByteArray, QByteArray>& header : headers){
		if (header.first.compare(name, Qt::CaseInsensitive) == 0){
			return true;
		}
	}

	return false;
}


}


CDemoClientController::CDemoClientController(QObject* parentPtr)
	:QObject(parentPtr),
	m_manager(this),
	m_exchangesPtr(new CTrafficLogModel(this)),
	m_wirePtr(new CWireLogModel(this))
{
	SetStatusText([](){
		return QCoreApplication::translate("CDemoClientController", "Not connected");
	});

	m_speedTimer.setInterval(500);
	connect(&m_speedTimer, &QTimer::timeout, this, &CDemoClientController::OnSpeedTick);

	// The errors of the certificate are not ignored, they only explain the failure of the exchange
	connect(&m_manager, &QNetworkAccessManager::sslErrors, this, [this](QNetworkReply* replyPtr, const QList<QSslError>& errors){
		QHash<QNetworkReply*, std::shared_ptr<Pending>>::ConstIterator iter = m_pending.constFind(replyPtr);
		if (iter != m_pending.constEnd()){
			QStringList texts;
			for (const QSslError& error : errors){
				texts << error.errorString();
			}

			iter.value()->sslErrorText = texts.join(QStringLiteral("; "));
		}
	});

	m_reloadTimer.setSingleShot(true);
	connect(&m_reloadTimer, &QTimer::timeout, this, &CDemoClientController::TryReloadSettings);
}


CDemoClientController::~CDemoClientController()
{
	abortAll();
}


QString CDemoClientController::GetServerUrl() const
{
	return m_serverUrl;
}


void CDemoClientController::SetServerUrl(const QString& serverUrl)
{
	if (m_serverUrl != serverUrl){
		m_serverUrl = serverUrl;

		// The proxy of the old server is not used any more
		if (m_proxyPtr != nullptr){
			m_proxyPtr->Stop();
		}

		Q_EMIT serverUrlChanged();
	}
}


QString CDemoClientController::GetToken() const
{
	return m_token;
}


void CDemoClientController::SetToken(const QString& token)
{
	if (m_token != token){
		m_token = token;

		Q_EMIT tokenChanged();
	}
}


bool CDemoClientController::IsCaptureEnabled() const
{
	return m_isCaptureEnabled;
}


void CDemoClientController::SetCaptureEnabled(bool isEnabled)
{
	if (m_isCaptureEnabled != isEnabled){
		m_isCaptureEnabled = isEnabled;

		Q_EMIT captureEnabledChanged();
	}
}


bool CDemoClientController::IsBusy() const
{
	return m_isBusy;
}


QString CDemoClientController::GetStatusText() const
{
	return m_statusText;
}


QVariantMap CDemoClientController::GetServerSettings() const
{
	return m_serverSettings;
}


QString CDemoClientController::GetSettingsStatus() const
{
	return m_settingsStatus;
}


QStringList CDemoClientController::GetTrustedCertificates() const
{
	return m_trustedFiles;
}


void CDemoClientController::SetTrustedCertificates(const QStringList& files)
{
	QList<QSslCertificate> certificates;
	for (const QString& file : files){
		if (file.trimmed().startsWith(QLatin1String("-----BEGIN"))){
			certificates.append(QSslCertificate::fromData(file.toUtf8(), QSsl::Pem));

			continue;
		}

		QFile certificateFile(file);
		if (certificateFile.open(QIODevice::ReadOnly)){
			certificates.append(QSslCertificate::fromData(certificateFile.readAll(), QSsl::Pem));
		}
	}

	m_trustedFiles = files;
	m_trustedCertificates = certificates;

	// A connection that is open was checked with the old trust
	m_manager.clearConnectionCache();

	Q_EMIT trustedCertificatesChanged();
}


QString CDemoClientController::GetServerInfo() const
{
	return m_serverInfo;
}


CTrafficLogModel* CDemoClientController::GetExchanges() const
{
	return m_exchangesPtr;
}


CWireLogModel* CDemoClientController::GetWire() const
{
	return m_wirePtr;
}


int CDemoClientController::sendRequest(const QVariantMap& options)
{
	const QByteArray method = options.value("method", "GET").toString().trimmed().toUpper().toLatin1();
	const QString path = options.value("path", "/").toString().trimmed();

	const QUrl logicalUrl = CreateLogicalUrl(path);
	if (method.isEmpty() || !logicalUrl.isValid() || logicalUrl.host().isEmpty()){
		return AddFailedExchange(method, path, QCoreApplication::translate("CDemoClientController", "The method or the URL is not valid"));
	}

	SDemoExchange exchange;
	exchange.method = method;
	exchange.url = logicalUrl.toString(QUrl::FullyEncoded);
	exchange.startTime = QDateTime::currentDateTime();
	exchange.requestHeaders = ParseHeaders(options.value("headers").toString());

	if (options.value("useToken", false).toBool() && !m_token.isEmpty() && !HasHeader(exchange.requestHeaders, "Authorization")){
		exchange.requestHeaders.append(qMakePair(QByteArray("Authorization"), "Bearer " + m_token.toUtf8()));
	}

	if (options.value("expectContinue", false).toBool() && !HasHeader(exchange.requestHeaders, "Expect")){
		exchange.requestHeaders.append(qMakePair(QByteArray("Expect"), QByteArray("100-continue")));
	}

	// The request goes to the proxy that shows the wire, the application sees the address of the server
	const bool isTls = logicalUrl.scheme() == QLatin1String("https");

	// The bytes of a TLS connection are encrypted, the proxy would show only them: https is not captured
	QUrl wireUrl = logicalUrl;
	if (m_isCaptureEnabled && !isTls && EnsureProxy()){
		wireUrl.setHost(QStringLiteral("127.0.0.1"));
		wireUrl.setPort(m_proxyPtr->GetPort());
	}

	// The body; with \c contentRange it is a part of a file that starts at \c rangeStart (an upload that can be resumed)
	const QString bodyType = options.value("bodyType", "none").toString();
	const bool isRanged = options.value("contentRange", false).toBool();
	const qint64 rangeStart = isRanged ? qMax<qint64>(0, options.value("rangeStart").toLongLong()) : 0;
	QByteArray bodyBytes;
	std::unique_ptr<QIODevice> devicePtr;

	if (bodyType == "text"){
		bodyBytes = options.value("bodyText").toString().toUtf8();
		exchange.requestBodySize = bodyBytes.size();
		exchange.requestBodyPreview = bodyBytes.left(CTrafficLogModel::s_previewLimit);

		// A private key that is sent to the server is not shown in the log
		if (options.value("settingsRole").toInt() == SR_APPLY && bodyBytes.contains("tlsKeyPem")){
			static const QRegularExpression keyExpression(R"("tlsKeyPem":\s*"[^"]*")");
			exchange.requestBodyPreview = QString::fromUtf8(bodyBytes).replace(keyExpression, QStringLiteral("\"tlsKeyPem\": \"(the private key is not shown)\"")).toUtf8().left(CTrafficLogModel::s_previewLimit);
		}
	}
	else if (bodyType == "file"){
		std::unique_ptr<QFile> filePtr(new QFile(options.value("bodyFile").toString()));
		if (!filePtr->open(QIODevice::ReadOnly)){
			return AddFailedExchange(method, exchange.url, QCoreApplication::translate("CDemoClientController", "The file of the body cannot be read: %1").arg(filePtr->errorString()));
		}

		if (rangeStart > filePtr->size() || !filePtr->seek(rangeStart)){
			return AddFailedExchange(method, exchange.url, QCoreApplication::translate("CDemoClientController", "The size of the body is not valid"));
		}

		// The library sends the bytes from the position of the device
		exchange.requestBodySize = filePtr->size() - rangeStart;
		exchange.requestBodyPreview = filePtr->read(CTrafficLogModel::s_previewLimit);
		filePtr->seek(rangeStart);
		devicePtr = std::move(filePtr);
	}
	else if (bodyType == "generated"){
		const qint64 size = options.value("bodySize").toLongLong();

		// A part of a big file continues the pattern from its position
		const qint64 offset = isRanged ? rangeStart : options.value("bodyOffset").toLongLong();
		if (size < 0){
			return AddFailedExchange(method, exchange.url, QCoreApplication::translate("CDemoClientController", "The size of the body is not valid"));
		}

		std::unique_ptr<CPatternDevice> patternPtr(new CPatternDevice(size, offset, false));
		patternPtr->open(QIODevice::ReadOnly);

		exchange.requestBodySize = size;
		exchange.requestBodyPreview = DemoProtocol::CreatePatternData(offset, qMin<qint64>(size, CTrafficLogModel::s_previewLimit));
		devicePtr = std::move(patternPtr);
	}

	if (isRanged && exchange.requestBodySize > 0 && !HasHeader(exchange.requestHeaders, "Content-Range")){
		const qint64 total = options.value("rangeTotal").toLongLong() > 0 ? options.value("rangeTotal").toLongLong() : rangeStart + exchange.requestBodySize;
		exchange.requestHeaders.append(qMakePair(QByteArray("Content-Range"),
					QByteArray("bytes ") + QByteArray::number(rangeStart) + "-" + QByteArray::number(rangeStart + exchange.requestBodySize - 1) + "/" + QByteArray::number(total)));
	}

	if (exchange.requestBodySize >= 0 && !HasHeader(exchange.requestHeaders, "Content-Type")){
		exchange.requestHeaders.append(qMakePair(QByteArray("Content-Type"), QByteArray(bodyType == "text" ? "text/plain; charset=utf-8" : "application/octet-stream")));
	}

	// A client library collects the body of a device that has no known size to calculate its length, so it never sends the chunks:
	// the body that must be sent in the chunks is written to the socket by the controller
	if (bodyType == "generated" && options.value("isChunked", false).toBool()){
		if (isTls){
			return AddFailedExchange(method, exchange.url, QCoreApplication::translate("CDemoClientController", "The upload in chunks and the raw bytes work without TLS only"));
		}

		return StartChunkedUpload(exchange, logicalUrl, options.value("bodySize").toLongLong(), options.value("bodyOffset").toLongLong());
	}

	QNetworkRequest request(wireUrl);

	if (isTls){
		// Only the certificates of the application are trusted, the host name must be the name of the certificate
		QSslConfiguration sslConfiguration = QSslConfiguration::defaultConfiguration();
		sslConfiguration.setCaCertificates(m_trustedCertificates);
		sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyPeer);
		request.setSslConfiguration(sslConfiguration);
	}
	request.setAttribute(QNetworkRequest::CacheLoadControlAttribute, QNetworkRequest::AlwaysNetwork);
	request.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
				options.value("followRedirects", false).toBool() ? QNetworkRequest::NoLessSafeRedirectPolicy : QNetworkRequest::ManualRedirectPolicy);
	request.setTransferTimeout(options.value("timeoutMs", 0).toInt());

	for (const QPair<QByteArray, QByteArray>& header : std::as_const(exchange.requestHeaders)){
		request.setRawHeader(header.first, header.second);
	}

	// The file of the response
	std::shared_ptr<Pending> pendingPtr = std::make_shared<Pending>();
	const QString saveTo = options.value("saveTo").toString();
	if (!saveTo.isEmpty()){
		pendingPtr->saveFilePtr.reset(new QFile(saveTo));
		if (!pendingPtr->saveFilePtr->open(QIODevice::WriteOnly)){
			return AddFailedExchange(method, exchange.url, QCoreApplication::translate("CDemoClientController", "The file of the response cannot be written: %1").arg(pendingPtr->saveFilePtr->errorString()));
		}
	}

	pendingPtr->isPatternChecked = options.value("checkPattern", false).toBool();
	pendingPtr->patternOffset = options.value("patternOffset").toLongLong();
	pendingPtr->isInfoRequest = options.value("isInfo", false).toBool();
	pendingPtr->settingsRole = options.value("settingsRole", int(SR_NONE)).toInt();
	pendingPtr->abortAfter = options.value("abortAfter").toLongLong();
	pendingPtr->isResumeState = options.contains("resumeFollowUp");
	pendingPtr->resumeOptions = options.value("resumeFollowUp").toMap();
	pendingPtr->resumeTotal = options.value("resumeTotal").toLongLong();
	pendingPtr->resumeStateAttempt = options.value("resumeStateAttempt").toInt();
	pendingPtr->resumeOriginalOptions = options.value("resumeOriginal").toMap();
	pendingPtr->resumeAttempt = options.value("resumeAttempt").toInt();
	if (pendingPtr->resumeAttempt > 0){
		pendingPtr->resumePartOptions = options;
	}
	pendingPtr->hashPtr.reset(new QCryptographicHash(QCryptographicHash::Sha256));
	pendingPtr->timer.start();
	pendingPtr->lastUpdateTimer.start();
	pendingPtr->exchangeId = m_exchangesPtr->AddExchange(exchange);

	QNetworkReply* replyPtr = nullptr;
	if (bodyType == "text"){
		replyPtr = m_manager.sendCustomRequest(request, method, bodyBytes);
	}
	else{
		replyPtr = m_manager.sendCustomRequest(request, method, devicePtr.get());
	}

	// The device lives while the reply works
	if (devicePtr != nullptr){
		devicePtr.release()->setParent(replyPtr);
	}

	m_pending.insert(replyPtr, pendingPtr);
	UpdateBusy();

	connect(replyPtr, &QNetworkReply::readyRead, this, [this, replyPtr](){
		OnReadyRead(replyPtr);
	});
	connect(replyPtr, &QNetworkReply::uploadProgress, this, [this, replyPtr](qint64 sent, qint64){
		OnUploadProgress(replyPtr, sent);
	});
	connect(replyPtr, &QNetworkReply::finished, this, [this, replyPtr](){
		OnFinished(replyPtr);
	});

	return pendingPtr->exchangeId;
}


int CDemoClientController::sendRaw(const QVariantMap& options)
{
	if (CreateLogicalUrl(QStringLiteral("/")).scheme() == QLatin1String("https")){
		return AddFailedExchange("RAW", QString(), QCoreApplication::translate("CDemoClientController", "The upload in chunks and the raw bytes work without TLS only"));
	}

	const QByteArray data = DecodeRawText(options.value("text").toString(), options.value("autoCrlf", true).toBool());
	if (data.isEmpty()){
		return AddFailedExchange("RAW", QString(), QCoreApplication::translate("CDemoClientController", "There is nothing to send"));
	}

	SDemoExchange exchange;
	exchange.method = "RAW";
	exchange.url = QStringLiteral("tcp://%1:%2").arg(GetTargetHost()).arg(GetTargetPort());
	exchange.startTime = QDateTime::currentDateTime();
	exchange.isRaw = true;
	exchange.requestBodySize = data.size();
	exchange.requestBodyPreview = data.left(CTrafficLogModel::s_previewLimit);

	std::shared_ptr<RawPending> rawPtr = std::make_shared<RawPending>();
	rawPtr->data = data;
	rawPtr->partSize = qMax(0, options.value("partSize", 0).toInt());
	rawPtr->delayMs = qMax(0, options.value("delayMs", 0).toInt());
	rawPtr->idleTimeoutMs = qMax(100, options.value("idleTimeoutMs", 2000).toInt());

	return StartRaw(exchange, rawPtr);
}


void CDemoClientController::testConnection()
{
	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/info");
	options.insert("isInfo", true);

	SetStatusText([this](){
		return QCoreApplication::translate("CDemoClientController", "Connecting to %1 ...").arg(m_serverUrl);
	});
	sendRequest(options);
}


void CDemoClientController::loadServerSettings()
{
	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/admin/settings");
	options.insert("useToken", true);
	options.insert("settingsRole", int(SR_LOAD));

	SetSettingsStatus(QCoreApplication::translate("CDemoClientController", "Reading the settings of the server ..."));
	sendRequest(options);
}


void CDemoClientController::applyTlsSettings(const QVariantMap& changes, const QStringList& trustedFiles)
{
	m_trustedBeforeApply = m_trustedFiles;
	m_trustedAfterApply = trustedFiles;
	m_isTrustChanging = true;

	QStringList both = m_trustedFiles;
	for (const QString& file : trustedFiles){
		if (!both.contains(file)){
			both.append(file);
		}
	}

	SetTrustedCertificates(both);
	applyServerSettings(changes);
}


void CDemoClientController::applyTlsPreset(const QString& presetId, const QStringList& trustedCertificates)
{
	QVariantMap changes;
	changes.insert("tlsPreset", presetId);
	applyTlsSettings(changes, trustedCertificates);
}


void CDemoClientController::applyTlsFiles(const QString& certificateFile, const QStringList& chainFiles, const QString& keyFile, const QStringList& trustedCertificates)
{
	auto readText = [](const QString& path, QString& text, QString& errorText){
		QFile file(path);
		if (path.isEmpty() || !file.open(QIODevice::ReadOnly)){
			errorText = QCoreApplication::translate("CDemoClientController", "The file '%1' cannot be read").arg(path);

			return false;
		}

		text = QString::fromUtf8(file.readAll());

		return true;
	};

	QString errorText;
	QString certificate;
	QString key;
	QVariantList chain;
	if (!readText(certificateFile, certificate, errorText) || !readText(keyFile, key, errorText)){
		SetSettingsStatus(errorText);

		return;
	}

	for (const QString& chainFile : chainFiles){
		QString text;
		if (!readText(chainFile, text, errorText)){
			SetSettingsStatus(errorText);

			return;
		}

		chain.append(text);
	}

	QVariantMap changes;
	changes.insert("tlsCertificatePem", certificate);
	changes.insert("tlsChainPem", chain);
	changes.insert("tlsKeyPem", key);
	applyTlsSettings(changes, trustedCertificates);
}


int CDemoClientController::resumeUpload(const QVariantMap& options)
{
	const QString bodyType = options.value("bodyType").toString();
	const QString path = options.value("path").toString().trimmed();

	qint64 total = 0;
	if (bodyType == "generated"){
		total = options.value("bodySize").toLongLong();
	}
	else if (bodyType == "file"){
		total = QFileInfo(options.value("bodyFile").toString()).size();
	}

	if (total <= 0){
		return AddFailedExchange("GET", path, QCoreApplication::translate("CDemoClientController", "A file or generated bytes can be resumed, and the size must be known"));
	}

	// The question: how much of the file the server has
	QVariantMap question;
	question.insert("method", "GET");
	question.insert("path", path);
	question.insert("useToken", options.value("useToken", false));
	QVariantMap followUp = options;
	followUp.remove("resumeStateAttempt");
	question.insert("resumeFollowUp", followUp);
	question.insert("resumeTotal", total);
	question.insert("resumeStateAttempt", options.value("resumeStateAttempt"));

	return sendRequest(question);
}


void CDemoClientController::OnResumeStateResponse(const Pending& pending, const SDemoExchange& exchange)
{
	const QJsonObject state = QJsonDocument::fromJson(exchange.responsePreview).object();

	qint64 received = 0;
	if (exchange.statusCode == 200){
		if (state.value("complete").toBool()){
			SetStatusText(QCoreApplication::translate("CDemoClientController", "The server has the whole file already"));

			return;
		}

		// The server still takes the bytes of the broken upload that were sent before the break: it is asked again in a moment
		if (state.value("inProgress").toBool() && pending.resumeStateAttempt < 30){
			SetStatusText(QCoreApplication::translate("CDemoClientController", "The server still writes the file, the state is asked again"));

			QVariantMap again = pending.resumeOptions;
			again.insert("resumeStateAttempt", pending.resumeStateAttempt + 1);
			QTimer::singleShot(500, this, [this, again](){
				resumeUpload(again);
			});

			return;
		}

		received = qint64(state.value("received").toDouble());
		if (state.value("total").toDouble() >= 0 && qint64(state.value("total").toDouble()) != pending.resumeTotal){
			SetStatusText(QCoreApplication::translate("CDemoClientController", "The server has another size of the file: %1 bytes, the file here is %2 bytes").arg(qint64(state.value("total").toDouble())).arg(pending.resumeTotal));

			return;
		}
	}
	else if (exchange.statusCode != 404){
		SetStatusText(QCoreApplication::translate("CDemoClientController", "The server did not tell the state of the upload: %1").arg(exchange.statusCode == 0 ? exchange.errorText : QString::number(exchange.statusCode)));

		return;
	}

	if (received >= pending.resumeTotal){
		SetStatusText(QCoreApplication::translate("CDemoClientController", "The server has all bytes of the file"));

		return;
	}

	SetStatusText(QCoreApplication::translate("CDemoClientController", "The server has %1 of %2 bytes, the rest is sent").arg(received).arg(pending.resumeTotal));

	// The rest of the file with the range
	QVariantMap rest = pending.resumeOptions;
	rest.insert("method", "PUT");
	rest.insert("contentRange", true);
	rest.insert("rangeStart", received);
	rest.insert("rangeTotal", pending.resumeTotal);
	rest.insert("resumeAttempt", 1);
	rest.insert("resumeOriginal", pending.resumeOptions);
	rest.remove("abortAfter");
	rest.remove("resumeFollowUp");
	if (rest.value("bodyType").toString() == "generated"){
		rest.insert("bodySize", pending.resumeTotal - received);
	}

	sendRequest(rest);
}


void CDemoClientController::applyServerSettings(const QVariantMap& changes)
{
	QJsonObject json;
	for (QVariantMap::ConstIterator iter = changes.constBegin(); iter != changes.constEnd(); ++iter){
		json.insert(iter.key(), QJsonValue::fromVariant(iter.value()));
	}

	QVariantMap options;
	options.insert("method", "PUT");
	options.insert("path", "/admin/settings");
	options.insert("useToken", true);
	options.insert("headers", QStringLiteral("Content-Type: application/json"));
	options.insert("bodyType", "text");
	options.insert("bodyText", QString::fromUtf8(QJsonDocument(json).toJson(QJsonDocument::Indented)));
	options.insert("settingsRole", int(SR_APPLY));

	m_reloadTimer.stop();
	SetSettingsStatus(QCoreApplication::translate("CDemoClientController", "Sending the settings to the server ..."));
	sendRequest(options);
}


QString CDemoClientController::makeCurlCommand(const QVariantMap& options) const
{
	const QString method = options.value("method", "GET").toString().trimmed().toUpper();
	const QUrl url = CreateLogicalUrl(options.value("path", "/").toString().trimmed());

	QString command = QStringLiteral("curl -v --noproxy '*' -X %1 %2").arg(method, QuoteForShell(url.toString(QUrl::FullyEncoded)));

	const SDemoExchange::Headers headers = ParseHeaders(options.value("headers").toString());
	for (const QPair<QByteArray, QByteArray>& header : headers){
		command += QStringLiteral(" -H %1").arg(QuoteForShell(QString::fromLatin1(header.first + ": " + header.second)));
	}

	if (options.value("useToken", false).toBool() && !m_token.isEmpty() && !HasHeader(headers, "Authorization")){
		command += QStringLiteral(" -H %1").arg(QuoteForShell("Authorization: Bearer " + m_token));
	}

	if (options.value("expectContinue", false).toBool()){
		command += QStringLiteral(" -H 'Expect: 100-continue'");
	}

	const QString bodyType = options.value("bodyType", "none").toString();
	if (bodyType == "text"){
		command += QStringLiteral(" --data-binary %1").arg(QuoteForShell(options.value("bodyText").toString()));
	}
	else if (bodyType == "file"){
		command += QStringLiteral(" --data-binary %1").arg(QuoteForShell('@' + options.value("bodyFile").toString()));
	}
	else if (bodyType == "generated"){
		command += QStringLiteral(" --data-binary @<file of %1 bytes>").arg(options.value("bodySize").toLongLong());

		if (options.value("isChunked", false).toBool()){
			command += QStringLiteral(" -H 'Transfer-Encoding: chunked'");
		}
	}

	if (!options.value("saveTo").toString().isEmpty()){
		command += QStringLiteral(" -o %1").arg(QuoteForShell(options.value("saveTo").toString()));
	}

	if (options.value("followRedirects", false).toBool()){
		command += QStringLiteral(" -L");
	}

	return command;
}


void CDemoClientController::Retranslate()
{
	m_exchangesPtr->Retranslate();
	m_wirePtr->Retranslate();

	// The status that is made of a pattern gets the text of the new language
	const std::function<QString()> statusBuilder = m_statusBuilder;
	if (statusBuilder){
		SetStatusText(statusBuilder);
	}
}


void CDemoClientController::clear()
{
	m_exchangesPtr->clear();
	m_wirePtr->clear();
}


void CDemoClientController::abortAll()
{
	const QList<QNetworkReply*> replies = m_pending.keys();
	for (QNetworkReply* replyPtr : replies){
		replyPtr->abort();
	}

	const QList<int> rawIds = m_rawPending.keys();
	for (int rawId : rawIds){
		FinishRaw(rawId);
	}
}


QByteArray CDemoClientController::DecodeRawText(const QString& text, bool isAutoCrlf)
{
	QByteArray retVal;

	for (int i = 0; i < text.size(); ++i){
		const QChar character = text[i];

		if (character == '\\' && i + 1 < text.size()){
			const QChar next = text[i + 1];

			if (next == 'r'){
				retVal += '\r';
				++i;
				continue;
			}

			if (next == 'n'){
				retVal += '\n';
				++i;
				continue;
			}

			if (next == 't'){
				retVal += '\t';
				++i;
				continue;
			}

			if (next == '\\'){
				retVal += '\\';
				++i;
				continue;
			}

			if (next == 'x' && i + 3 < text.size()){
				bool isNumber = false;
				const int value = text.mid(i + 2, 2).toInt(&isNumber, 16);
				if (isNumber){
					retVal += char(value);
					i += 3;
					continue;
				}
			}
		}

		if (isAutoCrlf && character == '\r'){
			continue;
		}

		if (isAutoCrlf && character == '\n'){
			retVal += "\r\n";
			continue;
		}

		retVal += QString(character).toUtf8();
	}

	return retVal;
}


SDemoExchange::Headers CDemoClientController::ParseHeaders(const QString& text)
{
	SDemoExchange::Headers retVal;

	const QStringList lines = text.split('\n');
	for (const QString& rawLine : lines){
		const QString line = rawLine.trimmed();
		const int colon = line.indexOf(':');
		if (colon <= 0){
			continue;
		}

		retVal.append({line.left(colon).trimmed().toLatin1(), line.mid(colon + 1).trimmed().toLatin1()});
	}

	return retVal;
}


// private methods

bool CDemoClientController::EnsureProxy()
{
	if (m_proxyPtr == nullptr){
		m_proxyPtr = new CWireCaptureProxy(this);

		connect(m_proxyPtr, &CWireCaptureProxy::ConnectionOpened, this, [this](int connectionId){
			m_wirePtr->AddEvent(connectionId, QCoreApplication::translate("CDemoClientController", "connection #%1 is opened").arg(connectionId));
		});
		connect(m_proxyPtr, &CWireCaptureProxy::DataCaptured, this, [this](int connectionId, int direction, const QByteArray& data){
			m_wirePtr->AddData(connectionId, direction == CWireCaptureProxy::D_CLIENT_TO_SERVER ? CWireLogModel::K_CLIENT_TO_SERVER : CWireLogModel::K_SERVER_TO_CLIENT, data);
		});
		connect(m_proxyPtr, &CWireCaptureProxy::ConnectionClosed, this, [this](int connectionId, int closedBy){
			m_wirePtr->AddEvent(connectionId, QCoreApplication::translate("CDemoClientController", "connection #%1 is closed by the %2").arg(connectionId).arg(closedBy == CWireCaptureProxy::D_SERVER_TO_CLIENT ? QCoreApplication::translate("CDemoClientController", "server") : QCoreApplication::translate("CDemoClientController", "client")));
		});
	}

	if (!m_proxyPtr->IsRunning() && !m_proxyPtr->Start(GetTargetHost(), GetTargetPort())){
		SetStatusText(QCoreApplication::translate("CDemoClientController", "The capture of the wire does not work: %1").arg(m_proxyPtr->GetErrorString()));

		return false;
	}

	return true;
}


int CDemoClientController::StartRaw(const SDemoExchange& exchange, const std::shared_ptr<RawPending>& rawPtr)
{
	QString host = GetTargetHost();
	int port = GetTargetPort();

	if (m_isCaptureEnabled && EnsureProxy()){
		host = QStringLiteral("127.0.0.1");
		port = m_proxyPtr->GetPort();
	}

	const int rawId = ++m_lastRawId;
	rawPtr->exchangeId = m_exchangesPtr->AddExchange(exchange);
	rawPtr->timer.start();

	QTcpSocket* socketPtr = new QTcpSocket(this);
	socketPtr->setSocketOption(QAbstractSocket::LowDelayOption, 1);
	rawPtr->socketPtr = socketPtr;

	QTimer* idleTimerPtr = new QTimer(socketPtr);
	idleTimerPtr->setSingleShot(true);
	rawPtr->idleTimerPtr = idleTimerPtr;

	m_rawPending.insert(rawId, rawPtr);
	UpdateBusy();

	connect(socketPtr, &QTcpSocket::connected, this, [this, rawId](){
		SendNextRawPart(rawId);
	});
	connect(socketPtr, &QTcpSocket::bytesWritten, this, [this, rawId](){
		QHash<int, std::shared_ptr<RawPending>>::ConstIterator iter = m_rawPending.constFind(rawId);
		if (iter != m_rawPending.constEnd() && iter.value()->isChunkedBody){
			SendNextRawPart(rawId);
		}
	});
	connect(socketPtr, &QTcpSocket::readyRead, this, [this, rawId](){
		QHash<int, std::shared_ptr<RawPending>>::ConstIterator iter = m_rawPending.constFind(rawId);
		if (iter == m_rawPending.constEnd() || iter.value()->socketPtr == nullptr){
			return;
		}

		RawPending& raw = *iter.value();
		const QByteArray received = raw.socketPtr->readAll();
		raw.responseSize += received.size();
		if (raw.responsePreview.size() < CTrafficLogModel::s_previewLimit){
			raw.responsePreview += received.left(CTrafficLogModel::s_previewLimit - int(raw.responsePreview.size()));
		}

		raw.responseTail = (raw.responseTail + received).right(8);

		const bool isRequestSent = raw.isChunkedBody ? raw.isLastChunkWritten : raw.sentSize >= raw.data.size();

		// The answer is complete: the exchange is over. The upload that is refused is not sent to the end.
		if (IsResponseComplete(raw.responsePreview, raw.responseSize, raw.responseTail) && (isRequestSent || raw.isChunkedBody)){
			QTimer::singleShot(0, this, [this, rawId](){
				FinishRaw(rawId);
			});

			return;
		}

		// The server is silent after the end of the sending: the exchange is over
		if (isRequestSent && raw.idleTimerPtr != nullptr){
			raw.idleTimerPtr->start(raw.idleTimeoutMs);
		}
	});
	connect(socketPtr, &QTcpSocket::disconnected, this, [this, rawId](){
		FinishRaw(rawId);
	});
	connect(socketPtr, &QTcpSocket::errorOccurred, this, [this, rawId](){
		FinishRaw(rawId);
	});
	connect(idleTimerPtr, &QTimer::timeout, this, [this, rawId](){
		FinishRaw(rawId);
	});

	socketPtr->connectToHost(host, quint16(port));

	return rawPtr->exchangeId;
}


int CDemoClientController::StartChunkedUpload(const SDemoExchange& exchange, const QUrl& logicalUrl, qint64 size, qint64 offset)
{
	if (size < 0){
		return AddFailedExchange(exchange.method, exchange.url, QCoreApplication::translate("CDemoClientController", "The size of the body is not valid"));
	}

	SDemoExchange chunkedExchange = exchange;
	chunkedExchange.requestHeaders.append(qMakePair(QByteArray("Transfer-Encoding"), QByteArray("chunked")));

	QString target = logicalUrl.path(QUrl::FullyEncoded);
	if (logicalUrl.hasQuery()){
		target += '?' + logicalUrl.query(QUrl::FullyEncoded);
	}

	QByteArray head = exchange.method + ' ' + target.toLatin1() + " HTTP/1.1\r\nHost: " + logicalUrl.authority().toLatin1() + "\r\n";
	for (const QPair<QByteArray, QByteArray>& header : chunkedExchange.requestHeaders){
		head += header.first + ": " + header.second + "\r\n";
	}

	head += "\r\n";

	std::shared_ptr<RawPending> rawPtr = std::make_shared<RawPending>();
	rawPtr->data = head;
	rawPtr->isChunkedBody = true;
	rawPtr->chunkedSize = size;
	rawPtr->chunkedOffset = offset;
	rawPtr->idleTimeoutMs = 5000;

	return StartRaw(chunkedExchange, rawPtr);
}


bool CDemoClientController::IsResponseComplete(const QByteArray& preview, qint64 totalSize, const QByteArray& tail)
{
	qsizetype position = 0;

	for (;;){
		const qsizetype headEnd = preview.indexOf("\r\n\r\n", position);
		if (headEnd < 0){
			return false;
		}

		const QByteArray head = preview.mid(position, headEnd - position);
		const int status = head.left(head.indexOf('\r')).split(' ').value(1).toInt();
		const qsizetype bodyStart = headEnd + 4;

		// The interim responses are not the answer
		if (status >= 100 && status < 200 && status != 101){
			position = bodyStart;

			continue;
		}

		if (status == 204 || status == 304 || status == 101){
			return true;
		}

		const QByteArray lowerHead = head.toLower();
		if (lowerHead.contains("transfer-encoding: chunked")){
			return tail.endsWith("0\r\n\r\n");
		}

		const qsizetype lengthPosition = lowerHead.indexOf("content-length:");
		if (lengthPosition >= 0){
			const qsizetype lineEnd = lowerHead.indexOf('\r', lengthPosition);
			const qint64 length = lowerHead.mid(lengthPosition + 15, lineEnd < 0 ? -1 : lineEnd - lengthPosition - 15).trimmed().toLongLong();

			return totalSize >= bodyStart + length;
		}

		// The end of the body is the end of the connection
		return false;
	}
}


QUrl CDemoClientController::CreateLogicalUrl(const QString& path) const
{
	QUrl base(m_serverUrl.trimmed());
	if (base.scheme().isEmpty()){
		base = QUrl(QStringLiteral("http://") + m_serverUrl.trimmed());
	}

	// The path with the query is taken as it is, no normalization
	const int queryStart = path.indexOf('?');
	base.setPath(queryStart >= 0 ? path.left(queryStart) : path, QUrl::TolerantMode);
	base.setQuery(queryStart >= 0 ? path.mid(queryStart + 1) : QString(), QUrl::TolerantMode);

	return base;
}


QString CDemoClientController::GetTargetHost() const
{
	return CreateLogicalUrl("/").host();
}


int CDemoClientController::GetTargetPort() const
{
	return CreateLogicalUrl("/").port(80);
}


int CDemoClientController::AddFailedExchange(const QByteArray& method, const QString& url, const QString& errorText)
{
	SDemoExchange exchange;
	exchange.method = method;
	exchange.url = url;
	exchange.startTime = QDateTime::currentDateTime();
	exchange.isFinished = true;
	exchange.errorText = errorText;

	const int exchangeId = m_exchangesPtr->AddExchange(exchange);

	SetStatusText(errorText);
	Q_EMIT exchangeFinished(exchangeId);

	return -1;
}


void CDemoClientController::OnReadyRead(QNetworkReply* replyPtr)
{
	QHash<QNetworkReply*, std::shared_ptr<Pending>>::ConstIterator iter = m_pending.constFind(replyPtr);
	if (iter == m_pending.constEnd()){
		return;
	}

	Pending& pending = *iter.value();

	// The data are taken at once: the body of any size is not kept by the library
	const QByteArray data = replyPtr->readAll();
	if (data.isEmpty()){
		return;
	}

	if (pending.saveFilePtr != nullptr){
		pending.saveFilePtr->write(data);
	}

	pending.hashPtr->addData(QByteArrayView(data));

	if (pending.settingsRole != SR_NONE && pending.settingsBody.size() < 1048576){
		pending.settingsBody += data;
	}

	if (pending.isPatternChecked && pending.patternMismatchAt < 0){
		for (qsizetype i = 0; i < data.size(); ++i){
			if (quint8(data[i]) != DemoProtocol::GetPatternByte(pending.patternOffset + pending.receivedSize + i)){
				pending.patternMismatchAt = pending.receivedSize + i;

				break;
			}
		}
	}

	const SDemoExchange* exchangePtr = m_exchangesPtr->FindExchange(pending.exchangeId);
	if (exchangePtr == nullptr){
		pending.receivedSize += data.size();

		return;
	}

	SDemoExchange exchange = *exchangePtr;
	const bool isPreviewGrowing = exchange.responsePreview.size() < CTrafficLogModel::s_previewLimit;
	if (isPreviewGrowing){
		exchange.responsePreview += data.left(CTrafficLogModel::s_previewLimit - int(exchange.responsePreview.size()));
	}

	pending.receivedSize += data.size();
	exchange.responseSize = pending.receivedSize;

	// A fast download would make the views work for every part: the model is told at most 10 times a second
	if (isPreviewGrowing || pending.lastUpdateTimer.elapsed() > 100){
		pending.lastUpdateTimer.restart();
		m_exchangesPtr->UpdateExchange(exchange);
	}
}


void CDemoClientController::OnUploadProgress(QNetworkReply* replyPtr, qint64 sent)
{
	QHash<QNetworkReply*, std::shared_ptr<Pending>>::ConstIterator iter = m_pending.constFind(replyPtr);
	if (iter == m_pending.constEnd()){
		return;
	}

	Pending& pending = *iter.value();
	pending.sentSize = sent;

	// The break of the connection for the demonstration: the client stops the upload, the server keeps what it has received
	if (pending.abortAfter > 0 && sent >= pending.abortAfter && !pending.isAbortedByClient){
		pending.isAbortedByClient = true;
		QTimer::singleShot(0, replyPtr, [replyPtr](){
			replyPtr->abort();
		});
	}

	if (pending.lastUpdateTimer.elapsed() < 100){
		return;
	}

	pending.lastUpdateTimer.restart();

	const SDemoExchange* exchangePtr = m_exchangesPtr->FindExchange(pending.exchangeId);
	if (exchangePtr != nullptr){
		SDemoExchange exchange = *exchangePtr;
		exchange.requestSentSize = sent;
		m_exchangesPtr->UpdateExchange(exchange);
	}
}


void CDemoClientController::OnFinished(QNetworkReply* replyPtr)
{
	QHash<QNetworkReply*, std::shared_ptr<Pending>>::Iterator iter = m_pending.find(replyPtr);
	if (iter == m_pending.end()){
		return;
	}

	std::shared_ptr<Pending> pendingPtr = iter.value();
	m_pending.erase(iter);

	// What is left in the buffer
	const QByteArray rest = replyPtr->readAll();
	const SDemoExchange* exchangePtr = m_exchangesPtr->FindExchange(pendingPtr->exchangeId);
	SDemoExchange exchange = exchangePtr != nullptr ? *exchangePtr : SDemoExchange();

	if (!rest.isEmpty()){
		if (pendingPtr->saveFilePtr != nullptr){
			pendingPtr->saveFilePtr->write(rest);
		}

		pendingPtr->hashPtr->addData(QByteArrayView(rest));

		if (pendingPtr->isPatternChecked && pendingPtr->patternMismatchAt < 0){
			for (qsizetype i = 0; i < rest.size(); ++i){
				if (quint8(rest[i]) != DemoProtocol::GetPatternByte(pendingPtr->patternOffset + pendingPtr->receivedSize + i)){
					pendingPtr->patternMismatchAt = pendingPtr->receivedSize + i;

					break;
				}
			}
		}

		if (exchange.responsePreview.size() < CTrafficLogModel::s_previewLimit){
			exchange.responsePreview += rest.left(CTrafficLogModel::s_previewLimit - int(exchange.responsePreview.size()));
		}

		pendingPtr->receivedSize += rest.size();
	}

	if (pendingPtr->saveFilePtr != nullptr){
		pendingPtr->saveFilePtr->close();
	}

	exchange.isFinished = true;
	exchange.durationMs = pendingPtr->timer.elapsed();
	exchange.responseSize = pendingPtr->receivedSize;
	exchange.statusCode = replyPtr->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	exchange.reasonPhrase = replyPtr->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toByteArray();
	exchange.requestSentSize = qMax(exchange.requestSentSize, qMax<qint64>(0, exchange.requestBodySize));

	// The upload that was stopped sent a part of the body only
	if (pendingPtr->isAbortedByClient){
		exchange.requestSentSize = pendingPtr->sentSize;
	}

	const double seconds = double(qMax<qint64>(1, exchange.durationMs)) / 1000.0;
	exchange.requestSpeed = 0;
	exchange.responseSpeed = 0;
	exchange.requestAverageSpeed = double(exchange.requestSentSize) / seconds;
	exchange.responseAverageSpeed = double(exchange.responseSize) / seconds;

	exchange.responseHeaders.clear();
	const QList<QNetworkReply::RawHeaderPair> headers = replyPtr->rawHeaderPairs();
	for (const QNetworkReply::RawHeaderPair& header : headers){
		exchange.responseHeaders.append({header.first, header.second});
	}

	const QSslConfiguration sslConfiguration = replyPtr->sslConfiguration();
	if (!sslConfiguration.isNull() && !sslConfiguration.sessionCipher().isNull()){
		exchange.tlsText = QStringLiteral("%1, %2, %3").arg(
					QSslSocket::sslLibraryVersionString().section(' ', 0, 1),
					sslConfiguration.sessionCipher().protocolString() + ' ' + sslConfiguration.sessionCipher().name(),
					QCoreApplication::translate("CDemoClientController", "certificates from the server: %1").arg(sslConfiguration.peerCertificateChain().size()));
	}

	// The status 4xx and 5xx is the answer of the server, it is not an error of the client
	if (exchange.statusCode == 0 && replyPtr->error() != QNetworkReply::NoError){
		exchange.errorText = replyPtr->errorString();

		if (!pendingPtr->sslErrorText.isEmpty()){
			exchange.errorText += QStringLiteral(" (%1)").arg(pendingPtr->sslErrorText);
		}

		if (pendingPtr->isAbortedByClient){
			exchange.errorText = QCoreApplication::translate("CDemoClientController", "The upload was stopped by the client after %1 bytes (the option abortAfter)").arg(pendingPtr->sentSize);
		}
	}

	exchange.verificationText = QStringLiteral("SHA-256 %1").arg(QString::fromLatin1(pendingPtr->hashPtr->result().toHex()));
	if (pendingPtr->isPatternChecked){
		exchange.verificationText += pendingPtr->patternMismatchAt < 0
					? QCoreApplication::translate("CDemoClientController", ", the pattern of the demo server is right (from the position %1)").arg(pendingPtr->patternOffset)
					: QCoreApplication::translate("CDemoClientController", ", the pattern DIFFERS at the byte %1").arg(pendingPtr->patternMismatchAt);
	}

	if (pendingPtr->saveFilePtr != nullptr){
		exchange.verificationText += QCoreApplication::translate("CDemoClientController", ", saved to %1").arg(pendingPtr->saveFilePtr->fileName());
	}

	m_exchangesPtr->UpdateExchange(exchange);

	if (pendingPtr->isInfoRequest){
		if (exchange.statusCode == 200){
			m_serverInfo = QString::fromUtf8(QJsonDocument::fromJson(exchange.responsePreview).toJson(QJsonDocument::Indented));
			Q_EMIT serverInfoChanged();
			SetStatusText([this](){
				return QCoreApplication::translate("CDemoClientController", "Connected to %1").arg(m_serverUrl);
			});
		}
		else{
			const QString reason = exchange.errorText.isEmpty() ? QString::number(exchange.statusCode) : exchange.errorText;
			SetStatusText([this, reason](){
				return QCoreApplication::translate("CDemoClientController", "There is no answer from %1: %2").arg(m_serverUrl, reason);
			});
		}
	}

	if (pendingPtr->settingsRole != SR_NONE){
		OnSettingsResponse(pendingPtr->settingsRole, exchange, pendingPtr->settingsBody);
	}

	if (pendingPtr->isResumeState){
		OnResumeStateResponse(*pendingPtr, exchange);
	}

	// The server has not noticed yet that the connection of the broken upload is closed: the part is sent again in a moment
	if (pendingPtr->resumeAttempt > 0 && exchange.statusCode == 409 && pendingPtr->resumeAttempt < 30){
		bool isInProgress = false;
		for (const QPair<QByteArray, QByteArray>& header : std::as_const(exchange.responseHeaders)){
			isInProgress = isInProgress || (header.first.toLower() == "upload-state" && header.second == "in-progress");
		}

		bool isOffsetMismatch = false;
		for (const QPair<QByteArray, QByteArray>& header : std::as_const(exchange.responseHeaders)){
			isOffsetMismatch = isOffsetMismatch || (header.first.toLower() == "upload-state" && header.second == "offset-mismatch");
		}

		if (isInProgress){
			QVariantMap again = pendingPtr->resumePartOptions;
			again.insert("resumeAttempt", pendingPtr->resumeAttempt + 1);
			QTimer::singleShot(500, this, [this, again](){
				sendRequest(again);
			});
		}
		else if (isOffsetMismatch && !pendingPtr->resumeOriginalOptions.isEmpty()){
			// The server has more bytes than the state said: the position is asked again
			QVariantMap again = pendingPtr->resumeOriginalOptions;
			again.insert("resumeStateAttempt", pendingPtr->resumeAttempt + 1);
			QTimer::singleShot(300, this, [this, again](){
				resumeUpload(again);
			});
		}
	}

	replyPtr->deleteLater();
	UpdateBusy();

	Q_EMIT exchangeFinished(pendingPtr->exchangeId);
}


void CDemoClientController::SendNextRawPart(int rawId)
{
	QHash<int, std::shared_ptr<RawPending>>::ConstIterator iter = m_rawPending.constFind(rawId);
	if (iter == m_rawPending.constEnd() || iter.value()->socketPtr == nullptr || iter.value()->isFinished){
		return;
	}

	RawPending& raw = *iter.value();
	if (raw.socketPtr->state() != QAbstractSocket::ConnectedState){
		return;
	}

	if (raw.isChunkedBody){
		// The head, then the chunks while the socket takes them (the body of any size does not wait in the memory)
		if (raw.sentSize < raw.data.size()){
			raw.socketPtr->write(raw.data);
			raw.sentSize = raw.data.size();
		}

		const qint64 chunkSize = 64 * 1024;
		while (raw.chunkedPosition < raw.chunkedSize && raw.socketPtr->bytesToWrite() < 1024 * 1024){
			const qint64 length = qMin(chunkSize, raw.chunkedSize - raw.chunkedPosition);

			raw.socketPtr->write(QByteArray::number(length, 16) + "\r\n");
			raw.socketPtr->write(DemoProtocol::CreatePatternData(raw.chunkedOffset + raw.chunkedPosition, length));
			raw.socketPtr->write("\r\n");

			raw.chunkedPosition += length;
		}

		if (raw.chunkedPosition >= raw.chunkedSize && !raw.isLastChunkWritten){
			raw.socketPtr->write("0\r\n\r\n");
			raw.isLastChunkWritten = true;

			if (raw.idleTimerPtr != nullptr){
				raw.idleTimerPtr->start(raw.idleTimeoutMs);
			}
		}

		const SDemoExchange* chunkedExchangePtr = m_exchangesPtr->FindExchange(raw.exchangeId);
		if (chunkedExchangePtr != nullptr && (raw.isLastChunkWritten || raw.chunkedPosition - chunkedExchangePtr->requestSentSize >= 256 * 1024)){
			SDemoExchange exchange = *chunkedExchangePtr;
			exchange.requestSentSize = raw.chunkedPosition;
			m_exchangesPtr->UpdateExchange(exchange);
		}

		return;
	}

	const QByteArray part = raw.partSize > 0 ? raw.data.mid(raw.sentSize, raw.partSize) : raw.data.mid(raw.sentSize);
	raw.socketPtr->write(part);
	raw.sentSize += part.size();

	const SDemoExchange* exchangePtr = m_exchangesPtr->FindExchange(raw.exchangeId);
	if (exchangePtr != nullptr){
		SDemoExchange exchange = *exchangePtr;
		exchange.requestSentSize = raw.sentSize;
		m_exchangesPtr->UpdateExchange(exchange);
	}

	if (raw.sentSize < raw.data.size()){
		QTimer::singleShot(raw.delayMs, this, [this, rawId](){
			SendNextRawPart(rawId);
		});
	}
	else if (raw.idleTimerPtr != nullptr){
		raw.idleTimerPtr->start(raw.idleTimeoutMs);
	}
}


void CDemoClientController::FinishRaw(int rawId)
{
	QHash<int, std::shared_ptr<RawPending>>::Iterator iter = m_rawPending.find(rawId);
	if (iter == m_rawPending.end() || iter.value()->isFinished){
		return;
	}

	std::shared_ptr<RawPending> rawPtr = iter.value();
	rawPtr->isFinished = true;
	m_rawPending.erase(iter);

	QString socketError;
	if (rawPtr->socketPtr != nullptr){
		// What came before the closing is taken
		const QByteArray rest = rawPtr->socketPtr->readAll();
		rawPtr->responseSize += rest.size();
		if (rawPtr->responsePreview.size() < CTrafficLogModel::s_previewLimit){
			rawPtr->responsePreview += rest.left(CTrafficLogModel::s_previewLimit - int(rawPtr->responsePreview.size()));
		}

		if (rawPtr->socketPtr->error() != QAbstractSocket::UnknownSocketError && rawPtr->socketPtr->error() != QAbstractSocket::RemoteHostClosedError){
			socketError = rawPtr->socketPtr->errorString();
		}

		rawPtr->socketPtr->abort();
		rawPtr->socketPtr->deleteLater();
	}

	const SDemoExchange* exchangePtr = m_exchangesPtr->FindExchange(rawPtr->exchangeId);
	SDemoExchange exchange = exchangePtr != nullptr ? *exchangePtr : SDemoExchange();
	exchange.isFinished = true;
	exchange.durationMs = rawPtr->timer.elapsed();
	exchange.requestSentSize = rawPtr->isChunkedBody ? rawPtr->chunkedPosition : rawPtr->sentSize;
	exchange.requestSpeed = 0;
	exchange.responseSpeed = 0;
	exchange.requestAverageSpeed = double(exchange.requestSentSize) / (double(qMax<qint64>(1, exchange.durationMs)) / 1000.0);
	exchange.errorText = socketError;

	// The response is described if it is an HTTP response, otherwise the raw bytes are shown. The interim responses (100) are skipped:
	// they are on the wire, but they are not the answer.
	const QByteArray& response = rawPtr->responsePreview;
	qsizetype headStart = 0;
	qsizetype headEnd = response.indexOf("\r\n\r\n");

	while (response.mid(headStart).startsWith("HTTP/") && headEnd > headStart){
		const int status = response.mid(headStart, headEnd - headStart).split(' ').value(1).toInt();
		if (status < 100 || status >= 200 || status == 101){
			break;
		}

		headStart = headEnd + 4;
		headEnd = response.indexOf("\r\n\r\n", headStart);
	}

	if (response.mid(headStart).startsWith("HTTP/") && headEnd > headStart){
		const QByteArrayList lines = response.mid(headStart, headEnd - headStart).split('\n');
		const QByteArrayList statusLine = lines.first().trimmed().split(' ');
		exchange.statusCode = statusLine.value(1).toInt();
		exchange.reasonPhrase = statusLine.mid(2).join(' ');

		exchange.responseHeaders.clear();
		for (int i = 1; i < lines.size(); ++i){
			const QByteArray line = lines[i].trimmed();
			const qsizetype colon = line.indexOf(':');
			if (colon > 0){
				exchange.responseHeaders.append({line.left(colon).trimmed(), line.mid(colon + 1).trimmed()});
			}
		}

		exchange.responsePreview = response.mid(headEnd + 4);
		exchange.responseSize = qMax<qint64>(0, rawPtr->responseSize - (headEnd + 4));
	}
	else{
		exchange.responsePreview = response;
		exchange.responseSize = rawPtr->responseSize;

		if (response.isEmpty() && exchange.errorText.isEmpty()){
			exchange.errorText = QCoreApplication::translate("CDemoClientController", "The server has closed the connection without a response");
		}
	}

	m_exchangesPtr->UpdateExchange(exchange);
	UpdateBusy();

	Q_EMIT exchangeFinished(rawPtr->exchangeId);
}


void CDemoClientController::SetStatusText(const std::function<QString()>& builder)
{
	m_statusBuilder = builder;

	SetStatusText(builder());

	// The text above has reset the builder, the status is made again after the change of the language
	m_statusBuilder = builder;
}


void CDemoClientController::SetStatusText(const QString& text)
{
	m_statusBuilder = nullptr;

	if (m_statusText != text){
		m_statusText = text;

		Q_EMIT statusTextChanged();
	}
}


void CDemoClientController::MeasureSpeed(SpeedState& state, qint64 sentSize, qint64 receivedSize, double seconds)
{
	if (seconds <= 0){
		return;
	}

	// The average of the last measurement and the previous speed: the number does not jump between the parts
	const double sentNow = double(sentSize - state.lastSentSize) / seconds;
	const double receivedNow = double(receivedSize - state.lastReceivedSize) / seconds;

	state.sentSpeed = state.sentSpeed > 0 ? (state.sentSpeed + sentNow) / 2 : sentNow;
	state.receivedSpeed = state.receivedSpeed > 0 ? (state.receivedSpeed + receivedNow) / 2 : receivedNow;
	state.lastSentSize = sentSize;
	state.lastReceivedSize = receivedSize;
}


void CDemoClientController::OnSpeedTick()
{
	const double seconds = double(m_speedElapsedTimer.restart()) / 1000.0;

	for (std::shared_ptr<Pending>& pendingPtr : m_pending){
		MeasureSpeed(pendingPtr->speed, pendingPtr->sentSize, pendingPtr->receivedSize, seconds);

		const SDemoExchange* exchangePtr = m_exchangesPtr->FindExchange(pendingPtr->exchangeId);
		if (exchangePtr != nullptr){
			SDemoExchange exchange = *exchangePtr;
			exchange.requestSpeed = pendingPtr->speed.sentSpeed;
			exchange.responseSpeed = pendingPtr->speed.receivedSpeed;
			exchange.requestSentSize = qMax(exchange.requestSentSize, pendingPtr->sentSize);
			exchange.responseSize = qMax(exchange.responseSize, pendingPtr->receivedSize);
			m_exchangesPtr->UpdateExchange(exchange);
		}
	}

	for (std::shared_ptr<RawPending>& rawPtr : m_rawPending){
		const qint64 sentSize = rawPtr->isChunkedBody ? rawPtr->chunkedPosition : rawPtr->sentSize;

		MeasureSpeed(rawPtr->speed, sentSize, rawPtr->responseSize, seconds);

		const SDemoExchange* exchangePtr = m_exchangesPtr->FindExchange(rawPtr->exchangeId);
		if (exchangePtr != nullptr){
			SDemoExchange exchange = *exchangePtr;
			exchange.requestSpeed = rawPtr->speed.sentSpeed;
			exchange.responseSpeed = rawPtr->speed.receivedSpeed;
			exchange.requestSentSize = qMax(exchange.requestSentSize, sentSize);
			exchange.responseSize = qMax(exchange.responseSize, rawPtr->responseSize);
			m_exchangesPtr->UpdateExchange(exchange);
		}
	}
}


void CDemoClientController::SetSettingsStatus(const QString& text)
{
	if (m_settingsStatus != text){
		m_settingsStatus = text;

		Q_EMIT settingsStatusChanged();
	}
}


void CDemoClientController::OnSettingsResponse(int role, const SDemoExchange& exchange, const QByteArray& responseBody)
{
	const QJsonObject body = QJsonDocument::fromJson(responseBody).object();

	if (role == SR_APPLY){
		if (exchange.statusCode != 200 && m_isTrustChanging){
			m_isTrustChanging = false;
			SetTrustedCertificates(m_trustedBeforeApply);
		}

		if (exchange.statusCode != 200){
			SetSettingsStatus(QCoreApplication::translate("CDemoClientController", "The server has not accepted the settings: %1").arg(
						exchange.statusCode == 0 ? exchange.errorText : body.value("error").toString(QString::number(exchange.statusCode))));

			return;
		}

		if (!body.value("restartScheduled").toBool()){
			SetSettingsStatus(QCoreApplication::translate("CDemoClientController", "Nothing is changed"));
			loadServerSettings();

			return;
		}

		// After the restart the server is on the new port with the new token, or, if it could not start, on the old ones
		QUrl newUrl(m_serverUrl);
		newUrl.setPort(body.value("port").toInt(newUrl.port()));
		newUrl.setScheme(body.value("scheme").toString(newUrl.scheme()));

		m_reloadCandidates.clear();
		m_reloadCandidates.append(qMakePair(newUrl.toString(), body.value("token").toString(m_token)));
		m_reloadCandidates.append(qMakePair(m_serverUrl, m_token));
		m_reloadAttempt = 0;

		SetSettingsStatus(QCoreApplication::translate("CDemoClientController", "The settings are accepted, the server restarts ..."));
		m_reloadTimer.start(body.value("restartDelayMs").toInt(300) + 200);

		return;
	}

	if (exchange.statusCode != 200){
		if (role == SR_RELOAD){
			m_reloadTimer.start(400);
		}
		else{
			SetSettingsStatus(QCoreApplication::translate("CDemoClientController", "The settings are not read: %1").arg(
						exchange.statusCode == 0 ? exchange.errorText : body.value("error").toString(QString::number(exchange.statusCode))));
		}

		return;
	}

	m_serverSettings = body.toVariantMap();
	Q_EMIT serverSettingsChanged();

	const QString lastError = body.value("lastError").toString();
	if (role == SR_RELOAD && m_isTrustChanging){
		m_isTrustChanging = false;
		SetTrustedCertificates(lastError.isEmpty() ? m_trustedAfterApply : m_trustedBeforeApply);
	}

	if (role == SR_RELOAD){
		SetSettingsStatus(lastError.isEmpty()
					? QCoreApplication::translate("CDemoClientController", "The settings are applied, the server works with them")
					: QCoreApplication::translate("CDemoClientController", "The server could not start with the new settings and works with the previous ones: %1").arg(lastError));
	}
	else{
		SetSettingsStatus(QCoreApplication::translate("CDemoClientController", "The settings are read"));
	}
}


void CDemoClientController::TryReloadSettings()
{
	const int maxAttempts = 16;
	if (m_reloadAttempt >= maxAttempts){
		SetSettingsStatus(QCoreApplication::translate("CDemoClientController", "The server does not answer after the restart"));

		return;
	}

	// The new address is tried at first, then the old one: the server goes back to it if the new settings are not started
	const QPair<QString, QString> candidate = m_reloadCandidates.value(m_reloadAttempt % m_reloadCandidates.size());
	++m_reloadAttempt;

	SetServerUrl(candidate.first);
	SetToken(candidate.second);

	QVariantMap options;
	options.insert("method", "GET");
	options.insert("path", "/admin/settings");
	options.insert("useToken", true);
	options.insert("timeoutMs", 2000);
	options.insert("settingsRole", int(SR_RELOAD));
	sendRequest(options);
}


void CDemoClientController::UpdateBusy()
{
	const bool isBusy = !m_pending.isEmpty() || !m_rawPending.isEmpty();

	if (isBusy && !m_speedTimer.isActive()){
		m_speedElapsedTimer.start();
		m_speedTimer.start();
	}

	if (!isBusy){
		m_speedTimer.stop();
	}

	if (m_isBusy != isBusy){
		m_isBusy = isBusy;

		Q_EMIT isBusyChanged();
	}
}


} // namespace imtrestdemo
