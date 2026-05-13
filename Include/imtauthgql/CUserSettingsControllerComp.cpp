// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauthgql/CUserSettingsControllerComp.h>


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonDocument>

// ACF includes
#include <istd/TOptDelPtr.h>
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>
#include <imod/TModelWrap.h>


namespace imtauthgql
{


// protected methods

// reimplemented (imtservergql::CGqlRepresentationDataControllerComp)

QJsonObject CUserSettingsControllerComp::CreateRepresentationFromRequest(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_userSettingsRepresentationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_userSettingsRepresentationControllerCompPtr' was not set", "CUserSettingsControllerComp");
		return QJsonObject();
	}

	if (!m_userSettingsCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_userSettingsCollectionCompPtr' was not set", "CUserSettingsControllerComp");
		return QJsonObject();
	}

	if (!m_userSettingsInfoFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_userSettingsInfoFactCompPtr' was not set", "CUserSettingsControllerComp");
		return QJsonObject();
	}

	QByteArray languageId;

	// Get user-ID from GqlContext
	const imtgql::IGqlContext* gqlContextPtr = gqlRequest.GetRequestContext();
	QByteArray userId;
	if (gqlContextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userId = userInfoPtr->GetId();
		}

		languageId = gqlContextPtr->GetLanguageId();
	}

	istd::TDelPtr<iprm::CParamsSet> paramsPtr = new imod::TModelWrap<iprm::CParamsSet>();

	auto languageIdParamPtr = new iprm::CIdParam();
	languageIdParamPtr->SetId(languageId);
	paramsPtr->SetEditableParameter("LanguageParam", languageIdParamPtr, true);

	imtauth::IUserSettingsSharedPtr userSettingsPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (!userId.isEmpty()){
		if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
			userSettingsPtr = dataPtr.dynamicCast<imtauth::IUserSettings>();
		}
	}

	if (!userSettingsPtr.IsValid()){
		userSettingsPtr = m_userSettingsInfoFactCompPtr.CreateInstance();
		Q_ASSERT(userSettingsPtr.IsValid());
		if (!userSettingsPtr.IsValid()){
			errorMessage = QString("Unable to create representation for user settings. Error: User settings is invalid.");
			SendErrorMessage(0, errorMessage, "CUserSettingsControllerComp");

			return QJsonObject();
		}

		userSettingsPtr->SetUserId(userId);
	}

	iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();
	Q_ASSERT(paramSetPtr != nullptr);
	if (paramSetPtr == nullptr){
		errorMessage = QString("Unable to create representation for user settings. Error: Params set from user settings is invalid.");
		SendErrorMessage(0, errorMessage, "CUserSettingsControllerComp");

		return QJsonObject();
	}

	QJsonObject dataModel;
	bool result = m_userSettingsRepresentationControllerCompPtr->GetRepresentationFromDataModel(*paramSetPtr, dataModel, paramsPtr.GetPtr());
	if (!result){
		errorMessage = QString("Unable to create representation for user settings.");
		SendErrorMessage(0, errorMessage, "CUserSettingsControllerComp");

		return QJsonObject();
	}

	QJsonObject rootObj;
	rootObj.insert(QStringLiteral("data"), dataModel);
	return rootObj;
}


bool CUserSettingsControllerComp::UpdateModelFromRepresentation(
			const imtgql::CGqlRequest& request,
			const QJsonObject& representation) const
{
	if (!m_userSettingsRepresentationControllerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_userSettingsRepresentationControllerCompPtr' was not set", "CUserSettingsControllerComp");
		return false;
	}

	if (!m_userSettingsCollectionCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_userSettingsCollectionCompPtr' was not set", "CUserSettingsControllerComp");
		return false;
	}

	if (!m_userSettingsInfoFactCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_userSettingsInfoFactCompPtr' was not set", "CUserSettingsControllerComp");
		return false;
	}

	// Get user-ID from GqlContext
	const imtgql::IGqlContext* gqlContextPtr = request.GetRequestContext();
	QByteArray userId;
	if (gqlContextPtr != nullptr){
		const imtauth::IUserInfo* userInfoPtr = gqlContextPtr->GetUserInfo();
		if (userInfoPtr != nullptr){
			userId = userInfoPtr->GetId();
		}
	}

	if (userId.isEmpty()){
		SendErrorMessage(0, QString("Unable to update model from representation. User-ID is empty!"));

		return false;
	}

	imtauth::IUserSettingsSharedPtr userSettingsPtr;
	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
		userSettingsPtr = dataPtr.dynamicCast<imtauth::IUserSettings>();
	}

	if (!userSettingsPtr.IsValid()){
		userSettingsPtr = m_userSettingsInfoFactCompPtr.CreateInstance();

		userSettingsPtr->SetUserId(userId);
	}

	iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();
	Q_ASSERT(paramSetPtr != nullptr);
	if (paramSetPtr == nullptr){
		SendErrorMessage(0, QString("Unable to get setting from user: '%1'.").arg(qPrintable(userId)));

		return false;
	}

	bool retVal = m_userSettingsRepresentationControllerCompPtr->GetDataModelFromRepresentation(representation, *paramSetPtr);
	if (retVal){
		imtbase::ICollectionInfo::Ids collectionIds = m_userSettingsCollectionCompPtr->GetElementIds();
		if (collectionIds.contains(userId)){
			m_userSettingsCollectionCompPtr->SetObjectData(userId, *userSettingsPtr);
		}
		else{
			m_userSettingsCollectionCompPtr->InsertNewObject("", "", "", userSettingsPtr.GetPtr(), userId);
		}
	}

	return retVal;
}


} // namespace imtauthgql

