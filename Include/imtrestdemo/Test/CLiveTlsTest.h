// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QTemporaryDir>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslError>

// Local includes
#include "CLiveTestBase.h"


/**
	Live server: TLS. The test certificates are made by the program \c openssl (the tests are skipped if it is not found): one self-signed
	certificate, a chain, several certificates in separate files, a concatenated file and an EC key. The server gets them at run time through
	\c /admin/settings (it restarts its TCP server), the client trusts only these certificates, by the decision of the application: nothing is
	put to the system or to its keychain, and the certificates of the system are not used. After every test the server works without TLS again.
*/
class CLiveTlsTest: public CLiveTestBase
{
	Q_OBJECT
private Q_SLOTS:
	void init();
	void cleanup();

	void SamplesAreMadeTest();
	void ServerFilesAreCheckedTest();
	void TlsScenarioTest_data();
	void TlsScenarioTest();
	void NoTrustIsRefusedTest();
	void WrongHostIsRefusedTest();
	void PlainHttpToTlsPortTest();
	void SwitchBetweenSamplesTest();
	void BrokenFilesTest();
	void LongBodiesOverTlsTest();
	void PresetTest_data();
	void PresetTest();
	void UnknownPresetTest();
	void UploadPemTest_data();
	void UploadPemTest();
	void StoredFilesAreCleanedTest();
	void UploadBrokenPemTest();

protected:
	// reimplemented (CLiveTestBase)
	virtual void CustomizeSettings(imtrestdemo::CDemoServerSettings& settings) override;

private:
	struct SResult
	{
		int statusCode = 0;
		QByteArray body;
		QString errorText;
		QList<QSslError::SslError> sslErrors;
		int chainSize = 0;
		QString protocol;
	};

	/**
		A request of a real client library (QNetworkAccessManager). The client trusts only the certificates of the list, the certificates of the
		system are not used. \c isVerified = \c false: the certificate of the server is not checked (only to return the server to http).
		\return \c true if an answer of the server is received.
	*/
	bool Send(
				const QByteArray& method,
				const QString& path,
				const QByteArray& body,
				bool isTls,
				const QList<QSslCertificate>& trusted,
				bool isVerified,
				SResult& result,
				const QByteArray& token = QByteArray("demo-token"),
				const QString& host = QStringLiteral("localhost")) const;

	QJsonObject GetSettings() const;
	bool PutSettings(const QJsonObject& changes, SResult& result) const;
	bool WaitForSettings(const std::function<bool(const QJsonObject&)>& isReady, QJsonObject* settingsPtr = nullptr, int timeoutMs = 15000) const;

	/**
		Give the server a set of the certificates and wait for its restart. \c sample is an object of \c DemoTlsTools::DescribeSamples().
	*/
	bool EnableTls(const QJsonObject& sample);

	/**
		Send the changes of the TLS settings and wait for the restart; after it the client trusts the certificates of the list.
	*/
	bool ApplyTls(const QJsonObject& changes, const QList<QSslCertificate>& trusted, bool isVerified = true);

	QString GetStorageDirectory() const;
	int CountStoredDirectories() const;
	static QString ReadText(const QString& path);

	QJsonObject FindSample(const QString& id) const;
	static QList<QSslCertificate> ReadTrusted(const QJsonObject& sample);
	static QJsonObject CreateTlsChanges(const QJsonObject& sample);

private:
	QTemporaryDir m_samplesDirectory;
	bool m_isSamplesReady = false;
	QJsonArray m_samples;
	bool m_isTls = false;
	bool m_isVerified = true;
	QList<QSslCertificate> m_trustedNow;
};

