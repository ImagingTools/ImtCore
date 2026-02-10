// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtcom/CSslConfigurationAdapterComp.h>


// Qt includes
#include <QtCore/QFile>

// ACF includes
#include <iprm/TParamsPtr.h>

// ImtCore includes
#include <imtcom/ISslConfigurationManager.h>
#include <iprm/ISelectionParam.h>
#include <iprm/INameParam.h>
#include <iprm/IEnableableParam.h>
#include <ifile/IFileNameParam.h>


namespace imtcom
{


// public methods

// reimplemented (imtcom::ISslConfigurationApplier)

bool CSslConfigurationAdapterComp::LoadLocalCertificateFromFile(const QString& path, QSsl::EncodingFormat format) const
{
	iprm::TEditableParamsPtr<iprm::IParamsSet> localCertificateParamPtr(m_sslConfigurationCompPtr.GetPtr(), ISslConfigurationManager::ParamKeys::s_localCertParamKey);
	if (!localCertificateParamPtr.IsValid()){
		return false;
	}

	if (!ApplyEncodingFormat(*localCertificateParamPtr.GetPtr(), format)){
		return false;
	}

	if (!ApplyFilePath(*localCertificateParamPtr.GetPtr(), path)){
		return false;
	}

	ProvideSslEnabled();

	return true;
}


bool CSslConfigurationAdapterComp::LoadPrivateKeyFromFile(
			const QString& path,
			QSsl::KeyAlgorithm algorithm,
			QSsl::EncodingFormat format,
			const QByteArray& passPhrase) const
{
	iprm::TEditableParamsPtr<iprm::IParamsSet> privateKeyParamPtr(m_sslConfigurationCompPtr.GetPtr(), ISslConfigurationManager::ParamKeys::s_privateKeyParamKey);
	if (!privateKeyParamPtr.IsValid()){
		return false;
	}

	if (!ApplyFilePath(*privateKeyParamPtr.GetPtr(), path)){
		return false;
	}

	iprm::TEditableParamsPtr<iprm::ISelectionParam> keyAlgorithmSelectionParamPtr(privateKeyParamPtr.GetPtr(), ISslConfigurationManager::ParamKeys::s_keyAlgorithmParamKey);
	if(!keyAlgorithmSelectionParamPtr.IsValid()){
		return false;
	}

	switch (algorithm){
	case QSsl::Rsa:
		keyAlgorithmSelectionParamPtr->SetSelectedOptionIndex(0);
		break;
	case QSsl::Dsa:
		keyAlgorithmSelectionParamPtr->SetSelectedOptionIndex(1);
		break;
	case QSsl::Ec:
		keyAlgorithmSelectionParamPtr->SetSelectedOptionIndex(2);
		break;
	case QSsl::Dh:
		keyAlgorithmSelectionParamPtr->SetSelectedOptionIndex(3);
		break;
	case QSsl::Opaque:
		return false;
	}

	if (!ApplyEncodingFormat(*privateKeyParamPtr.GetPtr(), format)){
		return false;
	}

	iprm::TEditableParamsPtr<iprm::INameParam> keyPasswordParamPtr(privateKeyParamPtr.GetPtr(), ISslConfigurationManager::ParamKeys::s_keyPasswordParamKey);
	if (!keyPasswordParamPtr.IsValid()){
		return false;
	}

	keyPasswordParamPtr->SetName(passPhrase);

	ProvideSslEnabled();

	return true;
}


bool CSslConfigurationAdapterComp::LoadCaCertificatesFromFile(const QString& path, QSsl::EncodingFormat format) const
{
	iprm::TEditableParamsPtr<iprm::IParamsSet> caCertificateParamsPtr(m_sslConfigurationCompPtr.GetPtr(), ISslConfigurationManager::ParamKeys::s_caCertParamKey);
	if (!caCertificateParamsPtr.IsValid()){
		return false;
	}

	if (!ApplyFilePath(*caCertificateParamsPtr.GetPtr(), path)){
		return false;
	}

	if (!ApplyEncodingFormat(*caCertificateParamsPtr.GetPtr(), format)){
		return false;
	}

	ProvideSslEnabled();

	return true;
}


bool CSslConfigurationAdapterComp::SetPeerVerifyMode(QSslSocket::PeerVerifyMode mode) const
{
	iprm::TEditableParamsPtr<iprm::ISelectionParam> verifyModeSelectionParamPtr(m_sslConfigurationCompPtr.GetPtr(), ISslConfigurationManager::ParamKeys::s_verifyModeParamKey);
	if(!verifyModeSelectionParamPtr.IsValid()){
		return false;
	}

	switch (mode){
	case QSslSocket::VerifyNone:
		verifyModeSelectionParamPtr->SetSelectedOptionIndex(0);
		break;
	case QSslSocket::QueryPeer:
		verifyModeSelectionParamPtr->SetSelectedOptionIndex(1);
		break;
	case QSslSocket::VerifyPeer:
		verifyModeSelectionParamPtr->SetSelectedOptionIndex(2);
		break;
	case QSslSocket::AutoVerifyPeer:
		verifyModeSelectionParamPtr->SetSelectedOptionIndex(3);
		break;
	}

	ProvideSslEnabled();

	return true;
}


bool CSslConfigurationAdapterComp::SetSslProtocol(QSsl::SslProtocol protocol) const
{
	iprm::TEditableParamsPtr<iprm::ISelectionParam> protocolSelectionParamPtr(m_sslConfigurationCompPtr.GetPtr(), ISslConfigurationManager::ParamKeys::s_protocolParamKey);
	if(!protocolSelectionParamPtr.IsValid()){
		return false;
	}

	switch (protocol){
	case QSsl::TlsV1_0:
		protocolSelectionParamPtr->SetSelectedOptionIndex(3);
		break;
	case QSsl::TlsV1_1:
		protocolSelectionParamPtr->SetSelectedOptionIndex(5);
		break;
	case QSsl::TlsV1_2:
		protocolSelectionParamPtr->SetSelectedOptionIndex(7);
		break;
	case QSsl::AnyProtocol:
		protocolSelectionParamPtr->SetSelectedOptionIndex(2);
		break;
	case QSsl::SecureProtocols:
		protocolSelectionParamPtr->SetSelectedOptionIndex(1);
		break;
	case QSsl::TlsV1_0OrLater:
		protocolSelectionParamPtr->SetSelectedOptionIndex(4);
		break;
	case QSsl::TlsV1_1OrLater:
		protocolSelectionParamPtr->SetSelectedOptionIndex(6);
		break;
	case QSsl::TlsV1_2OrLater:
		protocolSelectionParamPtr->SetSelectedOptionIndex(8);
		break;
	case QSsl::DtlsV1_0:
		protocolSelectionParamPtr->SetSelectedOptionIndex(9);
		break;
	case QSsl::DtlsV1_0OrLater:
		protocolSelectionParamPtr->SetSelectedOptionIndex(10);
		break;
	case QSsl::DtlsV1_2:
		protocolSelectionParamPtr->SetSelectedOptionIndex(11);
		break;
	case QSsl::DtlsV1_2OrLater:
		protocolSelectionParamPtr->SetSelectedOptionIndex(12);
		break;
	case QSsl::TlsV1_3:
		protocolSelectionParamPtr->SetSelectedOptionIndex(13);
		break;
	case QSsl::TlsV1_3OrLater:
		protocolSelectionParamPtr->SetSelectedOptionIndex(14);
		break;
	case QSsl::UnknownProtocol:
		protocolSelectionParamPtr->SetSelectedOptionIndex(0);
		break;
	}

	ProvideSslEnabled();

	return true;
}


// private methods

bool CSslConfigurationAdapterComp::ApplyEncodingFormat(iprm::IParamsSet& paramsSet, QSsl::EncodingFormat format) const
{
	iprm::TEditableParamsPtr<iprm::ISelectionParam> encodingFormatSelectionParamPtr(&paramsSet, ISslConfigurationManager::ParamKeys::s_encodingFormatParamKey);
	if(!encodingFormatSelectionParamPtr.IsValid()){
		return false;
	}

	switch (format){
	case QSsl::Pem:
		encodingFormatSelectionParamPtr->SetSelectedOptionIndex(0);
		break;
	case QSsl::Der:
		encodingFormatSelectionParamPtr->SetSelectedOptionIndex(1);
		break;
	}

	return true;
}


bool CSslConfigurationAdapterComp::ApplyFilePath(iprm::IParamsSet& paramsSet, const QString& path) const
{
	iprm::TEditableParamsPtr<ifile::IFileNameParam> filePathParamPtr(&paramsSet, ISslConfigurationManager::ParamKeys::s_filePathParamKey);
	if (!filePathParamPtr.IsValid()){
		return false;
	}

	filePathParamPtr->SetPath(path);

	return true;
}


void CSslConfigurationAdapterComp::ProvideSslEnabled() const
{
	iprm::TEditableParamsPtr<iprm::IEnableableParam> sslEnableParamPtr(m_sslConfigurationCompPtr.GetPtr(), ISslConfigurationManager::ParamKeys::s_enableSslModeParamKey);
	if (sslEnableParamPtr.IsValid()){
		sslEnableParamPtr->SetEnabled(true);
	}
}


} // namespace imtcom


