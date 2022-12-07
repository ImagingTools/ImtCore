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


imtbase::CTreeItemModel* CJoinDataProviderComp::GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext)
{
	imtbase::CTreeItemModel* rootModel = nullptr;
	if (m_representationDataProviderCompPtr.GetCount() > 0){
		rootModel = new imtbase::CTreeItemModel();
		rootModel->SetIsArray(true);
		for (int index = 0; index < m_representationDataProviderCompPtr.GetCount(); index++){
			imtbase::CTreeItemModel* externModel = m_representationDataProviderCompPtr[index]->GetTreeItemModel(params, fields, gqlContext);
			if (externModel != nullptr){
				for (int j = 0; j < externModel->GetItemsCount(); j++){
					int itemIndex = rootModel->InsertNewItem();
					rootModel->CopyItemDataFromModel(itemIndex, externModel, j);
				}
			}
		}
	}

	return rootModel;
}


// reimplemented (imtgql::IGqlMutationDataControllerDelegate)

bool CJoinDataProviderComp::UpdateModelFromRepresentation(const QList<imtgql::CGqlObject> &params, imtbase::CTreeItemModel *baseModelPtr, const imtgql::IGqlContext* gqlContext)
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	for (int index = 0; index < m_mutationDataDelegateCompPtr.GetCount(); index++){
//		imtbase::CTreeItemModel* pagesModel = new imtbase::CTreeItemModel();
//		baseModel->CopyItemDataToModel(index, pagesModel);

		m_mutationDataDelegateCompPtr[index]->UpdateModelFromRepresentation(params, baseModelPtr, gqlContext);

//		rootModel->InsertNewItem();
//		rootModel->CopyItemDataFromModel(index, externModel);
	}

	return rootModel;
}


} // namespace imtqml


