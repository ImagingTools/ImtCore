#include <imtgui/CPageBasedGuiModelComp.h>


namespace imtgui
{


// public methods

// reimplemented (iprm::IOptionsList)

int CPageBasedGuiModelComp::GetOptionsFlags() const
{
	return SCF_NONE;
}


int CPageBasedGuiModelComp::GetOptionsCount() const
{
	return m_pageIdsAttrPtr.GetCount();
}


QString CPageBasedGuiModelComp::GetOptionName(int index) const
{
	if (index < 0 || index >= m_pageNamesAttrPtr.GetCount()){
		return QString();
	}

	return m_pageNamesAttrPtr[index];
}


QString CPageBasedGuiModelComp::GetOptionDescription(int index) const
{
	return QString();
}


QByteArray CPageBasedGuiModelComp::GetOptionId(int index) const
{
	if (index < 0 || index >= m_pageIdsAttrPtr.GetCount()){
		return QByteArray();
	}

	return m_pageIdsAttrPtr[index];
}


bool CPageBasedGuiModelComp::IsOptionEnabled(int index) const
{
	if (index < 0 || index >= m_pageModesAttrPtr.GetCount()){
		return false;
	}

	return m_pageModesAttrPtr[index];
}


// reimplemented (iqtgui::IMultiVisualStatusProvider)

int CPageBasedGuiModelComp::GetStatusesCount() const
{
	return m_pageVisualStatusesCompPtr.GetCount();
}


const iqtgui::IVisualStatus* CPageBasedGuiModelComp::GetVisualStatus(int statusIndex) const
{
	if (statusIndex < 0 || statusIndex >= m_pageVisualStatusesCompPtr.GetCount()){
		return nullptr;
	}

	return m_pageVisualStatusesCompPtr[statusIndex];
}


// reimplemented (imtgui::IGuiElementContainer)

const IGuiElementContainer* CPageBasedGuiModelComp::GetElementCommands(const QByteArray& elementId) const
{
	return nullptr;
}


} // namespace imtgui


