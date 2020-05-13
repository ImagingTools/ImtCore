#include <imtlog/CLoginEventControllerComp.h>


// ImtCore includes
#include <imtbase/IMessageGroupInfoProvider.h>


namespace imtlog
{


// public methods

// reimplemented (imod::CSingleModelObserverBase)

void CLoginEventControllerComp::BeforeUpdate(imod::IModel* /*modelPtr*/)
{
	iauth::ILogin* loginPtr = GetObjectPtr();
	if (loginPtr == nullptr){
		return;
	}

	iauth::CUser *userPtr = loginPtr->GetLoggedUser();
	if (userPtr != nullptr){
		m_lastUserName = userPtr->GetUserName();
	}
}


void CLoginEventControllerComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	iauth::ILogin* loginPtr = GetObjectPtr();
	if (loginPtr == nullptr){
		return;
	}

	if (changeSet.ContainsExplicit(iauth::ILogin::CF_LOGIN)){
		SendInfoMessage(imtbase::IMessageGroupInfoProvider::MI_USER_LOGIN, loginPtr->GetLoggedUser()->GetUserName(), "LoginEventController");
	}

	if (changeSet.ContainsExplicit(iauth::ILogin::CF_LOGOUT)){
		SendInfoMessage(imtbase::IMessageGroupInfoProvider::MI_USER_LOGOUT, m_lastUserName, "LoginEventController");
	}
}


} // namespace imtlog


