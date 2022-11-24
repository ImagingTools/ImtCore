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


imtbase::CTreeItemModel* CCommandDataProviderCompBase::GetTreeItemModel(
		const QList<imtgql::CGqlObject>& params,
		const QByteArrayList& fields,
		const imtgql::IGqlContext* gqlContext)
{
	imtbase::CTreeItemModel* treeModelPtr = new imtbase::CTreeItemModel();
	for (int i = 0; i < m_commandsIdsAttrPtr.GetCount(); ++i){
		treeModelPtr->InsertNewItem();
		for (int indexField = 0; indexField < fields.count(); indexField++){
			if (fields[indexField] == CommandEnum::ID){
				treeModelPtr->SetData(CommandEnum::ID, m_commandsIdsAttrPtr[i], i);
			}
			if (fields[indexField] == CommandEnum::NAME && m_commandsNamesAttrPtr.GetCount() > i){
				if (m_translationManagerCompPtr.IsValid()){
					QByteArray languageId;
					if(gqlContext != nullptr){
						languageId = gqlContext->GetLanguageId();
					}
					QByteArray context = "Attribute";
					QString commandName = m_commandsNamesAttrPtr[i];
					QString commandNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), commandName.toUtf8(), languageId, context);

					treeModelPtr->SetData(CommandEnum::NAME, commandNameTr, i);
				}
				else{
					treeModelPtr->SetData(CommandEnum::NAME, m_commandsNamesAttrPtr[i].toUtf8(), i);
				}
			}

			if (fields[indexField] == CommandEnum::ICON && m_commandsDefaultStatusIconAttrPtr.GetCount() > i){
				treeModelPtr->SetData(CommandEnum::ICON, m_commandsDefaultStatusIconAttrPtr[i], i);
			}

			if (fields[indexField] == CommandEnum::MODE){
				treeModelPtr->SetData(CommandEnum::MODE, m_commandsDefaultModesAttrPtr[i], i);
			}

			if (fields[indexField] == "Visible"){
				treeModelPtr->SetData("Visible", true, i);
			}
		}
	}

	return treeModelPtr;
}


} // namespace imtqml


