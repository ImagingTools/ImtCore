// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CLdapUserGroupSynchronizerComp.h>


// Windows includes
#ifdef Q_OS_WIN
#include <windows.h>
#include <sddl.h>
#include <ActiveDS.h>
#include <Adshlp.h>
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "activeds.lib")
#pragma comment(lib, "adsiid.lib")
#endif

// Qt includes
#include <QtCore/QMap>
#include <QtCore/QSet>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeGroup.h>

// ImtCore includes
#include <imtauth/CUserGroupInfo.h>


namespace imtauth
{


#ifdef Q_OS_WIN
static QByteArray ConvertSidToString(void* sidDataPtr, DWORD sidSize)
{
	if (sidDataPtr == nullptr || sidSize == 0 || !IsValidSid(sidDataPtr) || GetLengthSid(sidDataPtr) > sidSize){
		return QByteArray();
	}

	QByteArray result;

	LPWSTR sidStringPtr = NULL;
	if (ConvertSidToStringSidW(sidDataPtr, &sidStringPtr)){
		result = QString::fromWCharArray(sidStringPtr).toUtf8();
		LocalFree(sidStringPtr);
	}

	return result;
}
#endif


// public methods

CLdapUserGroupSynchronizerComp::CLdapUserGroupSynchronizerComp()
	:m_synchronizationThread(*this)
{
}


QByteArray CLdapUserGroupSynchronizerComp::GetGroupObjectId(const QByteArray& sid)
{
	static const QUuid s_namespaceUuid(QStringLiteral("8f3c2a5e-6d41-4b7a-9e0f-2c5d8a1b7e93"));

	return QUuid::createUuidV5(s_namespaceUuid, sid).toByteArray(QUuid::WithoutBraces);
}


bool CLdapUserGroupSynchronizerComp::ReadLdapGroups(const QString& nameFilter, LdapGroups& groups)
{
	groups.clear();

#ifdef Q_OS_WIN
	// Only '*' is used as wildcard, all other special LDAP filter characters are escaped.
	QString escapedNameFilter = nameFilter.isEmpty() ? QStringLiteral("*") : nameFilter;
	escapedNameFilter.replace(QLatin1String("\\"), QLatin1String("\\5c"));
	escapedNameFilter.replace(QLatin1String("("), QLatin1String("\\28"));
	escapedNameFilter.replace(QLatin1String(")"), QLatin1String("\\29"));
	escapedNameFilter.replace(QLatin1Char('\0'), QLatin1String("\\00"));

	HRESULT hrCom = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	bool shouldUninit = SUCCEEDED(hrCom);
	if (!shouldUninit && hrCom != RPC_E_CHANGED_MODE){
		return false;
	}

	bool retVal = false;

	// Distinguished names are needed to resolve the nesting (memberOf) to SIDs.
	QMap<QString, QByteArray> sidsByDn;
	QMap<QByteArray, QStringList> parentDnsBySid;
	LdapGroups foundGroups;

	QString searchPath;

	IADs* rootDsePtr = nullptr;
	HRESULT hr = ADsGetObject(L"LDAP://rootDSE", IID_IADs, (void**)&rootDsePtr);
	if (SUCCEEDED(hr)){
		BSTR propName = SysAllocString(L"defaultNamingContext");
		VARIANT var;
		VariantInit(&var);
		hr = rootDsePtr->Get(propName, &var);
		SysFreeString(propName);

		if (SUCCEEDED(hr) && var.vt == VT_BSTR){
			searchPath = QStringLiteral("LDAP://%1").arg(QString::fromWCharArray(var.bstrVal));
		}

		VariantClear(&var);
		rootDsePtr->Release();
	}

	IDirectorySearch* searchPtr = nullptr;
	if (!searchPath.isEmpty()){
		hr = ADsGetObject(reinterpret_cast<LPCWSTR>(searchPath.utf16()), IID_IDirectorySearch, (void**)&searchPtr);
		if (FAILED(hr)){
			searchPtr = nullptr;
		}
	}

	if (searchPtr != nullptr){
		ADS_SEARCHPREF_INFO searchPreferences[2];
		searchPreferences[0].dwSearchPref = ADS_SEARCHPREF_SEARCH_SCOPE;
		searchPreferences[0].vValue.dwType = ADSTYPE_INTEGER;
		searchPreferences[0].vValue.Integer = ADS_SCOPE_SUBTREE;
		searchPreferences[1].dwSearchPref = ADS_SEARCHPREF_PAGESIZE;
		searchPreferences[1].vValue.dwType = ADSTYPE_INTEGER;
		searchPreferences[1].vValue.Integer = 500;
		searchPtr->SetSearchPreference(searchPreferences, 2);

		QString filter = QStringLiteral("(&(objectCategory=group)(name=%1))").arg(escapedNameFilter);
		std::wstring filterW = filter.toStdWString();

		WCHAR nameAttrName[] = L"name";
		WCHAR descriptionAttrName[] = L"description";
		WCHAR sidAttrName[] = L"objectSid";
		WCHAR dnAttrName[] = L"distinguishedName";
		WCHAR memberOfAttrName[] = L"memberOf";
		LPWSTR searchAttrs[] = {nameAttrName, descriptionAttrName, sidAttrName, dnAttrName, memberOfAttrName};

		ADS_SEARCH_HANDLE searchHandle = nullptr;
		hr = searchPtr->ExecuteSearch(filterW.data(), searchAttrs, 5, &searchHandle);
		if (SUCCEEDED(hr)){
			HRESULT rowResult = S_OK;
			while ((rowResult = searchPtr->GetNextRow(searchHandle)) == S_OK){
				LdapGroup group;
				QString dn;
				QStringList parentDns;

				ADS_SEARCH_COLUMN column;
				if (SUCCEEDED(searchPtr->GetColumn(searchHandle, sidAttrName, &column))){
					if (column.dwADsType == ADSTYPE_OCTET_STRING && column.dwNumValues > 0){
						group.sid = ConvertSidToString(column.pADsValues->OctetString.lpValue, column.pADsValues->OctetString.dwLength);
					}
					searchPtr->FreeColumn(&column);
				}

				if (SUCCEEDED(searchPtr->GetColumn(searchHandle, nameAttrName, &column))){
					if (column.dwADsType == ADSTYPE_CASE_IGNORE_STRING && column.dwNumValues > 0){
						group.name = QString::fromWCharArray(column.pADsValues->CaseIgnoreString);
					}
					searchPtr->FreeColumn(&column);
				}

				if (SUCCEEDED(searchPtr->GetColumn(searchHandle, descriptionAttrName, &column))){
					if (column.dwADsType == ADSTYPE_CASE_IGNORE_STRING && column.dwNumValues > 0){
						group.description = QString::fromWCharArray(column.pADsValues->CaseIgnoreString);
					}
					searchPtr->FreeColumn(&column);
				}

				if (SUCCEEDED(searchPtr->GetColumn(searchHandle, dnAttrName, &column))){
					if (column.dwADsType == ADSTYPE_DN_STRING && column.dwNumValues > 0){
						dn = QString::fromWCharArray(column.pADsValues->DNString);
					}
					searchPtr->FreeColumn(&column);
				}

				if (SUCCEEDED(searchPtr->GetColumn(searchHandle, memberOfAttrName, &column))){
					if (column.dwADsType == ADSTYPE_DN_STRING){
						for (DWORD index = 0; index < column.dwNumValues; ++index){
							parentDns << QString::fromWCharArray(column.pADsValues[index].DNString);
						}
					}
					searchPtr->FreeColumn(&column);
				}

				if (group.sid.isEmpty() || group.name.isEmpty()){
					continue;
				}

				if (!dn.isEmpty()){
					sidsByDn.insert(dn.toLower(), group.sid);
				}

				parentDnsBySid.insert(group.sid, parentDns);
				foundGroups << group;
			}

			retVal = (rowResult == S_ADS_NOMORE_ROWS);

			searchPtr->CloseSearchHandle(searchHandle);
		}

		searchPtr->Release();
	}

	if (shouldUninit){
		CoUninitialize();
	}

	if (!retVal){
		return false;
	}

	// Only groups covered by the filter can be parents.
	for (LdapGroup& group : foundGroups){
		for (const QString& parentDn : parentDnsBySid.value(group.sid)){
			const QByteArray parentSid = sidsByDn.value(parentDn.toLower());
			if (!parentSid.isEmpty() && !group.parentSids.contains(parentSid)){
				group.parentSids << parentSid;
			}
		}
	}

	groups = foundGroups;

	return true;
#else
	Q_UNUSED(nameFilter)

	return false;
#endif
}


bool CLdapUserGroupSynchronizerComp::SynchronizeGroups(const LdapGroups& ldapGroups)
{
	if (!m_userGroupCollectionCompPtr.IsValid()){
		return false;
	}

	imtbase::IObjectCollection& groupCollection = *m_userGroupCollectionCompPtr;

	const QByteArray ldapSystemId = m_ldapSystemIdAttrPtr.IsValid() ? *m_ldapSystemIdAttrPtr : QByteArrayLiteral("Ldap");

	IUserGroupInfo::SystemInfo ldapSystemInfo;
	ldapSystemInfo.systemId = ldapSystemId;
	ldapSystemInfo.systemName = m_ldapSystemNameAttrPtr.IsValid() ? *m_ldapSystemNameAttrPtr : QStringLiteral("LDAP");

	QMap<QByteArray, QByteArray> objectIdsBySid;
	QSet<QByteArray> ldapGroupIds;
	for (const LdapGroup& ldapGroup : ldapGroups){
		const QByteArray groupId = GetGroupObjectId(ldapGroup.sid);

		objectIdsBySid.insert(ldapGroup.sid, groupId);
		ldapGroupIds.insert(groupId);
	}

	// Groups are only changed after all stored groups were read, an unread LDAP group would be duplicated or not removed.
	QSet<QByteArray> storedLdapGroupIds;
	const imtbase::ICollectionInfo::Ids storedGroupIds = groupCollection.GetElementIds();
	for (const QByteArray& groupId : storedGroupIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!groupCollection.GetObjectData(groupId, dataPtr)){
			return false;
		}

		const IUserGroupInfo* groupInfoPtr = dynamic_cast<const IUserGroupInfo*>(dataPtr.GetPtr());
		if (groupInfoPtr == nullptr){
			continue;
		}

		for (const IUserGroupInfo::SystemInfo& systemInfo : groupInfoPtr->GetSystemInfos()){
			if (systemInfo.systemId == ldapSystemId){
				storedLdapGroupIds.insert(groupId);

				break;
			}
		}
	}

	bool retVal = true;

	istd::CChangeGroup changeGroup(&groupCollection);

	for (const LdapGroup& ldapGroup : ldapGroups){
		const QByteArray groupId = objectIdsBySid.value(ldapGroup.sid);

		QByteArrayList ldapParentGroupIds;
		for (const QByteArray& parentSid : ldapGroup.parentSids){
			const QByteArray parentGroupId = objectIdsBySid.value(parentSid);
			if (!parentGroupId.isEmpty() && parentGroupId != groupId && !ldapParentGroupIds.contains(parentGroupId)){
				ldapParentGroupIds << parentGroupId;
			}
		}

		if (!storedLdapGroupIds.contains(groupId)){
			CIdentifiableUserGroupInfo groupInfo;
			groupInfo.SetObjectUuid(groupId);
			groupInfo.SetId(groupId);
			groupInfo.SetName(ldapGroup.name);
			groupInfo.SetDescription(ldapGroup.description);
			groupInfo.AddToSystem(ldapSystemInfo);
			for (const QByteArray& parentGroupId : ldapParentGroupIds){
				groupInfo.AddParentGroup(parentGroupId);
			}

			if (groupCollection.InsertNewObject("Group", ldapGroup.name, ldapGroup.description, &groupInfo, groupId).isEmpty()){
				retVal = false;
			}

			continue;
		}

		imtbase::IObjectCollection::DataPtr dataPtr;
		if (!groupCollection.GetObjectData(groupId, dataPtr) || !dataPtr.IsValid()){
			retVal = false;

			continue;
		}

		istd::IChangeableUniquePtr groupCopyPtr = dataPtr->CloneMe();
		IUserGroupInfo* groupInfoPtr = dynamic_cast<IUserGroupInfo*>(groupCopyPtr.GetPtr());
		if (groupInfoPtr == nullptr){
			retVal = false;

			continue;
		}

		bool isChanged = false;

		if (groupInfoPtr->GetName() != ldapGroup.name){
			groupInfoPtr->SetName(ldapGroup.name);
			isChanged = true;
		}

		if (groupInfoPtr->GetDescription() != ldapGroup.description){
			groupInfoPtr->SetDescription(ldapGroup.description);
			isChanged = true;
		}

		// Parent groups from LDAP follow the directory, internal parent groups assigned by an administrator are kept.
		for (const QByteArray& parentGroupId : groupInfoPtr->GetParentGroups()){
			if (storedLdapGroupIds.contains(parentGroupId) && !ldapParentGroupIds.contains(parentGroupId)){
				groupInfoPtr->RemoveParentGroup(parentGroupId);
				isChanged = true;
			}
		}

		for (const QByteArray& parentGroupId : ldapParentGroupIds){
			if (groupInfoPtr->AddParentGroup(parentGroupId)){
				isChanged = true;
			}
		}

		if (isChanged && !groupCollection.SetObjectData(groupId, *groupCopyPtr)){
			retVal = false;
		}
	}

	imtbase::ICollectionInfo::Ids removedGroupIds;
	for (const QByteArray& groupId : std::as_const(storedLdapGroupIds)){
		if (!ldapGroupIds.contains(groupId)){
			removedGroupIds << groupId;
		}
	}

	if (!removedGroupIds.isEmpty() && !groupCollection.RemoveElements(removedGroupIds)){
		retVal = false;
	}

	return retVal;
}


// protected methods

// reimplemented (icomp::CComponentBase)

void CLdapUserGroupSynchronizerComp::OnComponentCreated()
{
	BaseClass::OnComponentCreated();

	if (!m_checkIntervalAttrPtr.IsValid() || *m_checkIntervalAttrPtr > 0){
		m_synchronizationThread.start();
	}
}


void CLdapUserGroupSynchronizerComp::OnComponentDestroyed()
{
	m_synchronizationThread.requestInterruption();
	m_synchronizationThread.wait();

	BaseClass::OnComponentDestroyed();
}


// public methods of the embedded class SynchronizationThread

CLdapUserGroupSynchronizerComp::SynchronizationThread::SynchronizationThread(CLdapUserGroupSynchronizerComp& parent)
	:m_parent(parent)
{
}


// protected methods of the embedded class SynchronizationThread

// reimplemented (QThread)

void CLdapUserGroupSynchronizerComp::SynchronizationThread::run()
{
	const int interval = m_parent.m_checkIntervalAttrPtr.IsValid() ? *m_parent.m_checkIntervalAttrPtr : 60;
	const QString nameFilter = m_parent.m_groupNameFilterAttrPtr.IsValid() ? *m_parent.m_groupNameFilterAttrPtr : QStringLiteral("*");

	while (!isInterruptionRequested()){
		// Groups are only synchronized after a complete read, an unreachable directory must not remove them.
		LdapGroups ldapGroups;
		if (ReadLdapGroups(nameFilter, ldapGroups)){
			m_parent.SynchronizeGroups(ldapGroups);
		}

		int count = interval;
		while (count-- > 0){
			if (isInterruptionRequested()){
				return;
			}

			QThread::sleep(1);
		}
	}
}


} // namespace imtauth


