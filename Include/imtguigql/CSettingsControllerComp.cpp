#include <imtguigql/CSettingsControllerComp.h>


namespace imtguigql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

imtbase::CTreeItemModel* CSettingsControllerComp::CreateResponse(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (m_modelIdsCompPtr.FindValue(gqlRequest.GetCommandId()) == -1){
		return nullptr;
	}

	const QList<imtgql::CGqlObject>* inputParamsPtr = gqlRequest.GetParams();
	imtgql::CGqlObject gqlObject;

	int operationType = OT_UNKNOWN;
	if (!GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType)){
		return nullptr;
	}

	Q_ASSERT(operationType != OT_UNKNOWN);

	switch (operationType){
	case OT_GET:
		return GetSettings(*inputParamsPtr, gqlObject, errorMessage);
	case OT_SAVE:
		return SaveSettings(*inputParamsPtr, gqlObject, errorMessage);
	}

	return nullptr;

//	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
//	imtbase::CTreeItemModel* settingDBModel = new imtbase::CTreeItemModel();
//	imtbase::CTreeItemModel* settingsModel = new imtbase::CTreeItemModel();
//	imtbase::CTreeItemModel* elements = new imtbase::CTreeItemModel();

//	QByteArrayList fields;

//	settingsModel->SetData("Id", "Preferences");

//	imtbase::CTreeItemModel* settingsTypes = settingsModel->AddTreeModel("SettingsTypes");

//	int index = settingsTypes->InsertNewItem();
//	settingsTypes->SetData("Id", "General", index);
//	settingsTypes->SetData("Name", "General", index);

//	imtbase::CTreeItemModel* settingBody = settingsTypes->AddTreeModel("SettingBody", index);

//	int indexBody = settingBody->InsertNewItem();

//	settingBody->SetData("Id", "DesignSchema", indexBody);
//	settingBody->SetData("Name", "Mode", indexBody);
//	settingBody->SetData("ComponentType", "ComboBox", indexBody);
//	elements = settingBody->AddTreeModel("Elements", indexBody);

//	int indexElem = elements->InsertNewItem();
//	elements->SetData("Id", "Light", indexElem);
//	elements->SetData("Name", "Light", indexElem);
//	elements->SetData("Value", 1, indexElem);

//	indexElem = elements->InsertNewItem();
//	elements->SetData("Id", "Dark", indexElem);
//	elements->SetData("Name", "Dark", indexElem);
//	elements->SetData("Value", 0, indexElem);

//	indexBody = settingBody->InsertNewItem();

//	settingBody->SetData("Id", "Language", indexBody);
//	settingBody->SetData("Name", "Language", indexBody);
//	settingBody->SetData("ComponentType", "ComboBox", indexBody);

//	elements = settingBody->AddTreeModel("Elements", indexBody);

//	indexElem = elements->InsertNewItem();
//	elements->SetData("Id", "en_US", indexElem);
//	elements->SetData("Name", "English", indexElem);
//	elements->SetData("Value", 1, indexElem);

//	indexElem = elements->InsertNewItem();
//	elements->SetData("Id", "de_DE", indexElem);
//	elements->SetData("Name", "German", indexElem);
//	elements->SetData("Value", 0, indexElem);

//	indexElem = elements->InsertNewItem();
//	elements->SetData("Id", "pl_PL", indexElem);
//	elements->SetData("Name", "Polish", indexElem);
//	elements->SetData("Value", 0, indexElem);

//	indexElem = elements->InsertNewItem();
//	elements->SetData("Id", "ru_RU", indexElem);
//	elements->SetData("Name", "Russian", indexElem);
//	elements->SetData("Value", 0, indexElem);

//	index = settingsTypes->InsertNewItem();
//	settingsTypes->SetData("Id", "DBSettings", index);
//	settingsTypes->SetData("Name", "DB settings", index);

//	settingBody = settingsTypes->AddTreeModel("SettingBody", index);
//	settingBody->SetIsArray(true);

////	settingDBModel = settingBody->AddTreeModel("DBSetting");

//	int dbIndex = settingBody->InsertNewItem();
//	settingBody->SetData("Id", "Lisa", dbIndex);
//	settingBody->SetData("Name", "Lisa", dbIndex);
//	settingBody->SetData("ComponentType", "DatabaseSettingsInput", dbIndex);
//	imtbase::CTreeItemModel* dbModel = settingBody->AddTreeModel("Elements", dbIndex);
//	indexBody = dbModel->InsertNewItem();

//	dbModel->SetData("Id", "DBName", indexBody);
//	dbModel->SetData("Name", "DB name", indexBody);
//	dbModel->SetData("Value", "lisa", indexBody);
//	dbModel->SetData("MultiLine", false, indexBody);
//	dbModel->SetData("ComponentType", "TextInput", indexBody);

//	indexBody = dbModel->InsertNewItem();

//	dbModel->SetData("Id", "Host", indexBody);
//	dbModel->SetData("Name", "Host", indexBody);
//	dbModel->SetData("Value", "localhost", indexBody);
//	dbModel->SetData("MultiLine", false, indexBody);
//	dbModel->SetData("ComponentType", "TextInput", indexBody);

//	indexBody = dbModel->InsertNewItem();

//	dbModel->SetData("Id", "Password", indexBody);
//	dbModel->SetData("Name", "Password", indexBody);
//	dbModel->SetData("Value", "roma1111", indexBody);
//	dbModel->SetData("MultiLine", false, indexBody);
//	dbModel->SetData("ComponentType", "TextInput", indexBody);

//	indexBody = dbModel->InsertNewItem();

//	dbModel->SetData("Id", "Port", indexBody);
//	dbModel->SetData("Name", "Port", indexBody);
//	dbModel->SetData("Value", "5432", indexBody);
//	dbModel->SetData("ComponentType", "IntegerInput", indexBody);

//	indexBody = dbModel->InsertNewItem();

//	dbModel->SetData("Id", "UserName", indexBody);
//	dbModel->SetData("Name", "User name", indexBody);
//	dbModel->SetData("Value", "postgres", indexBody);
//	dbModel->SetData("MultiLine", false, indexBody);
//	dbModel->SetData("ComponentType", "TextInput", indexBody);

//	rootModel->SetExternTreeModel("data", settingsModel);

//	return rootModel;
}


// protected methods
bool CSettingsControllerComp::GetOperationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			imtgql::CGqlObject& gqlObject,
			QString& errorMessage,
			int& operationType) const
{
	const QList<imtgql::CGqlObject>* fieldList = gqlRequest.GetFields();
	if(fieldList == nullptr){
		return false;
	}

	int count = fieldList->count();
	for (int i = 0; i < count; i++){
		if (fieldList->at(i).GetId() == "settings"){
			gqlObject = fieldList->at(i);
			operationType = OT_GET;
			return true;
		}
		if (fieldList->at(i).GetId() == "save"){
			gqlObject = fieldList->at(i);
			operationType = OT_SAVE;
			return true;
		}
	}
	return false;
}


imtbase::CTreeItemModel* CSettingsControllerComp::GetSettings(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* settingDBModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* settingsModel = new imtbase::CTreeItemModel();
	imtbase::CTreeItemModel* elements = new imtbase::CTreeItemModel();

	QByteArrayList fields;

	settingsModel->SetData("Id", "Preferences");

	imtbase::CTreeItemModel* settingsTypes = settingsModel->AddTreeModel("SettingsTypes");

	int index = settingsTypes->InsertNewItem();
	settingsTypes->SetData("Id", "General", index);
	settingsTypes->SetData("Name", "General", index);

	imtbase::CTreeItemModel* settingBody = settingsTypes->AddTreeModel("SettingBody", index);

	int indexBody = settingBody->InsertNewItem();

	settingBody->SetData("Id", "DesignSchema", indexBody);
	settingBody->SetData("Name", "Mode", indexBody);
	settingBody->SetData("ComponentType", "ComboBox", indexBody);
	elements = settingBody->AddTreeModel("Elements", indexBody);

	int indexElem = elements->InsertNewItem();
	elements->SetData("Id", "Light", indexElem);
	elements->SetData("Name", "Light", indexElem);
	elements->SetData("Value", 1, indexElem);

	indexElem = elements->InsertNewItem();
	elements->SetData("Id", "Dark", indexElem);
	elements->SetData("Name", "Dark", indexElem);
	elements->SetData("Value", 0, indexElem);

	indexBody = settingBody->InsertNewItem();

	settingBody->SetData("Id", "Language", indexBody);
	settingBody->SetData("Name", "Language", indexBody);
	settingBody->SetData("ComponentType", "ComboBox", indexBody);

	elements = settingBody->AddTreeModel("Elements", indexBody);

	indexElem = elements->InsertNewItem();
	elements->SetData("Id", "en_US", indexElem);
	elements->SetData("Name", "English", indexElem);
	elements->SetData("Value", 1, indexElem);

	indexElem = elements->InsertNewItem();
	elements->SetData("Id", "de_DE", indexElem);
	elements->SetData("Name", "German", indexElem);
	elements->SetData("Value", 0, indexElem);

	indexElem = elements->InsertNewItem();
	elements->SetData("Id", "pl_PL", indexElem);
	elements->SetData("Name", "Polish", indexElem);
	elements->SetData("Value", 0, indexElem);

	indexElem = elements->InsertNewItem();
	elements->SetData("Id", "ru_RU", indexElem);
	elements->SetData("Name", "Russian", indexElem);
	elements->SetData("Value", 0, indexElem);

	index = settingsTypes->InsertNewItem();
	settingsTypes->SetData("Id", "DBSettings", index);
	settingsTypes->SetData("Name", "DB settings", index);

	settingBody = settingsTypes->AddTreeModel("SettingBody", index);
	settingBody->SetIsArray(true);

//	settingDBModel = settingBody->AddTreeModel("DBSetting");

	int dbIndex = settingBody->InsertNewItem();
	settingBody->SetData("Id", "Lisa", dbIndex);
	settingBody->SetData("Name", "Lisa", dbIndex);
	settingBody->SetData("ComponentType", "DatabaseSettingsInput", dbIndex);
	imtbase::CTreeItemModel* dbModel = settingBody->AddTreeModel("Elements", dbIndex);
	indexBody = dbModel->InsertNewItem();

	dbModel->SetData("Id", "DBName", indexBody);
	dbModel->SetData("Name", "DB name", indexBody);
	dbModel->SetData("Value", "lisa", indexBody);
	dbModel->SetData("MultiLine", false, indexBody);
	dbModel->SetData("ComponentType", "TextInput", indexBody);

	indexBody = dbModel->InsertNewItem();

	dbModel->SetData("Id", "Host", indexBody);
	dbModel->SetData("Name", "Host", indexBody);
	dbModel->SetData("Value", "localhost", indexBody);
	dbModel->SetData("MultiLine", false, indexBody);
	dbModel->SetData("ComponentType", "TextInput", indexBody);

	indexBody = dbModel->InsertNewItem();

	dbModel->SetData("Id", "Password", indexBody);
	dbModel->SetData("Name", "Password", indexBody);
	dbModel->SetData("Value", "roma1111", indexBody);
	dbModel->SetData("MultiLine", false, indexBody);
	dbModel->SetData("ComponentType", "TextInput", indexBody);

	indexBody = dbModel->InsertNewItem();

	dbModel->SetData("Id", "Port", indexBody);
	dbModel->SetData("Name", "Port", indexBody);
	dbModel->SetData("Value", "5432", indexBody);
	dbModel->SetData("ComponentType", "IntegerInput", indexBody);

	indexBody = dbModel->InsertNewItem();

	dbModel->SetData("Id", "UserName", indexBody);
	dbModel->SetData("Name", "User name", indexBody);
	dbModel->SetData("Value", "postgres", indexBody);
	dbModel->SetData("MultiLine", false, indexBody);
	dbModel->SetData("ComponentType", "TextInput", indexBody);

	rootModel->SetExternTreeModel("data", settingsModel);

	return rootModel;
}


imtbase::CTreeItemModel* CSettingsControllerComp::SaveSettings(
		const QList<imtgql::CGqlObject> &inputParams,
		const imtgql::CGqlObject &gqlObject,
		QString &errorMessage) const
{
	return nullptr;
}

} // namespace imtgql


