#include <imtgql/CTableViewParamControllerComp.h>


// ACF includes
#include <istd/TOptDelPtr.h>

// ImtCore includes
#include <imtbase/ITableViewParam.h>


namespace imtgql
{


// protected methods

// reimplemented (imtgql::CGqlRequestHandlerCompBase)

imtbase::CTreeItemModel* CTableViewParamControllerComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_tableViewParamRepresentationControllerCompPtr.IsValid() || !m_userSettingsCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error");
		SendErrorMessage(0, errorMessage, "imtgql::CTableViewParamControllerComp");

		return nullptr;
	}

	QByteArray commandId = gqlRequest.GetCommandId();

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. Invalid GraphQL context.").arg(qPrintable(commandId));
		SendErrorMessage(0, errorMessage, "imtgql::CTableViewParamControllerComp");

		return nullptr;
	}

	imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. User info from GraphQL context is invalid").arg(qPrintable(commandId));
		SendErrorMessage(0, errorMessage, "imtgql::CTableViewParamControllerComp");

		return nullptr;
	}

	QByteArray userId = userInfoPtr->GetId();

	const imtgql::CGqlObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. Invalid GraphQL input params.").arg(qPrintable(commandId));
		SendErrorMessage(0, errorMessage, "imtgql::CTableViewParamControllerComp");

		return nullptr;
	}

	QByteArray tableId = gqlInputParamPtr->GetFieldArgumentValue("TableId").toByteArray();

	istd::TOptDelPtr<imtauth::IUserSettings> userSettingsPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
		userSettingsPtr.SetPtr(dynamic_cast<imtauth::IUserSettings*>(dataPtr.GetPtr()), false);
	}

	if (!userSettingsPtr.IsValid()){
		userSettingsPtr.SetPtr(m_userSettingsFactCompPtr.CreateInstance(), true);
		userSettingsPtr->SetUserId(userId);
	}

	iprm::IParamsSet* settingsPtr = userSettingsPtr->GetSettings();
	if (settingsPtr == nullptr){
		errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. Settings for user with ID: '%2' is invalid.").arg(qPrintable(commandId)).arg(qPrintable(userId));
		SendErrorMessage(0, errorMessage, "imtgql::CTableViewParamControllerComp");

		return nullptr;
	}

	imtbase::IObjectCollection* pageViewParamCollectionPtr = nullptr;
	iser::ISerializable* parameterPtr = settingsPtr->GetEditableParameter(*m_pageViewParamsKeyAttrPtr);
	if (parameterPtr != nullptr){
		pageViewParamCollectionPtr = dynamic_cast<imtbase::IObjectCollection*>(parameterPtr);
	}

	Q_ASSERT(pageViewParamCollectionPtr != nullptr);

	iprm::IParamsSet* paramSetPtr = nullptr;
	imtbase::IObjectCollection::DataPtr paramSetDataPtr;
	if (pageViewParamCollectionPtr->GetObjectData(tableId, paramSetDataPtr)){
		paramSetPtr = dynamic_cast<iprm::IParamsSet*>(paramSetDataPtr.GetPtr());
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

	if (gqlRequest.GetRequestType() == imtgql::IGqlRequest::RT_QUERY){
		if (paramSetPtr == nullptr){
			return nullptr;
		}

		imtbase::ITableViewParam* tableViewParamPtr = dynamic_cast<imtbase::ITableViewParam*>(paramSetPtr->GetEditableParameter("TableViewParam"));
		if (tableViewParamPtr == nullptr){
			return nullptr;
		}

		if (!m_tableViewParamRepresentationControllerCompPtr->GetRepresentationFromDataModel(*tableViewParamPtr, *dataModelPtr)){
			return nullptr;
		}
	}
	else if (gqlRequest.GetRequestType() == imtgql::IGqlRequest::RT_MUTATION){
		QByteArray tableViewParamsJson = gqlInputParamPtr->GetFieldArgumentValue("TableViewParams").toByteArray();

		imtbase::CTreeItemModel tableViewParamRepresentation;
		if (!tableViewParamRepresentation.CreateFromJson(tableViewParamsJson)){
			errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. Invalid table view params json.").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage, "imtgql::CTableViewParamControllerComp");

			return nullptr;
		}

		if (paramSetPtr == nullptr){
			paramSetPtr = m_paramSetFactCompPtr.CreateInstance();
		}

		imtbase::ITableViewParam* tableViewParamPtr =  dynamic_cast<imtbase::ITableViewParam*>(paramSetPtr->GetEditableParameter("TableViewParam"));
		if (tableViewParamPtr == nullptr){
			errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'.").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage, "imtgql::CTableViewParamControllerComp");

			return nullptr;
		}

		if (!m_tableViewParamRepresentationControllerCompPtr->GetDataModelFromRepresentation(tableViewParamRepresentation, *tableViewParamPtr)){
			errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. Error then trying to get a data model from representation model for table view param.").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage, "imtgql::CTableViewParamControllerComp");

			return nullptr;
		}

		if (pageViewParamCollectionPtr->GetElementIds().contains(tableId)){
			if (!pageViewParamCollectionPtr->SetObjectData(tableId, *paramSetPtr)){
				errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'.").arg(qPrintable(commandId));
				SendErrorMessage(0, errorMessage, "imtgql::CTableViewParamControllerComp");

				return nullptr;
			}
		}
		else{
			pageViewParamCollectionPtr->InsertNewObject("PageViewParam", "", "", paramSetPtr, tableId);
		}

		if (m_userSettingsCollectionCompPtr->GetElementIds().contains(userId)){
			if (!m_userSettingsCollectionCompPtr->SetObjectData(userId, *userSettingsPtr.GetPtr())){
				errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. Settings cannot be saved.").arg(qPrintable(commandId));
				SendErrorMessage(0, errorMessage, "imtgql::CTableViewParamControllerComp");

				return nullptr;
			}
		}
		else{
			m_userSettingsCollectionCompPtr->InsertNewObject("", "", "", userSettingsPtr.GetPtr(), userId);
		}

		dataModelPtr->SetData("Status", "Successful");
	}
	else{
		return nullptr;
	}

	return rootModelPtr.PopPtr();
}


} // namespace imtgql


