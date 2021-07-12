#include <imtqml/CCommandDataProviderCompBase.h>


namespace imtqml
{


// public methods

CCommandDataProviderCompBase::CCommandDataProviderCompBase() :
	CComponentBase()
{
}


// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CCommandDataProviderCompBase::GetModelId() const
{
	return *m_commandsModelIdAttrPtr;
}


imtbase::CTreeItemModel *CCommandDataProviderCompBase::GetTreeItemModel(const QList<QByteArray>& query, const imtrest::QweryParams& params)
{
	imtbase::CTreeItemModel* treeModel = nullptr;
	if (query.count() > 0 &&
			(params.value(CommandEnum::ID) == *m_commandsModelIdAttrPtr
			 || params.value(CommandEnum::ID) == "")){
		treeModel = new imtbase::CTreeItemModel();
		for (int i = 0; i < m_commandIdAttrPtr.GetCount(); ++i){
			treeModel->InsertNewItem();
			treeModel->SetData(CommandEnum::ID, m_commandIdAttrPtr[i], i);
			if (m_commandNameAttrPtr.GetCount() > i){
				treeModel->SetData(CommandEnum::NAME, m_commandNameAttrPtr[i], i);
			}
			if (m_commandDefaultStatusIcon.GetCount() > i){
				treeModel->SetData(CommandEnum::ICON, m_commandDefaultStatusIcon[i], i);
			}
			treeModel->SetData(CommandEnum::ENABLED,"true");
		}
	}
	return treeModel;
}


} // namespace imtqml


