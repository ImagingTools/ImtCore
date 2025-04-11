#include <imtservergql/CGqlContextControllerComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/ISelectionParam.h>
#include <iprm/IOptionsList.h>

// ImtCore includes
#include <imtauth/IUserSettings.h>
#include <imtauth/ISession.h>
#include <imtauth/IUserInfo.h>
#include <imtgql/CGqlContext.h>


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
	QMutexLocker lock(&m_mutex);

	if (!m_userCollectionCompPtr.IsValid()){
		Q_ASSERT(false);

		return nullptr;
	}

	if (!m_jwtSessionControllerCompPtr.IsValid()){
		Q_ASSERT(false);
		return nullptr;
	}

	QByteArray userObjectId = m_jwtSessionControllerCompPtr->GetUserFromJwt(token);
	if (userObjectId.isEmpty()){
		errorMessage = QString("Unable to get a GraphQL context for token '%1'. Error: Session model is invalid.").arg(qPrintable(token));
		SendErrorMessage(0, errorMessage, "CGqlContextControllerComp");

		return nullptr;
	}

	const imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr userDataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userObjectId, userDataPtr)){
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to get a GraphQL context for token '%1'. Error: User with ID '%2' was not found.").arg(qPrintable(token)).arg(qPrintable(userObjectId));
		SendErrorMessage(0, errorMessage, "CGqlContextControllerComp");

		return nullptr;
	}
	
	imtgql::CGqlContext* gqlContextPtr = new imtgql::CGqlContext();

	gqlContextPtr->SetUserInfo(userInfoPtr);
	gqlContextPtr->SetToken(token);

	QByteArray languageId;
	QByteArray designSchemeId;

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
								languageId = optionListPtr->GetOptionId(index);
							}
						}
					}

					iprm::TParamsPtr<iprm::ISelectionParam> designParamPtr(paramsSetPtr, "DesignSchema");
					if (designParamPtr.IsValid()){
						const iprm::IOptionsList* optionListPtr = designParamPtr->GetSelectionConstraints();
						if (optionListPtr != nullptr){
							int index = designParamPtr->GetSelectedOptionIndex();
							if (index >= 0){
								designSchemeId = optionListPtr->GetOptionId(index);
							}
						}
					}
				}
			}
		}
	}

	gqlContextPtr->SetLanguageId(languageId);
	gqlContextPtr->SetDesignScheme(designSchemeId);
	gqlContextPtr->SetHeaders(headers);

	return gqlContextPtr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CGqlContextControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QMutexLocker lock(&m_mutex);

	m_userCollectionCompPtr.EnsureInitialized();
	Q_ASSERT(m_userCollectionCompPtr.IsValid());
}


} // namespace imtservergql


