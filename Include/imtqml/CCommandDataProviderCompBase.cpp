#include <imtqml/CCommandDataProviderCompBase.h>


// Qt includes
#include <QtCore/QTranslator>


namespace imtqml
{


// public methods

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
				if (m_translationManagerCompPtr.IsValid()){
					QByteArray languageId = GetLanguageIdFromInputParams(params);

					int currentIndex = -1;
					if (languageId.isEmpty()){
						currentIndex = 0;
						languageId = "en_US";
					}
					else{
						currentIndex = iprm::FindOptionIndexById(languageId, m_translationManagerCompPtr->GetLanguagesInfo());
					}

					if (currentIndex >= 0){
						const QTranslator* translatorPtr = m_translationManagerCompPtr->GetLanguageTranslator(currentIndex);
						if (translatorPtr != nullptr){
							treeModel->SetData(CommandEnum::NAME, translatorPtr->translate("Attribute", m_commandNameAttrPtr[i].toUtf8()), i);
						}
					}
				}
				else{
					treeModel->SetData(CommandEnum::NAME, m_commandNameAttrPtr[i].toUtf8(), i);
				}
			}
			if (fields[indexField] == CommandEnum::ICON && m_commandDefaultStatusIcon.GetCount() > i){
				treeModel->SetData(CommandEnum::ICON, m_commandDefaultStatusIcon[i], i);
			}
			if (fields[indexField] == CommandEnum::MODE){
				treeModel->SetData(CommandEnum::MODE, "Disabled", i);
			}
		}
	}

	return treeModel;
}


} // namespace imtqml


