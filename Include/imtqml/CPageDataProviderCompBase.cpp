#include <imtqml/CPageDataProviderCompBase.h>


namespace imtqml
{


// public methods

CPageDataProviderCompBase::CPageDataProviderCompBase()
	:BaseClass()
{
}


// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CPageDataProviderCompBase::GetModelId() const
{
	return *m_pageIdAttrPtr;
}


imtbase::CTreeItemModel* CPageDataProviderCompBase::GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields)
{
	imtbase::CTreeItemModel* rootModel = nullptr;
	rootModel = new imtbase::CTreeItemModel();
	for (int indexField = 0; indexField < fields.count(); indexField++){
		if (fields[indexField] == PageEnum::ID){
			QString strId = QString(*m_pageIdAttrPtr);
			rootModel->SetData(PageEnum::ID, strId);
		}
		if (fields[indexField] == PageEnum::NAME){
			rootModel->SetData(PageEnum::NAME, *m_pageNameAttrPtr);
		}
		if (fields[indexField] == PageEnum::ICON){
			rootModel->SetData(PageEnum::ICON, *m_pageDefaultStatusIcon);
		}
		if (fields[indexField] == PageEnum::ICON_ON_SELECTED){
			rootModel->SetData(PageEnum::ICON_ON_SELECTED,*m_pageOnSelectedStatusIcon);
		}
		if (fields[indexField] == PageEnum::ICON_OFF_SELECTED){
			rootModel->SetData(PageEnum::ICON_OFF_SELECTED,*m_pageOffSelectedStatusIcon);
		}
		if (fields[indexField] == PageEnum::SOURCE){
			rootModel->SetData(PageEnum::SOURCE,*m_pageSourceItem);
		}
		if (fields[indexField] == PageEnum::ENABLED){
			rootModel->SetData(PageEnum::ENABLED,"true");
		}
	}

	return rootModel;
}


} // namespace imtqml


