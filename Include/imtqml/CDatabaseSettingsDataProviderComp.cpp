#include <imtqml/CDatabaseSettingsDataProviderComp.h>


// Qt includes
#include <QtCore/QTranslator>

// ACF includes
#include <istd/CChangeGroup.h>
#include <imod/TModelWrap.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>
#include <iprm/IParamsSet.h>

#include <imtgql/imtgql.h>


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
			const QByteArrayList& /*fields*/,
			const imtgql::IGqlContext* gqlContext)
{
	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	if (m_paramIdAttrPtr.IsValid()){
		rootModelPtr->SetData("Id", *m_paramIdAttrPtr);
	}

	const QTranslator* translatorPtr = nullptr;

	if (m_translationManagerCompPtr.IsValid()){
		QByteArray languageId;
		if(gqlContext != nullptr){
			languageId = gqlContext->GetLanguageId();
		}
		if (m_paramNameAttrPtr.IsValid()){
			QByteArray context = "Attribute";
			QString paramName = *m_paramNameAttrPtr;
			QString paramNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), paramName.toUtf8(), languageId, context);
			rootModelPtr->SetData("Name", *m_paramNameAttrPtr);
		}
    }// to copy

	if (m_paramNameAttrPtr.IsValid()){
		rootModelPtr->SetData("Name", *m_paramNameAttrPtr);
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

bool CDatabaseSettingsDataProviderComp::UpdateModelFromRepresentation(
		const QList<imtgql::CGqlObject>& params,
		imtbase::CTreeItemModel* baseModelPtr,
		const imtgql::IGqlContext* gqlContext)
{
	Q_ASSERT(baseModelPtr != nullptr);

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	if (!m_databaseSettingsCompPtr.IsValid()){
		return false;
	}

	QByteArray parameterId = *m_paramIdAttrPtr;

	const imtbase::CTreeItemModel* elementModelPtr = GetElementModel(parameterId, baseModelPtr);

	if (elementModelPtr->ContainsKey("Parameters")){
		imtbase::CTreeItemModel* parameters = elementModelPtr->GetTreeItemModel("Parameters");

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
		QByteArray paramId;
		QString paramName;
//		const QTranslator* translatorPtr = nullptr;
		if (m_translationManagerCompPtr.IsValid()){
			QByteArray languageId;
			if(gqlContext != nullptr){
				languageId = gqlContext->GetLanguageId();
			}

			if (baseModelPtr->ContainsKey("Name")){
				QByteArray context = "Attribute";
				paramName = baseModelPtr->GetData("Name").toString();
				QByteArray paramNameTr = imtbase::GetTranslation(m_translationManagerCompPtr.GetPtr(), paramName.toUtf8(), languageId, context);
				rootModel->SetData("Name", paramNameTr);
			}
		}
		else{
			rootModel->SetData("Name", paramName);
		}
		if (baseModelPtr->ContainsKey("Id")){
			paramId = baseModelPtr->GetData("Id").toByteArray();
			rootModel->SetData("Id", paramId);
		}


	} 

	return true;
}


} // namespace imtqml


