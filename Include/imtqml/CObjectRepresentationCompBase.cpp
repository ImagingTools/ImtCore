#include <imtqml/CObjectRepresentationCompBase.h>


namespace imtqml
{


// protected methods

const imtbase::CTreeItemModel* CObjectRepresentationCompBase::GetElementModel(const QByteArray &elementId, const imtbase::CTreeItemModel* elementsModelPtr) const
{
	for (int i = 0; i < elementsModelPtr->GetItemsCount(); i++){
		if (elementsModelPtr->ContainsKey("Id", i)){
			QByteArray currentElementId = elementsModelPtr->GetData("Id", i).toByteArray();
			if (currentElementId == elementId){
				return elementsModelPtr->GetModelFromItem(i);
			}
		}
	}

	return nullptr;
}


// reimplemented (imtgql::CGqlRepresentationDataControllerComp)

imtbase::CTreeItemModel *CObjectRepresentationCompBase::CreateRepresentationFromRequest(const imtgql::CGqlRequest &gqlRequest, QString &errorMessage) const
{
	return nullptr;
}


} // namespace imtqml


