#include <imtauth/CLdapUserCollectionControllerComp.h>


// Windows includes
#ifdef Q_OS_WIN
#include <windows.h>
#include <LMaccess.h>
#include <Lmapibuf.h>
#pragma comment(lib, "netapi32.lib")
#endif

// ACF includes
#include <istd/CChangeGroup.h>
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>

// ImtCore includes
#include <imtauth/CUserInfo.h>


namespace imtauth
{


// public methods

CLdapUserCollectionControllerComp::CLdapUserCollectionControllerComp()
	:m_checkLdapUsersThreadThread(*this)
{
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CLdapUserCollectionControllerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	m_checkLdapUsersThreadThread.Start();
}


void CLdapUserCollectionControllerComp::OnComponentDestroyed()
{
	m_checkLdapUsersThreadThread.requestInterruption();
	m_checkLdapUsersThreadThread.wait();

	BaseClass::OnComponentDestroyed();
}


// public methods of the embedded class CheckLdapUsersThread

CLdapUserCollectionControllerComp::CheckLdapUsersThread::CheckLdapUsersThread(CLdapUserCollectionControllerComp& parent)
	:m_parent(parent)
{
}


void CLdapUserCollectionControllerComp::CheckLdapUsersThread::Start()
{
	QThread::start();
}


QByteArrayList CLdapUserCollectionControllerComp::CheckLdapUsersThread::GetLdapUserIds() const
{
	QByteArrayList result;

#ifdef Q_OS_WIN
	_USER_INFO_3* l_pUserInfo = NULL;
	DWORD l_dwentriesread = 0;
	DWORD l_dwtotalentries = 0;

	NetUserEnum(NULL, 3, 0, (LPBYTE*)&l_pUserInfo, MAX_PREFERRED_LENGTH, &l_dwentriesread, &l_dwtotalentries, NULL);

	for(int i = 0; i < l_dwentriesread; i++){
		if(l_pUserInfo[i].usri3_flags & UF_ACCOUNTDISABLE){
			continue;
		}

		QByteArray userId = QString::fromWCharArray((l_pUserInfo+i)->usri3_name).toUtf8();
		result.append(userId);
	}

	NetApiBufferFree(l_pUserInfo);
#endif

	return result;
}


const imtauth::IUserInfo* CLdapUserCollectionControllerComp::CheckLdapUsersThread::GetUserInfoFromLdapUserId(const QByteArray& userId) const
{
#ifdef Q_OS_WIN
	LPUSER_INFO_3 userInfo3BufPtr = NULL;

	NetUserGetInfo(NULL, qUtf16Printable(userId), 3, (LPBYTE *) & userInfo3BufPtr);

	if (userInfo3BufPtr != nullptr){
		istd::TDelPtr<imtauth::CIdentifiableUserInfo> userInfoPtr;
		userInfoPtr.SetPtr(new imtauth::CIdentifiableUserInfo);

		userInfoPtr->SetObjectUuid(userId);

		imtauth::IUserInfo::SystemInfo systemInfo;
		systemInfo.systemId = "Ldap";
		systemInfo.systemName = "LDAP";

		userInfoPtr->SetSystemInfo(systemInfo);

		QByteArray userId = QString::fromWCharArray(userInfo3BufPtr->usri3_name).toUtf8();
		userInfoPtr->SetId(userId);

		QByteArray password = QString::fromWCharArray(userInfo3BufPtr->usri3_password).toUtf8();
		userInfoPtr->SetPasswordHash(password);

		QString name = QString::fromWCharArray(userInfo3BufPtr->usri3_full_name);
		userInfoPtr->SetName(name);

		QString description = QString::fromWCharArray(userInfo3BufPtr->usri3_comment);
		userInfoPtr->SetDescription(description);

		NetApiBufferFree(userInfo3BufPtr);

		return userInfoPtr.PopPtr();
	}
#endif

	return nullptr;
}


// protected methods of the embedded class CheckLdapUsersThread

// reimplemented (QThread)

void CLdapUserCollectionControllerComp::CheckLdapUsersThread::run()
{
	if (!m_parent.m_userCollectionCompPtr.IsValid()){
		return;
	}

	int interval = m_parent.m_checkIntervalAttrPtr.IsValid() ? *m_parent.m_checkIntervalAttrPtr : 60000;

	while (!isInterruptionRequested()){
		QByteArrayList expectedLdapUserIds = GetLdapUserIds();

		iprm::CParamsSet filterParam;
		iprm::CParamsSet paramsSet;

		iprm::CIdParam systemId;
		systemId.SetId("Ldap");

		paramsSet.SetEditableParameter("SystemId", &systemId);
		filterParam.SetEditableParameter("ObjectFilter", &paramsSet);

		imtbase::IObjectCollection::Ids actualLdapUserIds = m_parent.m_userCollectionCompPtr->GetElementIds(0, -1, &filterParam);

		istd::CChangeGroup changeGroup(m_parent.m_userCollectionCompPtr.GetPtr());

		// Add new LDAP users
		for (const QByteArray& userId : expectedLdapUserIds){
			if (!actualLdapUserIds.contains(userId)){
				istd::TDelPtr<const imtauth::IUserInfo> userInfoPtr(GetUserInfoFromLdapUserId(userId));
				if (userInfoPtr.IsValid()){
					m_parent.m_userCollectionCompPtr->InsertNewObject("", "", "", userInfoPtr.GetPtr(), userId);
				}
			}
		}

		// Remove missing LDAP users
		for (const QByteArray& userId : actualLdapUserIds){
			if (!expectedLdapUserIds.contains(userId)){
				m_parent.m_userCollectionCompPtr->RemoveElement(userId);
			}
		}

		sleep(interval);
	}
}


} // namespace imtauth


