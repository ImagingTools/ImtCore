// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CLdapUserCollectionControllerComp.h>


// Windows includes
#ifdef Q_OS_WIN
#include <windows.h>
#include <LMaccess.h>
#include <Lmapibuf.h>
#include <sddl.h>
#include <ActiveDS.h>
#include <Adshlp.h>
#pragma comment(lib, "netapi32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "activeds.lib")
#pragma comment(lib, "adsiid.lib")
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


// public static methods

QByteArray CLdapUserCollectionControllerComp::GetSidForUser(const QByteArray& username)
{
	QByteArray result;

#ifdef Q_OS_WIN
	// When username is in DOMAIN\Email format (e.g. "DOMAIN\user@email.com"),
	// LookupAccountNameW does not support "DOMAIN\UPN" notation.
	// In that case, use the email/UPN part directly, which already contains domain information.
	QByteArray lookupName = username;
	int backslashPos = username.indexOf('\\');
	if (backslashPos >= 0){
		QByteArray userPart = username.mid(backslashPos + 1);
		if (userPart.contains('@')){
			lookupName = userPart;
		}
	}

	DWORD sidSize = 0;
	DWORD domainNameSize = 0;
	SID_NAME_USE sidNameUse;

	LookupAccountNameW(NULL, qUtf16Printable(lookupName), NULL, &sidSize, NULL, &domainNameSize, &sidNameUse);
	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || sidSize == 0){
		return result;
	}

	QByteArray sidBuffer(sidSize, 0);
	QByteArray domainBuffer(domainNameSize * sizeof(wchar_t), 0);
	PSID sidPtr = reinterpret_cast<PSID>(sidBuffer.data());
	LPWSTR domainNamePtr = reinterpret_cast<LPWSTR>(domainBuffer.data());

	if (LookupAccountNameW(NULL, qUtf16Printable(lookupName), sidPtr, &sidSize, domainNamePtr, &domainNameSize, &sidNameUse)){
		LPWSTR sidStringPtr = NULL;
		if (ConvertSidToStringSidW(sidPtr, &sidStringPtr)){
			result = QString::fromWCharArray(sidStringPtr).toUtf8();
			LocalFree(sidStringPtr);
		}
	}
#else
	Q_UNUSED(username)
#endif

	return result;
}


QString CLdapUserCollectionControllerComp::GetEmailForUser(const QByteArray& username)
{
	QString result;

#ifdef Q_OS_WIN
	// Extract sAMAccountName from DOMAIN\username format
	QByteArray samName = username;
	int backslashPos = username.indexOf('\\');
	if (backslashPos >= 0){
		samName = username.mid(backslashPos + 1);
	}

	// If user part is in UPN format (user@domain), extract just the username
	int atPos = samName.indexOf('@');
	if (atPos >= 0){
		samName = samName.left(atPos);
	}

	if (samName.isEmpty()){
		return result;
	}

	// Use ADSI to query the mail attribute from Active Directory
	HRESULT hrCom = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	bool shouldUninit = SUCCEEDED(hrCom);

	if (shouldUninit || hrCom == RPC_E_CHANGED_MODE){
		IADs* rootDsePtr = nullptr;
		HRESULT hr = ADsGetObject(L"LDAP://rootDSE", IID_IADs, (void**)&rootDsePtr);
		if (SUCCEEDED(hr)){
			BSTR propName = SysAllocString(L"defaultNamingContext");
			VARIANT var;
			VariantInit(&var);
			hr = rootDsePtr->Get(propName, &var);
			SysFreeString(propName);

			if (SUCCEEDED(hr) && var.vt == VT_BSTR){
				QString searchPath = QString("LDAP://%1").arg(QString::fromWCharArray(var.bstrVal));
				VariantClear(&var);

				IDirectorySearch* searchPtr = nullptr;
				hr = ADsGetObject(reinterpret_cast<LPCWSTR>(searchPath.utf16()), IID_IDirectorySearch, (void**)&searchPtr);
				if (SUCCEEDED(hr)){
					// Escape special LDAP filter characters in sAMAccountName
					QString escapedSamName = QString::fromUtf8(samName);
					escapedSamName.replace(QLatin1String("\\"), QLatin1String("\\5c"));
					escapedSamName.replace(QLatin1String("*"), QLatin1String("\\2a"));
					escapedSamName.replace(QLatin1String("("), QLatin1String("\\28"));
					escapedSamName.replace(QLatin1String(")"), QLatin1String("\\29"));
					escapedSamName.replace(QLatin1Char('\0'), QLatin1String("\\00"));
					escapedSamName.replace(QLatin1String("/"), QLatin1String("\\2f"));

					QString filter = QString("(&(objectClass=user)(sAMAccountName=%1))").arg(escapedSamName);
					std::wstring filterW = filter.toStdWString();
					WCHAR mailAttrName[] = L"mail";
					LPWSTR searchAttrs[] = { mailAttrName };
					ADS_SEARCH_HANDLE searchHandle = nullptr;

					hr = searchPtr->ExecuteSearch(filterW.data(), searchAttrs, 1, &searchHandle);
					if (SUCCEEDED(hr)){
						if (searchPtr->GetNextRow(searchHandle) == S_OK){
							ADS_SEARCH_COLUMN column;
							hr = searchPtr->GetColumn(searchHandle, mailAttrName, &column);
							if (SUCCEEDED(hr)){
								if (column.pADsValues != nullptr && column.dwNumValues > 0
									&& column.dwADsType == ADSTYPE_CASE_IGNORE_STRING){
									result = QString::fromWCharArray(column.pADsValues->CaseIgnoreString);
								}
								searchPtr->FreeColumn(&column);
							}
						}
						searchPtr->CloseSearchHandle(searchHandle);
					}
					searchPtr->Release();
				}
			}
			else{
				VariantClear(&var);
			}
			rootDsePtr->Release();
		}
	}

	if (shouldUninit){
		CoUninitialize();
	}
#else
	Q_UNUSED(username)
#endif

	return result;
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

		QByteArray sid = CLdapUserCollectionControllerComp::GetSidForUser(userName);
		userInfoPtr->SetSid(sid);

		QString name = QString::fromWCharArray(userInfo3BufPtr->usri3_full_name);
		if (!name.isEmpty()){
			userInfoPtr->SetName(name);
		}
		else{
			userInfoPtr->SetName(userName);
		}

		QString description = QString::fromWCharArray(userInfo3BufPtr->usri3_comment);
		userInfoPtr->SetDescription(description);

		QString email = CLdapUserCollectionControllerComp::GetEmailForUser(userId);
		if (!email.isEmpty()){
			userInfoPtr->SetMail(email);
		}

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


