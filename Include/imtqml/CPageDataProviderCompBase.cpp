#include <imtqml/CPageDataProviderCompBase.h>


// Qt includes
#include <QtCore/QTranslator>


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
			if (m_translationManagerCompPtr.IsValid()){
				QByteArray languageId = GetLanguageIdFromInputParams(params);
				int currentIndex = -1;

				if (languageId.isEmpty()){
//					currentIndex = 0;

//					languageId = "en_US";

					currentIndex = m_translationManagerCompPtr->GetCurrentLanguageIndex();
				}
				else{
					currentIndex = iprm::FindOptionIndexById(languageId, m_translationManagerCompPtr->GetLanguagesInfo());
				}

				if (currentIndex >= 0){
					const QTranslator* translatorPtr = m_translationManagerCompPtr->GetLanguageTranslator(currentIndex);
					if (translatorPtr != nullptr){
						QString pageName = (*m_pageNameAttrPtr);
						QString tr = translatorPtr->translate("Attribute", pageName.toUtf8());
//						rootModelPtr->SetData(PageEnum::NAME, translatorPtr->translate("Attribute", (*m_pageNameAttrPtr).toUtf8()));

						rootModelPtr->SetData(PageEnum::NAME, pageName);
					}
				}
			}
			else{
				rootModelPtr->SetData(PageEnum::NAME, *m_pageNameAttrPtr);
			}
		}
		if (fields[indexField] == PageEnum::ICON){
			rootModelPtr->SetData(PageEnum::ICON, *m_pageDefaultStatusIconAttrPtr);
		}
		else if (fields[indexField] == PageEnum::ICON_ON_SELECTED){
			rootModelPtr->SetData(PageEnum::ICON_ON_SELECTED, *m_pageOnSelectedStatusIconAttrPtr);
		}
		else if (fields[indexField] == PageEnum::ICON_OFF_SELECTED){
			rootModelPtr->SetData(PageEnum::ICON_OFF_SELECTED, *m_pageOffSelectedStatusIconAttrPtr);
		}
		else if (fields[indexField] == PageEnum::SOURCE){
			rootModelPtr->SetData(PageEnum::SOURCE, *m_pageSourceItemAttrPtr);
		}
		else if (fields[indexField] == PageEnum::ENABLED){
			rootModelPtr->SetData(PageEnum::ENABLED, "true");
		}
		else if (fields[indexField] == "StartItem"){
			if (m_startSourceItemAttrPtr.IsValid()){
				rootModelPtr->SetData("StartItem", *m_startSourceItemAttrPtr);
			}
			else{
				rootModelPtr->SetData("StartItem", "");
			}
		}
	}

	if (m_subPagesDataProviderCompPtr.IsValid()){
		imtbase::CTreeItemModel* subPagesModel =  m_subPagesDataProviderCompPtr->GetTreeItemModel(params, fields);
		rootModelPtr->SetExternTreeModel("SubPages", subPagesModel);
	}
	rootModelPtr->SetData("isOpened", false);

	return rootModelPtr;
}


} // namespace imtqml


