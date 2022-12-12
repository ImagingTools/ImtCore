#include <imtqml/CCompositeObjectRepresentationComp.h>


namespace imtqml
{


// public methods

// reimplemented (imtgql::IItemBasedRepresentationProvider)

imtbase::CTreeItemModel* CCompositeObjectRepresentationComp::GetRepresentation(
			const QList<imtgql::CGqlObject>& params,
			const QByteArrayList& fields,
			const imtgql::IGqlContext* gqlContext)
{
	if (m_paramSubElementsCompPtr.IsValid()){
		imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

		imtbase::CTreeItemModel* elementListModelPtr = new imtbase::CTreeItemModel();
		elementListModelPtr->SetIsArray(true);

		for (int i = 0; i < m_paramSubElementsCompPtr.GetCount(); i++){
			imtgql::IItemBasedRepresentationDataProvider* subProviderPtr = m_paramSubElementsCompPtr[i];
			if (subProviderPtr != nullptr){
				imtbase::CTreeItemModel* subModelPtr = m_paramSubElementsCompPtr[i]->GetRepresentation(params, fields, gqlContext);
				if (subModelPtr != nullptr){
					int elementIndex = elementListModelPtr->InsertNewItem();

					elementListModelPtr->CopyItemDataFromModel(elementIndex, subModelPtr);
				}
			}
		}

        rootModelPtr->SetExternTreeModel("Elements", elementListModelPtr);

        return rootModelPtr;
    }

    return nullptr;
}


} // namespace imtqml


