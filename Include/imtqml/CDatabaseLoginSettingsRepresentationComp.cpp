#include <imtqml/CDatabaseLoginSettingsRepresentationComp.h>


// Qt includes
#include <QtCore/QTranslator>


namespace imtqml
{


// public methods

// reimplemented (imtgql::IItemBasedRepresentationProvider)

imtbase::CTreeItemModel* CDatabaseLoginSettingsRepresentationComp::GetRepresentation(
			const QList<imtgql::CGqlObject>& params,
			const QByteArrayList& fields,
			const imtgql::IGqlContext* gqlContext)
{
	if (!m_databaseSettingsCompPtr.IsValid()){
		return nullptr;
	}

	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	QByteArray paramId = *m_paramIdAttrPtr;
	QString paramName = *m_paramNameAttrPtr;

	rootModelPtr->SetData("Id", paramId);
	rootModelPtr->SetData("Name", paramName);

	imtbase::CTreeItemModel* parametersModelPtr = rootModelPtr->AddTreeModel("Parameters");

	Q_ASSERT(parametersModelPtr != nullptr);

	int index = parametersModelPtr->InsertNewItem();

	QString dbName = m_databaseSettingsCompPtr->GetDatabaseName();

	return rootModelPtr;
}


// reimplemented (imtgql::IGqlModelEditor)

bool CDatabaseLoginSettingsRepresentationComp::UpdateModelFromRepresentation(
		const QList<imtgql::CGqlObject> &params,
		imtbase::CTreeItemModel *baseModelPtr,
		const imtgql::IGqlContext* gqlContext)
{
	if (!m_databaseSettingsCompPtr.IsValid()){
		return false;
	}

	QByteArray parameterId = *m_paramIdAttrPtr;

	const imtbase::CTreeItemModel* elementModelPtr = GetElementModel(parameterId, baseModelPtr);
	if (elementModelPtr != nullptr){
		if (elementModelPtr->ContainsKey("Parameters")){
			imtbase::CTreeItemModel* parametersModelPtr = elementModelPtr->GetTreeItemModel("Parameters");
			if (parametersModelPtr != nullptr){
				for (int k = 0; k < parametersModelPtr->GetItemsCount(); k++){
					QByteArray parameterId;
					if (parametersModelPtr->ContainsKey("Id", k)){
						parameterId = parametersModelPtr->GetData("Id", k).toByteArray();
					}

					QString parameterValue;
					if (parametersModelPtr->ContainsKey("Value", k)){
						parameterValue = parametersModelPtr->GetData("Value", k).toString();
					}

					if (parameterId == "DBName"){
						m_databaseSettingsCompPtr->SetDatabaseName(parameterValue);
					}
					else if (parameterId == "Host"){
						m_databaseSettingsCompPtr->SetHost(parameterValue);
					}
					else if (parameterId == "Password"){
						m_databaseSettingsCompPtr->SetPassword(parameterValue);
					}
					else if (parameterId == "Port"){
						m_databaseSettingsCompPtr->SetPort(parameterValue.toInt());
					}
					else if (parameterId == "Username"){
						m_databaseSettingsCompPtr->SetUserName(parameterValue);
					}
				}

				return true;
			}
		}
	}

	return false;
}


} // namespace imtqml


