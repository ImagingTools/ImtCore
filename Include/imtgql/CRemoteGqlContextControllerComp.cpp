#include <imtgql/CRemoteGqlContextControllerComp.h>

#include <QtCore/QThread>

// ACF includes
#include <iser/CJsonMemReadArchive.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/CSessionInfo.h>
#include <imtgql/CGqlContext.h>


namespace imtgql
{


// public methods

CRemoteGqlContextControllerComp::CRemoteGqlContextControllerComp()
{
}


// reimplemented (imtgql::IGqlContextController)

imtgql::IGqlContext* CRemoteGqlContextControllerComp::GetRequestContext(
			const imtgql::CGqlRequest& gqlRequest,
			const QByteArray& token,
			QString& errorMessage) const
{
	qptrdiff threadId = (qptrdiff)QThread::currentThreadId();

	QString connectionName = QString("GetConnectionName - %1").arg(threadId);

	if (m_threadName.isEmpty()){
		m_threadName = connectionName;
	}
	else{
		if (m_threadName != connectionName){
			Q_ASSERT(false);
		}
	}

	if (m_cacheMap.contains(token)){
		istd::TDelPtr<imtgql::IGqlContext> gqlContextPtr;
		gqlContextPtr.SetCastedOrRemove(m_cacheMap[token]->CloneMe());

		return gqlContextPtr.PopPtr();
	}

	if (!m_gqlRequestHandlerCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray commandId = gqlRequest.GetCommandId();
	qDebug() << "Command-ID" << commandId;

	imtgql::CGqlRequest sessionGqlRequest(imtgql::CGqlRequest::RT_QUERY, "GetSessionInfo");
	imtgql::CGqlObject inputParams("input");
	inputParams.InsertField(QByteArray("Id"), QVariant(token));
	sessionGqlRequest.AddParam(inputParams);

	imtgql::CGqlContext* sessionGqlContextPtr = new imtgql::CGqlContext();
	sessionGqlContextPtr->SetToken(token);

	sessionGqlRequest.SetGqlContext(sessionGqlContextPtr);

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

		istd::TDelPtr<imtgql::CGqlContext> gqlContextPtr = new imtgql::CGqlContext();
		gqlContextPtr->SetToken(token);
		gqlContextPtr->SetUserInfo(userInstancePtr.PopPtr());

		m_cacheMap.insert(token, gqlContextPtr.PopPtr());

		istd::TDelPtr<imtgql::IGqlContext> retValPtr;
		retValPtr.SetCastedOrRemove(m_cacheMap[token]->CloneMe());

		return retValPtr.PopPtr();

//		return gqlContextPtr.PopPtr();
	}

	return nullptr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CRemoteGqlContextControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QObject::connect(&m_timer, &QTimer::timeout, this, &CRemoteGqlContextControllerComp::OnTimeout);

	int interval = 60000;
	if (m_cacheClearingIntervalAttrPtr.IsValid()){
		interval = *m_cacheClearingIntervalAttrPtr * 1000;
	}

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


} // namespace imtgql


