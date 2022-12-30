#include <imtbase/CDatabaseAccessSettingsRepresentationControllerComp.h>


// ImtCore includes
#include <imtdb/IDatabaseLoginSettings.h>


namespace imtbase
{


// public methods

// reimplemented (IRepresentationController)

bool CDatabaseAccessSettingsRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtdb::IDatabaseLoginSettings* databaseLoginSettingsPtr = dynamic_cast<const imtdb::IDatabaseLoginSettings*>(&dataModel);
	if (databaseLoginSettingsPtr != nullptr){
		return true;
	}

	return false;
}


bool CDatabaseAccessSettingsRepresentationControllerComp::GetRepresentationFromDataModel(const istd::IChangeable& dataModel, CTreeItemModel& representation) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const imtdb::IDatabaseLoginSettings* databaseLoginSettingsPtr = dynamic_cast<const imtdb::IDatabaseLoginSettings*>(&dataModel);
	if (databaseLoginSettingsPtr != nullptr){
		representation.SetData("Id", *m_paramIdAttrPtr);
		representation.SetData("Name", *m_paramNameAttrPtr);

		if (m_qmlPathAttrPtr.IsValid()){
			representation.SetData("Source", *m_qmlPathAttrPtr);
		}

		CTreeItemModel* elementsModelPtr = representation.AddTreeModel("Parameters");
		Q_ASSERT(elementsModelPtr != nullptr);

		int index = elementsModelPtr->InsertNewItem();

		QString dbName = databaseLoginSettingsPtr->GetDatabaseName();

		elementsModelPtr->SetData("Id", "DatabaseName", index);
		elementsModelPtr->SetData("Name", QT_TR_NOOP("Database Name"), index);
		elementsModelPtr->SetData("Value", dbName, index);
		if (m_dbNamePathAttrPtr.IsValid()){
			elementsModelPtr->SetData("Source", *m_dbNamePathAttrPtr, index);
		}

		index = elementsModelPtr->InsertNewItem();

		QString hostName = databaseLoginSettingsPtr->GetHost();

		elementsModelPtr->SetData("Id", "Host", index);
		elementsModelPtr->SetData("Name", QT_TR_NOOP("Host"), index);
		elementsModelPtr->SetData("Value", hostName, index);
		if (m_hostPathAttrPtr.IsValid()){
			elementsModelPtr->SetData("Source", *m_hostPathAttrPtr, index);
		}

		index = elementsModelPtr->InsertNewItem();

		QString password = databaseLoginSettingsPtr->GetPassword();

		elementsModelPtr->SetData("Id", "Password", index);
		elementsModelPtr->SetData("Name", QT_TR_NOOP("Password"), index);
		elementsModelPtr->SetData("Value", password, index);
		if (m_passwordPathAttrPtr.IsValid()){
			elementsModelPtr->SetData("Source", *m_passwordPathAttrPtr, index);
		}

		index = elementsModelPtr->InsertNewItem();

		int port = databaseLoginSettingsPtr->GetPort();

		elementsModelPtr->SetData("Id", "Port", index);
		elementsModelPtr->SetData("Name", QT_TR_NOOP("Port"), index);
		elementsModelPtr->SetData("Value", port, index);
		if (m_portAttrPtr.IsValid()){
			elementsModelPtr->SetData("Source", *m_portAttrPtr, index);
		}

		index = elementsModelPtr->InsertNewItem();

		QString userName = databaseLoginSettingsPtr->GetUserName();

		elementsModelPtr->SetData("Id", "Username", index);
		elementsModelPtr->SetData("Name", QT_TR_NOOP("Username"), index);
		elementsModelPtr->SetData("Value", userName, index);
		if (m_usernamePathAttrPtr.IsValid()){
			elementsModelPtr->SetData("Source", *m_usernamePathAttrPtr, index);
		}

		return true;
	}

	return false;
}


bool CDatabaseAccessSettingsRepresentationControllerComp::GetDataModelFromRepresentation(const CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	imtdb::IDatabaseLoginSettings* databaseLoginSettingsPtr = dynamic_cast<imtdb::IDatabaseLoginSettings*>(&dataModel);
	if (databaseLoginSettingsPtr != nullptr){
		for (int i = 0; i < representation.GetItemsCount(); ++i){
			QByteArray parameterId;
			if (representation.ContainsKey("Id", i)){
				parameterId = representation.GetData("Id", i).toByteArray();
			}

			QString parameterValue;
			if (representation.ContainsKey("Value", i)){
				parameterValue = representation.GetData("Value", i).toString();
			}

			if (parameterId == "DBName"){
				databaseLoginSettingsPtr->SetDatabaseName(parameterValue);
			}
			else if (parameterId == "Host"){
				databaseLoginSettingsPtr->SetHost(parameterValue);
			}
			else if (parameterId == "Password"){
				databaseLoginSettingsPtr->SetPassword(parameterValue);
			}
			else if (parameterId == "Port"){
				databaseLoginSettingsPtr->SetPort(parameterValue.toInt());
			}
			else if (parameterId == "Username"){
				databaseLoginSettingsPtr->SetUserName(parameterValue);
			}
		}

		return true;
	}


	return false;
}


} // namespace imtbase


