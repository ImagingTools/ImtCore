#include <imtlic/CProductInfoProviderComp.h>


namespace imtlic
{


QByteArray CProductInfoProviderComp::GetModelId() const
{
	return *m_modelIdAttrPtr;
}


imtbase::CTreeItemModel *CProductInfoProviderComp::GetTreeItemModel(const QList<imtgql::CGqlObject> &params, const QByteArrayList &fields, const imtgql::IGqlContext *gqlContext)
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	for (int i = 0; i < m_productIdsAttrPtr.GetCount(); i++){
		int index = rootModel->InsertNewItem();

		rootModel->SetData("Id", m_productIdsAttrPtr[i], index);
		rootModel->SetData("Name", m_productNamesAttrPtr[i], index);

		if (m_permissionsProviderCompPtr.IsValid()){
			imtbase::CTreeItemModel* permissionModel = m_permissionsProviderCompPtr[i]->GetTreeItemModel(params, fields);
			rootModel->SetExternTreeModel("Permissions", permissionModel, i);
		}
	}

	return rootModel;
}


} // namespace imtlic


