#pragma once


// Acf includes
#include <iser/ISerializable.h>


namespace imtcom
{


class ISslCertificate;
class ISslKey;


class ISslConfiguration: virtual public iser::ISerializable
{
public:
	typedef const ISslCertificate* const SslCertificatePtr;
	typedef const ISslKey* const SslKeyPtr;
	typedef QList<const ISslCertificate* const> SslCertificatePtrList;

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

	virtual const SslCertificatePtr GetLocalCertificate() const = 0;

	/// \note if returned list is not empty ALL values MUST be a valid pointer
	virtual SslCertificatePtrList GetLocalCertificateChain() const = 0;
	virtual SslCertificatePtr GetCaCertificate() const = 0;

	/// \note if returned list is not empty ALL values MUST be a valid pointer
	virtual SslCertificatePtrList GetCaCertificateChain() const = 0;
	virtual SslKeyPtr GetPrivateKey() const = 0;
	virtual PeerVerifyMode GetPeerVerifyMode() const = 0;
	virtual SslProtocol GetProtocol() const = 0;
};


} // namespace imtcom


