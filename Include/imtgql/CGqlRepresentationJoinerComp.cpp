#include <imtgql/CGqlRepresentationJoinerComp.h>


// ACF includes
#include <istd/TDelPtr.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel* CGqlRepresentationJoinerComp::CreateRepresentationFromRequest(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_representationControllersCompPtr.IsValid()){
		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> commonRepresentationModelPtr(new imtbase::CTreeItemModel);
	imtbase::CTreeItemModel* dataModelPtr = commonRepresentationModelPtr->AddTreeModel("data");
	for (int i = 0; i < m_representationControllersCompPtr.GetCount(); i++){
		imtgql::IGqlRepresentationController* representationControllerPtr = m_representationControllersCompPtr[i];
		if (representationControllerPtr != nullptr){
			imtbase::CTreeItemModel* representationModelPtr = representationControllerPtr->CreateRepresentationFromRequest(gqlRequest, errorMessage);
			if (representationModelPtr != nullptr){
				if (representationModelPtr->ContainsKey("data")){
					representationModelPtr = representationModelPtr->GetTreeItemModel("data");
				}

				int index = dataModelPtr->InsertNewItem();

				dataModelPtr->CopyItemDataFromModel(index, representationModelPtr);
			}
		}
	}

	return commonRepresentationModelPtr.PopPtr();
}


bool CGqlRepresentationJoinerComp::UpdateModelFromRepresentation(const imtgql::CGqlRequest& request, imtbase::CTreeItemModel* representationPtr) const
{
	if (!m_representationControllersCompPtr.IsValid()){
		return false;
	}

	Q_ASSERT(representationPtr != nullptr);
	if (representationPtr == nullptr){
		return false;
	}

	for (int i = 0; i < m_representationControllersCompPtr.GetCount(); i++){
		imtgql::IGqlRepresentationController* representationControllerPtr = m_representationControllersCompPtr[i];
		if (representationControllerPtr != nullptr){
			istd::TDelPtr<imtbase::CTreeItemModel> subModelPtr = representationPtr->GetModelFromItem(i);
			if (subModelPtr.IsValid()){
				bool result = representationControllerPtr->UpdateModelFromRepresentation(request, subModelPtr.GetPtr());
				if (!result){
					return false;
				}
			}
		}
	}

	return true;
}


} // namespace imtgql


