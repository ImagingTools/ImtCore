#pragma once


// Qt includes
#include <QtCore/QSharedPointer>

// Acf includes
#include <ilog/TLoggerCompWrap.h>
#include <iprm/ISelectionParam.h>
#include <imod/IModel.h>
#include <imod/CModelUpdateBridge.h>

// ImtCore includes
#include <imtcom/ISslConfiguration.h>
#include <imtcom/ISslCertificate.h>
#include <imtcom/ISslKey.h>


namespace imtcom
{


class CSslConfigurationComp:
			public ilog::CLoggerComponentBase,
			virtual public ISslConfiguration
{
public:
	typedef ilog::CLoggerComponentBase BaseClass;

	I_BEGIN_COMPONENT(CSslConfigurationComp)
		I_REGISTER_INTERFACE(ISslConfiguration)
		I_ASSIGN(m_localCertificateCompPtr, "LocalCertificate", "The certificate to be presented to the peer during the SSL handshake process.", false, "LocalCertificate")
		I_ASSIGN_TO(m_localCertificateModelCompPtr, m_localCertificateCompPtr, true)
		I_ASSIGN(m_caCertificateCompPtr, "CaCertificate", "A digital certificate certifies the ownership of a public key by the named subject of the certificate.", false, "CaCertificate")
		I_ASSIGN_TO(m_caCertificateModelCompPtr, m_caCertificateCompPtr, true)
		I_ASSIGN(m_privateKeyCompPtr, "PrivateKey", "The connection's private key. The private key and the local certificate are used to prove their identity to SSL peers.", false, "PrivateKey")
		I_ASSIGN_TO(m_privateKeyModelCompPtr, m_privateKeyCompPtr, true)
		I_ASSIGN(m_verifyModeSelectionParamCompPtr, "VerifyModeSelection", "The verify mode. This mode decides whether SSL Socket should request a certificate from the peer (i.e., the client requests a certificate from the server, or a server requesting a certificate from the client), and whether it should require that this certificate is valid.", true, "VerifyModeSelection")
		I_ASSIGN_TO(m_verifyModeSelectionParamModelCompPtr, m_verifyModeSelectionParamCompPtr, true)
		I_ASSIGN(m_protocolSelectionParamCompPtr, "ProtocolSelection", "The SSL protocol for the configuration.", true, "ProtocolSelection")
		I_ASSIGN_TO(m_protocolSelectionParamModelCompPtr, m_protocolSelectionParamCompPtr, true)
	I_END_COMPONENT

	CSslConfigurationComp();

	// reimplemented (ISslConfiguration)
	virtual const SslCertificatePtr GetLocalCertificate() const override;

	/// \warning this method does not supported yet
	virtual SslCertificatePtrList GetLocalCertificateChain() const override;
	virtual SslCertificatePtr GetCaCertificate() const override;

	/// \warning this method does not supported yet
	virtual SslCertificatePtrList GetCaCertificateChain() const override;
	virtual SslKeyPtr GetPrivateKey() const override;
	virtual PeerVerifyMode GetPeerVerifyMode() const override;
	virtual SslProtocol GetProtocol() const override;

	// reimplemented (iser::ISerializable)
	virtual bool Serialize(iser::IArchive& archive) override;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;
	virtual void OnComponentDestroyed() override;

private:
	I_REF(ISslCertificate, m_localCertificateCompPtr);
	I_REF(imod::IModel, m_localCertificateModelCompPtr);
	I_REF(ISslCertificate, m_caCertificateCompPtr);
	I_REF(imod::IModel, m_caCertificateModelCompPtr);
	I_REF(ISslKey, m_privateKeyCompPtr);
	I_REF(imod::IModel, m_privateKeyModelCompPtr);
	I_REF(iprm::ISelectionParam, m_verifyModeSelectionParamCompPtr);
	I_REF(imod::IModel, m_verifyModeSelectionParamModelCompPtr);
	I_REF(iprm::ISelectionParam, m_protocolSelectionParamCompPtr);
	I_REF(imod::IModel, m_protocolSelectionParamModelCompPtr);

	imod::CModelUpdateBridge m_localCertificateUpdateBridge;
	imod::CModelUpdateBridge m_caCertificateUpdateBridge;
	imod::CModelUpdateBridge m_keyUpdateBridge;
	imod::CModelUpdateBridge m_verifyModeUpdateBridge;
	imod::CModelUpdateBridge m_protocolUpdateBridge;

	int m_configurationVersion;
};


} // namespace imtcom


