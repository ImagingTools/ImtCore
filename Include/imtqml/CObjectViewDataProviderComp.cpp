#include <imtqml/CObjectViewDataProviderComp.h>


namespace imtqml
{


// public methods

// reimplemented (imtgql::IItemBasedRepresentationProvider)

QByteArray CObjectViewDataProviderComp::GetModelId() const
{
	QString str = *m_objectViewModelIdAttrPtr;
	return str.toUtf8();
}


imtbase::CTreeItemModel* CObjectViewDataProviderComp::GetRepresentation(
		const QList<imtgql::CGqlObject>& params,
		const QByteArrayList& /*fields*/,
		const imtgql::IGqlContext* gqlContext)
{
	imtbase::CTreeItemModel* treeModel = new imtbase::CTreeItemModel();

	if (m_objectViewAttrPtr.IsValid()){
		treeModel->SetData("Id", "ObjectViewProvider");
		treeModel->SetData("Path", *m_objectViewAttrPtr);
		treeModel->SetData("CommandsId", *m_objectViewModelIdAttrPtr);
	}

	return treeModel;
}


} // namespace imtqml


