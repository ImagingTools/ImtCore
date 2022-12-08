#include <imtqml/CHeadersDataProviderComp.h>


// Qt includes
#include <QtCore/QTranslator>


namespace imtqml
{


// public methods

// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CHeadersDataProviderComp::GetModelId() const
{
	QString str = *m_headersModelIdAttrPtr;
	return str.toUtf8();
}


imtbase::CTreeItemModel* CHeadersDataProviderComp::GetTreeItemModel(
		const QList<imtgql::CGqlObject>& params,
		const QByteArrayList& /*fields*/,
		const imtgql::IGqlContext* gqlContext)
{
	imtbase::CTreeItemModel* dataModelPtr = new imtbase::CTreeItemModel();

	if (m_headersIdsAttrPtr.IsValid()){
		QByteArray languageId;
		imtbase::CTreeItemModel* headersModelPtr = dataModelPtr->AddTreeModel("Headers");
		for (int i = 0; i < m_headersIdsAttrPtr.GetCount(); i++){
			headersModelPtr->InsertNewItem();
			headersModelPtr->SetData("Id", m_headersIdsAttrPtr[i], i);

			if (m_translationManagerCompPtr.IsValid()){
				if(gqlContext != nullptr){
					languageId =  gqlContext->GetLanguageId();
				}

				QByteArray context = "Attribute";
				QString headerName = m_headersNamesAttrPtr[i];
				QString headerNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), headerName.toUtf8(), languageId, context);
				headersModelPtr->SetData("Name", headerNameTr, i);
			}
			else{
				headersModelPtr->SetData("Name", m_headersNamesAttrPtr[i], i);
			}
		}
//		dataModelPtr->SetData("LanguageId", languageId);
	}

	if (m_headersSearchByFilterAttrPtr.IsValid()){
		imtbase::CTreeItemModel* searchModel = dataModelPtr->AddTreeModel("FilterSearch");
		for (int i = 0; i < m_headersSearchByFilterAttrPtr.GetCount(); i++){
			int index = searchModel->InsertNewItem();
			searchModel->SetData("Id", m_headersSearchByFilterAttrPtr[i], index);
		}
	}

	return dataModelPtr;
}



} // namespace imtqml


