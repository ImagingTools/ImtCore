#include <imtbase/CPageBasedGuiRepresentationControllerComp.h>


// ImtCore includes
#include <imtgui/IGuiElementContainer.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CPageBasedGuiRepresentationControllerComp::IsModelSupported(const istd::IChangeable &dataModel) const
{
	const imtgui::IGuiElementContainer* guiElementPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
	if (guiElementPtr != nullptr) {
		return true;
	}

	return false;
}


bool CPageBasedGuiRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable &dataModel, CTreeItemModel &representation) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const imtgui::IGuiElementContainer* guiElementPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
	if (guiElementPtr != nullptr) {
		int count = guiElementPtr->GetOptionsCount();

		for (int i = 0; i < count; i++){
			QByteArray optionId = guiElementPtr->GetOptionId(i);
			QString optionName = guiElementPtr->GetOptionName(i);
			QString optionDescription = guiElementPtr->GetOptionDescription(i);

			int index = representation.InsertNewItem();

			representation.SetData("Id", optionId, index);
			representation.SetData("Name", optionName, index);
			representation.SetData("Description", optionDescription, index);

			const iqtgui::IVisualStatus* visualStatusPtr = guiElementPtr->GetVisualStatus(i);
			if (visualStatusPtr != nullptr){
				QIcon statusIcon = visualStatusPtr->GetStatusIcon();
				QString statusText =  visualStatusPtr->GetStatusText();

				representation.SetData("StatusIcon", statusIcon.name(), index);
				representation.SetData("StatusText", statusText, index);
			}
		}

		return true;
	}

	return false;
}


bool CPageBasedGuiRepresentationControllerComp::GetDataModelFromRepresentation(const CTreeItemModel &representation, istd::IChangeable &dataModel) const
{
	return false;
}


} // namespace imtbase


