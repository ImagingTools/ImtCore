#include <imtservergql/CGqlContextControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtbase/imtbase.h>
#include <imtauth/IUserSettings.h>
#include <imtgql/CGqlContext.h>
#include <imtgql/CGqlRequestContextManager.h>


namespace imtservergql
{


// public methods

// reimplemented (imtgql::IGqlContextController)

imtgql::IGqlContext* CGqlContextControllerComp::GetRequestContext(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			const QByteArray& token,
			const imtgql::IGqlContext::Headers& headers,
			QString& errorMessage) const
{
	QByteArray userObjectId;
	
	// Extract user ID based on token prefix: pat_ for PAT tokens, otherwise JWT
	if (token.startsWith("pat_") && token.size() > 4){
		// PAT token - validate with PAT manager to extract userId
		if (m_patManagerCompPtr.IsValid()){
			QByteArrayList scopes;
			if (!m_patManagerCompPtr->ValidateToken(token, userObjectId, scopes)){
				// PAT validation failed - this is unexpected since validation already passed in servlet
				SendWarningMessage(0, QString("PAT validation failed in GetRequestContext for token '%1'").arg(qPrintable(token)), "CGqlContextControllerComp");
			}
		}
	}
	else{
		// JWT token - extract user from JWT controller
		if (m_jwtSessionControllerCompPtr.IsValid()){
			userObjectId = m_jwtSessionControllerCompPtr->GetUserFromJwt(token);
		}
	}
	
	// If neither JWT nor PAT validation succeeded
	if (userObjectId.isEmpty()){
		errorMessage = QString("Unable to get a GraphQL context for token '%1'. Error: Invalid authentication token.").arg(qPrintable(token));
		SendErrorMessage(0, errorMessage, "CGqlContextControllerComp");

		return nullptr;
	}

	imtgql::CGqlContext* gqlContextPtr = new imtgql::CGqlContext();
	gqlContextPtr->SetUserId(userObjectId);
	gqlContextPtr->SetToken(token);
	gqlContextPtr->SetHeaders(headers);

	imtgql::CGqlRequestContextManager::SetContext(gqlContextPtr);

	if (headers.contains(imtbase::s_productIdHeaderId)){
		QByteArray productId = headers.value(imtbase::s_productIdHeaderId);
		gqlContextPtr->SetProductId(productId);
	}

	if (m_userCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr userDataPtr;
		if (m_userCollectionCompPtr->GetObjectData(userObjectId, userDataPtr)){
			const imtauth::IUserInfo* userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());
			gqlContextPtr->SetUserInfo(userInfoPtr);
		}
	}

	if (m_userSettingsCollectionCompPtr.IsValid()){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (m_userSettingsCollectionCompPtr->GetObjectData(userObjectId, dataPtr)){
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


