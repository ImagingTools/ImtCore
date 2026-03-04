// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CGuiElementContainerComp.h>


namespace imtserverapp
{


// public methods

// reimplemented (imtserverapp::IGuiElementContainer)

QByteArrayList CGuiElementContainerComp::GetElementIds() const
{
	QByteArrayList retVal;

	for (int i = 0; i < m_guiElementModelsCompPtr.GetCount(); i++){
		imtserverapp::IGuiElementModel* guiElementPtr = m_guiElementModelsCompPtr[i];
		if (guiElementPtr != nullptr){
			retVal << guiElementPtr->GetElementId();
		}
	}

	if (m_slaveGuiElementContainerCompPtr.IsValid()){
		retVal << m_slaveGuiElementContainerCompPtr->GetElementIds();
	}

	return retVal;
}


const imtserverapp::IGuiElementModel* CGuiElementContainerComp::GetGuiElementModel(const QByteArray& elementId) const
{
	for (int i = 0; i < m_guiElementModelsCompPtr.GetCount(); i++){
		imtserverapp::IGuiElementModel* guiElementPtr = m_guiElementModelsCompPtr[i];
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


