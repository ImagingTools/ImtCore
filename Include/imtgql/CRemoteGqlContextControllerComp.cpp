#include <imtgql/CRemoteGqlContextControllerComp.h>


// ImtCore includes
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

imtgql::IGqlContext* CRemoteGqlContextControllerComp::GetRequestContext(const imtgql::CGqlRequest& gqlRequest, const QByteArray& token) const
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

	const QList<imtgql::CGqlObject> requestParamsPtr = gqlRequest.GetParams();

	QByteArray productId;
	if (!requestParamsPtr.empty()){
		productId = requestParamsPtr.at(0).GetFieldArgumentValue("ProductId").toByteArray();
	}

	imtgql::CGqlRequest sessionGqlRequest(imtgql::CGqlRequest::RT_QUERY, "SessionItem");
	imtgql::CGqlObject queryFields("item");
	queryFields.InsertField("UserId");
	sessionGqlRequest.AddField(queryFields);

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
			if (dataModelPtr->ContainsKey("UserId")){
				userId = dataModelPtr->GetData("UserId").toByteArray();
			}
		}
	}

	if (!userId.isEmpty()){
		imtgql::CGqlRequest gqlUserRequest(imtgql::CGqlRequest::RT_QUERY, "UserItem");
		imtgql::CGqlObject userInputParams("input");
		userInputParams.InsertField(QByteArray("ProductId"), QVariant(productId));
		userInputParams.InsertField(QByteArray("Id"), QVariant(userId));
		gqlUserRequest.AddParam(userInputParams);

		imtgql::CGqlObject queryUserFields("item");
		queryUserFields.InsertField("Id");
		queryUserFields.InsertField("Username");
		queryUserFields.InsertField("Name");
		queryUserFields.InsertField("Email");
		queryUserFields.InsertField("Permissions");
		queryUserFields.InsertField("Roles");
		queryUserFields.InsertField("Groups");
		gqlUserRequest.AddField(queryUserFields);

		imtgql::CGqlContext* userGqlContextPtr = new imtgql::CGqlContext();
		userGqlContextPtr->SetToken(token);

		gqlUserRequest.SetGqlContext(userGqlContextPtr);

		imtbase::CTreeItemModel* userModelPtr = m_gqlRequestHandlerCompPtr->CreateResponse(gqlUserRequest, errorMessage);
		if (userModelPtr == nullptr){
			return nullptr;
		}

		istd::TDelPtr<imtauth::IUserInfo> userInstancePtr = m_userInfoFactCompPtr.CreateInstance();
		if (!userInstancePtr.IsValid()){
			return nullptr;
		}

		if (!userModelPtr->ContainsKey("data")){
			return nullptr;
		}

		imtbase::CTreeItemModel* dataModelPtr = userModelPtr->GetTreeItemModel("data");
		if (dataModelPtr == nullptr){
			return nullptr;
		}

		if (dataModelPtr->ContainsKey("Username")){
			QByteArray userId = dataModelPtr->GetData("Username").toByteArray();

			userInstancePtr->SetId(userId);
		}

		if (dataModelPtr->ContainsKey("Name")){
			QString name = dataModelPtr->GetData("Name").toString();

			userInstancePtr->SetName(name);
		}

		if (dataModelPtr->ContainsKey("Email")){
			QByteArray email = dataModelPtr->GetData("Email").toByteArray();

			userInstancePtr->SetMail(email);
		}

		if (dataModelPtr->ContainsKey("Permissions")){
			QByteArray permissions = dataModelPtr->GetData("Permissions").toByteArray();
			if (!permissions.isEmpty()){
				QByteArrayList permissionsIDs = permissions.split(';');

				userInstancePtr->SetLocalPermissions(productId, permissionsIDs);
			}
		}

		if (dataModelPtr->ContainsKey("Groups")){
			QByteArray groups = dataModelPtr->GetData("Groups").toByteArray();\
			if (!groups.isEmpty()){
				QByteArrayList groupIDs = groups.split(';');

				for (const QByteArray& groupId : groupIDs){
					userInstancePtr->AddToGroup(groupId);
				}
			}
		}

		istd::TDelPtr<imtgql::CGqlContext> gqlContextPtr = new imtgql::CGqlContext();
		gqlContextPtr->SetToken(token);
		gqlContextPtr->SetUserInfo(userInstancePtr.PopPtr());

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


