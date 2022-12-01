#include <imtqml/CSettingsDatabasePageProviderComp.h>


// Qt includes
#include <QtCore/QTranslator>

// ImtCore includes
#include <imtauth/CUserSettings.h>


namespace imtqml
{


// public methods

// reimplemented (imtbase::IItemBasedRepresentationProvider)

QByteArray CSettingsDatabasePageProviderComp::GetModelId() const
{
	return *m_paramIdAttrPtr;
}


imtbase::CTreeItemModel* CSettingsDatabasePageProviderComp::GetTreeItemModel(
			const QList<imtgql::CGqlObject>& params,
			const QByteArrayList& fields,
			const imtgql::IGqlContext* gqlContext)
{
	imtbase::CTreeItemModel* dataModelPtr = nullptr;
	if (m_userSettingsCollectionCompPtr.IsValid() && gqlContext != nullptr){
		imtauth::IUserInfo* userInfoPtr = gqlContext->GetUserInfo();
		if (userInfoPtr != nullptr){
			QByteArray userId = userInfoPtr->GetUserId();
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
				const imtauth::IUserSettings* userSettingsPtr = dynamic_cast<const imtauth::IUserSettings*>(dataPtr.GetPtr());
				if (userSettingsPtr != nullptr){
					dataModelPtr = new imtbase::CTreeItemModel();
					QByteArray settingsData = userSettingsPtr->GetSettings();
					dataModelPtr->CreateFromJson(settingsData);
				}
			}
			else if (m_settingsDataProviderCompPtr.IsValid()){
				dataModelPtr = m_settingsDataProviderCompPtr->GetTreeItemModel(params, fields, gqlContext);
			}
		}
	}

	return dataModelPtr;
}


// reimplemented (imtgql::IGqlMutationDataControllerDelegate)

imtbase::CTreeItemModel* CSettingsDatabasePageProviderComp::UpdateBaseModelFromRepresentation(
		const QList<imtgql::CGqlObject>& params,
		imtbase::CTreeItemModel* baseModelPtr,
		const imtgql::IGqlContext* gqlContext)
{
	Q_ASSERT(baseModelPtr != nullptr);

	QByteArray parameterId = *m_paramIdAttrPtr;

	imtbase::CTreeItemModel* elementModelPtr = const_cast<imtbase::CTreeItemModel*>(GetElementModel(parameterId, baseModelPtr));
	if (elementModelPtr != nullptr){
		if (m_userSettingsCollectionCompPtr.IsValid() && gqlContext != nullptr){
			imtauth::IUserInfo* userInfoPtr = gqlContext->GetUserInfo();
			if (userInfoPtr != nullptr){
				QByteArray userId = userInfoPtr->GetUserId();
				QByteArray itemData = elementModelPtr->toJSON().toUtf8();
				if (!itemData.isEmpty()){
					istd::TDelPtr<imtauth::CUserSettings> userSettingsPtr = new imtauth::CUserSettings();
					userSettingsPtr->SetUserId(userId);
					userSettingsPtr->SetSettings(itemData);

					imtbase::ICollectionInfo::Ids collectionIds = m_userSettingsCollectionCompPtr->GetElementIds();
					if (collectionIds.contains(userId)){
						m_userSettingsCollectionCompPtr->SetObjectData(userId, *userSettingsPtr);
					}
					else{
						m_userSettingsCollectionCompPtr->InsertNewObject("", "", "", userSettingsPtr.PopPtr(), userId);
					}
				}
			}
		}
	}

	imtbase::CTreeItemModel* rootModel = new imtbase::CTreeItemModel();

//	if (m_userSettingsCollectionCompPtr.IsValid() && gqlContext != nullptr){
//		imtauth::IUserInfo* userInfoPtr = gqlContext->GetUserInfo();
//		if (userInfoPtr != nullptr){
//			QByteArray userId = userInfoPtr->GetUsername();
//			QByteArray itemData = baseModelPtr->toJSON().toUtf8();
//			if (!itemData.isEmpty()){
//				QJsonDocument jsonResponse = QJsonDocument::fromJson(itemData);
//				if (jsonResponse.isArray()){
//					QJsonArray jsonArray = jsonResponse.array();
//					QJsonValue saveValue;
//					for (const QJsonValue & value : jsonArray){
//						QJsonObject obj = value.toObject();
//						if (obj["Id"] == "General"){
//							saveValue = value;
//							break;
//						}
//					}

//					if (!saveValue.isNull()){
//						istd::TDelPtr<imtauth::CUserSettings> userSettingsPtr = new imtauth::CUserSettings();
//						userSettingsPtr->SetUserId(userId);
//						userSettingsPtr->SetSettings(saveValue.toString().toUtf8());

//						imtbase::ICollectionInfo::Ids collectionIds = m_userSettingsCollectionCompPtr->GetElementIds();
//						if (collectionIds.contains(userId)){
//							m_userSettingsCollectionCompPtr->SetObjectData(userId, *userSettingsPtr);
//						}
//						else{
//							m_userSettingsCollectionCompPtr->InsertNewObject("", "", "", userSettingsPtr.PopPtr(), userId);
//						}
//					}
//				}

//				rootModel->SetData("Status", "Ok");
//			}
//		}
//	}

	return rootModel;
}


} // namespace imtqml
