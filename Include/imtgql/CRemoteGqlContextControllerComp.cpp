#include <imtgql/CRemoteGqlContextControllerComp.h>


// ACF includes
#include <iser/CJsonMemReadArchive.h>
//#include <iprm/TParamsPtr.h>
//#include <iprm/ISelectionParam.h>
//#include <iprm/IOptionsList.h>

// ImtCore includes
//#include <imtauth/IUserSettings.h>
#include <imtauth/CUserInfo.h>
#include <imtgql/CGqlContext.h>


namespace imtgql
{


// public methods

#if QT_VERSION >= 0x060000
CRemoteGqlContextControllerComp::CRemoteGqlContextControllerComp()
{
}
#else
CRemoteGqlContextControllerComp::CRemoteGqlContextControllerComp()
	:m_mutex(QMutex::Recursive)
{
}
#endif


// reimplemented (imtgql::IGqlContextController)

imtgql::IGqlContext* CRemoteGqlContextControllerComp::GetRequestContext(
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& token) const
{
	QMutexLocker lock(&m_mutex);

	if (m_cacheMap.contains(token)){
		istd::TDelPtr<imtgql::IGqlContext> gqlContextPtr;
		gqlContextPtr.SetCastedOrRemove(m_cacheMap[token]->CloneMe());

		return gqlContextPtr.PopPtr();
	}

	if (!m_gqlRequestHandlerCompPtr.IsValid()){
		return nullptr;
	}

	imtgql::CGqlRequest sessionGqlRequest(imtgql::CGqlRequest::RT_QUERY, "GetSessionInfo");
	imtgql::CGqlObject inputParams("input");
	inputParams.InsertField(QByteArray("Id"), QVariant(token));
	sessionGqlRequest.AddParam(inputParams);

	imtgql::CGqlContext* sessionGqlContextPtr = new imtgql::CGqlContext();
	sessionGqlContextPtr->SetToken(token);

	sessionGqlRequest.SetGqlContext(sessionGqlContextPtr);

	QString errorMessage;
	imtbase::CTreeItemModel* userSessionModelPtr = m_gqlRequestHandlerCompPtr->CreateResponse(sessionGqlRequest, errorMessage);
	if (userSessionModelPtr == nullptr){
		return nullptr;
	}

	QByteArray userId;
	if (userSessionModelPtr->ContainsKey("data")){
		imtbase::CTreeItemModel* dataModelPtr = userSessionModelPtr->GetTreeItemModel("data");
		if (dataModelPtr != nullptr){
			QByteArray sessionJson = dataModelPtr->toJSON().toUtf8();

			istd::TDelPtr<imtauth::ISession> sessionInstancePtr = m_sessionInfoFactCompPtr.CreateInstance();
			if (!sessionInstancePtr.IsValid()){
				return nullptr;
			}

			iser::CJsonMemReadArchive archive(sessionJson);
			if (!sessionInstancePtr->Serialize(archive)){
				return nullptr;
			}

			userId = sessionInstancePtr->GetUserId();
		}
	}

	if (!userId.isEmpty()){
		imtgql::CGqlRequest gqlUserRequest(imtgql::CGqlRequest::RT_QUERY, "GetUserInfo");
		imtgql::CGqlObject userInputParams("input");
		userInputParams.InsertField(QByteArray("Id"), QVariant(userId));
		gqlUserRequest.AddParam(userInputParams);

		imtgql::CGqlContext* userGqlContextPtr = new imtgql::CGqlContext();
		userGqlContextPtr->SetToken(token);

		gqlUserRequest.SetGqlContext(userGqlContextPtr);

		imtbase::CTreeItemModel* userModelPtr = m_gqlRequestHandlerCompPtr->CreateResponse(gqlUserRequest, errorMessage);
		if (userModelPtr == nullptr){
			return nullptr;
		}

		if (!userModelPtr->ContainsKey("data")){
			return nullptr;
		}

		imtbase::CTreeItemModel* dataModelPtr = userModelPtr->GetTreeItemModel("data");
		if (dataModelPtr == nullptr){
			return nullptr;
		}

		QByteArray userJson = dataModelPtr->toJSON().toUtf8();

		istd::TDelPtr<imtauth::IUserInfo> userInstancePtr = m_userInfoFactCompPtr.CreateInstance();
		if (!userInstancePtr.IsValid()){
			return nullptr;
		}

		iser::CJsonMemReadArchive archive(userJson);
		if (!userInstancePtr->Serialize(archive)){
			return nullptr;
		}

//		QByteArray languageId;
//		QByteArray designSchemeId;

//		QByteArray username = userInstancePtr->GetId();

//		if (m_userSettingsCollectionCompPtr.IsValid()){
//			imtbase::IObjectCollection::DataPtr dataPtr;
//			if (m_userSettingsCollectionCompPtr->GetObjectData(username, dataPtr)){
//				imtauth::IUserSettings* userSettingsPtr = dynamic_cast<imtauth::IUserSettings*>(dataPtr.GetPtr());
//				if (userSettingsPtr != nullptr){
//					iprm::IParamsSet* paramsSetPtr = userSettingsPtr->GetSettings();
//					if (paramsSetPtr != nullptr){
//						iprm::TParamsPtr<iprm::ISelectionParam> languageParamPtr(paramsSetPtr, "Language");
//						if (languageParamPtr.IsValid()){
//							const iprm::IOptionsList* optionListPtr = languageParamPtr->GetSelectionConstraints();
//							if (optionListPtr != nullptr){
//								int index = languageParamPtr->GetSelectedOptionIndex();
//								if (index >= 0){
//									languageId = optionListPtr->GetOptionId(index);
//								}
//							}
//						}

//						iprm::TParamsPtr<iprm::ISelectionParam> designParamPtr(paramsSetPtr, "DesignSchema");
//						if (designParamPtr.IsValid()){
//							const iprm::IOptionsList* optionListPtr = designParamPtr->GetSelectionConstraints();
//							if (optionListPtr != nullptr){
//								int index = designParamPtr->GetSelectedOptionIndex();
//								if (index >= 0){
//									designSchemeId = optionListPtr->GetOptionId(index);
//								}
//							}
//						}
//					}
//				}
//			}
//		}

		istd::TDelPtr<imtgql::CGqlContext> gqlContextPtr = new imtgql::CGqlContext();
		gqlContextPtr->SetToken(token);
		gqlContextPtr->SetUserInfo(userInstancePtr.PopPtr());
//		gqlContextPtr->SetLanguageId(languageId);
//		gqlContextPtr->SetDesignScheme(designSchemeId);

		m_cacheMap.insert(token, gqlContextPtr.PopPtr());

		istd::TDelPtr<imtgql::IGqlContext> retValPtr;
		retValPtr.SetCastedOrRemove(m_cacheMap[token]->CloneMe());

		return retValPtr.PopPtr();
	}

	return nullptr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CRemoteGqlContextControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QObject::connect(&m_timer, &QTimer::timeout, this, &CRemoteGqlContextControllerComp::OnTimeout);

	int interval = m_cacheClearingIntervalAttrPtr.IsValid() ? *m_cacheClearingIntervalAttrPtr * 1000 : 60000;

	m_timer.start(interval);
}


void CRemoteGqlContextControllerComp::OnComponentDestroyed()
{
	ClearCache();

	BaseClass::OnComponentDestroyed();
}


// private methods

void CRemoteGqlContextControllerComp::ClearCache()
{
	qDeleteAll(m_cacheMap);

	m_cacheMap.clear();
}


// private slots

void CRemoteGqlContextControllerComp::OnTimeout()
{
	ClearCache();
}


} // namespace imtgql


