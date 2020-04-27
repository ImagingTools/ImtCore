#include <imtlog/CLoginEventControllerComp.h>


// ACF includes
#include <istd/CChangeNotifier.h>
#include <iser/IObject.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <ilog/CMessage.h>
#include <ilog/CExtMessage.h>

// ImtCore includes
#include <imtbase/IMessageGroupInfoProvider.h>


namespace imtlog
{


// public methods

CLoginEventControllerComp::CLoginEventControllerComp()
{
}


// reimplemented (ilog::CLogCompBase)

void CLoginEventControllerComp::WriteMessageToLog(const MessagePtr& /*messagePtr*/)
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

		ilog::IMessageConsumer::MessagePtr messagePtr(
					new ilog::CMessage(
								istd::IInformationProvider::IC_INFO,
								imtbase::IMessageGroupInfoProvider::MI_USER_LOGIN,
								userName,
								"LoginEventController"));
		BaseClass::AddMessage(messagePtr);
	}

	if (changeSet.Contains(iauth::ILogin::CF_LOGOUT)){
		ilog::IMessageConsumer::MessagePtr messagePtr(
					new ilog::CMessage(
								istd::IInformationProvider::IC_INFO,
								imtbase::IMessageGroupInfoProvider::MI_USER_LOGOUT,
								"",
								"LoginEventController"));
		BaseClass::AddMessage(messagePtr);
	}
}


bool CLoginEventControllerComp::OnModelAttached(imod::IModel* modelPtr, istd::IChangeable::ChangeSet& changeMask)
{
	return BaseClass2::OnModelAttached(modelPtr, changeMask);
}


} // namespace imtlog


