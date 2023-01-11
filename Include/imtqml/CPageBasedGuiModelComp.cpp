#include <imtqml/CPageBasedGuiModelComp.h>


namespace imtqml
{


// public methods

// reimplemented (imtqml::IPageElementContainer)

QString CPageBasedGuiModelComp::GetStartSourceItem(const QByteArray& elementId) const
{
	int index = m_itemIdsAttrPtr.FindValue(elementId);

	if (index >= 0){
		int count = m_startSourceItemAttrPtr.GetCount();
		if (index < count){
			return m_startSourceItemAttrPtr[index];
		}
	}

	return QString();
}


QString CPageBasedGuiModelComp::GetPageSourceItem(const QByteArray& elementId) const
{
	int index = m_itemIdsAttrPtr.FindValue(elementId);

	if (index >= 0){
		int count = m_pageSourceItemAttrPtr.GetCount();
		if (index < count){
			return m_pageSourceItemAttrPtr[index];
		}
	}

	return QString();
}


} // namespace imtqml


