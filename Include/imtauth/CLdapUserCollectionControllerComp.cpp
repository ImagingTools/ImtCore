// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
#include <istd/TSingleFactory.h>
#include <iprm/CParamsSet.h>
#include <iprm/CIdParam.h>
#include <iprm/CTextParam.h>

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

	typedef istd::TSingleFactory<istd::IChangeable, imtauth::CIdentifiableUserInfo> FactoryUserImpl;
	RegisterFactory<FactoryUserImpl>("User");

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

	for(DWORD i = 0; i < l_dwentriesread; i++){
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

		userInfoPtr->AddToSystem(systemInfo);

		QByteArray userName = QString::fromWCharArray(userInfo3BufPtr->usri3_name).toUtf8();
		userInfoPtr->SetId(userName);

		QByteArray password = QString::fromWCharArray(userInfo3BufPtr->usri3_password).toUtf8();
		userInfoPtr->SetPasswordHash(password);

		QString name = QString::fromWCharArray(userInfo3BufPtr->usri3_full_name);
		if (!name.isEmpty()){
			userInfoPtr->SetName(name);
		}
		else{
			userInfoPtr->SetName(userName);
		}

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
	int interval = m_parent.m_checkIntervalAttrPtr.IsValid() ? *m_parent.m_checkIntervalAttrPtr : 60;

	while (!isInterruptionRequested()){
		QByteArrayList expectedUserIds = GetLdapUserIds();
		imtbase::ICollectionInfo::Ids actualUserIds = m_parent.GetElementIds();

		{
			istd::CChangeGroup changeGroup(&m_parent);

			for (const QByteArray& userId : actualUserIds){
				if (!expectedUserIds.contains(userId)){
					imtbase::ICollectionInfo::Ids elementIds;
					elementIds << userId;
					m_parent.RemoveElements(elementIds);
				}
			}
		}

		int count = interval;
		while (count-- > 0){
			if (isInterruptionRequested()){
				return;
			}

			sleep(1);
		}
	}
}


} // namespace imtauth


