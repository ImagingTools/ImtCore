#include <imtrest/CGuiElementContainerComp.h>


namespace imtrest
{


// public methods

// reimplemented (imtrest::IGuiElementContainer)

QByteArrayList CGuiElementContainerComp::GetElementIds() const
{
	QByteArrayList retVal;

	for (int i = 0; i < m_guiElementModelsCompPtr.GetCount(); i++){
		imtrest::IGuiElementModel* guiElementPtr = m_guiElementModelsCompPtr[i];
		if (guiElementPtr != nullptr){
			retVal << guiElementPtr->GetElementId();
		}
	}

	if (m_slaveGuiElementContainerCompPtr.IsValid()){
		retVal << m_slaveGuiElementContainerCompPtr->GetElementIds();
	}

	return retVal;
}


const imtrest::IGuiElementModel* CGuiElementContainerComp::GetGuiElementModel(const QByteArray& elementId) const
{
	for (int i = 0; i < m_guiElementModelsCompPtr.GetCount(); i++){
		imtrest::IGuiElementModel* guiElementPtr = m_guiElementModelsCompPtr[i];
		if (guiElementPtr != nullptr){
			QByteArray guiElementId = guiElementPtr->GetElementId();
			if (guiElementId == elementId){
				return guiElementPtr;
			}
		}
	}

	if (m_slaveGuiElementContainerCompPtr.IsValid()){
		return m_slaveGuiElementContainerCompPtr->GetGuiElementModel(elementId);
	}

	return nullptr;
}


} // namespace imtgui


