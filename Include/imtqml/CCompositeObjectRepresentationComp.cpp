#include <imtqml/CCompositeObjectRepresentationComp.h>


namespace imtqml
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CCompositeObjectRepresentationComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_representationDataControllersCompPtr.IsValid()){
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

//		imtbase::CTreeItemModel* elementListModelPtr = new imtbase::CTreeItemModel();

//		for (int i = 0; i < m_representationDataControllersCompPtr.GetCount(); i++){
//			imtbase::IRepresentationController* subProviderPtr = m_representationDataControllersCompPtr[i];
//			if (subProviderPtr != nullptr){
//				imtbase::CTreeItemModel* subModelPtr = subProviderPtr->
//				if (subModelPtr != nullptr){
//					int elementIndex = rootModelPtr->InsertNewItem();

//					rootModelPtr->CopyItemDataFromModel(elementIndex, subModelPtr);
//				}
//			}
//		}

//		rootModelPtr->SetExternTreeModel("Elements", elementListModelPtr);

		return rootModelPtr.PopPtr();
	}

	return nullptr;
}


bool CCompositeObjectRepresentationComp::UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const
{
	bool retVal = true;

//	for (int i = 0; i < m_representationDataControllersCompPtr.GetCount(); i++){
//		imtgql::CGqlRepresentationDataControllerComp* subProviderPtr = m_representationDataControllersCompPtr[i];
//		if (subProviderPtr != nullptr){
//			retVal = retVal && subProviderPtr->UpdateModelFromRepresentation(request, representationPtr);
//		}
//	}

	return retVal;
}


} // namespace imtqml


