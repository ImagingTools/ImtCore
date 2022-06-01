#include <imtqml/CDatabaseSettingsDataProviderComp.h>


// Qt includes
#include <QtCore/QTranslator>

// ACF includes
#include <istd/CChangeGroup.h>
#include <imod/TModelWrap.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>
#include <iprm/IParamsSet.h>


namespace imtqml
{


// public methods

// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CDatabaseSettingsDataProviderComp::GetModelId() const
{
	return *m_paramIdAttrPtr;
}


imtbase::CTreeItemModel* CDatabaseSettingsDataProviderComp::GetTreeItemModel(
			const QList<imtgql::CGqlObject>& params,
			const QByteArrayList& /*fields*/)
{
	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	QByteArray paramId;
	QString paramName;

	if (m_paramIdAttrPtr.IsValid()){
		paramId = *m_paramIdAttrPtr;
		rootModelPtr->SetData("Id", *m_paramIdAttrPtr);
	}

	const QTranslator* translatorPtr = nullptr;
	int currentIndex = -1;
	if (m_translationManagerCompPtr.IsValid()){
		QByteArray languageId = GetLanguageIdFromInputParams(params);

		if (languageId.isEmpty()){
			currentIndex = 0;
			languageId = "en_US";
		}
		else{
			currentIndex = iprm::FindOptionIndexById(languageId, m_translationManagerCompPtr->GetLanguagesInfo());
		}

		if (currentIndex >= 0){
			translatorPtr = m_translationManagerCompPtr->GetLanguageTranslator(currentIndex);
		}
	}

	if (m_paramNameAttrPtr.IsValid()){
		paramName = *m_paramNameAttrPtr;
		if (translatorPtr != nullptr){
			if (m_translationManagerCompPtr->GetSlaveTranslationManager() != nullptr && currentIndex >= 0){
				const QTranslator* slaveTranslatorPtr = m_translationManagerCompPtr->GetSlaveTranslationManager()->GetLanguageTranslator(currentIndex);
				QString text1 = slaveTranslatorPtr->translate("Attribute", paramName.toUtf8());
				rootModelPtr->SetData("Name", slaveTranslatorPtr->translate("Attribute", paramName.toUtf8()));
			}
		}
		else{
			rootModelPtr->SetData("Name", paramName);
		}
	}

	rootModelPtr->SetData("ComponentType", "DatabaseSettingsInput");

	if (m_databaseSettingsCompPtr.IsValid()){
		imtbase::CTreeItemModel* parametersPtr = rootModelPtr->AddTreeModel("Parameters");

		int index = parametersPtr->InsertNewItem();

		QString dbName = m_databaseSettingsCompPtr->GetDatabaseName();
		parametersPtr->SetData("Id", "DBName", index);
		if (translatorPtr != nullptr){
			parametersPtr->SetData("Name", translatorPtr->translate("imtqml::CDatabaseSettingsDataProviderComp", "DB Name"), index);
		}
		else{
			parametersPtr->SetData("Name", QT_TR_NOOP("DB Name"), index);
		}
		parametersPtr->SetData("Value", dbName, index);
		parametersPtr->SetData("ComponentType", "TextInput", index);

		index = parametersPtr->InsertNewItem();

		QString hostName = m_databaseSettingsCompPtr->GetHost();
		parametersPtr->SetData("Id", "Host", index);
		if (translatorPtr != nullptr){
			parametersPtr->SetData("Name", translatorPtr->translate("imtqml::CDatabaseSettingsDataProviderComp", "Host"), index);
		}
		else{
			parametersPtr->SetData("Name", QT_TR_NOOP("Host"), index);
		}
		parametersPtr->SetData("Value", hostName, index);
		parametersPtr->SetData("ComponentType", "TextInput", index);

		index = parametersPtr->InsertNewItem();

		QString password = m_databaseSettingsCompPtr->GetPassword();
		parametersPtr->SetData("Id", "Password", index);
		if (translatorPtr != nullptr){
			parametersPtr->SetData("Name", translatorPtr->translate("imtqml::CDatabaseSettingsDataProviderComp", "Password"), index);
		}
		else{
			parametersPtr->SetData("Name", QT_TR_NOOP("Password"), index);
		}
		parametersPtr->SetData("Value", password, index);
		parametersPtr->SetData("ComponentType", "TextInput", index);

		index = parametersPtr->InsertNewItem();

		int port = m_databaseSettingsCompPtr->GetPort();
		parametersPtr->SetData("Id", "Port", index);
		if (translatorPtr != nullptr){
			parametersPtr->SetData("Name", translatorPtr->translate("imtqml::CDatabaseSettingsDataProviderComp", "Port"), index);
		}
		else{
			parametersPtr->SetData("Name", QT_TR_NOOP("Port"), index);
		}
		parametersPtr->SetData("Value", port, index);
		parametersPtr->SetData("ComponentType", "IntegerInput", index);

		index = parametersPtr->InsertNewItem();

		QString userName = m_databaseSettingsCompPtr->GetUserName();
		parametersPtr->SetData("Id", "Username", index);
		if (translatorPtr != nullptr){
			parametersPtr->SetData("Name", translatorPtr->translate("imtqml::CDatabaseSettingsDataProviderComp", "User name"), index);
		}
		else{
			parametersPtr->SetData("Name", QT_TR_NOOP("User name"), index);
		}

		parametersPtr->SetData("Value", userName, index);
		parametersPtr->SetData("ComponentType", "TextInput", index);

		rootModelPtr->SetData("Status", "OK");
	}

	return rootModelPtr;
}


// reimplemented (imtgql::IGqlMutationDataControllerDelegate)

imtbase::CTreeItemModel* CDatabaseSettingsDataProviderComp::UpdateBaseModelFromRepresentation(
		const QList<imtgql::CGqlObject>& params,
		imtbase::CTreeItemModel* baseModelPtr)
{
	Q_ASSERT(baseModelPtr != nullptr);

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	if (!m_databaseSettingsCompPtr.IsValid()){
		return nullptr;
	}

	if (baseModelPtr->ContainsKey("Parameters")){
		imtbase::CTreeItemModel* parameters = baseModelPtr->GetTreeItemModel("Parameters");

		for (int k = 0; k < parameters->GetItemsCount(); k++){
			QByteArray parameterId = parameters->GetData("Id", k).toByteArray();
			QString parameterValue = parameters->GetData("Value", k).toString();

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

		const QTranslator* translatorPtr = nullptr;
		if (m_translationManagerCompPtr.IsValid()){
			QByteArray languageId = GetLanguageIdFromInputParams(params);

			int currentIndex = -1;
			if (languageId.isEmpty()){
				currentIndex = 2;
				languageId = "ru_RU";
			}
			else{
				currentIndex = iprm::FindOptionIndexById(languageId, m_translationManagerCompPtr->GetLanguagesInfo());
			}

			if (currentIndex >= 0){
				translatorPtr = m_translationManagerCompPtr->GetLanguageTranslator(currentIndex);
			}
		}

		QByteArray paramId;
		QString paramName;

		if (baseModelPtr->ContainsKey("Id")){
			paramId = baseModelPtr->GetData("Id").toByteArray();
			rootModel->SetData("Id", paramId);
		}

		if (baseModelPtr->ContainsKey("Name")){
			paramName = baseModelPtr->GetData("Name").toString();
			if (translatorPtr != nullptr){
				rootModel->SetData("Name", translatorPtr->translate("Attribute", paramName.toUtf8()));
			}
			else{
				rootModel->SetData("Name", paramName);
			}
		}
	} 

	return rootModel;
}


} // namespace imtqml


