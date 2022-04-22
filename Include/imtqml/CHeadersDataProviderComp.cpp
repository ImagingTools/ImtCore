#include <imtqml/CHeadersDataProviderComp.h>


namespace imtqml
{


// public methods

CHeadersDataProviderComp::CHeadersDataProviderComp() :
	CComponentBase()
{
}


// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CHeadersDataProviderComp::GetModelId() const
{
	QString str = *m_headersModelIdAttrPtr;
	return str.toUtf8();
}


imtbase::CTreeItemModel* CHeadersDataProviderComp::GetTreeItemModel(
		const QList<imtgql::CGqlObject>& /*params*/,
		const QByteArrayList& /*fields*/)
{
	imtbase::CTreeItemModel* treeModel = new imtbase::CTreeItemModel();
	for (int i = 0; i < m_headersIdsAttrPtr.GetCount(); ++i){
		treeModel->InsertNewItem();

		treeModel->SetData("Id", m_headersIdsAttrPtr[i], i);
		treeModel->SetData("Name", m_headersNamesAttrPtr[i], i);
	}

	return treeModel;
}



} // namespace imtqml


