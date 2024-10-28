#include <imtservergql/CRemoteGqlContextControllerComp.h>


// ACF includes
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtgql/CGqlContext.h>


namespace imtservergql
{


QMutex s_mutex;


// public methods

// reimplemented (imtgql::IGqlContextController)

imtgql::IGqlContext* CRemoteGqlContextControllerComp::GetRequestContext(
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& token,
			const imtgql::IGqlContext::Headers& headers,
			QString& errorMessage) const
{
	if (m_cacheMap.contains(token)){
		istd::TDelPtr<imtgql::IGqlContext> gqlContextPtr;
		gqlContextPtr.SetCastedOrRemove(m_cacheMap[token]->CloneMe());

		return gqlContextPtr.PopPtr();
	}

	QMutexLocker lock(&s_mutex);

	if (!m_gqlRequestHandlerCompPtr.IsValid()){
		Q_ASSERT_X(false, "Attribute 'm_gqlRequestHandlerCompPtr' was not set", "CRemoteGqlContextControllerComp");
		return nullptr;
	}

	QByteArray commandId = gqlRequest.GetCommandId();

	imtgql::CGqlRequest sessionGqlRequest(imtgql::CGqlRequest::RT_QUERY, "GetSessionInfo");
	imtgql::CGqlObject inputParams;
	inputParams.InsertField(QByteArray("Id"), QVariant(token));
	sessionGqlRequest.AddParam("input", inputParams);

	imtgql::CGqlContext* sessionGqlContextPtr = new imtgql::CGqlContext();
	sessionGqlContextPtr->SetToken(token);
	sessionGqlContextPtr->SetHeaders(headers);

	sessionGqlRequest.SetGqlContext(sessionGqlContextPtr);

	istd::TDelPtr<imtbase::CTreeItemModel> userSessionModelPtr = m_gqlRequestHandlerCompPtr->CreateResponse(sessionGqlRequest, errorMessage);
	if (!userSessionModelPtr.IsValid()){
		SendErrorMessage(0, errorMessage, "CRemoteGqlContextControllerComp");
		return nullptr;
	}

	QByteArray userId;
	if (userSessionModelPtr->ContainsKey("data")){
		imtbase::CTreeItemModel* dataModelPtr = userSessionModelPtr->GetTreeItemModel("data");
		if (dataModelPtr != nullptr){
			QByteArray sessionJson = dataModelPtr->ToJson().toUtf8();

			istd::TDelPtr<imtauth::ISession> sessionInstancePtr = m_sessionInfoFactCompPtr.CreateInstance();
			if (!sessionInstancePtr.IsValid()){
				errorMessage = QString("Unable to get a GraphQL context for token '%1'. Error: Session factory created an invalid object.").arg(qPrintable(token));
				SendErrorMessage(0, errorMessage, "CRemoteGqlContextControllerComp");

				return nullptr;
			}

			iser::CJsonMemReadArchive archive(sessionJson);
			if (!sessionInstancePtr->Serialize(archive)){
				errorMessage = QString("Unable to get a GraphQL context for token '%1'. Error: Session JSON '%2' cannot be serialized.")
							.arg(qPrintable(token))
							.arg(qPrintable(sessionJson));
				SendErrorMessage(0, errorMessage, "CRemoteGqlContextControllerComp");

				return nullptr;
			}

			userId = sessionInstancePtr->GetUserId();
		}
	}

	if (userId.isEmpty()){
		errorMessage = QString("Unable to get a GraphQL context for token '%1'. Error: User-ID cannot be found.");
		SendErrorMessage(0, errorMessage, "CRemoteGqlContextControllerComp");

		return nullptr;
	}

	imtgql::CGqlRequest gqlUserRequest(imtgql::CGqlRequest::RT_QUERY, "GetUserInfo");
	imtgql::CGqlObject userInputParams;
	userInputParams.InsertField(QByteArray("Id"), QVariant(userId));
	gqlUserRequest.AddParam("input", userInputParams);

	imtgql::CGqlContext* userGqlContextPtr = new imtgql::CGqlContext();
	userGqlContextPtr->SetToken(token);
	userGqlContextPtr->SetHeaders(headers);

	gqlUserRequest.SetGqlContext(userGqlContextPtr);

	istd::TDelPtr<imtbase::CTreeItemModel> userModelPtr = m_gqlRequestHandlerCompPtr->CreateResponse(gqlUserRequest, errorMessage);
	if (!userModelPtr.IsValid()){
		SendErrorMessage(0, errorMessage, "CRemoteGqlContextControllerComp");

		return nullptr;
	}

	imtbase::CTreeItemModel* dataModelPtr = userModelPtr->GetTreeItemModel("data");
	if (dataModelPtr == nullptr){
		errorMessage = QString("Unable to get a GraphQL context for token '%1'. Error: The user's response model does not contain 'data'.");
		SendErrorMessage(0, errorMessage, "CRemoteGqlContextControllerComp");

		return nullptr;
	}

	QByteArray userJson = dataModelPtr->ToJson().toUtf8();

	istd::TDelPtr<imtauth::IUserInfo> userInstancePtr = m_userInfoFactCompPtr.CreateInstance();
	if (!userInstancePtr.IsValid()){
		errorMessage = QString("Unable to get a GraphQL context for token '%1'. Error: User factory created an invalid object.").arg(qPrintable(token));
		SendErrorMessage(0, errorMessage, "CRemoteGqlContextControllerComp");

		return nullptr;
	}

	iser::CJsonMemReadArchive archive(userJson);
	if (!userInstancePtr->Serialize(archive)){
		errorMessage = QString("Unable to get a GraphQL context for token '%1'. Error: User JSON '%2' cannot be serialized.").arg(qPrintable(token)).arg(qPrintable(userJson));
		SendErrorMessage(0, errorMessage, "CRemoteGqlContextControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtgql::CGqlContext> gqlContextPtr = new imtgql::CGqlContext();
	gqlContextPtr->SetToken(token);
	gqlContextPtr->SetUserInfo(userInstancePtr.PopPtr());
	gqlContextPtr->SetHeaders(headers);

	m_cacheMap.insert(token, gqlContextPtr.PopPtr());

	istd::TDelPtr<imtgql::IGqlContext> retValPtr;
	retValPtr.SetCastedOrRemove(m_cacheMap[token]->CloneMe());

	return retValPtr.PopPtr();
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CRemoteGqlContextControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QObject::connect(&m_timer, &QTimer::timeout, this, &CRemoteGqlContextControllerComp::OnTimeout);

	int interval = m_cacheClearingIntervalAttrPtr.IsValid() ? *m_cacheClearingIntervalAttrPtr * 1000 : 60000;

	if (m_cacheClearingIntervalParamCompPtr.IsValid()){
		QString intervalText = m_cacheClearingIntervalParamCompPtr->GetText();

		bool ok = true;
		int intervalParam = intervalText.toInt(&ok);
		if (ok){
			interval = intervalParam * 1000;
		}
	}

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


} // namespace imtservergql


