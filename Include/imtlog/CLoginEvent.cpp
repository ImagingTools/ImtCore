#include <imtlog/CLoginEvent.h>


// ACF includes
#include <iser/IArchive.h>
#include <istd/TDelPtr.h>
#include <iser/CArchiveTag.h>
#include <istd/CClassInfo.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <ilog/CMessageContainer.h>


namespace imtlog
{


// public methods

CLoginEvent::CLoginEvent()
{
}


CLoginEvent::CLoginEvent(
			const LoginEventInfo& loginEventInfo,
			istd::IInformationProvider::InformationCategory category,
			const QString& text,
			const QString& source,
			int flags,
			const QDateTime* timeStampPtr)
	:BaseClass(category, MI_LOGIN_EVENT, text, source, flags, timeStampPtr),
	m_loginEventInfo(loginEventInfo)
{
}


const CLoginEvent::LoginEventInfo& CLoginEvent::GetLoginEventInfo() const
{
	return m_loginEventInfo;
}


// reimplemented (iser::ISerializable)

bool CLoginEvent::Serialize(iser::IArchive& archive)
{
	bool retVal = BaseClass::Serialize(archive);

	static iser::CArchiveTag actionTag("Action", "Action", iser::CArchiveTag::TT_LEAF);
	static iser::CArchiveTag userNameTag("UserName", "User name", iser::CArchiveTag::TT_LEAF);

	retVal = retVal && archive.BeginTag(actionTag);
	retVal = retVal && archive.Process(m_loginEventInfo.action);
	retVal = retVal && archive.EndTag(actionTag);

	retVal = retVal && archive.BeginTag(userNameTag);
	retVal = retVal && archive.Process(m_loginEventInfo.userName);
	retVal = retVal && archive.EndTag(userNameTag);

	return retVal;
}


// reimplemented (iser::IObject)

QByteArray CLoginEvent::GetFactoryId() const
{
	return GetTypeId();
}


// reimplemented (istd::IChangeable)

bool CLoginEvent::CopyFrom(const istd::IChangeable& object, istd::IChangeable::CompatibilityMode mode)
{
	const CLoginEvent* sourcePtr = dynamic_cast<const CLoginEvent*>(&object);
	if (sourcePtr != nullptr){
		m_loginEventInfo = sourcePtr->m_loginEventInfo;
		return true;
	}

	return false;
}


istd::IChangeable* CLoginEvent::CloneMe(istd::IChangeable::CompatibilityMode mode) const
{
	return new CLoginEvent(
				m_loginEventInfo,
				m_category,
				m_text,
				m_source,
				m_flags,
				&m_timeStamp);
}


// public static methods

QByteArray CLoginEvent::GetTypeId()
{
	return istd::CClassInfo::GetName<CLoginEvent>();
}


I_REGISTER_MESSAGE_TYPE(CLoginEvent, CLoginEvent::GetTypeId());


} // namespace imtlog


