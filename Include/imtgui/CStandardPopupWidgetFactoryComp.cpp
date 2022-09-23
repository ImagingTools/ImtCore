#include <imtgui/CStandardPopupWidgetFactoryComp.h>


// ImtCore includes
#include <imtgui/CStandardPopupWidget.h>


namespace imtgui
{


// public methods

// reimplemented (istd::TIFactory)

IPopupWidget* CStandardPopupWidgetFactoryComp::CreateInstance(const QByteArray& keyId) const
{
	if (keyId.isEmpty()){
		return new CStandardPopupWidget();
	}

	return nullptr;
}


// reimplemented (istd::IFactoryInfo)

istd::IFactoryInfo::KeyList CStandardPopupWidgetFactoryComp::GetFactoryKeys() const
{
	static QSet<QByteArray> keyList({QByteArray()});

	return keyList;
}


} // namespace imtgui


