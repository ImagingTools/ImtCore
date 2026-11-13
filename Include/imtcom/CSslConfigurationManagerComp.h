// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtCore/QList>
#include <QtCore/QSharedPointer>
#include <QtNetwork/QSsl>
#include <QtNetwork/QSslCertificate>
#include <QtNetwork/QSslSocket>

// ACF includes
#include <ilog/TLoggerCompWrap.h>

// ImtCore includes
#include <imtcom/ISslConfigurationManager.h>


// forward declaration
class QSslKey;

namespace iprm
{
class IParamsManager;
}

namespace imtcom
{

class CSslConfigurationManagerComp:
			public ilog::CLoggerComponentBase,
			virtual public ISslConfigurationManager
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;
	I_BEGIN_COMPONENT(CSslConfigurationManagerComp)
		I_REGISTER_INTERFACE(ISslConfigurationManager)
	I_END_COMPONENT

	QSsl::KeyAlgorithm GetKeyAlgorithmFromParams(const iprm::IParamsSet& params) const;;
	QSsl::EncodingFormat GetEncodingFormatFromParams(const iprm::IParamsSet& params) const;
	QSsl::KeyType GetKeyTypeFromParams(const iprm::IParamsSet& params) const;
	QSsl::SslProtocol GetSslProtocolFromParams(const iprm::IParamsSet& params) const;
	QSslSocket::PeerVerifyMode GetVerifyModeFromParams(const iprm::IParamsSet& params) const;
	QSharedPointer<QSslKey> CreateSslKeyFromParams(const iprm::IParamsSet& params) const;
	QSharedPointer<QSslCertificate> CreateSslCertificateFromParams(const iprm::IParamsSet& params) const;

	/**
		Read all certificates of the file or of the encoded data of the parameters, in their order. A PEM file can hold a certificate
		and its chain (as the file of a certificate authority does), a DER file holds one certificate.
		\return An empty list if the file cannot be read or has no certificates.
	*/
	QList<QSslCertificate> CreateSslCertificatesFromParams(const iprm::IParamsSet& params) const;

	/**
		Make the chain that a server sends to a client: the certificates of the server first (the first one is the certificate of
		the server), then the additional ones (the intermediate certificates) that are not in the chain yet.
		If there are no certificates of the server, the additional ones are the chain.
	*/
	static QList<QSslCertificate> ComposeCertificateChain(const QList<QSslCertificate>& certificates, const QList<QSslCertificate>& additionalCertificates);

	// reimplemented (ISslConfigurationManager)
	virtual bool CreateSslConfiguration(const iprm::IParamsSet& params, QSslConfiguration& output) const override;

private:
	/**
		Read the certificates of all parameter sets of the manager in their order.
		\return \c false if a set is missing or has no certificates.
	*/
	bool CreateSslCertificatesFromManager(const iprm::IParamsManager& manager, QList<QSslCertificate>& certificates) const;
};




} // namespace imtcom


