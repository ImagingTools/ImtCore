#include <imtqml/CCompositeRepresentationComp.h>


namespace imtqml
{


// public methods

// reimplemented (imtgql::IItemBasedRepresentationProvider)

QByteArray CCompositeRepresentationComp::GetModelId() const
{
	return *m_commandIdAttrPtr;
}


imtbase::CTreeItemModel* CCompositeRepresentationComp::GetRepresentation(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields, const imtgql::IGqlContext* gqlContext)
{
	imtbase::CTreeItemModel* rootModel = nullptr;

	if (m_representationDataProviderCompPtr.GetCount() > 0){
		rootModel = new imtbase::CTreeItemModel();
		rootModel->SetIsArray(true);

		for (int index = 0; index < m_representationDataProviderCompPtr.GetCount(); index++){
			imtbase::CTreeItemModel* externModel = m_representationDataProviderCompPtr[index]->GetRepresentation(params, fields, gqlContext);
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


// reimplemented (imtgql::IGqlModelEditor)

bool CCompositeRepresentationComp::UpdateModelFromRepresentation(const QList<imtgql::CGqlObject> &params, imtbase::CTreeItemModel *baseModelPtr, const imtgql::IGqlContext* gqlContext)
{
	bool retVal = true;

	for (int index = 0; index < m_mutationDataDelegateCompPtr.GetCount(); index++){
		retVal = retVal && m_mutationDataDelegateCompPtr[index]->UpdateModelFromRepresentation(params, baseModelPtr, gqlContext);
	}

	return retVal;
}


} // namespace imtqml


