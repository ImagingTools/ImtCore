// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CTableViewParamControllerComp.h>


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonArray>

// ACF includes
#include <istd/TOptDelPtr.h>

// ImtCore includes
#include <imtbase/ITableViewParam.h>
#include <imtauth/CUserInfo.h>


namespace imtservergql
{


// protected methods

// reimplemented (imtservergql::CGqlRequestHandlerCompBase)

QJsonObject CTableViewParamControllerComp::CreateInternalResponse(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_tableViewParamRepresentationControllerCompPtr.IsValid() || !m_userSettingsCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error");
		SendErrorMessage(0, errorMessage, "imtservergql::CTableViewParamControllerComp");

		return QJsonObject();
	}

	QByteArray commandId = gqlRequest.GetCommandId();

	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	if (gqlContextPtr == nullptr){
		errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. Invalid GraphQL context.").arg(qPrintable(commandId));
		SendErrorMessage(0, errorMessage, "imtservergql::CTableViewParamControllerComp");

		return QJsonObject();
	}

	const imtgql::CGqlParamObject* gqlInputParamPtr = gqlRequest.GetParamObject("input");
	if (gqlInputParamPtr == nullptr){
		errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. Invalid GraphQL input params.").arg(qPrintable(commandId));
		SendErrorMessage(0, errorMessage, "imtservergql::CTableViewParamControllerComp");

		return QJsonObject();
	}
	
	const imtauth::CIdentifiableUserInfo* userInfoPtr = dynamic_cast<const imtauth::CIdentifiableUserInfo*>(gqlContextPtr->GetUserInfo());
	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. User info from GraphQL context is invalid").arg(qPrintable(commandId));
		SendErrorMessage(0, errorMessage, "imtservergql::CTableViewParamControllerComp");
		
		return QJsonObject();
	}
	
	QByteArray userId = userInfoPtr->GetObjectUuid();

	QByteArray tableId = gqlInputParamPtr->GetParamArgumentValue("TableId").toByteArray();

	imtauth::IUserSettingsSharedPtr userSettingsPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
		userSettingsPtr.SetCastedPtr(dataPtr);
	}

	if (!userSettingsPtr.IsValid()){
		userSettingsPtr.FromUnique(m_userSettingsFactCompPtr.CreateInstance());

		userSettingsPtr->SetUserId(userId);
	}

	iprm::IParamsSet* settingsPtr = userSettingsPtr->GetSettings();
	if (settingsPtr == nullptr){
		errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. Settings for user with ID: '%2' is invalid.").arg(qPrintable(commandId), qPrintable(userId));
		SendErrorMessage(0, errorMessage, "imtservergql::CTableViewParamControllerComp");

		return QJsonObject();
	}

	imtbase::IObjectCollection* pageViewParamCollectionPtr = nullptr;
	iser::ISerializable* parameterPtr = settingsPtr->GetEditableParameter(*m_pageViewParamsKeyAttrPtr);
	if (parameterPtr != nullptr){
		pageViewParamCollectionPtr = dynamic_cast<imtbase::IObjectCollection*>(parameterPtr);
	}

	Q_ASSERT(pageViewParamCollectionPtr != nullptr);

	iprm::IParamsSetSharedPtr paramSetPtr;
	imtbase::IObjectCollection::DataPtr paramSetDataPtr;
	if (pageViewParamCollectionPtr->GetObjectData(tableId, paramSetDataPtr)){
		paramSetPtr.SetCastedPtr(paramSetDataPtr);
	}

	QJsonObject rootObj;
	QJsonObject dataObj;

	if (gqlRequest.GetRequestType() == imtgql::IGqlRequest::RT_QUERY){
		if (!paramSetPtr.IsValid()){
			return QJsonObject();
		}

		imtbase::ITableViewParam* tableViewParamPtr = dynamic_cast<imtbase::ITableViewParam*>(paramSetPtr->GetEditableParameter("TableViewParam"));
		if (tableViewParamPtr == nullptr){
			return QJsonObject();
		}

		QJsonObject dataModel;
		if (!m_tableViewParamRepresentationControllerCompPtr->GetRepresentationFromDataModel(*tableViewParamPtr, dataModel)){
			return QJsonObject();
		}

		dataObj = dataModel;
	}
	else if (gqlRequest.GetRequestType() == imtgql::IGqlRequest::RT_MUTATION){
		QByteArray tableViewParamsJson = gqlInputParamPtr->GetParamArgumentValue("TableViewParams").toByteArray();

		QJsonDocument doc = QJsonDocument::fromJson(tableViewParamsJson);
		if (doc.isNull() || (!doc.isObject() && !doc.isArray())){
			errorMessage = QString("Unable to process GraphQL request with ID: '%1'. Invalid table view params json.").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage, "imtservergql::CTableViewParamControllerComp");

			return QJsonObject();
		}
		QJsonObject tableViewParamRepresentation;
		if (doc.isObject()){
			tableViewParamRepresentation = doc.object();
		}
		else{
			// TableViewParams QML still sends the representation as a top-level array.
			tableViewParamRepresentation.insert(QStringLiteral("items"), doc.array());
		}

		if (!paramSetPtr.IsValid()){
			paramSetPtr.FromUnique(m_paramSetFactCompPtr.CreateInstance());
		}

		imtbase::ITableViewParam* tableViewParamPtr = dynamic_cast<imtbase::ITableViewParam*>(paramSetPtr->GetEditableParameter("TableViewParam"));
		if (tableViewParamPtr == nullptr){
			errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'.").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage, "imtservergql::CTableViewParamControllerComp");

			return QJsonObject();
		}
		
		tableViewParamPtr->ResetData();

		if (!m_tableViewParamRepresentationControllerCompPtr->GetDataModelFromRepresentation(tableViewParamRepresentation, *tableViewParamPtr)){
			errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. Error then trying to get a data model from representation model for table view param.").arg(qPrintable(commandId));
			SendErrorMessage(0, errorMessage, "imtservergql::CTableViewParamControllerComp");

			return QJsonObject();
		}

		if (pageViewParamCollectionPtr->GetElementIds().contains(tableId)){
			if (!pageViewParamCollectionPtr->SetObjectData(tableId, *paramSetPtr)){
				errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'.").arg(qPrintable(commandId));
				SendErrorMessage(0, errorMessage, "imtservergql::CTableViewParamControllerComp");

				return QJsonObject();
			}
		}
		else{
			pageViewParamCollectionPtr->InsertNewObject("PageViewParam", "", "", paramSetPtr.GetPtr(), tableId);
		}

		if (m_userSettingsCollectionCompPtr->GetElementIds().contains(userId)){
			if (!m_userSettingsCollectionCompPtr->SetObjectData(userId, *userSettingsPtr.GetPtr())){
				errorMessage = QString("Unable to create response for GraphQL request with ID: '%1'. Settings cannot be saved.").arg(qPrintable(commandId));
				SendErrorMessage(0, errorMessage, "imtservergql::CTableViewParamControllerComp");

				return QJsonObject();
			}
		}
		else{
			QByteArray retVal = m_userSettingsCollectionCompPtr->InsertNewObject("UserSettings", "", "", userSettingsPtr.GetPtr(), userId);
			if (retVal.isEmpty()){
				errorMessage = QString("Unable to create settings for user '%1'. Error: Insert object failed").arg(qPrintable(userId));
				SendErrorMessage(0, errorMessage, "CTableViewParamControllerComp");
				return QJsonObject();
			}
		}

		dataObj.insert(QStringLiteral("Status"), QJsonValue::fromVariant(QVariant("Successful")));
	}
	else{
		return QJsonObject();
	}

	rootObj.insert(QStringLiteral("data"), dataObj);
	return rootObj;
}


} // namespace imtservergql
