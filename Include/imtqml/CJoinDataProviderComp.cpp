#include <imtqml/CJoinDataProviderComp.h>


namespace imtqml
{


// public methods

CJoinDataProviderComp::CJoinDataProviderComp() :
	CComponentBase()
{
}


// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CJoinDataProviderComp::GetModelId() const
{
	return *m_commandIdAttrPtr;
}


imtbase::CTreeItemModel* CJoinDataProviderComp::GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields)
{
	imtbase::CTreeItemModel* rootModel = nullptr;
	if (m_representationDataProviderCompPtr.GetCount() > 0){
		rootModel = new imtbase::CTreeItemModel();
		rootModel->SetIsArray(true);
		for (int index = 0; index < m_representationDataProviderCompPtr.GetCount(); index++){
			imtbase::CTreeItemModel* externModel = m_representationDataProviderCompPtr[index]->GetTreeItemModel(params, fields);
			rootModel->InsertNewItem();
			rootModel->CopyItemDataFromModel(index,externModel);
		}
	}

	return rootModel;
}


} // namespace imtqml


