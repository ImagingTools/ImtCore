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
		const QByteArrayList& /*fields*/)
{
	imtbase::CTreeItemModel* treeModel = new imtbase::CTreeItemModel();

	if (m_headersIdsAttrPtr.IsValid()){
		QByteArray languageId;
		imtbase::CTreeItemModel* headersModel = treeModel->AddTreeModel("Headers");
		for (int i = 0; i < m_headersIdsAttrPtr.GetCount(); i++){
			headersModel->InsertNewItem();
			headersModel->SetData("Id", m_headersIdsAttrPtr[i], i);

			if (m_translationManagerCompPtr.IsValid()){
				languageId = GetLanguageIdFromInputParams(params);

				int currentIndex;
				if (languageId.isEmpty()){
					currentIndex = m_translationManagerCompPtr->GetCurrentLanguageIndex();
//					currentIndex = 0;
//					languageId = "en_US";
				}
				else{
					currentIndex = iprm::FindOptionIndexById(languageId, m_translationManagerCompPtr->GetLanguagesInfo());
				}

				if (currentIndex >= 0){
					const QTranslator* translatorPtr = m_translationManagerCompPtr->GetLanguageTranslator(currentIndex);
					if (translatorPtr != nullptr){
//						headersModel->SetData("Name", translatorPtr->translate("Attribute", m_headersNamesAttrPtr[i].toUtf8()), i);

						headersModel->SetData("Name", m_headersNamesAttrPtr[i], i);
					}
				}
			}
			else{
				headersModel->SetData("Name", m_headersNamesAttrPtr[i], i);
			}
		}
		treeModel->SetData("LanguageId", languageId);
	}

	if (m_headersSearchByFilterAttrPtr.IsValid()){
		imtbase::CTreeItemModel* searchModel = treeModel->AddTreeModel("FilterSearch");
		for (int i = 0; i < m_headersSearchByFilterAttrPtr.GetCount(); i++){
			int index = searchModel->InsertNewItem();
			searchModel->SetData("Id", m_headersSearchByFilterAttrPtr[i], index);
		}
	}

	return treeModel;
}



} // namespace imtqml


