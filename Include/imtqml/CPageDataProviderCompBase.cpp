#include <imtqml/CPageDataProviderCompBase.h>


namespace imtqml
{


// public methods

CPageDataProviderCompBase::CPageDataProviderCompBase() :
	CComponentBase()
{
}


// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CPageDataProviderCompBase::GetModelId() const
{
	return *m_pageIdAttrPtr;
}


imtbase::CTreeItemModel *CPageDataProviderCompBase::GetTreeItemModel(const QList<QByteArray>& query, const imtrest::QueryParams& params)
{
	imtbase::CTreeItemModel* rootModel = nullptr;
//	if (modelId == *m_pageIdAttrPtr){
	if (query.count() > 0 && params.value(PageEnum::ID) == *m_pageIdAttrPtr){
		if (query.count() == 1){
			rootModel = new imtbase::CTreeItemModel();
			rootModel->SetData(PageEnum::ID,*m_pageIdAttrPtr);
			rootModel->SetData(PageEnum::NAME, *m_pageNameAttrPtr);
			rootModel->SetData(PageEnum::ICON, *m_pageDefaultStatusIcon);
			rootModel->SetData(PageEnum::ICON_ON_SELECTED,*m_pageOnSelectedStatusIcon);
			rootModel->SetData(PageEnum::ICON_OFF_SELECTED,*m_pageOffSelectedStatusIcon);
			rootModel->SetData(PageEnum::ENABLED,"true");
		}
		else{
			QList<QByteArray> commandQuery = query;
			commandQuery.removeFirst();
			rootModel = new imtbase::CTreeItemModel();
			rootModel->SetData(PageEnum::ID,*m_pageIdAttrPtr);
			imtbase::CTreeItemModel* commandsModel = m_representationCommandProviderCompPtr->GetTreeItemModel(commandQuery, params);
			rootModel->SetExternTreeModel(PageEnum::COMMANDS, commandsModel);
		}
	}

	return rootModel;
}


} // namespace imtqml


