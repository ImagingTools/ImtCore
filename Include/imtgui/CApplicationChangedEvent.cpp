#include <imtgui/CApplicationChangedEvent.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CClassInfo.h>
#include <istd/CChangeNotifier.h>
#include <istd/CChangeGroup.h>
#include <ilog/CMessageContainer.h>


namespace imtgui
{


// public methods

CApplicationChangedEvent::CApplicationChangedEvent()
	:BaseClass(nullptr)
{
}


CApplicationChangedEvent::CApplicationChangedEvent(
			const ApplicationChangesInfo& applicationChangesInfo,
			istd::IInformationProvider::InformationCategory category,
			const QString& text,
			const QString& source,
			int flags,
			const QDateTime* timeStampPtr,
			const iser::IObjectFactory* factoryPtr)
	:BaseClass(category, MI_DOCUMENT_CHANGED, text, source, flags, timeStampPtr, factoryPtr),
	m_applicationChangesInfo(applicationChangesInfo)
{
}


const CApplicationChangedEvent::ApplicationChangesInfo& CApplicationChangedEvent::GetApplicationChangesInfo() const
{
	return m_applicationChangesInfo;
}


// reimplemented (iser::IObject)

QByteArray CApplicationChangedEvent::GetFactoryId() const
{
	return GetTypeId();
}


// public static methods

QByteArray CApplicationChangedEvent::GetTypeId()
{
	return istd::CClassInfo::GetName<CApplicationChangedEvent>();
}


I_REGISTER_MESSAGE_TYPE(CApplicationChangedEvent, CApplicationChangedEvent::GetTypeId());


} // namespace imtgui


