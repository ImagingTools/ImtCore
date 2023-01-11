#include <imtgui/CGuiElementModelComp.h>


namespace imtgui
{


// public methods

// reimplemented (icomp::CComponentBase)

void CGuiElementModelComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	for (int i = 0; i < m_itemIdsAttrPtr.GetCount(); i++){
		QByteArray itemId = m_itemIdsAttrPtr[i];
		QString itemName = m_itemNamesAttrPtr[i];
		bool isEnabled = m_itemModesAttrPtr[i];

		m_collectionInfo.InsertItem(itemId, itemName, "");
		m_collectionInfo.SetElementEnabled(itemId, isEnabled);
	}
}


void CGuiElementModelComp::OnComponentDestroyed()
{
	m_collectionInfo.ResetData();

	BaseClass::OnComponentDestroyed();
}


// reimplemented (imtgui::IGuiElementContainer)

const IGuiElementContainer* CGuiElementModelComp::GetElementCommands(const QByteArray& elementId) const
{
	return nullptr;
}


QString CGuiElementModelComp::GetElementItemPath(const QByteArray& elementId) const
{
	int index = m_itemIdsAttrPtr.FindValue(elementId);

	if (index >= 0){
		int count = m_elementItemPathAttrPtr.GetCount();
		if (index < count){
			return m_elementItemPathAttrPtr[index];
		}
	}

	return QString();
}


QString CGuiElementModelComp::GetElementStatus(const QByteArray& elementId) const
{
	int index = m_itemIdsAttrPtr.FindValue(elementId);

	if (index >= 0){
		int count = m_elementItemStatusesAttrPtr.GetCount();
		if (index < count){
			return m_elementItemStatusesAttrPtr[index];
		}
	}

	return QString();
}


const imtbase::ICollectionInfo& CGuiElementModelComp::GetElementList() const
{
	return m_collectionInfo;
}


} // namespace imtgui


