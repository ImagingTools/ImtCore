// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QSslKey>

// ACF includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/IParamsSet.h>

// ImtCore includes
#include <imtcom/ISslConfigurationApplier.h>


namespace imtcom
{


class CSslConfigurationAdapterComp:
			public ilog::CLoggerComponentBase,
			virtual public ISslConfigurationApplier
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSslConfigurationAdapterComp)
		I_REGISTER_INTERFACE(imtcom::ISslConfigurationApplier)
		I_ASSIGN(m_sslConfigurationCompPtr, "SslConfiguration", "SSL Configuration is used by networking classes to relay information about an open SSL connection and to allow the server to control certain features of that connection.", true, "SslConfiguration")
	I_END_COMPONENT

	// reimplemented (imtcom::ISslConfigurationApplier)
	virtual bool LoadLocalCertificateFromFile(const QString& path, QSsl::EncodingFormat format = QSsl::Pem) const override;
	virtual bool LoadPrivateKeyFromFile(
				const QString& path,
				QSsl::KeyAlgorithm algorithm,
				QSsl::EncodingFormat format = QSsl::Pem,
				const QByteArray& passPhrase = {}) const override;
	virtual bool LoadCaCertificatesFromFile(const QString& path, QSsl::EncodingFormat format = QSsl::Pem) const override;
	virtual bool SetPeerVerifyMode(QSslSocket::PeerVerifyMode mode) const override;
	virtual bool SetSslProtocol(QSsl::SslProtocol protocol) const override;

private:
	bool ApplyEncodingFormat(iprm::IParamsSet& paramsSet, QSsl::EncodingFormat format) const;
	bool ApplyFilePath(iprm::IParamsSet& paramsSet, const QString& path) const;
	void ProvideSslEnabled() const;

private:
	I_REF(iprm::IParamsSet, m_sslConfigurationCompPtr);
};


} // namespace imtcom


