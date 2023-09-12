#include <imtgql/CGqlContextControllerComp.h>


// ImtCore includes
#include <imtauth/CUserInfo.h>
#include <imtauth/ISession.h>
#include <imtauth/IUserInfo.h>
#include <imtgql/CGqlContext.h>


namespace imtgql
{


// public methods

// reimplemented (imtgql::IGqlContextController)

imtgql::IGqlContext* CGqlContextControllerComp::GetRequestContext(
			const imtgql::CGqlRequest& /*gqlRequest*/,
			const QByteArray& token,
			QString& errorMessage) const
{
	if (!m_sessionCollectionCompPtr.IsValid() || !m_userCollectionCompPtr.IsValid()){
		return nullptr;
	}

	QByteArray userId;
	imtbase::IObjectCollection::DataPtr sessionDataPtr;
	if (m_sessionCollectionCompPtr->GetObjectData(token, sessionDataPtr)){
		const imtauth::ISession* sessionInfoPtr = dynamic_cast<const imtauth::ISession*>(sessionDataPtr.GetPtr());
		if (sessionInfoPtr != nullptr){
			userId = sessionInfoPtr->GetUserId();
		}
	}

	if (userId.isEmpty()){
		return nullptr;
	}

	const imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr userDataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userId, userDataPtr)){
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		return nullptr;
	}

	const istd::IChangeable* clonedPtr = userInfoPtr->CloneMe();
	if (clonedPtr == nullptr){
		return nullptr;
	}

	const imtauth::IUserInfo* userInfoClonedPtr = dynamic_cast<const imtauth::IUserInfo*>(clonedPtr);
	if (userInfoClonedPtr == nullptr){
		return nullptr;
	}

	imtgql::CGqlContext* gqlContextPtr = new imtgql::CGqlContext();

	gqlContextPtr->SetUserInfo(userInfoClonedPtr);

	return gqlContextPtr;
}


} // namespace imtgql


