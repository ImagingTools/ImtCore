// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CGqlContextCreatorComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtauth/IUserSettings.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlContextCreator)

imtgql::IGqlContextUniquePtr CGqlContextCreatorComp::CreateGqlContext(
			const QByteArray& token,
			const QByteArray& productId,
			const QByteArray& userId,
			const imtgql::IGqlContext::Headers& headers,
			QString& errorMessage) const
{
	if (!m_gqlContextFactCompPtr.IsValid()){
		return nullptr;
	}

	imtgql::IGqlContextUniquePtr gqlContextPtr = m_gqlContextFactCompPtr.CreateInstance();
	if (!gqlContextPtr.IsValid()){
		return nullptr;
	}

	gqlContextPtr->SetUserId(userId);
	gqlContextPtr->SetToken(token);
	gqlContextPtr->SetHeaders(headers);
	gqlContextPtr->SetProductId(productId);

	imtgql::CGqlRequestContextManager::SetContext(dynamic_cast<imtgql::IGqlContext*>(gqlContextPtr->CloneMe().PopInterfacePtr()));

	if (m_userCollectionCompPtr.IsValid() && !userId.isEmpty()){
		imtbase::IObjectCollection::DataPtr userDataPtr;
		if (m_userCollectionCompPtr->GetObjectData(userId, userDataPtr)){
			const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());
			gqlContextPtr->SetUserInfo(userInfoPtr);
		}
	}

	if (m_userSettingsCollectionCompPtr.IsValid() && !userId.isEmpty()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userSettingsCollectionCompPtr->GetObjectData(userId, dataPtr)){
			imtauth::IUserSettings* userSettingsPtr = dynamic_cast<imtauth::IUserSettings*>(dataPtr.GetPtr());
			if (userSettingsPtr != nullptr){
				iprm::IParamsSet* paramsSetPtr = userSettingsPtr->GetSettings();
				if (paramsSetPtr != nullptr){
					iprm::TParamsPtr<iprm::ISelectionParam> languageParamPtr(paramsSetPtr, "Language");
					if (languageParamPtr.IsValid()){
						const iprm::IOptionsList* optionListPtr = languageParamPtr->GetSelectionConstraints();
						if (optionListPtr != nullptr){
							int index = languageParamPtr->GetSelectedOptionIndex();
							if (index >= 0){
								QByteArray languageId = optionListPtr->GetOptionId(index);
								gqlContextPtr->SetLanguageId(languageId);
							}
						}
					}

					iprm::TParamsPtr<iprm::ISelectionParam> designParamPtr(paramsSetPtr, "DesignSchema");
					if (designParamPtr.IsValid()){
						const iprm::IOptionsList* optionListPtr = designParamPtr->GetSelectionConstraints();
						if (optionListPtr != nullptr){
							int index = designParamPtr->GetSelectedOptionIndex();
							if (index >= 0){
								QByteArray designSchemeId = optionListPtr->GetOptionId(index);
								gqlContextPtr->SetDesignScheme(designSchemeId);
							}
						}
					}
				}
			}
		}
	}

	return gqlContextPtr;
}


} // namespace imtservergql


