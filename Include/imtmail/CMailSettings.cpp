#include <imtmail/CMailSettings.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>

// ImtCore includes
#include <imtcore/Version.h>


namespace imtmail
{


CMailSettings::CMailSettings()
	:m_port(0),
	m_requestTimeout(30000),
	m_ignoreSslErrors(false)
{
}


QString CMailSettings::GetHost() const
{
	return m_host;
}


void CMailSettings::SetHost(const QString& host)
{
	if (m_host != host){
		istd::CChangeNotifier changeNotifier(this);

		m_host = host;
	}
}


int CMailSettings::GetPort() const
{
	return m_port;
}


void CMailSettings::SetPort(int port)
{
	if (m_port != port){
		istd::CChangeNotifier changeNotifier(this);

		m_port = port;
	}
}


QString CMailSettings::GetUserName() const
{
	return m_userName;
}


void CMailSettings::SetUserName(const QString& userName)
{
	if (m_userName != userName){
		istd::CChangeNotifier changeNotifier(this);

		m_userName = userName;
	}
}


QString CMailSettings::GetPassword() const
{
	return m_password;
}


void CMailSettings::SetPassword(const QString& password)
{
	if (m_password != password){
		istd::CChangeNotifier changeNotifier(this);

		m_password = password;
	}
}


QString CMailSettings::GetSenderAddress() const
{
	return m_senderAddress;
}


void CMailSettings::SetSenderAddress(const QString& address)
{
	if (m_senderAddress != address){
		istd::CChangeNotifier changeNotifier(this);

		m_senderAddress = address;
	}
}


int CMailSettings::GetRequestTimeout() const
{
	return m_requestTimeout;
}


void CMailSettings::SetRequestTimeout(int msec)
{
	if (m_requestTimeout != msec){
		istd::CChangeNotifier changeNotifier(this);

		m_requestTimeout = msec;
	}
}


bool CMailSettings::GetIgnoreSslErrors() const
{
	return m_ignoreSslErrors;
}


void CMailSettings::SetIgnoreSslErrors(bool ignore)
{
	if (m_ignoreSslErrors != ignore){
		istd::CChangeNotifier changeNotifier(this);

		m_ignoreSslErrors = ignore;
	}
}


bool CMailSettings::Serialize(iser::IArchive& archive)
{
	istd::CChangeNotifier notifier(archive.IsStoring() ? nullptr : this);

	iser::CArchiveTag hostTag("Host", "SMTP server host", iser::CArchiveTag::TT_LEAF);
	bool retVal = archive.BeginTag(hostTag);
	retVal = retVal && archive.Process(m_host);
	retVal = retVal && archive.EndTag(hostTag);

	iser::CArchiveTag portTag("Port", "SMTP server port", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(portTag);
	retVal = retVal && archive.Process(m_port);
	retVal = retVal && archive.EndTag(portTag);

	iser::CArchiveTag userNameTag("UserName", "User Name", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(userNameTag);
	retVal = retVal && archive.Process(m_userName);
	retVal = retVal && archive.EndTag(userNameTag);

	iser::CArchiveTag passwordTag("Password", "Password", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(passwordTag);
	retVal = retVal && archive.Process(m_password);
	retVal = retVal && archive.EndTag(passwordTag);

	iser::CArchiveTag senderAddressTag("SenderAddress", "Sender Address", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(senderAddressTag);
	retVal = retVal && archive.Process(m_senderAddress);
	retVal = retVal && archive.EndTag(senderAddressTag);

	iser::CArchiveTag requestTimeoutTag("RequestTimeout", "Request Timeout", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(requestTimeoutTag);
	retVal = retVal && archive.Process(m_requestTimeout);
	retVal = retVal && archive.EndTag(requestTimeoutTag);

	iser::CArchiveTag ignoreSslErrorsTag("IgnoreSslErrors", "Ignore SSL Errors", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(ignoreSslErrorsTag);
	retVal = retVal && archive.Process(m_ignoreSslErrors);
	retVal = retVal && archive.EndTag(ignoreSslErrorsTag);

	return retVal;
}


int CMailSettings::GetSupportedOperations() const
{
	return SO_COPY | SO_COMPARE | SO_CLONE | SO_RESET;
}


bool CMailSettings::CopyFrom(const IChangeable& object, CompatibilityMode mode)
{
	const imtmail::IMailSettings* sourcePtr = dynamic_cast<const imtmail::IMailSettings*>(&object);
	if (sourcePtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_host = sourcePtr->GetHost();
		m_port = sourcePtr->GetPort();
		m_userName = sourcePtr->GetUserName();
		m_password = sourcePtr->GetPassword();
		m_senderAddress = sourcePtr->GetSenderAddress();
		m_requestTimeout = sourcePtr->GetRequestTimeout();
		m_ignoreSslErrors = sourcePtr->GetIgnoreSslErrors();

		return true;
	}

	return false;
}


bool CMailSettings::IsEqual(const IChangeable& object) const
{
	const imtmail::IMailSettings* sourcePtr = dynamic_cast<const imtmail::IMailSettings*>(&object);
	if (sourcePtr != nullptr){
		return
			m_host == sourcePtr->GetHost() &&
			m_port == sourcePtr->GetPort() &&
			m_userName == sourcePtr->GetUserName() &&
			m_password == sourcePtr->GetPassword() &&
			m_senderAddress == sourcePtr->GetSenderAddress() &&
			m_requestTimeout == sourcePtr->GetRequestTimeout() &&
			m_ignoreSslErrors == sourcePtr->GetIgnoreSslErrors();
	}

	return false;
}


istd::IChangeable* CMailSettings::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CMailSettings> clonePtr(new CMailSettings);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CMailSettings::ResetData(CompatibilityMode mode)
{
	istd::CChangeNotifier changeNotifier(this);

	m_host.clear();
	m_port = 0;
	m_userName.clear();
	m_password.clear();
	m_senderAddress.clear();
	m_requestTimeout = 30000;
	m_ignoreSslErrors = false;

	return true;
}


} // namespace imtmail
