// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcom/CSslConfigurationManagerComp.h>


// Qt includes
#include <QtCore/QFile>
#include <QtNetwork/QSslConfiguration>
#include <QtNetwork/QSslKey>
#include <QtNetwork/QSslCertificate>

// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/IEnableableParam.h>
#include <iprm/INameParam.h>
#include <iprm/IOptionsList.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IParamsSet.h>
#include <iprm/IParamsManager.h>
#include <ifile/IFileNameParam.h>


namespace imtcom
{


// public methods

QSsl::KeyAlgorithm CSslConfigurationManagerComp::GetKeyAlgorithmFromParams(const iprm::IParamsSet& params) const
{
	QSsl::KeyAlgorithm retVal = QSsl::Opaque;
	KeyAlgorithm sslKeyAlgorithm = KA_OPAQUE;

	iprm::TParamsPtr<iprm::ISelectionParam> keyAlgorithmSelectionParamPtr(&params, ParamKeys::s_keyAlgorithmParamKey);
	if(!keyAlgorithmSelectionParamPtr.IsValid()){
		SendWarningMessage(0, "No selection param for SSL KeyAlgorithm is set. OPAQUE will be returned. Set KeyAlgorithmSelection to fix it.");

		return retVal;
	}

	const int selectedIndex = keyAlgorithmSelectionParamPtr->GetSelectedOptionIndex();
	const iprm::IOptionsList* keyAlgorithmListPtr = keyAlgorithmSelectionParamPtr->GetSelectionConstraints();
	if (keyAlgorithmListPtr == nullptr){
		SendCriticalMessage(0, "No options for SSL KeyAlgorithm is set. OPAQUE will be returned. Set KeyAlgorithmSelection's constraints to fix it.");
		I_CRITICAL();

		return retVal;
	}

	const QByteArray optionId = keyAlgorithmListPtr->GetOptionId(selectedIndex);
	const bool isOptionValid = FromString(optionId, sslKeyAlgorithm);
	if (!isOptionValid){
		SendCriticalMessage(0, "Unexpected option detected for SSL KeyAlgorithm. Set correct KeyAlgorithmSelection's constraints to fix it.");
		I_CRITICAL();
	}

	switch (sslKeyAlgorithm){
	case KA_RSA:		retVal = QSsl::Rsa;		break;
	case KA_DSA:		retVal = QSsl::Dsa;		break;
	case KA_EC:			retVal = QSsl::Ec;		break;
	case KA_DH:			retVal = QSsl::Dh;		break;
	case KA_OPAQUE:		retVal = QSsl::Opaque;	break;
	default:
		SendCriticalMessage(0, "Unexpected SSL KeyAlgorithm.");
		I_CRITICAL();
	}

	return  retVal;
}


QSsl::EncodingFormat CSslConfigurationManagerComp::GetEncodingFormatFromParams(const iprm::IParamsSet& params) const
{
	iprm::TParamsPtr<iprm::ISelectionParam> encodingFormatSelectionParamPtr(&params, ParamKeys::s_encodingFormatParamKey);
	EncodingFormat sslEncodingFormat = EF_PEM;
	QSsl::EncodingFormat retVal = QSsl::Pem;

	if(encodingFormatSelectionParamPtr.IsValid()){
		const int selectedIndex = encodingFormatSelectionParamPtr->GetSelectedOptionIndex();
		const iprm::IOptionsList* encodingFormatListPtr = encodingFormatSelectionParamPtr->GetSelectionConstraints();
		if (encodingFormatListPtr == nullptr){
			SendCriticalMessage(0, "No options for SSL EncodingFormat is set. PEM will be returned. Set EncodingFormatSelection's constraints to fix it.");
			I_CRITICAL();

			return retVal;
		}

		const QByteArray optionId = encodingFormatListPtr->GetOptionId(selectedIndex);
		const bool isOptionValid = FromString(optionId, sslEncodingFormat);
		if (!isOptionValid){
			SendCriticalMessage(0, "Unexpected option detected for SSL EncodingFormat. Set correct EncodingFormatSelection's constraints to fix it.");
			I_CRITICAL();
		}
	}
	else{
		SendWarningMessage(0, QString("No selection param for SSL EncodingFormat is set. PEM will be setted. Set '%1' param to fix it.").arg(qPrintable(ParamKeys::s_encodingFormatParamKey)));
	}

	switch (sslEncodingFormat){
	case EF_DER: retVal = QSsl::Der; break;
	case EF_PEM: retVal = QSsl::Pem; break;
	default:
		SendCriticalMessage(0, QString("Unexpected format %1.").arg(QString::number(sslEncodingFormat)), "Setup SSL format");
		I_CRITICAL();
	}

	return retVal;
}


QSsl::KeyType CSslConfigurationManagerComp::GetKeyTypeFromParams(const iprm::IParamsSet& params) const
{
	KeyType sslKeyType = KT_PRIVATE_KEY;
	QSsl::KeyType retVal = QSsl::PrivateKey;

	iprm::TParamsPtr<iprm::ISelectionParam> keyTypeSelectionParamPtr(&params, ParamKeys::s_keyTypeParamKey);
	if(!keyTypeSelectionParamPtr.IsValid()){
		SendWarningMessage(0, "No selection param for SSL KeyType is set. PRIVATE_KEY will be returned. Set KeyTypeSelection to fix it.");

		return retVal;
	}

	const int selectedIndex = keyTypeSelectionParamPtr->GetSelectedOptionIndex();
	const iprm::IOptionsList* keyTypeListPtr = keyTypeSelectionParamPtr->GetSelectionConstraints();
	if (keyTypeListPtr == nullptr){
		SendCriticalMessage(0, "No options for SSL KeyType is set. PRIVATE_KEY will be returned. Set KeyTypeSelection's constraints to fix it.");
		I_CRITICAL();

		return retVal;
	}

	const QByteArray optionId = keyTypeListPtr->GetOptionId(selectedIndex);
	const bool isOptionValid = FromString(optionId, sslKeyType);
	if (!isOptionValid){
		SendCriticalMessage(0, "Unexpected option detected for SSL KeyType. Set correct KeyTypeSelection's constraints to fix it.");
		I_CRITICAL();
	}

	switch (sslKeyType){
	case KT_PRIVATE_KEY: retVal = QSsl::PrivateKey; break;
	case KT_PUBLIC_KEY:  retVal = QSsl::PublicKey;  break;
	default:
		SendCriticalMessage(0, "Unexpected SSL KeyType.");
		I_CRITICAL();
	}

	return retVal;
}


QSsl::SslProtocol CSslConfigurationManagerComp::GetSslProtocolFromParams(const iprm::IParamsSet& params) const
{
	QSsl::SslProtocol retVal = QSsl::UnknownProtocol;

	iprm::TParamsPtr<iprm::ISelectionParam> protocolSelectionParamPtr(&params, ParamKeys::s_protocolParamKey);
	if(!protocolSelectionParamPtr.IsValid()){
		SendWarningMessage(0, "No selection param for SSL protocol is set. NONE will be returned. Set ProtocolSelection to fix it.");

		return retVal;
	}

	const int selectedIndex = protocolSelectionParamPtr->GetSelectedOptionIndex();
	const iprm::IOptionsList* protocolListPtr = protocolSelectionParamPtr->GetSelectionConstraints();
	if (protocolListPtr == nullptr){
		SendCriticalMessage(0, "No options for SSL protocol is set. NONE will be returned. Set ProtocolSelection's constraints to fix it.");
		I_CRITICAL();

		return retVal;
	}

	SslProtocol sslProtocol = SP_UNKNOWN_PROTOCOL;
	const QByteArray optionId = protocolListPtr->GetOptionId(selectedIndex);
	const bool isOptionValid = FromString(optionId, sslProtocol);
	if (!isOptionValid){
		SendCriticalMessage(0, "Unexpected option detected for SSL protocol. Set correct ProtocolSelection's constraints to fix it.");
		I_CRITICAL();
	}

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
	switch (sslProtocol){
	case SP_TLS_V1_0:			retVal = QSsl::TlsV1_0;			break;
	case SP_TLS_V1_0_OR_LATER:	retVal = QSsl::TlsV1_0OrLater;	break;
	case SP_TLS_V1_1:			retVal = QSsl::TlsV1_1;			break;
	case SP_TLS_V1_1_OR_LATER:	retVal = QSsl::TlsV1_1OrLater;	break;
	case SP_TLS_V1_2:			retVal = QSsl::TlsV1_2;			break;
	case SP_TLS_V1_2_OR_LATER:	retVal = QSsl::TlsV1_2OrLater;	break;
	case SP_DTLS_V1_0:			retVal = QSsl::DtlsV1_0;		break;
	case SP_DTLS_V1_0_OR_LATER:	retVal = QSsl::DtlsV1_0OrLater;	break;
	case SP_DTLS_V1_2:			retVal = QSsl::DtlsV1_2;		break;
	case SP_DTLS_V1_2_OR_LATER:	retVal = QSsl::DtlsV1_2OrLater;	break;
	case SP_TLS_V1_3:			retVal = QSsl::TlsV1_3;			break;
	case SP_TLS_V1_3_OR_LATER:	retVal = QSsl::TlsV1_3OrLater;	break;
	case SP_UNKNOWN_PROTOCOL:	retVal = QSsl::UnknownProtocol;	break;
	case SP_ANY_PROTOCOL:		retVal = QSsl::AnyProtocol;		break;
	case SP_SECURE_PROTOCOLS:	retVal = QSsl::SecureProtocols;	break;
	default:
		Q_ASSERT_X(false, "Setup SSL protocol", "Unexpected protocol.");
		break;
	}
QT_WARNING_POP

	return retVal;
}


QSslSocket::PeerVerifyMode CSslConfigurationManagerComp::GetVerifyModeFromParams(const iprm::IParamsSet& params) const
{
	QSslSocket::PeerVerifyMode retVal = QSslSocket::VerifyNone;

	iprm::TParamsPtr<iprm::ISelectionParam> verifyModeSelectionParamPtr(&params, ParamKeys::s_verifyModeParamKey);
	if(!verifyModeSelectionParamPtr.IsValid()){
		SendWarningMessage(0, "No selection param for verify mode is set. NONE will be returned. Set VerifyModeSelection to fix it.");

		return retVal;
	}

	const int selectedIndex = verifyModeSelectionParamPtr->GetSelectedOptionIndex();
	const iprm::IOptionsList* verifyModeListPtr = verifyModeSelectionParamPtr->GetSelectionConstraints();
	if (verifyModeListPtr == nullptr){
		SendCriticalMessage(0, "No options for verify mode is set. NONE will be returned. Set VerifyModeSelection's constraints to fix it.");
		I_CRITICAL();

		return retVal;
	}

	PeerVerifyMode verifyMode = PVM_NONE;
	const QByteArray optionId = verifyModeListPtr->GetOptionId(selectedIndex);
	const bool isOptionValid = FromString(optionId, verifyMode);
	if (!isOptionValid){
		SendCriticalMessage(0, "Unexpected option detected for verify mode. Set correct VerifyModeSelection's constraints to fix it.");
		I_CRITICAL();
	}

	switch (verifyMode){
	case PVM_NONE:			retVal = QSslSocket::VerifyNone;		break;
	case PVM_QUERY:			retVal = QSslSocket::QueryPeer;			break;
	case PVM_VERIFY:		retVal = QSslSocket::VerifyPeer;		break;
	case PVM_AUTO_VERIFY:	retVal = QSslSocket::AutoVerifyPeer;	break;
	default:
		SendCriticalMessage(0, "Unexpected Verify Mode.", "Setup SSL Verify Mode");
		break;
	}

	return retVal;
}


QSharedPointer<QSslKey> CSslConfigurationManagerComp::CreateSslKeyFromParams(const iprm::IParamsSet& params) const
{
	QSharedPointer<QSslKey> retVal;

	QSsl::KeyAlgorithm qAlgorithm = GetKeyAlgorithmFromParams(params);
	QSsl::EncodingFormat qFormat = GetEncodingFormatFromParams(params);
	QSsl::KeyType qType = GetKeyTypeFromParams(params);

	QByteArray passPhrase;
	iprm::TParamsPtr<iprm::INameParam> keyPasswordParamPtr(&params, ParamKeys::s_keyPasswordParamKey, false);
	if (keyPasswordParamPtr.IsValid()){
		passPhrase = keyPasswordParamPtr->GetName().toUtf8();
	}

	iprm::TParamsPtr<ifile::IFileNameParam> filePathParamPtr(&params, ParamKeys::s_filePathParamKey, false);
	iprm::TParamsPtr<iprm::INameParam> encodedDataParamPtr(&params, ParamKeys::s_encodedDataParamKey, false);
	if (filePathParamPtr.IsValid()){
		QFile keyFile(filePathParamPtr->GetPath());
		if (!keyFile.open(QIODevice::ReadOnly)){
			SendErrorMessage(0, "Unable to open SSL key file.");
			return retVal;
		}
		retVal.reset(new QSslKey(&keyFile, qAlgorithm, qFormat, qType, passPhrase));
	}
	else if (encodedDataParamPtr.IsValid()){
		const QByteArray encodedData = encodedDataParamPtr->GetName().toUtf8();
		retVal.reset(new QSslKey(encodedData, qAlgorithm, qFormat, qType, passPhrase));
	}
	else{
		SendCriticalMessage(0, "Unable to create SSL key. No file or data for SSL key is provided!");
		I_CRITICAL();
	}

	return retVal;
}


QSharedPointer<QSslCertificate> CSslConfigurationManagerComp::CreateSslCertificateFromParams(const iprm::IParamsSet& params) const
{
	QSharedPointer<QSslCertificate> retVal;

	// setup format
	iprm::TParamsPtr<iprm::ISelectionParam> encodingFormatSelectionParamPtr(&params, ParamKeys::s_encodingFormatParamKey);
	QSsl::EncodingFormat qFormat = GetEncodingFormatFromParams(params);

	iprm::TParamsPtr<ifile::IFileNameParam> filePathParamPtr(&params, ParamKeys::s_filePathParamKey, false);
	iprm::TParamsPtr<iprm::INameParam> encodedDataParamPtr(&params, ParamKeys::s_encodedDataParamKey, false);
	if (filePathParamPtr.IsValid()){
		QFile certFile(filePathParamPtr->GetPath());
		if (!certFile.open(QIODevice::ReadOnly)){
			return retVal;
		}
		retVal.reset(new QSslCertificate(&certFile, qFormat));
	}
	else if (encodedDataParamPtr.IsValid()){
		const QByteArray encodedData = encodedDataParamPtr->GetName().toUtf8();
		retVal.reset(new QSslCertificate(encodedData, qFormat));
	}
	else{
		SendCriticalMessage(0, "Unable to create SSL cert. File path and data is not provided", "Setup SSL cert");
		I_CRITICAL();
	}

	return retVal;
}


// reimplemented (ISslConfigurationManager)

bool CSslConfigurationManagerComp::CreateSslConfiguration(const iprm::IParamsSet& params, QSslConfiguration& output) const
{
	bool retValue = true;
	// setup local certificate
	iprm::TParamsPtr<iprm::IParamsSet> localCertificateParamPtr(&params, ParamKeys::s_localCertParamKey, false);
	if (localCertificateParamPtr.IsValid()){
		QSharedPointer<QSslCertificate> sslCertificalePtr = CreateSslCertificateFromParams(*localCertificateParamPtr);
		if (!sslCertificalePtr.isNull()){
			output.setLocalCertificate(*sslCertificalePtr);
		}
		else{
			retValue = false;
		}
	}

	// setup local certificate chain
	iprm::TParamsPtr<iprm::IParamsManager> localCertChainParamsPtr(&params, ParamKeys::s_localCertChainParamKey, false);
	if (localCertChainParamsPtr.IsValid()){
		QList<QSslCertificate> localCertificateChain;
		/// \todo implement chain
		SendErrorMessage(0, "Local certificate chain is not supported yet.");
		if (!localCertificateChain.isEmpty()){
			output.setLocalCertificateChain(localCertificateChain);
		}
		else{
			retValue = false;
		}
	}

	// setup CA certificate
	iprm::TParamsPtr<iprm::IParamsSet> caCertificateParamsPtr(&params, ParamKeys::s_caCertParamKey, false);
	if (caCertificateParamsPtr != nullptr){
		QSharedPointer<QSslCertificate> sslCaCertificalePtr = CreateSslCertificateFromParams(*caCertificateParamsPtr);
		if (!sslCaCertificalePtr.isNull()){
			output.addCaCertificate(*sslCaCertificalePtr);
		}
		else{
			retValue = false;
		}
	}

	// setup CA certificate chain
	iprm::TParamsPtr<iprm::IParamsManager> caCertificateChainParamsPtr(&params, ParamKeys::s_caChainCertParamKey, false);
	if (caCertificateChainParamsPtr.IsValid()){
		QList<QSslCertificate> caCertificateChain;
		SendErrorMessage(0, "CA certificate chain is not supported yet.");
		if (!caCertificateChain.isEmpty()){
			output.setCaCertificates(caCertificateChain);
		}
		else{
			retValue = false;
		}
	}

	// setup private key
	iprm::TParamsPtr<iprm::IParamsSet> privateKeyParamPtr(&params, ParamKeys::s_privateKeyParamKey, false);
	if (privateKeyParamPtr != nullptr){
		QSharedPointer<QSslKey> qSslKeyPtr = CreateSslKeyFromParams(*privateKeyParamPtr);
		if (!qSslKeyPtr.isNull()){
			output.setPrivateKey(*qSslKeyPtr);
		}
		else{
			retValue = false;
		}
	}

	// setup verify mode
	const QSslSocket::PeerVerifyMode verifyMode = GetVerifyModeFromParams(params);
	output.setPeerVerifyMode(verifyMode);

	// setup protocol
	const QSsl::SslProtocol sslProtocol = GetSslProtocolFromParams(params);
	output.setProtocol(sslProtocol);

	return retValue;
}



} // namespace imtcom


