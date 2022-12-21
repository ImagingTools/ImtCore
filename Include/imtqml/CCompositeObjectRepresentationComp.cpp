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

		QByteArray paramId = *m_paramIdAttrPtr;
		QString paramName = *m_paramNameAttrPtr;

		rootModelPtr->SetData("Id", paramId);
		rootModelPtr->SetData("Name", paramName);

		imtbase::CTreeItemModel* elementListModelPtr = new imtbase::CTreeItemModel();

		for (int i = 0; i < m_paramSubElementsCompPtr.GetCount(); i++){
			imtgql::IItemBasedRepresentationDataProvider* subProviderPtr = m_paramSubElementsCompPtr[i];
			if (subProviderPtr != nullptr){
				imtbase::CTreeItemModel* subModelPtr = subProviderPtr->GetRepresentation(params, fields, gqlContext);
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


bool CCompositeObjectRepresentationComp::UpdateModelFromRepresentation(const QList<imtgql::CGqlObject>& params, imtbase::CTreeItemModel* baseModelPtr, const imtgql::IGqlContext* gqlContext)
{
	if (params.size() > 0){
		QByteArray parameterId = params.at(0).GetFieldArgumentValue("Id").toByteArray();

		if (parameterId != *m_paramIdAttrPtr){
			return false;
		}
	}

	bool retVal = true;

	for (int i = 0; i < m_mutationDataDelegateCompPtr.GetCount(); i++){
		imtgql::IGqlModelEditor* mutationDelegatePtr = m_mutationDataDelegateCompPtr[i];
		if (mutationDelegatePtr != nullptr){
			retVal = retVal && m_mutationDataDelegateCompPtr[i]->UpdateModelFromRepresentation(params, baseModelPtr, gqlContext);
		}
	}

	return retVal;
}


} // namespace imtqml


