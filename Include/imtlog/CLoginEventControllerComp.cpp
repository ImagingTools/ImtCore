#include <imtlog/CLoginEventControllerComp.h>


// ImtCore includes
#include <imtlog/CLoginEvent.h>


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
		CLoginEvent::LoginEventInfo info;
		info.action = CLoginEvent::ACT_LOGIN;
		info.userName = loginPtr->GetLoggedUser()->GetUserName();

		CLoginEvent* event = new CLoginEvent(info, istd::IInformationProvider::IC_INFO);

		SendUserMessage(event);
	}

	if (changeSet.ContainsExplicit(iauth::ILogin::CF_LOGOUT)){
		CLoginEvent::LoginEventInfo info;
		info.action = CLoginEvent::ACT_LOGOUT;
		info.userName = m_lastUserName;

		CLoginEvent* event = new CLoginEvent(info, istd::IInformationProvider::IC_INFO);

		SendUserMessage(event);
	}
}


} // namespace imtlog


