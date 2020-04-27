#include <imtlog/CLoginEventControllerComp.h>


// ImtCore includes
#include <imtbase/IMessageGroupInfoProvider.h>


namespace imtlog
{


// public methods

CLoginEventControllerComp::CLoginEventControllerComp()
{
}


// reimplemented (imod::CSingleModelObserverBase)

void CLoginEventControllerComp::OnUpdate(const istd::IChangeable::ChangeSet& changeSet)
{
	iauth::ILogin* loginPtr = GetObjectPtr();
	if (loginPtr == nullptr){
		return;
	}

	if (changeSet.Contains(iauth::ILogin::CF_LOGIN)){
		QString userName = loginPtr->GetLoggedUser()->GetUserName();

		SendInfoMessage(imtbase::IMessageGroupInfoProvider::MI_USER_LOGIN, userName, "LoginEventController");
	}

	if (changeSet.Contains(iauth::ILogin::CF_LOGOUT)){
		SendInfoMessage(imtbase::IMessageGroupInfoProvider::MI_USER_LOGOUT, "", "LoginEventController");
	}
}


bool CLoginEventControllerComp::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	return BaseClass2::OnModelAttached(modelPtr, changeMask);
}


} // namespace imtlog


