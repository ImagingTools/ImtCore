#include <imtbase/CPageBasedGuiRepresentationControllerComp.h>


// ImtCore includes
#include <imtqml/IPageElementContainer.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CPageBasedGuiRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtqml::IPageElementContainer* guiElementPtr = dynamic_cast<const imtqml::IPageElementContainer*>(&dataModel);
	if (guiElementPtr != nullptr) {
		return true;
	}

	return false;
}


bool CPageBasedGuiRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation, const iprm::IParamsSet* paramsPtr) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	bool result = BaseClass::GetRepresentationFromDataModel(dataModel, representation);
	if (result){
		const imtqml::IPageElementContainer* guiElementPtr = dynamic_cast<const imtqml::IPageElementContainer*>(&dataModel);
		if (guiElementPtr != nullptr){
			const imtbase::ICollectionInfo& collectionInfo = guiElementPtr->GetElementList();
			imtbase::ICollectionInfo::Ids elementIds = collectionInfo.GetElementIds();
			int index = 0;
			for (const imtbase::ICollectionInfo::Id& elementId : elementIds){
				QString pageSourceItem = guiElementPtr->GetPageSourceItem(elementId);
				QString startSourceItem = guiElementPtr->GetStartSourceItem(elementId);

				representation.SetData("Source", pageSourceItem, index);
				representation.SetData("StartItem", startSourceItem, index);

				index++;
			}

			return true;
		}
	}

	return false;
}


} // namespace imtbase


