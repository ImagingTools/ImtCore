#include <imtgql/CGqlContextControllerComp.h>


// ImtCore includes
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
		Q_ASSERT_X(false, "Attribute 'm_sessionCollectionCompPtr' was not set", "CGqlContextControllerComp");

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
		errorMessage = QString("Unable to get a GraphQL context for token '%1'. Error: Session model is invalid.").arg(qPrintable(token));
		SendErrorMessage(0, errorMessage, "CGqlContextControllerComp");

		return nullptr;
	}

	const imtauth::IUserInfo* userInfoPtr = nullptr;
	imtbase::IObjectCollection::DataPtr userDataPtr;
	if (m_userCollectionCompPtr->GetObjectData(userId, userDataPtr)){
		userInfoPtr = dynamic_cast<const imtauth::IUserInfo*>(userDataPtr.GetPtr());
	}

	if (userInfoPtr == nullptr){
		errorMessage = QString("Unable to get a GraphQL context for token '%1'. Error: User with ID '%1' was not found.").arg(qPrintable(token)).arg(qPrintable(userId));

		SendErrorMessage(0, errorMessage, "CGqlContextControllerComp");

		return nullptr;
	}

	istd::TDelPtr<imtauth::IUserInfo> clonedUserInfoPtr;
	clonedUserInfoPtr.SetCastedOrRemove(userInfoPtr->CloneMe());
	if (!clonedUserInfoPtr.IsValid()){
		errorMessage = QString("Unable to get a GraphQL context for token '%1'. Error: Error when trying to clone an user.").arg(qPrintable(token)).arg(qPrintable(userId));

		SendErrorMessage(0, errorMessage, "CGqlContextControllerComp");

		return nullptr;
	}

	imtgql::CGqlContext* gqlContextPtr = new imtgql::CGqlContext();

	gqlContextPtr->SetUserInfo(clonedUserInfoPtr.PopPtr());
	gqlContextPtr->SetToken(token);

	return gqlContextPtr;
}


} // namespace imtgql


