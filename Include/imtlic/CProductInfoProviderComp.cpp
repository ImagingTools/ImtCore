#include <imtlic/CProductInfoProviderComp.h>


namespace imtlic
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

bool CProductInfoProviderComp::IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const
{
	return true;
}


imtbase::CTreeItemModel* CProductInfoProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	for (int i = 0; i < m_productIdsAttrPtr.GetCount(); i++){
		int index = rootModelPtr->InsertNewItem();

		rootModelPtr->SetData("Id", m_productIdsAttrPtr[i], index);
		rootModelPtr->SetData("Name", m_productNamesAttrPtr[i], index);

		imtgql::IGqlRequestHandler* representationDataProvider = m_permissionsProviderCompPtr[i];
		if (representationDataProvider != nullptr){
			imtbase::CTreeItemModel* permissionsModelPtr = representationDataProvider->CreateResponse(gqlRequest, errorMessage);
			if (permissionsModelPtr != nullptr){
-				rootModelPtr->SetExternTreeModel("Permissions", permissionsModelPtr, i);
			}
		}
	}

	QString json = rootModelPtr->toJSON();

	return rootModelPtr.PopPtr();
}


} // namespace imtlic


