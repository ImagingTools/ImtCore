#include <imtqml/CSettingsDatabasePageProviderComp.h>


// Qt includes
#include <QtCore/QTranslator>

// ACF includes
#include <iprm/ISelectionParam.h>

// ImtCore includes
#include <imtauth/CUserSettings.h>


namespace imtqml
{


// public methods

// reimplemented (imtgql::IItemBasedRepresentationProvider)

QByteArray CSettingsDatabasePageProviderComp::GetModelId() const
{
	return *m_paramIdAttrPtr;
}


imtbase::CTreeItemModel* CSettingsDatabasePageProviderComp::GetRepresentation(
		const QList<imtgql::CGqlObject>& params,
		const QByteArrayList& fields,
		const imtgql::IGqlContext* gqlContext)
{
	int designSchemeIndex = 0;
	int languageIndex = 0;

	if (m_userSettingsCollectionCompPtr.IsValid() && gqlContext != nullptr){
		imtauth::IUserInfo* userInfoPtr = gqlContext->GetUserInfo();
		if (userInfoPtr != nullptr){
			QByteArray userId = userInfoPtr->GetUserId();
			imtbase::IObjectCollection::DataPtr dataPtr;
			if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
				iprm::IParamsSet* settingsParamSetPtr = nullptr;

				const imtauth::IUserSettings* userSettingsPtr = dynamic_cast<const imtauth::IUserSettings*>(dataPtr.GetPtr());
				if (userSettingsPtr != nullptr){
					settingsParamSetPtr = userSettingsPtr->GetSettings();
				}

				if (settingsParamSetPtr != nullptr){
					const iprm::ISelectionParam* designSchemeParamPtr = dynamic_cast<const iprm::ISelectionParam*>(settingsParamSetPtr->GetParameter("DesignSchema"));
					if (designSchemeParamPtr != nullptr){
						designSchemeIndex = designSchemeParamPtr->GetSelectedOptionIndex();
					}

					const iprm::ISelectionParam* languageParamPtr = dynamic_cast<const iprm::ISelectionParam*>(settingsParamSetPtr->GetParameter("Language"));
					if (languageParamPtr != nullptr){
						languageIndex = languageParamPtr->GetSelectedOptionIndex();
					}
				}
			}
		}
	}

	imtbase::CTreeItemModel* representationModelPtr = nullptr;
	if (m_settingsDataProviderCompPtr.IsValid()){
		representationModelPtr = m_settingsDataProviderCompPtr->GetRepresentation(params, fields, gqlContext);
		if (representationModelPtr != nullptr){
			imtbase::CTreeItemModel* elementsModelPtr = representationModelPtr->GetTreeItemModel("Elements");
			if (elementsModelPtr != nullptr){
				for (int i = 0; i < elementsModelPtr->GetItemsCount(); i++){
					QByteArray elementId = elementsModelPtr->GetData("Id", i).toByteArray();

					if (elementId == "Mode"){
						elementsModelPtr->SetData("Value", designSchemeIndex, i);
					}
					else if (elementId == "Language"){
						elementsModelPtr->SetData("Value", languageIndex, i);
					}
				}
			}
		}
	}

	return representationModelPtr;
}


// reimplemented (imtgql::IGqlModelEditor)

bool CSettingsDatabasePageProviderComp::UpdateModelFromRepresentation(
		const QList<imtgql::CGqlObject>& params,
		imtbase::CTreeItemModel* baseModelPtr,
		const imtgql::IGqlContext* gqlContext)
{
	if (!m_userSettingsInfoFactCompPtr.IsValid()){
		return false;
	}

	Q_ASSERT(baseModelPtr != nullptr);

	QByteArray parameterId = *m_paramIdAttrPtr;

	imtbase::CTreeItemModel* elementModelPtr = const_cast<imtbase::CTreeItemModel*>(GetElementModel(parameterId, baseModelPtr));
	if (elementModelPtr != nullptr){
		if (m_userSettingsCollectionCompPtr.IsValid() && gqlContext != nullptr){
			imtauth::IUserInfo* userInfoPtr = gqlContext->GetUserInfo();
			if (userInfoPtr != nullptr){
				istd::TDelPtr<imtauth::IUserSettings> userSettingsPtr = m_userSettingsInfoFactCompPtr.CreateInstance();

				QByteArray userId = userInfoPtr->GetUserId();
				userSettingsPtr->SetUserId(userId);

				int designSchemeIndex = 0;
				int languageIndex = 0;

				imtbase::CTreeItemModel* elementsModelPtr = elementModelPtr->GetTreeItemModel("Elements");
				if (elementsModelPtr != nullptr){
					for (int i = 0; i < elementsModelPtr->GetItemsCount(); i++){
						QByteArray elementId = elementsModelPtr->GetData("Id", i).toByteArray();
						int index = elementsModelPtr->GetData("Value", i).toInt();

						if (elementId == "DesignSchema"){
							designSchemeIndex = index;
						}
						else if (elementId == "Language"){
							languageIndex = index;
						}
					}
				}

				iprm::IParamsSet* paramSetPtr = userSettingsPtr->GetSettings();

				Q_ASSERT(paramSetPtr != nullptr);

				iprm::ISelectionParam* designSchemeParamPtr = dynamic_cast<iprm::ISelectionParam*>(paramSetPtr->GetEditableParameter("DesignSchema"));
				if (designSchemeParamPtr != nullptr){
					designSchemeParamPtr->SetSelectedOptionIndex(designSchemeIndex);
				}

				iprm::ISelectionParam* languageParamPtr = dynamic_cast<iprm::ISelectionParam*>(paramSetPtr->GetEditableParameter("Language"));
				if (languageParamPtr != nullptr){
					languageParamPtr->SetSelectedOptionIndex(languageIndex);
				}

				imtbase::ICollectionInfo::Ids collectionIds = m_userSettingsCollectionCompPtr->GetElementIds();
				if (collectionIds.contains(userId)){
					m_userSettingsCollectionCompPtr->SetObjectData(userId, *userSettingsPtr);
				}
				else{
					m_userSettingsCollectionCompPtr->InsertNewObject("", "", "", userSettingsPtr.GetPtr(), userId);
				}

				return true;
			}
		}
	}

	return false;
}


} // namespace imtqml
