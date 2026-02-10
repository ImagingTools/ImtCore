// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// Qt includes
#include <QtNetwork/QSslSocket>

// ACF includes
#include <istd/IPolymorphic.h>


namespace imtcom
{


/*!
	\class ISslConfigurationApplier
	\brief Interface for applying SSL-related settings to an SSL configuration.

	The \c ISslConfigurationApplier interface defines a write-only contract for
	applying SSL-related parameters to an existing SSL configuration.

	Implementations of this interface translate high-level SSL settings
	(such as certificates, private keys, CA chains, protocols, and peer
	verification modes) into a concrete SSL configuration representation
	used by networking components.

	This interface does not own or expose a \l QSslConfiguration instance.
	Instead, it applies changes to an underlying configuration managed
	elsewhere (for example, by an \c ISslConfigurationManager or a similar
	component).

	Typical use cases include configuring SSL settings via configuration
	files, command-line parameters, or UI-driven setup flows.

	\note Implementations are expected to be idempotent where possible and
	to perform validation of input parameters before applying them.

	\sa QSslConfiguration, QSslSocket
*/
class ISslConfigurationApplier: virtual public istd::IPolymorphic
{
public:
	/*!
		\fn bool ISslConfigurationApplier::LoadLocalCertificateFromFile(
			const QString& path,
			QSsl::EncodingFormat format) const

		Loads a local SSL certificate from the specified file and applies it
		to the underlying SSL configuration.
	
		The certificate file may be encoded in PEM or DER format.
		If the file contains multiple certificates, the first one is used
		as the local certificate.

		\param path Path to the certificate file.
		\param format Encoding format of the certificate data.
		\return \c true if the certificate was successfully applied;
				otherwise returns \c false.
	*/
	virtual bool LoadLocalCertificateFromFile(const QString& path, QSsl::EncodingFormat format = QSsl::Pem) const = 0;

	/*!
		\fn bool ISslConfigurationApplier::LoadPrivateKeyFromFile(
			const QString& path,
			QSsl::KeyAlgorithm algorithm,
			QSsl::EncodingFormat format,
			const QByteArray& passPhrase) const

		Loads a private key from the specified file and applies it to the
		underlying SSL configuration.
	
		The private key may be encrypted. In this case, the provided
		passphrase is used to decrypt the key.

		\param path Path to the private key file.
		\param algorithm Algorithm used by the private key (for example,
						  RSA or EC).
		\param format Encoding format of the key data.
		\param passPhrase Passphrase used to decrypt the private key.
		\return \c true if the private key was successfully applied;
				otherwise returns \c false.

		\sa QSslKey
	*/
	virtual bool LoadPrivateKeyFromFile(
				const QString& path,
				QSsl::KeyAlgorithm algorithm,
				QSsl::EncodingFormat format = QSsl::Pem,
				const QByteArray& passPhrase = {}) const = 0;

	/*!
		\fn bool ISslConfigurationApplier::LoadCaCertificatesFromFile(
			const QString& path,
			QSsl::EncodingFormat format) const

		Loads one or more Certificate Authority (CA) certificates from the
		specified file and applies them to the underlying SSL configuration.

		The file may contain multiple CA certificates. All successfully parsed
		certificates are applied and used for peer verification.

		\param path Path to the CA certificate file.
		\param format Encoding format of the certificate data.
		\return \c true if at least one CA certificate was successfully applied;
				otherwise returns \c false.
	*/
	virtual bool LoadCaCertificatesFromFile(const QString& path, QSsl::EncodingFormat format = QSsl::Pem) const = 0;

	/*!
		\fn bool ISslConfigurationApplier::SetPeerVerifyMode(
			QSslSocket::PeerVerifyMode mode) const

		Sets the peer verification mode for the underlying SSL configuration.

		This setting controls how peer certificates are verified during
		the SSL/TLS handshake.

		\param mode Peer verification mode to apply.
		\return \c true if the verification mode was successfully applied;
				otherwise returns \c false.

		\sa QSslSocket::PeerVerifyMode
	*/
	virtual bool SetPeerVerifyMode(QSslSocket::PeerVerifyMode mode) const = 0;

	/*!
		\fn bool ISslConfigurationApplier::SetSslProtocol(
			QSsl::SslProtocol protocol) const

		Sets the SSL/TLS protocol version for the underlying SSL configuration.

		This setting determines which SSL/TLS protocol versions are allowed
		during connection establishment.

		\param protocol SSL/TLS protocol to apply.
		\return \c true if the protocol was successfully applied;
				otherwise returns \c false.

		\sa QSsl::SslProtocol
	*/
	virtual bool SetSslProtocol(QSsl::SslProtocol protocol) const = 0;
};


} // namespace imtcom


