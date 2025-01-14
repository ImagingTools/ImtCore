#pragma once


// ACF includes
#include <icomp/CComponentBase.h>

// ImtCore includes
#include <imtmail/CMailSettings.h>


namespace imtmail
{


class CMailSettingsComp:
						  public icomp::CComponentBase,
						  public CMailSettings
{
public:
	typedef icomp::CComponentBase BaseClass;
	typedef CMailSettings BaseClass2;

	I_BEGIN_COMPONENT(CMailSettingsComp);
		I_REGISTER_INTERFACE(IMailSettings);
		I_REGISTER_INTERFACE(iser::ISerializable);
		I_ASSIGN(m_hostNameAttrPtr, "HostName", "The property holds connection's host name.", true, "smtp.gmail.com");
		I_ASSIGN(m_portAttrPtr, "Port", "The property holds connection's port number", true, 465);
		I_ASSIGN(m_userNameAttrPtr, "UserName", "The property holds connection's user name", true, "");
		I_ASSIGN(m_passwordAttrPtr, "Password", "The property holds connection's password.", true, "");
		I_ASSIGN(m_senderAddressAttrPtr, "SenderAddress", "The property holds sender address", true, "");
		I_ASSIGN(m_requestTimeoutAttrPtr, "RequestTimeout", "The property holds request timeout", true, 30000);
		I_ASSIGN(m_ignoreSslErrorsAttrPtr, "IgnoreSslErrors", "The property holds ignore ssl errors", true, false);
	I_END_COMPONENT;

protected:
	// reimplemented (icomp::CComponentBase)
	virtual void OnComponentCreated() override;

private:
	I_ATTR(QByteArray, m_hostNameAttrPtr);
	I_ATTR(int, m_portAttrPtr);
	I_ATTR(QByteArray, m_userNameAttrPtr);
	I_ATTR(QByteArray, m_passwordAttrPtr);
	I_ATTR(QByteArray, m_senderAddressAttrPtr);
	I_ATTR(int, m_requestTimeoutAttrPtr);
	I_ATTR(bool, m_ignoreSslErrorsAttrPtr);
};


} // namespace imtmail



