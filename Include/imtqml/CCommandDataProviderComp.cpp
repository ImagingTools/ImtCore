#include <imtqml/CCommandDataProviderComp.h>


// ImtCore includes
#include <imtbase/imtbase.h>


namespace imtqml
{


// protected methods

// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

imtbase::CTreeItemModel* CCommandDataProviderComp::CreateInternalResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	const QList<imtgql::CGqlObject> fieldsPtr = gqlRequest.GetFields();

	QByteArrayList fieldsList;
	for (int i = 0; i < fieldsPtr.count(); i++){
		if (fieldsPtr.at(i).GetId() == "items"){
			fieldsList = fieldsPtr.at(i).GetFieldIds();
			break;
		}
	}

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();

	QByteArray languageId;
	if(gqlContextPtr != nullptr){
		languageId = gqlContextPtr->GetLanguageId();
	}

	istd::TDelPtr<imtbase::CTreeItemModel> treeModelPtr(new imtbase::CTreeItemModel());

	for (int i = 0; i < m_commandsIdsAttrPtr.GetCount(); ++i){
		treeModelPtr->InsertNewItem();
		for (int indexField = 0; indexField < fieldsList.count(); indexField++){
			if (fieldsList[indexField] == "Id"){
				treeModelPtr->SetData("Id", m_commandsIdsAttrPtr[i], i);
			}
			else if (fieldsList[indexField] == "Name" && m_commandsNamesAttrPtr.GetCount() > i){
				QString commandName = m_commandsNamesAttrPtr[i];
				if (m_translationManagerCompPtr.IsValid()){
					QByteArray context = "Attribute";
					QString commandNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), commandName.toUtf8(), languageId, context);

					commandName = commandNameTr;
				}

				treeModelPtr->SetData("Name", commandName, i);
			}
			else if (fieldsList[indexField] == "Icon" && m_commandsDefaultStatusIconAttrPtr.GetCount() > i){
				treeModelPtr->SetData("Icon", m_commandsDefaultStatusIconAttrPtr[i], i);
			}
			else if (fieldsList[indexField] == "Mode"){
				treeModelPtr->SetData("Mode", m_commandsDefaultModesAttrPtr[i], i);
			}
			else if (fieldsList[indexField] == "Visible"){
				treeModelPtr->SetData("Visible", true, i);
			}
		}
	}

	return treeModelPtr.PopPtr();
}


} // namespace imtqml


