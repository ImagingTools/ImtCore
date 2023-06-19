#include <imtgql/CGqlContextCacheControllerComp.h>


// ImtCore includes
#include <imtauth/CUserInfo.h>


namespace imtgql
{


// public methods

CGqlContextCacheControllerComp::~CGqlContextCacheControllerComp()
{
	ClearCache();
}


// reimplemented (imtgql::CGqlRepresentationControllerCompBase)

bool CGqlContextCacheControllerComp::TokenIsCached(const QByteArray& token) const
{
	return m_cacheMap.contains(token);
}


bool CGqlContextCacheControllerComp::CacheToken(const QByteArray& token)
{
	if (TokenIsCached(token)){
		return false;
	}

	imtgql::CGqlRequest gqlRequest(imtgql::CGqlRequest::RT_QUERY, "SessionItem");
	imtgql::CGqlObject queryFields("item");
	queryFields.InsertField("UserId");
	gqlRequest.AddField(queryFields);

	imtgql::CGqlObject inputParams("input");
	inputParams.InsertField(QByteArray("Id"), QVariant(token));
	gqlRequest.AddParam(inputParams);

	QString errorMessage;
	imtbase::CTreeItemModel* sessionModelPtr = m_gqlRequestCompPtr->CreateResponse(gqlRequest, errorMessage);
	if (sessionModelPtr == nullptr){
		return false;
	}

	QByteArray userId;
	if (sessionModelPtr->ContainsKey("data")){
		imtbase::CTreeItemModel* dataModelPtr = sessionModelPtr->GetTreeItemModel("data");
		if (dataModelPtr != nullptr){
			if (dataModelPtr->ContainsKey("UserId")){
				userId = dataModelPtr->GetData("UserId").toByteArray();
			}
		}
	}

	if (!userId.isEmpty()){
		imtgql::CGqlRequest gqlUserRequest(imtgql::CGqlRequest::RT_QUERY, "UserItem");
		imtgql::CGqlObject userInputParams("input");
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

		imtbase::CTreeItemModel* userModelPtr = m_gqlRequestCompPtr->CreateResponse(gqlUserRequest, errorMessage);
		if (userModelPtr == nullptr){
			return false;
		}

		imtauth::IUserInfo* userInstancePtr = m_userInfoFactCompPtr.CreateInstance();
		if (userInstancePtr == nullptr){
			return false;
		}

		if (!userModelPtr->ContainsKey("data")){
			return false;
		}

		imtbase::CTreeItemModel* dataModelPtr = userModelPtr->GetTreeItemModel("data");
		if (dataModelPtr == nullptr){
			return false;
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
			QByteArrayList permissionsIDs = permissions.split(';');

			userInstancePtr->SetLocalPermissions("", permissionsIDs);
		}

		if (dataModelPtr->ContainsKey("Groups")){
			QByteArray groups = dataModelPtr->GetData("Groups").toByteArray();
			QByteArrayList groupIDs = groups.split(';');

			for (const QByteArray& groupId : groupIDs){
				userInstancePtr->AddToGroup(groupId);
			}
		}

		const istd::IChangeable* clonedPtr = userInstancePtr->CloneMe();
		if (clonedPtr == nullptr){
			return false;
		}

		const imtauth::IUserInfo* userInfoClonedPtr = dynamic_cast<const imtauth::IUserInfo*>(clonedPtr);
		if (userInfoClonedPtr == nullptr){
			return false;
		}

		m_cacheMap.insert(token, userInfoClonedPtr);

		return true;
	}

	return false;
}


bool CGqlContextCacheControllerComp::RemoveCache(const QByteArray& token)
{
	if (!m_cacheMap.contains(token)){
		return false;
	}

	const imtauth::IUserInfo* userInfoPtr = m_cacheMap.take(token);
	delete userInfoPtr;

	return true;
}


const imtauth::IUserInfo* CGqlContextCacheControllerComp::GetUserInfo(const QByteArray& token) const
{
	const imtauth::IUserInfo* userInfoPtr = m_cacheMap.value(token);
	return userInfoPtr;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CGqlContextCacheControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	QObject::connect(&m_timer, &QTimer::timeout, this, &CGqlContextCacheControllerComp::OnTimeout);

	int interval = m_cacheClearingIntervalAttrPtr.IsValid() ? *m_cacheClearingIntervalAttrPtr : 60000;

	m_timer.start(interval);
}


// private methods

void CGqlContextCacheControllerComp::ClearCache()
{
	qDeleteAll(m_cacheMap);
	m_cacheMap.clear();
}


// private slots

void CGqlContextCacheControllerComp::OnTimeout()
{
	ClearCache();
}


} // namespace imtgql


