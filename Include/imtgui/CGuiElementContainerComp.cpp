#include <imtgui/CGuiElementContainerComp.h>


namespace imtgui
{


// public methods

// reimplemented (imtgui::IGuiElementContainer)

QByteArrayList CGuiElementContainerComp::GetElementIds() const
{
	QByteArrayList retVal;

	for (int i = 0; i < m_guiElementModelsCompPtr.GetCount(); i++){
		imtgui::IGuiElementModel* guiElementPtr = m_guiElementModelsCompPtr[i];
		if (guiElementPtr != nullptr){
			retVal << guiElementPtr->GetElementId();
		}
	}

	return retVal;
}


const imtgui::IGuiElementModel* CGuiElementContainerComp::GetGuiElementModel(const QByteArray& elementId) const
{
	if (!m_guiElementModelsCompPtr.IsValid()){
		return nullptr;
	}

	for (int i = 0; i < m_guiElementModelsCompPtr.GetCount(); i++){
		imtgui::IGuiElementModel* guiElementPtr = m_guiElementModelsCompPtr[i];
		if (guiElementPtr != nullptr){
			QByteArray guiElementId = guiElementPtr->GetElementId();
			if (guiElementId == elementId){
				return guiElementPtr;
			}
		}
	}

	return nullptr;
}


} // namespace imtgui


