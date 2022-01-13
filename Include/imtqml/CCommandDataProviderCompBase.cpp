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
	QString str = *m_commandsModelIdAttrPtr;
	return str.toUtf8();
}


imtbase::CTreeItemModel* CCommandDataProviderCompBase::GetTreeItemModel(const QList<imtgql::CGqlObject>& params,const QByteArrayList& fields)
{
	imtbase::CTreeItemModel* treeModel = new imtbase::CTreeItemModel();
	for (int i = 0; i < m_commandIdAttrPtr.GetCount(); ++i){
		treeModel->InsertNewItem();
		for (int indexField = 0; indexField < fields.count(); indexField++){
			if (fields[indexField] == CommandEnum::ID){
				treeModel->SetData(CommandEnum::ID, m_commandIdAttrPtr[i], i);
			}
			if (fields[indexField] == CommandEnum::NAME && m_commandNameAttrPtr.GetCount() > i){
				treeModel->SetData(CommandEnum::NAME, m_commandNameAttrPtr[i], i);
			}
			if (fields[indexField] == CommandEnum::ICON && m_commandDefaultStatusIcon.GetCount() > i){
				treeModel->SetData(CommandEnum::ICON, m_commandDefaultStatusIcon[i], i);
			}
			if (fields[indexField] == CommandEnum::MODE){
				treeModel->SetData(CommandEnum::MODE, "Active", i);
			}
		}
	}

	return treeModel;
}


} // namespace imtqml


