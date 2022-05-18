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
	for (int i = 0; i < m_headersIdsAttrPtr.GetCount(); i++){
		treeModel->InsertNewItem();
		treeModel->SetData("Id", m_headersIdsAttrPtr[i], i);
		treeModel->SetData("Name", m_headersNamesAttrPtr[i], i);
		QString headerName = m_headersNamesAttrPtr[i];
//		if (m_translationManagerCompPtr.IsValid()){
//			QByteArray languageId = GetLanguageIdFromInputParams(params);
//			int currentIndex = iprm::FindOptionIndexById(languageId, m_translationManagerCompPtr->GetLanguagesInfo());
//			if (languageId != "" && currentIndex >= 0){
//				const QTranslator* translatorPtr = m_translationManagerCompPtr->GetLanguageTranslator(currentIndex);
//				if (translatorPtr != nullptr){

//					QString text1 = translatorPtr->translate("", m_headersNamesAttrPtr[i].toUtf8());
//					QString text2 = translatorPtr->translate("Attribute", m_headersNamesAttrPtr[i].toUtf8());
//					treeModel->SetData("Name", translatorPtr->translate("", m_headersNamesAttrPtr[i].toUtf8()), i);
//				}
//			}
//		}
//		else{
//			treeModel->SetData("Name", m_headersNamesAttrPtr[i], i);
//		}
	}

	return treeModel;
}



} // namespace imtqml


