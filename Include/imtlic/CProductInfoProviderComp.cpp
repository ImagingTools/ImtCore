#include <imtlic/CProductInfoProviderComp.h>


namespace imtlic
{


QByteArray CProductInfoProviderComp::GetModelId() const
{
	return *m_modelIdAttrPtr;
}


imtbase::CTreeItemModel *CProductInfoProviderComp::GetRepresentation(const QList<imtgql::CGqlObject> &params, const QByteArrayList &fields, const imtgql::IGqlContext *gqlContext)
{
	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	for (int i = 0; i < m_productIdsAttrPtr.GetCount(); i++){
		int index = rootModelPtr->InsertNewItem();

		rootModelPtr->SetData("Id", m_productIdsAttrPtr[i], index);
		rootModelPtr->SetData("Name", m_productNamesAttrPtr[i], index);

		imtgql::IItemBasedRepresentationDataProvider* representationDataProvider = m_permissionsProviderCompPtr[i];
		if (representationDataProvider != nullptr){
			imtbase::CTreeItemModel* permissionsModel = representationDataProvider->GetRepresentation(params, fields);
			if (permissionsModel != nullptr){
				rootModelPtr->SetExternTreeModel("Permissions", permissionsModel, i);
			}
		}
	}

	return rootModelPtr;
}


} // namespace imtlic


