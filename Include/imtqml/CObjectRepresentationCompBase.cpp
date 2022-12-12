#include <imtqml/CObjectRepresentationCompBase.h>


// Qt includes
#include <QtCore/QTranslator>

// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>
#include <iprm/ITextParam.h>


namespace imtqml
{


// public methods

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


// reimplemented (imtgql::IItemBasedRepresentationProvider)

QByteArray CObjectRepresentationCompBase::GetModelId() const
{
	return *m_paramIdAttrPtr;
}


imtbase::CTreeItemModel* CObjectRepresentationCompBase::GetRepresentation(
			const QList<imtgql::CGqlObject>& params,
			const QByteArrayList& fields,
			const imtgql::IGqlContext* gqlContext)
{
	return nullptr;
}


// reimplemented (imtgql::IGqlModelEditor)

bool CObjectRepresentationCompBase::UpdateModelFromRepresentation(
		const QList<imtgql::CGqlObject> &params,
		imtbase::CTreeItemModel *baseModel,
		const imtgql::IGqlContext* gqlContext)
{
	return true;
}


} // namespace imtqml


