#include <imtcom/CSslConfigurationComp.h>


// Acf includes
#include <iprm/IOptionsList.h>


namespace imtcom
{


// public methods

CSslConfigurationComp::CSslConfigurationComp():
	m_localCertificateUpdateBridge(this),
	m_caCertificateUpdateBridge(this),
	m_keyUpdateBridge(this),
	m_verifyModeUpdateBridge(this),
	m_protocolUpdateBridge(this),
	m_configurationVersion(1)
{
}


// reimplemented (ISslConfiguration)

ISslConfiguration::SslCertificatePtr CSslConfigurationComp::GetLocalCertificate() const
{
	if (!m_localCertificateCompPtr.IsValid()){
		return SslCertificatePtr();
	}

	return m_localCertificateCompPtr.GetPtr();
}


ISslConfiguration::SslCertificatePtrList CSslConfigurationComp::GetLocalCertificateChain() const
{
	return SslCertificatePtrList();
}


ISslConfiguration::SslCertificatePtr CSslConfigurationComp::GetCaCertificate() const
{
	if (!m_caCertificateCompPtr.IsValid()){
		return SslCertificatePtr();
	}

	return m_caCertificateCompPtr.GetPtr();
}


ISslConfiguration::SslCertificatePtrList CSslConfigurationComp::GetCaCertificateChain() const
{
	return SslCertificatePtrList();
}


ISslConfiguration::SslKeyPtr CSslConfigurationComp::GetPrivateKey() const
{
	if (!m_privateKeyCompPtr.IsValid()){
		return SslKeyPtr();
	}

	return m_privateKeyCompPtr.GetPtr();
}


ISslConfiguration::PeerVerifyMode CSslConfigurationComp::GetPeerVerifyMode() const
{
	PeerVerifyMode verifyMode = PVM_NONE;

	if(!m_verifyModeSelectionParamCompPtr.IsValid()){
		SendWarningMessage(0, "No selection param for verify mode is set. NONE will be returned. Set VerifyModeSelection to fix it.");

		return verifyMode;
	}

	const int selectedIndex = m_verifyModeSelectionParamCompPtr->GetSelectedOptionIndex();
	const iprm::IOptionsList* verifyModeListPtr = m_verifyModeSelectionParamCompPtr->GetSelectionConstraints();
	if (verifyModeListPtr == nullptr){
		SendCriticalMessage(0, "No options for verify mode is set. NONE will be returned. Set VerifyModeSelection's constraints to fix it.");
		I_CRITICAL();

		return verifyMode;
	}

	const QByteArray optionId = verifyModeListPtr->GetOptionId(selectedIndex);
	const bool isOptionValid = ISslConfiguration::FromString(optionId, verifyMode);
	if (!isOptionValid){
		SendCriticalMessage(0, "Unexpected option detected for verify mode. Set correct VerifyModeSelection's constraints to fix it.");
		I_CRITICAL();
	}

	return verifyMode;
}


ISslConfiguration::SslProtocol CSslConfigurationComp::GetProtocol() const
{
	SslProtocol sslProtocol = SP_UNKNOWN_PROTOCOL;

	if(!m_protocolSelectionParamCompPtr.IsValid()){
		SendWarningMessage(0, "No selection param for SSL protocol is set. NONE will be returned. Set ProtocolSelection to fix it.");

		return sslProtocol;
	}

	const int selectedIndex = m_protocolSelectionParamCompPtr->GetSelectedOptionIndex();
	const iprm::IOptionsList* protocolListPtr = m_protocolSelectionParamCompPtr->GetSelectionConstraints();
	if (protocolListPtr == nullptr){
		SendCriticalMessage(0, "No options for SSL protocol is set. NONE will be returned. Set ProtocolSelection's constraints to fix it.");
		I_CRITICAL();

		return sslProtocol;
	}

	const QByteArray optionId = protocolListPtr->GetOptionId(selectedIndex);
	const bool isOptionValid = ISslConfiguration::FromString(optionId, sslProtocol);
	if (!isOptionValid){
		SendCriticalMessage(0, "Unexpected option detected for SSL protocol. Set correct ProtocolSelection's constraints to fix it.");
		I_CRITICAL();
	}

	return sslProtocol;
}


// reimplemented iser::ISerializable

bool CSslConfigurationComp::Serialize(iser::IArchive& archive)
{
	bool retVal = true;

	iser::CArchiveTag mainTag("SslConfiguration", "", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(mainTag);

	iser::CArchiveTag versionTag("ConfigurationVersion", "", iser::CArchiveTag::TT_LEAF, &mainTag);
	retVal = retVal && archive.BeginTag(versionTag);
	retVal = retVal && archive.Process(m_configurationVersion);
	retVal = retVal && archive.EndTag(versionTag);

	iser::CArchiveTag localCertTag("LocalCertificate", "", iser::CArchiveTag::TT_GROUP, &mainTag);
	retVal = retVal && archive.BeginTag(localCertTag);
	if (m_localCertificateCompPtr.IsValid()){
		retVal = retVal && m_localCertificateCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(localCertTag);

	iser::CArchiveTag caCertTag("CaCertificate", "", iser::CArchiveTag::TT_GROUP, &mainTag);
	retVal = retVal && archive.BeginTag(caCertTag);
	if (m_caCertificateCompPtr.IsValid()){
		retVal = retVal && m_caCertificateCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(caCertTag);

	iser::CArchiveTag privateKeyCertTag("PrivateKey", "", iser::CArchiveTag::TT_GROUP, &mainTag);
	retVal = retVal && archive.BeginTag(privateKeyCertTag);
	if (m_privateKeyCompPtr.IsValid()){
		retVal = retVal && m_privateKeyCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(privateKeyCertTag);

	iser::CArchiveTag verifyModeTag("VerifyMode", "", iser::CArchiveTag::TT_GROUP, &mainTag);
	retVal = retVal && archive.BeginTag(verifyModeTag);
	if (m_verifyModeSelectionParamCompPtr.IsValid()){
		retVal = retVal && m_verifyModeSelectionParamCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(verifyModeTag);

	iser::CArchiveTag protocolTag("Protocol", "", iser::CArchiveTag::TT_GROUP, &mainTag);
	retVal = retVal && archive.BeginTag(protocolTag);
	if (m_protocolSelectionParamCompPtr.IsValid()){
		retVal = retVal && m_protocolSelectionParamCompPtr->Serialize(archive);
	}
	retVal = retVal && archive.EndTag(protocolTag);

	retVal = retVal && archive.EndTag(mainTag);

	return retVal;
}


// protected methods


// reimplemented (icomp::CComponentBase)
void CSslConfigurationComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (m_localCertificateModelCompPtr.IsValid()){
		m_localCertificateModelCompPtr->AttachObserver(&m_localCertificateUpdateBridge);
	}

	if (m_caCertificateModelCompPtr.IsValid()){
		m_caCertificateModelCompPtr->AttachObserver(&m_caCertificateUpdateBridge);
	}

	if (m_privateKeyModelCompPtr.IsValid()){
		m_privateKeyModelCompPtr->AttachObserver(&m_keyUpdateBridge);
	}

	if (m_verifyModeSelectionParamModelCompPtr.IsValid()){
		m_verifyModeSelectionParamModelCompPtr->AttachObserver(&m_verifyModeUpdateBridge);
	}

	if (m_protocolSelectionParamModelCompPtr.IsValid()){
		m_protocolSelectionParamModelCompPtr->AttachObserver(&m_protocolUpdateBridge);
	}
}


void CSslConfigurationComp::OnComponentDestroyed()
{
	if (m_localCertificateModelCompPtr.IsValid()){
		if (m_localCertificateModelCompPtr->IsAttached(&m_localCertificateUpdateBridge)){
			m_localCertificateModelCompPtr->DetachObserver(&m_localCertificateUpdateBridge);
		}
	}

	if (m_caCertificateModelCompPtr.IsValid()){
		if (m_caCertificateModelCompPtr->IsAttached(&m_caCertificateUpdateBridge)){
			m_caCertificateModelCompPtr->DetachObserver(&m_caCertificateUpdateBridge);
		}
	}

	if (m_privateKeyModelCompPtr.IsValid()){
		if (m_privateKeyModelCompPtr->IsAttached(&m_keyUpdateBridge)){
			m_privateKeyModelCompPtr->DetachObserver(&m_keyUpdateBridge);
		}
	}

	if (m_verifyModeSelectionParamModelCompPtr.IsValid()){
		if (m_verifyModeSelectionParamModelCompPtr->IsAttached(&m_verifyModeUpdateBridge)){
			m_verifyModeSelectionParamModelCompPtr->IsAttached(&m_verifyModeUpdateBridge);
		}
	}

	if (m_protocolSelectionParamModelCompPtr.IsValid()){
		if (m_protocolSelectionParamModelCompPtr->IsAttached(&m_protocolUpdateBridge)){
			m_protocolSelectionParamModelCompPtr->DetachObserver(&m_protocolUpdateBridge);
		}
	}

	BaseClass::OnComponentDestroyed();
}


} // namespace imtcom
