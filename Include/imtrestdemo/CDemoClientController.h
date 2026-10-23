// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// STL includes
#include <functional>
#include <memory>

// Qt includes
#include <QtCore/QCryptographicHash>
#include <QtCore/QElapsedTimer>
#include <QtCore/QFile>
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <QtCore/QUrl>
#include <QtCore/QVariantMap>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslError>
#include <QtNetwork/QTcpSocket>

// ImtCore includes
#include <imtrestdemo/CTrafficLogModel.h>
#include <imtrestdemo/CWireCaptureProxy.h>
#include <imtrestdemo/CWireLogModel.h>


namespace imtrestdemo
{


/**
	Logic of the demo client of imtrest: sends the requests to the demo server (or to any HTTP server) and keeps two logs for
	the debug preview:
	- the exchanges (CTrafficLogModel): what the application asked and what it got, with the check of the bodies;
	- the wire (CWireLogModel): the exact bytes that were sent and received. They are taken by the proxy that is between the client
	  and the server (CWireCaptureProxy), so they include everything that the client library added: the headers, the framing of
	  the chunks, \c Expect: 100-continue and the interim responses.
	The class has no dependence on QML: the QML interface of the application uses it as a context property, and the tests use it directly.

	Options of sendRequest():
	- \c method, \c path (with the query), \c headers (the lines \c Name: value), \c useToken, \c expectContinue, \c followRedirects, \c timeoutMs;
	- \c bodyType: \c none, \c text (\c bodyText), \c file (\c bodyFile) or \c generated (\c bodySize bytes of the pattern from
	  \c bodyOffset, sent with \c Transfer-Encoding: chunked if \c isChunked);
	- \c saveTo: the file for the body of the response, otherwise it is only counted and checked;
	- \c checkPattern: check that the body of the response is the pattern of the demo server, starting from the position \c patternOffset.
*/
class CDemoClientController: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString serverUrl READ GetServerUrl WRITE SetServerUrl NOTIFY serverUrlChanged)
	Q_PROPERTY(QString token READ GetToken WRITE SetToken NOTIFY tokenChanged)
	Q_PROPERTY(bool captureEnabled READ IsCaptureEnabled WRITE SetCaptureEnabled NOTIFY captureEnabledChanged)
	Q_PROPERTY(bool isBusy READ IsBusy NOTIFY isBusyChanged)
	Q_PROPERTY(QString statusText READ GetStatusText NOTIFY statusTextChanged)
	Q_PROPERTY(QString serverInfo READ GetServerInfo NOTIFY serverInfoChanged)
	Q_PROPERTY(QVariantMap serverSettings READ GetServerSettings NOTIFY serverSettingsChanged)
	Q_PROPERTY(QStringList trustedCertificates READ GetTrustedCertificates WRITE SetTrustedCertificates NOTIFY trustedCertificatesChanged)
	Q_PROPERTY(QString settingsStatus READ GetSettingsStatus NOTIFY settingsStatusChanged)
	Q_PROPERTY(QObject* exchanges READ GetExchangesObject CONSTANT)
	Q_PROPERTY(QObject* wire READ GetWireObject CONSTANT)
public:
	explicit CDemoClientController(QObject* parentPtr = nullptr);
	~CDemoClientController();

	QString GetServerUrl() const;
	void SetServerUrl(const QString& serverUrl);
	QString GetToken() const;
	void SetToken(const QString& token);
	bool IsCaptureEnabled() const;
	void SetCaptureEnabled(bool isEnabled);
	bool IsBusy() const;
	QString GetStatusText() const;
	QString GetServerInfo() const;
	QVariantMap GetServerSettings() const;
	QString GetSettingsStatus() const;

	/**
		Certificates that the client trusts for https: every item is a path of a PEM file or the PEM text itself (the sets of the test certificates
		that the server shows have the text, so the client does not need the files of the server). Only these certificates are used: the certificates
		of the system and its keychain are neither used nor changed, so the trust is the decision of the application. The list is empty by default,
		then no certificate of a server is accepted.
	*/
	QStringList GetTrustedCertificates() const;
	void SetTrustedCertificates(const QStringList& files);

	CTrafficLogModel* GetExchanges() const;
	CWireLogModel* GetWire() const;

	// The QML properties are of the type QObject*
	QObject* GetExchangesObject() const
	{
		return m_exchangesPtr;
	}

	QObject* GetWireObject() const
	{
		return m_wirePtr;
	}

	/**
		Send a request, see the options in the description of the class.
		\return The id of the exchange in the log (a negative value if the request is not started, the error is in the log).
	*/
	Q_INVOKABLE int sendRequest(const QVariantMap& options);

	/**
		Send the bytes as they are, without HTTP client library: for the requests that the library does not make (in parts, wrong).
		Options: \c text (the escapes \\r \\n \\t \\xNN are replaced by the bytes), \c autoCrlf (a line break of the text is CR LF),
		\c partSize and \c delayMs (send by parts with a pause), \c idleTimeoutMs (close the connection when the server is silent so long).
		\return The id of the exchange.
	*/
	Q_INVOKABLE int sendRaw(const QVariantMap& options);

	/**
		Ask the server for \c /info.
	*/
	Q_INVOKABLE void testConnection();

	/**
		Read the settings of the running server (\c GET /admin/settings with the token), the result is in \c serverSettings.
	*/
	Q_INVOKABLE void loadServerSettings();

	/**
		Change the settings of the running server (\c PUT /admin/settings with the token): only the settings of the map are sent.
		The server restarts in a moment. The controller waits for it and reads the settings again: if the port or the token is changed,
		\c serverUrl and \c token are changed too; if the server could not start with the new settings and went back to the previous ones,
		the reason is in \c settingsStatus.
	*/
	Q_INVOKABLE void applyServerSettings(const QVariantMap& changes);

	/**
		Send a file that was not sent to the end (the connection broke, the upload was stopped) or was not sent at all, so that only the rest goes
		to the server: the client asks the server how much of the file it has (\c GET of the path, \c received) and sends the bytes from that
		position with \c Content-Range. The options are the options of sendRequest() of an upload (\c path, \c bodyType \c file or \c generated,
		\c bodyFile or \c bodySize, \c useToken, ...). If another request of the server still writes the file (the server has not noticed the
		break yet), the client waits and tries again.
		\return The id of the exchange with the question of the state.
	*/
	Q_INVOKABLE int resumeUpload(const QVariantMap& options);

	/**
		Change the settings of the TLS (the same \c applyServerSettings()) and the trust of the client with them: while the server restarts the client
		trusts the old and the new certificates, so it can read the settings from the server with either of them; after the restart it trusts only
		\c trustedFiles, or, if the server could not start with the new settings, only what it trusted before.
	*/
	Q_INVOKABLE void applyTlsSettings(const QVariantMap& changes, const QStringList& trustedFiles);

	/**
		Switch the server to one of its sets of the test certificates (the item of \c serverSettings.tlsSamples) with one request and trust its
		certificates (\c trustedCertificates are the items of \c trustedPem of the set, or paths of files).
	*/
	Q_INVOKABLE void applyTlsPreset(const QString& presetId, const QStringList& trustedCertificates);

	/**
		Send the certificates and the key of this computer to the server: the files are read here and their content goes to the server, which keeps
		it in its own directory and uses it (the server can be on another computer). The key is not shown in the log of the exchanges.
		\param certificateFile	PEM file of the certificate (can have the whole chain).
		\param chainFiles	PEM files of the intermediate certificates.
		\param keyFile	PEM file of the private key, without a password.
		\param trustedCertificates	What the client trusts after that (paths of files or PEM texts), for example the file of the root.
	*/
	Q_INVOKABLE void applyTlsFiles(const QString& certificateFile, const QStringList& chainFiles, const QString& keyFile, const QStringList& trustedCertificates);

	/**
		Get the command of curl that does the same as the request (with the options of sendRequest()).
	*/
	Q_INVOKABLE QString makeCurlCommand(const QVariantMap& options) const;

	Q_INVOKABLE void clear();

	/**
		Refresh the texts of the logs after the change of the language.
	*/
	void Retranslate();
	Q_INVOKABLE void abortAll();

	/**
		Decode the text of a raw request: \c \\r, \c \\n, \c \\t, \c \\\\ and \c \\xNN are replaced by the bytes.
	*/
	static QByteArray DecodeRawText(const QString& text, bool isAutoCrlf);

	/**
		Parse the lines \c Name: value.
	*/
	static SDemoExchange::Headers ParseHeaders(const QString& text);

	/**
		Check that the final response is received completely. The interim responses (with the status 1xx) are skipped; the response
		is complete by \c Content-Length, by the last chunk or, if the response has no body (HEAD, 204, 304, the status 1xx), by its head.
		\param	preview		The beginning of the received bytes (the heads are in it).
		\param	totalSize	Number of all received bytes.
		\param	tail		The last received bytes (at least 5, if there are so many).
	*/
	static bool IsResponseComplete(const QByteArray& preview, qint64 totalSize, const QByteArray& tail);

Q_SIGNALS:
	void serverUrlChanged();
	void tokenChanged();
	void captureEnabledChanged();
	void isBusyChanged();
	void statusTextChanged();
	void serverInfoChanged();
	void serverSettingsChanged();
	void settingsStatusChanged();
	void trustedCertificatesChanged();
	void exchangeFinished(int exchangeId);

private:
	// The role of a request in the changing of the settings of the server
	enum SettingsRole
	{
		SR_NONE = 0,
		SR_LOAD,
		SR_APPLY,
		SR_RELOAD
	};

	// State of the measurement of the speed of an exchange between two measurements
	struct SpeedState
	{
		qint64 lastSentSize = 0;
		qint64 lastReceivedSize = 0;
		double sentSpeed = 0;
		double receivedSpeed = 0;
	};

	struct Pending
	{
		SpeedState speed;
		qint64 sentSize = 0;
		int exchangeId = 0;
		QElapsedTimer timer;
		QElapsedTimer lastUpdateTimer;
		bool isInfoRequest = false;
		int settingsRole = SR_NONE;
		QString sslErrorText;

		// The upload that is stopped by the client after this number of bytes (a break of the connection for the demonstration), 0 - no
		qint64 abortAfter = 0;
		bool isAbortedByClient = false;

		// The question about the state of an upload: after the answer the rest of the file is sent with these options
		bool isResumeState = false;
		QVariantMap resumeOptions;
		qint64 resumeTotal = 0;
		int resumeStateAttempt = 0;

		// The part of the resumed upload: the number of the attempt, and the options to repeat it
		int resumeAttempt = 0;
		QVariantMap resumePartOptions;
		QVariantMap resumeOriginalOptions;

		// The whole body of the answer of /admin/settings, the preview of the log is shorter
		QByteArray settingsBody;

		std::unique_ptr<QFile> saveFilePtr;
		bool isPatternChecked = false;
		qint64 patternOffset = 0;
		qint64 patternMismatchAt = -1;
		qint64 receivedSize = 0;
		std::unique_ptr<QCryptographicHash> hashPtr;
	};

	struct RawPending
	{
		SpeedState speed;
		int exchangeId = 0;
		QPointer<QTcpSocket> socketPtr;
		QPointer<QTimer> idleTimerPtr;
		QByteArray data;
		qint64 sentSize = 0;
		int partSize = 0;
		int delayMs = 0;
		int idleTimeoutMs = 2000;
		QElapsedTimer timer;
		QByteArray responsePreview;
		QByteArray responseTail;
		qint64 responseSize = 0;
		bool isFinished = false;

		/**
			The body of the request is generated and is sent in the chunks after the head (\c data): a client library sends a sequential
			body with the known length (it collects the body to know it), so the chunks are written by the controller.
		*/
		bool isChunkedBody = false;
		qint64 chunkedSize = 0;
		qint64 chunkedOffset = 0;
		qint64 chunkedPosition = 0;
		bool isLastChunkWritten = false;
	};

	bool EnsureProxy();
	int StartRaw(const SDemoExchange& exchange, const std::shared_ptr<RawPending>& rawPtr);
	int StartChunkedUpload(const SDemoExchange& exchange, const QUrl& logicalUrl, qint64 size, qint64 offset);
	QUrl CreateLogicalUrl(const QString& path) const;
	QString GetTargetHost() const;
	int GetTargetPort() const;
	int AddFailedExchange(const QByteArray& method, const QString& url, const QString& errorText);
	void OnReadyRead(QNetworkReply* replyPtr);
	void OnFinished(QNetworkReply* replyPtr);
	void OnUploadProgress(QNetworkReply* replyPtr, qint64 sent);
	void SendNextRawPart(int rawId);
	void FinishRaw(int rawId);
	void SetStatusText(const QString& text);

	/**
		Set the status that is made again after the change of the language.
	*/
	void SetStatusText(const std::function<QString()>& builder);
	void UpdateBusy();
	void OnSpeedTick();
	void OnResumeStateResponse(const Pending& pending, const SDemoExchange& exchange);
	void SetSettingsStatus(const QString& text);
	void OnSettingsResponse(int role, const SDemoExchange& exchange, const QByteArray& responseBody);
	void TryReloadSettings();
	static void MeasureSpeed(SpeedState& state, qint64 sentSize, qint64 receivedSize, double seconds);

private:
	QNetworkAccessManager m_manager;
	CTrafficLogModel* m_exchangesPtr;
	CWireLogModel* m_wirePtr;
	CWireCaptureProxy* m_proxyPtr = nullptr;

	QString m_serverUrl = QStringLiteral("http://localhost:7400");
	QString m_token = QStringLiteral("demo-token");
	bool m_isCaptureEnabled = true;
	bool m_isBusy = false;
	QString m_statusText;
	QString m_serverInfo;

	QHash<QNetworkReply*, std::shared_ptr<Pending>> m_pending;
	QHash<int, std::shared_ptr<RawPending>> m_rawPending;
	int m_lastRawId = 0;

	// While the exchanges work, the speed is measured every half of a second
	// The status text that is made of a pattern (empty if the status is a fixed text)
	std::function<QString()> m_statusBuilder;

	QTimer m_speedTimer;

	// The settings of the server and the waiting for its restart after the changes
	QVariantMap m_serverSettings;
	QStringList m_trustedFiles;
	QStringList m_trustedBeforeApply;
	QStringList m_trustedAfterApply;
	bool m_isTrustChanging = false;
	QList<QSslCertificate> m_trustedCertificates;
	QString m_settingsStatus;
	QTimer m_reloadTimer;
	QList<QPair<QString, QString>> m_reloadCandidates;
	int m_reloadAttempt = 0;
	QElapsedTimer m_speedElapsedTimer;
};


} // namespace imtrestdemo


