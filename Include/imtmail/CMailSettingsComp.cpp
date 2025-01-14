#include <imtmail/CMailSettingsComp.h>


namespace imtmail
{


// reimplemented (icomp::CComponentBase)

void CMailSettingsComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	BaseClass2::SetHost(*m_hostNameAttrPtr);
	BaseClass2::SetPort(*m_portAttrPtr);
	BaseClass2::SetUserName(*m_userNameAttrPtr);
	BaseClass2::SetPassword(*m_passwordAttrPtr);
	BaseClass2::SetSenderAddress(*m_senderAddressAttrPtr);
	BaseClass2::SetRequestTimeout(*m_requestTimeoutAttrPtr);
	BaseClass2::SetIgnoreSslErrors(*m_ignoreSslErrorsAttrPtr);
}


} // namespace imtmail
