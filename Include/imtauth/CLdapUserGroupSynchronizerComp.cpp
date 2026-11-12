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
#include <QtCore/QMutexLocker>
#include <QtCore/QRegularExpression>
#include <QtCore/QSet>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeGroup.h>
#include <ilog/CMessageContainer.h>

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


static HRESULT ReadStringColumn(
			IDirectorySearch& search,
			ADS_SEARCH_HANDLE searchHandle,
			LPWSTR attributeName,
			ADSTYPE valueType,
			QStringList& values)
{
	values.clear();

	ADS_SEARCH_COLUMN column;
	HRESULT retVal = search.GetColumn(searchHandle, attributeName, &column);
	if (FAILED(retVal)){
		return retVal;
	}

	retVal = E_UNEXPECTED;
	if (column.dwADsType == valueType){
		for (DWORD index = 0; index < column.dwNumValues; ++index){
			const ADSVALUE& value = column.pADsValues[index];
			values << QString::fromWCharArray((valueType == ADSTYPE_DN_STRING) ? value.DNString : value.CaseIgnoreString);
		}

		retVal = S_OK;
	}

	search.FreeColumn(&column);

	return retVal;
}
#endif


typedef QMap<QByteArray, QByteArrayList> ParentGroupsMap;


static bool HasAncestorGroup(const ParentGroupsMap& parentGroupsMap, const QByteArray& groupId, const QByteArray& ancestorGroupId)
{
	QSet<QByteArray> visitedGroupIds;
	QByteArrayList pendingGroupIds = parentGroupsMap.value(groupId);
	while (!pendingGroupIds.isEmpty()){
		const QByteArray currentGroupId = pendingGroupIds.takeLast();
		if (currentGroupId == ancestorGroupId){
			return true;
		}

		if (!visitedGroupIds.contains(currentGroupId)){
			visitedGroupIds.insert(currentGroupId);
			pendingGroupIds << parentGroupsMap.value(currentGroupId);
		}
	}

	return false;
}


static bool IsSystemGroup(const IUserGroupInfo& groupInfo, const QByteArray& systemId)
{
	for (const IUserGroupInfo::SystemInfo& systemInfo : groupInfo.GetSystemInfos()){
		if (systemInfo.systemId == systemId){
			return true;
		}
	}

	return false;
}


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


bool CLdapUserGroupSynchronizerComp::IsMatchingName(const QString& nameFilter, const QString& name)
{
	QStringList filterParts = (nameFilter.isEmpty() ? QStringLiteral("*") : nameFilter).split(QLatin1Char('*'));
	for (QString& filterPart : filterParts){
		filterPart = QRegularExpression::escape(filterPart);
	}

	const QRegularExpression expression(
				QRegularExpression::anchoredPattern(filterParts.join(QStringLiteral(".*"))),
				QRegularExpression::CaseInsensitiveOption);

	return expression.match(name).hasMatch();
}


bool CLdapUserGroupSynchronizerComp::ReadLdapGroups(const QString& nameFilter, LdapGroups& groups)
{
	groups.clear();

#ifdef Q_OS_WIN
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

		// Without paging the result is silently truncated, all preferences must be accepted.
		hr = searchPtr->SetSearchPreference(searchPreferences, 2);

		// All groups are read: parents outside of the name filter and mirrors that left the filter must be known.
		WCHAR filter[] = L"(objectCategory=group)";

		WCHAR nameAttrName[] = L"name";
		WCHAR descriptionAttrName[] = L"description";
		WCHAR sidAttrName[] = L"objectSid";
		WCHAR dnAttrName[] = L"distinguishedName";
		WCHAR memberOfAttrName[] = L"memberOf";
		LPWSTR searchAttrs[] = {nameAttrName, descriptionAttrName, sidAttrName, dnAttrName, memberOfAttrName};

		ADS_SEARCH_HANDLE searchHandle = nullptr;
		if (hr == S_OK){
			hr = searchPtr->ExecuteSearch(filter, searchAttrs, 5, &searchHandle);
		}

		if (SUCCEEDED(hr)){
			bool areRowsComplete = true;

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

				QStringList values;
				if (ReadStringColumn(*searchPtr, searchHandle, nameAttrName, ADSTYPE_CASE_IGNORE_STRING, values) == S_OK && !values.isEmpty()){
					group.name = values.first();
				}

				if (ReadStringColumn(*searchPtr, searchHandle, dnAttrName, ADSTYPE_DN_STRING, values) == S_OK && !values.isEmpty()){
					dn = values.first();
				}

				// A group without description or parent groups has no such column, any other error is an incomplete read.
				HRESULT descriptionResult = ReadStringColumn(*searchPtr, searchHandle, descriptionAttrName, ADSTYPE_CASE_IGNORE_STRING, values);
				if (descriptionResult == S_OK && !values.isEmpty()){
					group.description = values.first();
				}

				HRESULT memberOfResult = ReadStringColumn(*searchPtr, searchHandle, memberOfAttrName, ADSTYPE_DN_STRING, parentDns);

				const bool isDescriptionRead = (descriptionResult == S_OK) || (descriptionResult == E_ADS_COLUMN_NOT_SET);
				const bool isMemberOfRead = (memberOfResult == S_OK) || (memberOfResult == E_ADS_COLUMN_NOT_SET);
				if (group.sid.isEmpty() || group.name.isEmpty() || dn.isEmpty() || !isDescriptionRead || !isMemberOfRead){
					areRowsComplete = false;

					break;
				}

				group.isMatching = IsMatchingName(nameFilter, group.name);

				sidsByDn.insert(dn.toLower(), group.sid);
				parentDnsBySid.insert(group.sid, parentDns);
				foundGroups << group;
			}

			retVal = areRowsComplete && (rowResult == S_ADS_NOMORE_ROWS);

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

	// Parent groups outside of the directory search (e.g. of other domains) are ignored.
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
	static const QString s_messageSource = QStringLiteral("LDAP group synchronization");

	if (!m_userGroupCollectionCompPtr.IsValid()){
		SendErrorMessage(0, QStringLiteral("Invalid component configuration: user group collection missing"), s_messageSource);

		return false;
	}

	// An empty system ID is the internal system: the mirrored groups would become internal groups.
	const QByteArray ldapSystemId = m_ldapSystemIdAttrPtr.IsValid() ? *m_ldapSystemIdAttrPtr : QByteArrayLiteral("Ldap");
	if (ldapSystemId.isEmpty()){
		SendErrorMessage(0, QStringLiteral("Invalid component configuration: LDAP system ID is empty"), s_messageSource);

		return false;
	}

	IUserGroupInfo::SystemInfo ldapSystemInfo;
	ldapSystemInfo.systemId = ldapSystemId;
	ldapSystemInfo.systemName = m_ldapSystemNameAttrPtr.IsValid() ? *m_ldapSystemNameAttrPtr : QStringLiteral("LDAP");

	QMap<QByteArray, QString> ldapGroupNamesBySid;
	for (const LdapGroup& ldapGroup : ldapGroups){
		if (ldapGroup.sid.isEmpty() || ldapGroup.name.isEmpty() || ldapGroupNamesBySid.contains(ldapGroup.sid)){
			SendErrorMessage(0, QStringLiteral("Incomplete LDAP group snapshot, the groups are not synchronized"), s_messageSource);

			return false;
		}

		ldapGroupNamesBySid.insert(ldapGroup.sid, ldapGroup.name);
	}

	imtbase::IObjectCollection& groupCollection = *m_userGroupCollectionCompPtr;

	// Group changes of other clients are not overwritten between the read and the write of the groups.
	QMutexLocker locker(&CUserGroupInfo::GetUpdateMutex());

	// Groups are only changed after all stored groups were read, an unread LDAP group would be duplicated or not removed.
	ilog::CMessageContainer readMessages;
	const imtbase::ICollectionInfo::Ids storedGroupIds = groupCollection.GetElementIds(0, -1, nullptr, &readMessages);
	if (readMessages.GetWorstCategory() >= istd::IInformationProvider::IC_ERROR){
		SendErrorMessage(0, QStringLiteral("Unable to read the user groups, the groups are not synchronized"), s_messageSource);

		return false;
	}

	QMap<QByteArray, imtbase::IObjectCollection::DataPtr> storedDataPtrs;
	QSet<QByteArray> storedLdapGroupIds;
	for (const QByteArray& groupId : storedGroupIds){
		imtbase::IObjectCollection::DataPtr dataPtr;
		const IUserGroupInfo* groupInfoPtr = nullptr;
		if (groupCollection.GetObjectData(groupId, dataPtr)){
			groupInfoPtr = dynamic_cast<const IUserGroupInfo*>(dataPtr.GetPtr());
		}

		if (groupInfoPtr == nullptr){
			SendErrorMessage(0, QStringLiteral("Unable to read the user group '%1', the groups are not synchronized").arg(QString::fromUtf8(groupId)), s_messageSource);

			return false;
		}

		storedDataPtrs.insert(groupId, dataPtr);

		if (IsSystemGroup(*groupInfoPtr, ldapSystemId)){
			storedLdapGroupIds.insert(groupId);
		}
	}

	// The groups mirrored by this synchronization: groups of the filter and the existing mirrors of directory groups.
	QMap<QByteArray, QByteArray> objectIdsBySid;
	QSet<QByteArray> mirroredGroupIds;
	QList<const LdapGroup*> mirroredLdapGroups;
	for (const LdapGroup& ldapGroup : ldapGroups){
		const QByteArray groupId = GetGroupObjectId(ldapGroup.sid);

		// A mirror stored without the system flag (e.g. a repository without version information) must not become a duplicate.
		if (storedDataPtrs.contains(groupId) && !storedLdapGroupIds.contains(groupId)){
			SendErrorMessage(
						0,
						QStringLiteral("The user group '%1' mirrors the LDAP group '%2' but is not stored as an LDAP group. Check that the version information is connected to the user group repository").arg(QString::fromUtf8(groupId), ldapGroup.name),
						s_messageSource);

			return false;
		}

		if (ldapGroup.isMatching || storedLdapGroupIds.contains(groupId)){
			objectIdsBySid.insert(ldapGroup.sid, groupId);
			mirroredGroupIds.insert(groupId);
			mirroredLdapGroups << &ldapGroup;
		}
	}

	imtbase::ICollectionInfo::Ids removedGroupIds;
	for (const QByteArray& groupId : storedGroupIds){
		if (storedLdapGroupIds.contains(groupId) && !mirroredGroupIds.contains(groupId)){
			removedGroupIds << groupId;
		}
	}

	// The hierarchy after the synchronization without the parent groups managed by LDAP.
	ParentGroupsMap parentGroupsMap;
	QSet<QString> otherGroupNames;
	for (const QByteArray& groupId : storedGroupIds){
		if (removedGroupIds.contains(groupId)){
			continue;
		}

		const IUserGroupInfo* groupInfoPtr = dynamic_cast<const IUserGroupInfo*>(storedDataPtrs.value(groupId).GetPtr());
		Q_ASSERT(groupInfoPtr != nullptr);

		const bool isMirrored = mirroredGroupIds.contains(groupId);
		if (!isMirrored){
			otherGroupNames.insert(groupInfoPtr->GetName());
		}

		QByteArrayList& parentGroupIds = parentGroupsMap[groupId];
		for (const QByteArray& parentGroupId : groupInfoPtr->GetParentGroups()){
			if (!removedGroupIds.contains(parentGroupId) && (!isMirrored || !storedLdapGroupIds.contains(parentGroupId))){
				parentGroupIds << parentGroupId;
			}
		}
	}

	// LDAP parent groups closing a cycle are omitted, existing links are preferred to keep the result stable.
	QMap<QByteArray, QByteArrayList> ldapParentGroupIdsMap;
	for (int pass = 0; pass < 2; ++pass){
		for (const LdapGroup* ldapGroupPtr : mirroredLdapGroups){
			const QByteArray groupId = objectIdsBySid.value(ldapGroupPtr->sid);

			const IUserGroupInfo* storedGroupInfoPtr = dynamic_cast<const IUserGroupInfo*>(storedDataPtrs.value(groupId).GetPtr());
			const QByteArrayList storedParentGroupIds = (storedGroupInfoPtr != nullptr) ? storedGroupInfoPtr->GetParentGroups() : QByteArrayList();

			QByteArrayList& ldapParentGroupIds = ldapParentGroupIdsMap[groupId];
			for (const QByteArray& parentSid : ldapGroupPtr->parentSids){
				const QByteArray parentGroupId = objectIdsBySid.value(parentSid);
				if (parentGroupId.isEmpty() || parentGroupId == groupId || ldapParentGroupIds.contains(parentGroupId)){
					continue;
				}

				if (storedParentGroupIds.contains(parentGroupId) != (pass == 0)){
					continue;
				}

				if (HasAncestorGroup(parentGroupsMap, parentGroupId, groupId)){
					SendWarningMessage(
								0,
								QStringLiteral("The nesting of the LDAP group '%1' into the group '%2' is ignored, it would create a cycle").arg(ldapGroupPtr->name, ldapGroupNamesBySid.value(parentSid)),
								s_messageSource);

					continue;
				}

				ldapParentGroupIds << parentGroupId;
				parentGroupsMap[groupId] << parentGroupId;
			}
		}
	}

	// Group names are unique: a name used by another group or by several LDAP groups gets the SID.
	QMap<QString, int> ldapNameCounts;
	for (const LdapGroup* ldapGroupPtr : mirroredLdapGroups){
		++ldapNameCounts[ldapGroupPtr->name];
	}

	QMap<QByteArray, QString> groupNames;
	QSet<QString> usedGroupNames = otherGroupNames;
	for (const LdapGroup* ldapGroupPtr : mirroredLdapGroups){
		QString groupName = ldapGroupPtr->name;
		if (otherGroupNames.contains(groupName) || ldapNameCounts.value(groupName) > 1){
			groupName = QStringLiteral("%1 (%2)").arg(ldapGroupPtr->name, QString::fromUtf8(ldapGroupPtr->sid));
		}

		if (usedGroupNames.contains(groupName)){
			SendErrorMessage(0, QStringLiteral("The name of the LDAP group '%1' is already used, the groups are not synchronized").arg(groupName), s_messageSource);

			return false;
		}

		usedGroupNames.insert(groupName);
		groupNames.insert(ldapGroupPtr->sid, groupName);
	}

	// All changes are prepared before the first write.
	struct GroupChange
	{
		QByteArray groupId;
		QString name;
		QString description;
		bool isNew = false;
		imtbase::IObjectCollection::DataPtr groupPtr;
	};

	QList<GroupChange> groupChanges;
	for (const LdapGroup* ldapGroupPtr : mirroredLdapGroups){
		GroupChange groupChange;
		groupChange.groupId = objectIdsBySid.value(ldapGroupPtr->sid);
		groupChange.name = groupNames.value(ldapGroupPtr->sid);
		groupChange.description = ldapGroupPtr->description;

		const QByteArrayList ldapParentGroupIds = ldapParentGroupIdsMap.value(groupChange.groupId);

		if (!storedDataPtrs.contains(groupChange.groupId)){
			CIdentifiableUserGroupInfo* groupInfoPtr = new CIdentifiableUserGroupInfo;
			groupChange.groupPtr = imtbase::IObjectCollection::DataPtr::CreateFromUnique(istd::IChangeableUniquePtr(groupInfoPtr));
			groupChange.isNew = true;

			groupInfoPtr->SetObjectUuid(groupChange.groupId);
			groupInfoPtr->SetId(groupChange.groupId);
			groupInfoPtr->SetName(groupChange.name);
			groupInfoPtr->SetDescription(groupChange.description);
			groupInfoPtr->AddToSystem(ldapSystemInfo);
			for (const QByteArray& parentGroupId : ldapParentGroupIds){
				groupInfoPtr->AddParentGroup(parentGroupId);
			}

			groupChanges << groupChange;

			continue;
		}

		groupChange.groupPtr = imtbase::IObjectCollection::DataPtr::CreateFromUnique(storedDataPtrs.value(groupChange.groupId)->CloneMe());
		IUserGroupInfo* groupInfoPtr = dynamic_cast<IUserGroupInfo*>(groupChange.groupPtr.GetPtr());
		if (groupInfoPtr == nullptr){
			SendErrorMessage(0, QStringLiteral("Unable to copy the user group '%1', the groups are not synchronized").arg(QString::fromUtf8(groupChange.groupId)), s_messageSource);

			return false;
		}

		bool isChanged = false;

		if (groupInfoPtr->GetName() != groupChange.name){
			groupInfoPtr->SetName(groupChange.name);
			isChanged = true;
		}

		if (groupInfoPtr->GetDescription() != groupChange.description){
			groupInfoPtr->SetDescription(groupChange.description);
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

		if (isChanged){
			groupChanges << groupChange;
		}
	}

	if (groupChanges.isEmpty() && removedGroupIds.isEmpty()){
		return true;
	}

	const bool isTransactionUsed = m_transactionManagerCompPtr.IsValid();
	if (isTransactionUsed && !m_transactionManagerCompPtr->StartTransaction()){
		SendErrorMessage(0, QStringLiteral("Unable to start the transaction, the groups are not synchronized"), s_messageSource);

		return false;
	}

	bool retVal = true;

	{
		istd::CChangeGroup changeGroup(&groupCollection);

		// The groups are only removed after all other changes were written.
		for (const GroupChange& groupChange : groupChanges){
			if (groupChange.isNew){
				retVal = !groupCollection.InsertNewObject("Group", groupChange.name, groupChange.description, groupChange.groupPtr.GetPtr(), groupChange.groupId).isEmpty();
			}
			else{
				retVal = groupCollection.SetObjectData(groupChange.groupId, *groupChange.groupPtr);
			}

			if (!retVal){
				SendErrorMessage(0, QStringLiteral("Unable to write the user group '%1'").arg(groupChange.name), s_messageSource);

				break;
			}
		}

		if (retVal && !removedGroupIds.isEmpty() && !groupCollection.RemoveElements(removedGroupIds)){
			SendErrorMessage(0, QStringLiteral("Unable to remove the user groups of the LDAP groups removed from the directory"), s_messageSource);

			retVal = false;
		}
	}

	if (isTransactionUsed){
		if (retVal){
			retVal = m_transactionManagerCompPtr->EndTransaction();
		}
		else{
			m_transactionManagerCompPtr->CancelTransaction();
		}
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


