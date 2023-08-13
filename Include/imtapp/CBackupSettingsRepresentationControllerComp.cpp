#include <imtapp/CBackupSettingsRepresentationControllerComp.h>


// Qt includes
#include <QtCore/QDateTime>

// ImtCore includes
#include <imtapp/IBackupSettings.h>


namespace imtapp
{


// public methods

// reimplemented (IRepresentationController)

bool CBackupSettingsRepresentationControllerComp::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtapp::IBackupSettings* backupSettingsPtr = dynamic_cast<const imtapp::IBackupSettings*>(&dataModel);
	if (backupSettingsPtr != nullptr){
		return true;
	}

	return false;
}


bool CBackupSettingsRepresentationControllerComp::GetRepresentationFromDataModel(
			const istd::IChangeable& dataModel,
			imtbase::CTreeItemModel& representation,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	const imtapp::IBackupSettings* backupSettingsPtr = dynamic_cast<const imtapp::IBackupSettings*>(&dataModel);
	if (backupSettingsPtr != nullptr){
		representation.SetData("Id", *m_paramIdAttrPtr);
		representation.SetData("Name", "Backup");
		representation.SetData("Source", "qrc:/qml/imtgui/AuxComponents/DatabaseInput.qml");

		imtbase::CTreeItemModel* elementsModelPtr = representation.AddTreeModel("Parameters");
		Q_ASSERT(elementsModelPtr != nullptr);

		int index = elementsModelPtr->InsertNewItem();

		QDateTime startTime = backupSettingsPtr->GetStartTime();

		elementsModelPtr->SetData("Id", "StartTime", index);
		elementsModelPtr->SetData("Name", QT_TR_NOOP("Start Time"), index);
		elementsModelPtr->SetData("Value", startTime.toString("HH:mm"), index);
		elementsModelPtr->SetData("Source", "qrc:/qml/imtgui/AuxComponents/SettingsTimePicker.qml", index);

		index = elementsModelPtr->InsertNewItem();

		QString backupFolderPath = backupSettingsPtr->GetPath();

		elementsModelPtr->SetData("Id", "BackupFolder", index);
		elementsModelPtr->SetData("Name", QT_TR_NOOP("Backup Folder"), index);
		elementsModelPtr->SetData("Value", backupFolderPath, index);
		elementsModelPtr->SetData("Source", "qrc:/qml/imtgui/AuxComponents/PathPicker.qml", index);

		index = elementsModelPtr->InsertNewItem();

		int backupInterval = backupSettingsPtr->GetInterval();

		elementsModelPtr->SetData("Id", "Interval", index);
		elementsModelPtr->SetData("Name", QT_TR_NOOP("Backup Interval"), index);
		elementsModelPtr->SetData("Value", backupInterval, index);
		elementsModelPtr->SetData("Source", "qrc:/qml/imtgui/AuxComponents/SettingIntegerInput.qml", index);

		return true;
	}

	return false;
}


bool CBackupSettingsRepresentationControllerComp::GetDataModelFromRepresentation(const imtbase::CTreeItemModel& representation, istd::IChangeable& dataModel) const
{
	if (!IsModelSupported(dataModel)){
		return false;
	}

	imtapp::IBackupSettings* backupSettingsPtr = dynamic_cast<imtapp::IBackupSettings*>(&dataModel);
	if (backupSettingsPtr != nullptr){
		imtbase::CTreeItemModel* parametersPtr = representation.GetTreeItemModel("Parameters");
		if (parametersPtr != nullptr){
			for (int i = 0; i < parametersPtr->GetItemsCount(); ++i){
				QByteArray parameterId;
				if (parametersPtr->ContainsKey("Id", i)){
					parameterId = parametersPtr->GetData("Id", i).toByteArray();
				}

				QString parameterValue;
				if (parametersPtr->ContainsKey("Value", i)){
					parameterValue = parametersPtr->GetData("Value", i).toString();
				}

				if (parameterId == "StartTime"){
					QDateTime dateTime = QDateTime::fromString(parameterValue, "HH:mm");
					backupSettingsPtr->SetStartTime(dateTime);
				}
				else if (parameterId == "BackupFolder"){
					backupSettingsPtr->SetPath(parameterValue);
				}
				else if (parameterId == "Interval"){
					backupSettingsPtr->SetInterval(parameterValue.toInt());
				}
			}

			return true;
		}
	}

	return false;
}


} // namespace imtapp


