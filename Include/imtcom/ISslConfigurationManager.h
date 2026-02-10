// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#pragma once


// ACF includes
#include <istd/IPolymorphic.h>


// forward declaration

class QSslConfiguration;

namespace iprm
{
class IParamsSet;
}


namespace imtcom
{


class ISslConfigurationManager: virtual public istd::IPolymorphic
{
public:
	struct ParamKeys
	{
		static inline const QByteArray s_enableSslModeParamKey =		QByteArrayLiteral("EnableSsl");
		static inline const QByteArray s_keyAlgorithmParamKey =			QByteArrayLiteral("KeyAlgorithm");
		static inline const QByteArray s_encodingFormatParamKey =		QByteArrayLiteral("EncodingFormat");
		static inline const QByteArray s_keyTypeParamKey =				QByteArrayLiteral("KeyType");
		static inline const QByteArray s_verifyModeParamKey =			QByteArrayLiteral("VerifyMode");
		static inline const QByteArray s_protocolParamKey =				QByteArrayLiteral("Protocol");
		static inline const QByteArray s_keyPasswordParamKey =			QByteArrayLiteral("KeyPassword");
		static inline const QByteArray s_filePathParamKey =				QByteArrayLiteral("FilePath");
		static inline const QByteArray s_encodedDataParamKey =			QByteArrayLiteral("EncodedData");
		static inline const QByteArray s_localCertParamKey =			QByteArrayLiteral("LocalCert");
		static inline const QByteArray s_localCertChainParamKey =		QByteArrayLiteral("LocalCertChain");
		static inline const QByteArray s_caCertParamKey =				QByteArrayLiteral("CaCert");
		static inline const QByteArray s_caChainCertParamKey =			QByteArrayLiteral("CaChainCert");
		static inline const QByteArray s_privateKeyParamKey =			QByteArrayLiteral("PrivateKey");
	};

	enum PeerVerifyMode
	{
		PVM_NONE,			///< (default) SslSocket will not request a certificate from the peer. You can set this mode if you are not interested in the identity of the other side of the connection. The connection will still be encrypted, and your socket will still send its local certificate to the peer if it's requested.
		PVM_QUERY,			///< SslSocket will request a certificate from the peer, but does not require this certificate to be valid. This is useful when you want to display peer certificate details to the user without affecting the actual SSL handshake. This mode is the default for servers. Note: In Schannel this value acts the same as VerifyNone.
		PVM_VERIFY,			///< SslSocket will request a certificate from the peer during the SSL handshake phase, and requires that this certificate is valid. On failure, SslSocket will emit the QSslSocket::sslErrors() signal. This mode is the default for clients.
		PVM_AUTO_VERIFY		///< SslSocket will automatically use QueryPeer for server sockets and VerifyPeer for client sockets.
	};
	I_DECLARE_ENUM(PeerVerifyMode,
				PVM_NONE,
				PVM_QUERY,
				PVM_VERIFY,
				PVM_AUTO_VERIFY)

	enum SslProtocol
	{
		SP_UNKNOWN_PROTOCOL,	///< The cipher's protocol cannot be determined.
		SP_SECURE_PROTOCOLS,	///< The default option, using protocols known to be secure.
		SP_ANY_PROTOCOL,		///< Any supported protocol. This value is used by QSslSocket only (clients)
		SP_TLS_V1_0,			///< TLSv1.0						\deprecated You SHOULD use \c SP_TLS_V1_2_OR_LATER
		SP_TLS_V1_0_OR_LATER,	///< TLSv1.0 and later versions.	\deprecated You SHOULD use \c SP_TLS_V1_2_OR_LATER
		SP_TLS_V1_1,			///< TLSv1.1.						\deprecated You SHOULD use \c SP_TLS_V1_2_OR_LATER
		SP_TLS_V1_1_OR_LATER,	///< TLSv1.1 and later versions.	\deprecated You SHOULD use \c SP_TLS_V1_2_OR_LATER
		SP_TLS_V1_2,			///< TLSv1.2.
		SP_TLS_V1_2_OR_LATER,	///< TLSv1.2 and later versions.
		SP_DTLS_V1_0,			///< DTLSv1.0						\deprecated You SHOULD use \c SP_DTLS_V1_2_OR_LATER
		SP_DTLS_V1_0_OR_LATER,	///< DTLSv1.0 and later versions.	\deprecated You SHOULD use \c SP_DTLS_V1_2_OR_LATER
		SP_DTLS_V1_2,			///< DTLSv1.2
		SP_DTLS_V1_2_OR_LATER,	///< DTLSv1.2 and later versions.
		SP_TLS_V1_3,			///< TLSv1.3.
		SP_TLS_V1_3_OR_LATER,	///< TLSv1.3 and later versions.
	};
	I_DECLARE_ENUM(SslProtocol,
				SP_UNKNOWN_PROTOCOL,
				SP_SECURE_PROTOCOLS,
				SP_ANY_PROTOCOL,
				SP_TLS_V1_0,
				SP_TLS_V1_0_OR_LATER,
				SP_TLS_V1_1,
				SP_TLS_V1_1_OR_LATER,
				SP_TLS_V1_2,
				SP_TLS_V1_2_OR_LATER,
				SP_DTLS_V1_0,
				SP_DTLS_V1_0_OR_LATER,
				SP_DTLS_V1_2,
				SP_DTLS_V1_2_OR_LATER,
				SP_TLS_V1_3,
				SP_TLS_V1_3_OR_LATER)

	enum KeyAlgorithm
	{
		KA_RSA,		///< The RSA algorithm.
		KA_DSA,		///< The DSA algorithm.
		KA_EC,		///< The Elliptic Curve algorithm.
		KA_DH,		///< The Diffie-Hellman algorithm.
		KA_OPAQUE	///< A key that should be treated as a 'black box'.
	};
	I_DECLARE_ENUM(KeyAlgorithm,
				KA_RSA,
				KA_DSA,
				KA_EC,
				KA_DH,
				KA_OPAQUE)

	enum EncodingFormat
	{
		EF_PEM,	///< The PEM format. Used by default.
		EF_DER	///< The DER format.
	};
	I_DECLARE_ENUM(EncodingFormat,
				EF_PEM,
				EF_DER)

	enum KeyType
	{
		KT_PRIVATE_KEY,	///< A private key.
		KT_PUBLIC_KEY	///< A public key.
	};
	I_DECLARE_ENUM(KeyType,
				KT_PRIVATE_KEY,
				KT_PUBLIC_KEY)

	virtual bool CreateSslConfiguration(const iprm::IParamsSet& params, QSslConfiguration& output) const = 0;

};


} // namespace imtcom


