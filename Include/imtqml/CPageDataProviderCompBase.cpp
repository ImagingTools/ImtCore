#include <imtqml/CPageDataProviderCompBase.h>


namespace imtqml
{


// public methods

// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CPageDataProviderCompBase::GetModelId() const
{
	return *m_pageIdAttrPtr;
}


imtbase::CTreeItemModel* CPageDataProviderCompBase::GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields)
{
	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	for (int indexField = 0; indexField < fields.count(); indexField++){
		if (fields[indexField] == PageEnum::ID){
			QString strId = QString(*m_pageIdAttrPtr);
			rootModelPtr->SetData(PageEnum::ID, strId);
		}
		if (fields[indexField] == PageEnum::NAME){
			rootModelPtr->SetData(PageEnum::NAME, *m_pageNameAttrPtr);
		}
		if (fields[indexField] == PageEnum::ICON){
			rootModelPtr->SetData(PageEnum::ICON, *m_pageDefaultStatusIconAttrPtr);
		}
		if (fields[indexField] == PageEnum::ICON_ON_SELECTED){
			rootModelPtr->SetData(PageEnum::ICON_ON_SELECTED,*m_pageOnSelectedStatusIconAttrPtr);
		}
		if (fields[indexField] == PageEnum::ICON_OFF_SELECTED){
			rootModelPtr->SetData(PageEnum::ICON_OFF_SELECTED,*m_pageOffSelectedStatusIconAttrPtr);
		}
		if (fields[indexField] == PageEnum::SOURCE){
			rootModelPtr->SetData(PageEnum::SOURCE,*m_pageSourceItemAttrPtr);
		}
		if (fields[indexField] == PageEnum::ENABLED){
			rootModelPtr->SetData(PageEnum::ENABLED,"true");
		}
	}

	return rootModelPtr;
}


} // namespace imtqml


