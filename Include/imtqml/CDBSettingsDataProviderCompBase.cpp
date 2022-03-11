#include <imtqml/CDBSettingsDataProviderCompBase.h>

// ACF includes
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>
#include <istd/CChangeGroup.h>
#include <imod/TModelWrap.h>

#include <iprm/IParamsSet.h>

namespace imtqml
{


// public methods

// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CDBSettingsDataProviderCompBase::GetModelId() const
{
	return *m_paramIdAttrPtr;
}


imtbase::CTreeItemModel* CDBSettingsDataProviderCompBase::GetTreeItemModel(const QList<imtgql::CGqlObject>& params,
																		const QByteArrayList& fields)
{
	imtbase::CTreeItemModel* rootModelPtr = new imtbase::CTreeItemModel();

	QByteArray paramId;
	QString paramName;

	if (m_paramIdAttrPtr.IsValid()){
		paramId = *m_paramIdAttrPtr;
		rootModelPtr->SetData("Id", *m_paramIdAttrPtr);
	}

	if (m_paramNameAttrPtr.IsValid()){
		paramName = *m_paramNameAttrPtr;
		rootModelPtr->SetData("Name", paramName);
	}

	rootModelPtr->SetData("ComponentType", "DatabaseSettingsInput");

	if (m_databaseSettingsCompPtr.IsValid()){

		imtbase::CTreeItemModel* parametersPtr = rootModelPtr->AddTreeModel("Parameters");

		int index = parametersPtr->InsertNewItem();

		QString dbName = m_databaseSettingsCompPtr->GetDatabaseName();
		parametersPtr->SetData("Id", "DBName", index);
		parametersPtr->SetData("Name", "DB Name", index);
		parametersPtr->SetData("Value", dbName, index);
		parametersPtr->SetData("ComponentType", "TextInput", index);

		index = parametersPtr->InsertNewItem();

		QString hostName = m_databaseSettingsCompPtr->GetHost();
		parametersPtr->SetData("Id", "Host", index);
		parametersPtr->SetData("Name", "Host", index);
		parametersPtr->SetData("Value", hostName, index);
		parametersPtr->SetData("ComponentType", "TextInput", index);

		index = parametersPtr->InsertNewItem();

		QString password = m_databaseSettingsCompPtr->GetPassword();
		parametersPtr->SetData("Id", "Password", index);
		parametersPtr->SetData("Name", "Password", index);
		parametersPtr->SetData("Value", password, index);
		parametersPtr->SetData("ComponentType", "TextInput", index);

		index = parametersPtr->InsertNewItem();

		int port = m_databaseSettingsCompPtr->GetPort();
		parametersPtr->SetData("Id", "Port", index);
		parametersPtr->SetData("Name", "Port", index);
		parametersPtr->SetData("Value", port, index);
		parametersPtr->SetData("ComponentType", "IntegerInput", index);

		index = parametersPtr->InsertNewItem();

		QString userName = m_databaseSettingsCompPtr->GetUserName();
		parametersPtr->SetData("Id", "Username", index);
		parametersPtr->SetData("Name", "User name", index);
		parametersPtr->SetData("Value", userName, index);
		parametersPtr->SetData("ComponentType", "TextInput", index);

		rootModelPtr->SetData("Status", "OK");
	}

	return rootModelPtr;
}


// reimplemented (imtgql::IGqlMutationDataControllerDelegate)

imtbase::CTreeItemModel* CDBSettingsDataProviderCompBase::UpdateBaseModelFromRepresentation(
		const QList<imtgql::CGqlObject> &params,
		imtbase::CTreeItemModel *baseModel)
{
	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

	if (!m_databaseSettingsCompPtr.IsValid()){
		return nullptr;
	}

	if (baseModel->ContainsKey("Parameters")){
		imtbase::CTreeItemModel* parameters = baseModel->GetTreeItemModel("Parameters");

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

		if (baseModel->ContainsKey("Id")){
			paramId = baseModel->GetData("Id").toByteArray();
			rootModel->SetData("Id", paramId);
		}

		if (baseModel->ContainsKey("Name")){
			paramName = baseModel->GetData("Name").toString();
			rootModel->SetData("Name", paramName);
		}
	}

	return rootModel;
}


} // namespace imtqml


