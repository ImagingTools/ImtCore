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
	rootModelPtr->SetData("Source", *m_qmlPathAttrPtr);

	imtbase::CTreeItemModel* parametersModelPtr = rootModelPtr->AddTreeModel("Parameters");

	Q_ASSERT(parametersModelPtr != nullptr);

	int index = parametersModelPtr->InsertNewItem();

	QString dbName = m_databaseSettingsCompPtr->GetDatabaseName();

	parametersModelPtr->SetData("Id", "DatabaseName", index);
	parametersModelPtr->SetData("Name", QT_TR_NOOP("Database Name"), index);
	parametersModelPtr->SetData("Value", dbName, index);
	parametersModelPtr->SetData("Source", "SettingsTextInput.qml", index);

	index = parametersModelPtr->InsertNewItem();

	QString hostName = m_databaseSettingsCompPtr->GetHost();

	parametersModelPtr->SetData("Id", "Host", index);
	parametersModelPtr->SetData("Name", QT_TR_NOOP("Host"), index);
	parametersModelPtr->SetData("Value", hostName, index);
	parametersModelPtr->SetData("Source", "SettingsTextInput.qml", index);

	index = parametersModelPtr->InsertNewItem();

	QString password = m_databaseSettingsCompPtr->GetPassword();

	parametersModelPtr->SetData("Id", "Password", index);
	parametersModelPtr->SetData("Name", QT_TR_NOOP("Password"), index);
	parametersModelPtr->SetData("Value", password, index);
	parametersModelPtr->SetData("Source", "SettingsTextInput.qml", index);

	index = parametersModelPtr->InsertNewItem();

	int port = m_databaseSettingsCompPtr->GetPort();

	parametersModelPtr->SetData("Id", "Port", index);
	parametersModelPtr->SetData("Name", QT_TR_NOOP("Port"), index);
	parametersModelPtr->SetData("Value", port, index);
	parametersModelPtr->SetData("Source", "SettingsIntegerInput.qml", index);

	index = parametersModelPtr->InsertNewItem();

	QString userName = m_databaseSettingsCompPtr->GetUserName();

	parametersModelPtr->SetData("Id", "Username", index);
	parametersModelPtr->SetData("Name", QT_TR_NOOP("Username"), index);
	parametersModelPtr->SetData("Value", userName, index);
	parametersModelPtr->SetData("Source", "SettingsTextInput.qml", index);

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


