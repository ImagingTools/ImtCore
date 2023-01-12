#include <imtbase/CGuiElementRepresentationControllerComp.h>


// ImtCore includes
#include <imtgui/IGuiElementContainer.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CGuiElementRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtgui::IGuiElementContainer* guiElementPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
	if (guiElementPtr != nullptr) {
		return true;
	}

	return false;
}


bool CGuiElementRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const imtgui::IGuiElementContainer* guiElementPtr = dynamic_cast<const imtgui::IGuiElementContainer*>(&dataModel);
	if (guiElementPtr != nullptr){
		const imtbase::ICollectionInfo& collectionInfo = guiElementPtr->GetElementList();
		imtbase::ICollectionInfo::Ids elementIds = collectionInfo.GetElementIds();
		for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
			int index = representation.InsertNewItem();

			QString name = collectionInfo.GetElementInfo(elementId, imtbase::ICollectionInfo::EIT_NAME).toString();
			bool isEnabled = collectionInfo.GetElementInfo(elementId, imtbase::ICollectionInfo::EIT_ENABLED).toBool();

			QString elementPath = guiElementPtr->GetElementItemPath(elementId);
			QString elementStatus = guiElementPtr->GetElementStatus(elementId);

			representation.SetData("Id", elementId, index);
			representation.SetData("Name", name, index);
			representation.SetData("IsEnabled", isEnabled, index);

			representation.SetData("Icon", elementPath, index);
			representation.SetData("Status", elementStatus, index);
		}

		return true;
	}

	return false;
}


bool CGuiElementRepresentationControllerComp::GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	return true;
}


} // namespace imtbase


