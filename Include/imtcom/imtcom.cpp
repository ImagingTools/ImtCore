#pragma once


// Qt includes
#include <QtCore/QFile>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslCertificate>

// ImtCore includes
#include <imtcom/ISslConfiguration.h>
#include <imtcom/ISslKey.h>
#include <imtcom/ISslCertificate.h>

namespace imtcom
{


QSharedPointer<QSslKey> CreateSslKey(const ISslKey& sslKey)
{
	QSharedPointer<QSslKey> retVal;

	// setup algorithm
	ISslKey::KeyAlgorithm algorithm = sslKey.GetKeyAlgorithm();
	QSsl::KeyAlgorithm qAlgorithm;
	switch (algorithm) {
	case ISslKey::KA_RSA:		qAlgorithm = QSsl::Rsa;		break;
	case ISslKey::KA_DSA:		qAlgorithm = QSsl::Dsa;		break;
	case ISslKey::KA_EC:		qAlgorithm = QSsl::Ec;		break;
	case ISslKey::KA_DH:		qAlgorithm = QSsl::Dh;		break;
	case ISslKey::KA_OPAQUE:	qAlgorithm = QSsl::Opaque;	break;
	default:
		Q_ASSERT_X(false, "Setup SSL algorithm", "Unexpected algorithm.");
		return retVal;
	}

	// setup format
	ISslKey::EncodingFormat format = sslKey.GetEncodingFormat();
	QSsl::EncodingFormat qFormat;
	switch (format) {
	case ISslKey::EF_DER: qFormat = QSsl::Der; break;
	case ISslKey::EF_PEM: qFormat = QSsl::Pem; break;
	default:
		Q_ASSERT_X(false, "Setup SSL format", "Unexpected format.");
		return retVal;
	}

	// setup type
	ISslKey::KeyType type = sslKey.GetKeyType();
	QSsl::KeyType qType;
	switch (type) {
	case imtcom::ISslKey::KT_PRIVATE_KEY: qType = QSsl::PrivateKey; break;
	case imtcom::ISslKey::KT_PUBLIC_KEY:  qType = QSsl::PublicKey;  break;
	default:
		Q_ASSERT_X(false, "Setup SSL type", "Unexpected type.");
		return retVal;
	}

	const QByteArray passPhrase = sslKey.GetPassPhrase();

	if (sslKey.IsFile()){
		QFile keyFile(sslKey.GetFilePath());
		if (!keyFile.open(QIODevice::ReadOnly)){
			Q_ASSERT_X(false, "Open SSL key file", "Unable to open SSL key file.");
			return retVal;
		}
		retVal.reset(new QSslKey(&keyFile, qAlgorithm, qFormat, qType, passPhrase));
	}
	else {
		const QByteArray encodedData = sslKey.GetEncodedData();
		retVal.reset(new QSslKey(encodedData, qAlgorithm, qFormat, qType, passPhrase));
	}

	return retVal;
}


QSharedPointer<QSslCertificate> CreateSslCertificate(const ISslCertificate& sslCertificate)
{
	QSharedPointer<QSslCertificate> retVal;

	// setup format
	ISslCertificate::EncodingFormat format = sslCertificate.GetEncodingFormat();
	QSsl::EncodingFormat qFormat;
	switch (format) {
	case ISslCertificate::EF_DER: qFormat = QSsl::Der; break;
	case ISslCertificate::EF_PEM: qFormat = QSsl::Pem; break;
	default:
		Q_ASSERT_X(false, "Setup SSL format", "Unexpected format.");
		return retVal;
	}

	if (sslCertificate.IsFile()){
		QFile keyFile(sslCertificate.GetFilePath());
		if (!keyFile.open(QIODevice::ReadOnly)){
			Q_ASSERT_X(false, "Open SSL key file", "Unable to open SSL key file.");
			return retVal;
		}
		retVal.reset(new QSslCertificate(&keyFile, qFormat));
	}
	else {
		const QByteArray encodedData = sslCertificate.GetEncodedData();
		retVal.reset(new QSslCertificate(encodedData, qFormat));
	}

	return retVal;
}


void CreateSslConfiguration(const ISslConfiguration& input, QSslConfiguration& output)
{
	// setup local certificate
	const ISslConfiguration::SslCertificatePtr localCertificatePtr = input.GetLocalCertificate();
	if (localCertificatePtr != nullptr){
		QSharedPointer<QSslCertificate> sslCertificalePtr = CreateSslCertificate(*localCertificatePtr);
		if (sslCertificalePtr.isNull()){
			Q_ASSERT_X(false, "Setup Key", "Unable to setup SSL certificate.");
			return;
		}
		output.setLocalCertificate(*sslCertificalePtr);
	}

	// setup local certificate chain
	const ISslConfiguration::SslCertificatePtrList localCertificateChainPtrList = input.GetLocalCertificateChain();
	QList<QSslCertificate> localCertificateChain;
	for (const ISslConfiguration::SslCertificatePtr& localSslCertificatePtr: localCertificateChainPtrList){
		QSharedPointer<QSslCertificate> sslCertificalePtr = CreateSslCertificate(*localSslCertificatePtr);
		if (sslCertificalePtr.isNull()){
			Q_ASSERT_X(false, "Setup Key", "Unable to setup SSL certificate.");
			return;
		}
		localCertificateChain << (*sslCertificalePtr);
	}
	if (!localCertificateChain.isEmpty()){
		output.setLocalCertificateChain(localCertificateChain);
	}

	// setup CA certificate
	const ISslConfiguration::SslCertificatePtr caCertificatePtr = input.GetCaCertificate();
	if (caCertificatePtr != nullptr){
		QSharedPointer<QSslCertificate> sslCaCertificalePtr = CreateSslCertificate(*caCertificatePtr);
		if (sslCaCertificalePtr.isNull()){
			Q_ASSERT_X(false, "Setup Key", "Unable to setup SSL CA certificate.");
			return;
		}
		output.addCaCertificate(*sslCaCertificalePtr);
	}

	// setup CA certificate chain
	const ISslConfiguration::SslCertificatePtrList caCertificateChainPtrList = input.GetCaCertificateChain();
	QList<QSslCertificate> caCertificateChain;
	for (const ISslConfiguration::SslCertificatePtr& caSslCertificatePtr: caCertificateChainPtrList){
		QSharedPointer<QSslCertificate> sslCertificalePtr = CreateSslCertificate(*caSslCertificatePtr);
		if (sslCertificalePtr.isNull()){
			Q_ASSERT_X(false, "Setup Key", "Unable to setup SSL certificate.");
			return;
		}
		caCertificateChain << (*sslCertificalePtr);
	}
	if (!caCertificateChain.isEmpty()){
		output.setCaCertificates(caCertificateChain);
	}

	// setup private key
	const ISslConfiguration::SslKeyPtr privateKeyPtr = input.GetPrivateKey();
	if (privateKeyPtr != nullptr){
		QSharedPointer<QSslKey> qSslKeyPtr = CreateSslKey(*privateKeyPtr);
		if (qSslKeyPtr.isNull()){
			Q_ASSERT_X(false, "Setup Key", "Unable to setup SSL key.");
			return;
		}
		output.setPrivateKey(*qSslKeyPtr);
	}

	// setup verify mode
	ISslConfiguration::PeerVerifyMode verifyMode = input.GetPeerVerifyMode();
	switch (verifyMode) {
	case ISslConfiguration::PVM_NONE:			output.setPeerVerifyMode(QSslSocket::VerifyNone);		break;
	case ISslConfiguration::PVM_QUERY:			output.setPeerVerifyMode(QSslSocket::QueryPeer);		break;
	case ISslConfiguration::PVM_VERIFY:			output.setPeerVerifyMode(QSslSocket::VerifyPeer);		break;
	case ISslConfiguration::PVM_AUTO_VERIFY:	output.setPeerVerifyMode(QSslSocket::AutoVerifyPeer);	break;
	default:
		Q_ASSERT_X(false, "Setup SSL Verify Mode", "Unexpected Verify Mode.");
		break;
	}

	// setup protocol (disable deprecated warnings)
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
	ISslConfiguration::SslProtocol protocol = input.GetProtocol();
	switch (protocol) {
	case ISslConfiguration::SP_TLS_V1_0:			output.setProtocol(QSsl::TlsV1_0);			break;
	case ISslConfiguration::SP_TLS_V1_0_OR_LATER:	output.setProtocol(QSsl::TlsV1_0OrLater);	break;
	case ISslConfiguration::SP_TLS_V1_1:			output.setProtocol(QSsl::TlsV1_1);			break;
	case ISslConfiguration::SP_TLS_V1_1_OR_LATER:	output.setProtocol(QSsl::TlsV1_1OrLater);	break;
	case ISslConfiguration::SP_TLS_V1_2:			output.setProtocol(QSsl::TlsV1_2);			break;
	case ISslConfiguration::SP_TLS_V1_2_OR_LATER:	output.setProtocol(QSsl::TlsV1_2OrLater);	break;
	case ISslConfiguration::SP_DTLS_V1_0:			output.setProtocol(QSsl::DtlsV1_0);			break;
	case ISslConfiguration::SP_DTLS_V1_0_OR_LATER:	output.setProtocol(QSsl::DtlsV1_0OrLater);	break;
	case ISslConfiguration::SP_DTLS_V1_2:			output.setProtocol(QSsl::DtlsV1_2);			break;
	case ISslConfiguration::SP_DTLS_V1_2_OR_LATER:	output.setProtocol(QSsl::DtlsV1_2OrLater);	break;
	case ISslConfiguration::SP_TLS_V1_3:			output.setProtocol(QSsl::TlsV1_3);			break;
	case ISslConfiguration::SP_TLS_V1_3_OR_LATER:	output.setProtocol(QSsl::TlsV1_3OrLater);	break;
	case ISslConfiguration::SP_UNKNOWN_PROTOCOL:	output.setProtocol(QSsl::UnknownProtocol);	break;
	case ISslConfiguration::SP_ANY_PROTOCOL:		output.setProtocol(QSsl::AnyProtocol);		break;
	case ISslConfiguration::SP_SECURE_PROTOCOLS:	output.setProtocol(QSsl::SecureProtocols);	break;
	default:
		Q_ASSERT_X(false, "Setup SSL protocol", "Unexpected protocol.");
		break;
	}
QT_WARNING_POP

}



} // namespace imtcom


