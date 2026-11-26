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

// STL includes
#include <algorithm>
#include <utility>

// Qt includes
#include <QtCore/QMap>
#include <QtCore/QMutexLocker>
#include <QtCore/QRegularExpression>
#include <QtCore/QSet>
#include <QtCore/QUuid>

// ACF includes
#include <istd/CChangeGroup.h>
#include <istd/TInterfacePtr.h>
#include <ilog/CMessageContainer.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <imtcore/Version.h>
#include <imtbase/IObjectCollectionIterator.h>
#include <imtauth/CUserGroupFilter.h>
#include <imtauth/CUserGroupInfo.h>
#include <imtauth/CUserInfo.h>


namespace imtauth
{


// First ImtCore version storing the system infos of a user group (see CUserGroupInfo::Serialize).
static const quint32 s_minSystemInfoImtCoreVersion = 23950;


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


/**
	Get the NetBIOS name of the domain of the account with the given SID, empty if it cannot be resolved.
*/
static QString LookupDomainName(void* sidDataPtr)
{
	if (sidDataPtr == nullptr || !IsValidSid(sidDataPtr)){
		return QString();
	}

	DWORD nameSize = 0;
	DWORD domainNameSize = 0;
	SID_NAME_USE sidNameUse;
	LookupAccountSidW(NULL, sidDataPtr, NULL, &nameSize, NULL, &domainNameSize, &sidNameUse);
	if (GetLastError() != ERROR_INSUFFICIENT_BUFFER || nameSize == 0 || domainNameSize == 0){
		return QString();
	}

	std::wstring name(nameSize, L'\0');
	std::wstring domainName(domainNameSize, L'\0');
	if (!LookupAccountSidW(NULL, sidDataPtr, &name[0], &nameSize, &domainName[0], &domainNameSize, &sidNameUse)){
		return QString();
	}

	return QString::fromWCharArray(domainName.c_str(), int(domainNameSize));
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


static HRESULT ReadIntegerColumn(
			IDirectorySearch& search,
			ADS_SEARCH_HANDLE searchHandle,
			LPWSTR attributeName,
			DWORD& value)
{
	value = 0;

	ADS_SEARCH_COLUMN column;
	HRESULT retVal = search.GetColumn(searchHandle, attributeName, &column);
	if (FAILED(retVal)){
		return retVal;
	}

	retVal = E_UNEXPECTED;
	if (column.dwADsType == ADSTYPE_INTEGER && column.dwNumValues > 0){
		value = column.pADsValues->Integer;

		retVal = S_OK;
	}

	search.FreeColumn(&column);

	return retVal;
}


/**
	Open the directory search of the given ADSI path, an empty path is the default naming context.
*/
static IDirectorySearch* OpenDirectorySearch(const QString& path)
{
	QString searchPath = path;

	HRESULT hr = S_OK;
	if (searchPath.isEmpty()){
		IADs* rootDsePtr = nullptr;
		hr = ADsGetObject(L"LDAP://rootDSE", IID_IADs, (void**)&rootDsePtr);
		if (SUCCEEDED(hr) && rootDsePtr != nullptr){
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
	}

	if (searchPath.isEmpty()){
		return nullptr;
	}

	IDirectorySearch* searchPtr = nullptr;
	hr = ADsGetObject(reinterpret_cast<LPCWSTR>(searchPath.utf16()), IID_IDirectorySearch, (void**)&searchPtr);
	if (FAILED(hr)){
		return nullptr;
	}

	return searchPtr;
}
#endif


typedef CLdapUserGroupSynchronizerComp::LdapUser LdapUser;


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


static bool IsSystemUser(const IUserInfo& userInfo, const QByteArray& systemId)
{
	for (const IUserInfo::SystemInfo& systemInfo : userInfo.GetSystemInfos()){
		if (systemInfo.systemId == systemId){
			return true;
		}
	}

	return false;
}


// Only the direct members are changed: GetUsers() also returns the users of the parent groups.
static IUserGroupInfo::UserIds GetOwnUserIds(const IUserGroupInfo& groupInfo)
{
	const CUserGroupInfo* groupImplPtr = dynamic_cast<const CUserGroupInfo*>(&groupInfo);
	if (groupImplPtr != nullptr){
		return groupImplPtr->GetOwnUsers();
	}

	return groupInfo.GetUsers();
}


/**
	Find a stored user without SID having the login of the LDAP user: the user principal name, the account name or DOMAIN\account name.
	The DOMAIN\account name is matched only with the domain of the LDAP user, a login with another domain is another account.
*/
static QByteArray FindUserIdByLogin(const QMap<QString, QByteArrayList>& userIdsByLogin, const LdapUser& ldapUser)
{
	QStringList logins;
	logins << QString::fromUtf8(ldapUser.login).toLower();
	if (!ldapUser.accountName.isEmpty()){
		logins << QString::fromUtf8(ldapUser.accountName).toLower();
	}
	if (!ldapUser.domainAccountName.isEmpty()){
		logins << QString::fromUtf8(ldapUser.domainAccountName).toLower();
	}

	for (const QString& login : std::as_const(logins)){
		const QByteArrayList userIds = userIdsByLogin.value(login);
		if (!userIds.isEmpty()){
			return userIds.first();
		}
	}

	return QByteArray();
}


static void SortGroupIndexesByName(const CLdapUserGroupSynchronizerComp::LdapGroups& groups, QList<int>& indexes)
{
	std::sort(indexes.begin(), indexes.end(), [&groups](int first, int second){
		int result = groups[first].name.compare(groups[second].name, Qt::CaseInsensitive);
		if (result != 0){
			return result < 0;
		}

		return groups[first].sid < groups[second].sid;
	});
}


static void AddGroupBranch(
			const QMap<int, QList<int>>& childIndexes,
			int index,
			int level,
			QSet<int>& branchIndexes,
			QSet<int>& addedIndexes,
			CLdapUserGroupSynchronizerComp::LdapGroupTree& tree)
{
	CLdapUserGroupSynchronizerComp::LdapGroupTreeItem item;
	item.groupIndex = index;
	item.level = level;
	tree.append(item);

	addedIndexes.insert(index);

	// Cyclic group memberships are shown only once in a branch.
	branchIndexes.insert(index);

	for (int childIndex : childIndexes.value(index)){
		if (!branchIndexes.contains(childIndex)){
			AddGroupBranch(childIndexes, childIndex, level + 1, branchIndexes, addedIndexes, tree);
		}
	}

	branchIndexes.remove(index);
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


QByteArray CLdapUserGroupSynchronizerComp::GetUserObjectId(const QByteArray& sid)
{
	static const QUuid s_namespaceUuid(QStringLiteral("3b6e9d1c-5f27-4a80-b4c3-7e1a9f2d6c58"));

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


CLdapUserGroupSynchronizerComp::LdapGroupTree CLdapUserGroupSynchronizerComp::CreateGroupTree(const LdapGroups& groups)
{
	QMap<QByteArray, int> shownIndexes;
	for (int index = 0; index < groups.count(); ++index){
		const LdapGroup& group = groups[index];
		if (group.isMatching && !group.sid.isEmpty() && !shownIndexes.contains(group.sid)){
			shownIndexes[group.sid] = index;
		}
	}

	QMap<int, QList<int>> childIndexes;
	QList<int> rootIndexes;
	for (int index : std::as_const(shownIndexes)){
		QSet<int> parentIndexes;
		for (const QByteArray& parentSid : groups[index].parentSids){
			int parentIndex = shownIndexes.value(parentSid, -1);
			if (parentIndex >= 0 && parentIndex != index && !parentIndexes.contains(parentIndex)){
				parentIndexes.insert(parentIndex);
				childIndexes[parentIndex].append(index);
			}
		}

		if (parentIndexes.isEmpty()){
			rootIndexes.append(index);
		}
	}

	SortGroupIndexesByName(groups, rootIndexes);
	for (QList<int>& indexes : childIndexes){
		SortGroupIndexesByName(groups, indexes);
	}

	LdapGroupTree tree;
	QSet<int> branchIndexes;
	QSet<int> addedIndexes;
	for (int index : std::as_const(rootIndexes)){
		AddGroupBranch(childIndexes, index, 0, branchIndexes, addedIndexes, tree);
	}

	// Groups only reachable through a membership cycle have no root group, they are shown on the top level.
	QList<int> remainingIndexes;
	for (int index : std::as_const(shownIndexes)){
		if (!addedIndexes.contains(index)){
			remainingIndexes.append(index);
		}
	}

	SortGroupIndexesByName(groups, remainingIndexes);
	for (int index : std::as_const(remainingIndexes)){
		if (!addedIndexes.contains(index)){
			AddGroupBranch(childIndexes, index, 0, branchIndexes, addedIndexes, tree);
		}
	}

	return tree;
}


QString CLdapUserGroupSynchronizerComp::GetSearchPath(const QString& baseDn)
{
	static const QString s_ldapScheme = QStringLiteral("LDAP://");

	const QString trimmedBaseDn = baseDn.trimmed();
	if (trimmedBaseDn.isEmpty()){
		return QString();
	}

	if (trimmedBaseDn.startsWith(s_ldapScheme, Qt::CaseInsensitive)){
		// The provider name of an ADSI path is case-sensitive.
		return s_ldapScheme + trimmedBaseDn.mid(s_ldapScheme.size());
	}

	// A slash separates the server from the DN in an ADSI path, inside of the DN it must be escaped.
	QString escapedBaseDn;
	for (int index = 0; index < trimmedBaseDn.size(); ++index){
		const QChar character = trimmedBaseDn.at(index);
		if (character == QLatin1Char('\\') && index + 1 < trimmedBaseDn.size()){
			// Already escaped characters are kept.
			escapedBaseDn += character;
			escapedBaseDn += trimmedBaseDn.at(++index);
		}
		else if (character == QLatin1Char('/')){
			escapedBaseDn += QStringLiteral("\\/");
		}
		else if (character == QLatin1Char(',')){
			escapedBaseDn += character;
			while (index + 1 < trimmedBaseDn.size() && trimmedBaseDn.at(index + 1).isSpace()){
				++index;
			}
		}
		else{
			escapedBaseDn += character;
		}
	}

	return s_ldapScheme + escapedBaseDn;
}


QString CLdapUserGroupSynchronizerComp::GetSearchFilter(const QString& nameFilter)
{
	static const QString s_groupFilter = QStringLiteral("(objectCategory=group)");

	// Special characters are escaped as defined by RFC 4515, consecutive wildcards are not allowed in a substring filter.
	QString nameValue;
	for (const QChar character : nameFilter){
		if (character == QLatin1Char('*')){
			if (!nameValue.endsWith(QLatin1Char('*'))){
				nameValue += character;
			}
		}
		else if (character == QLatin1Char('\\')){
			nameValue += QStringLiteral("\\5c");
		}
		else if (character == QLatin1Char('(')){
			nameValue += QStringLiteral("\\28");
		}
		else if (character == QLatin1Char(')')){
			nameValue += QStringLiteral("\\29");
		}
		else if (character.isNull()){
			nameValue += QStringLiteral("\\00");
		}
		else{
			nameValue += character;
		}
	}

	if (nameValue.isEmpty() || nameValue == QStringLiteral("*")){
		return s_groupFilter;
	}

	return QStringLiteral("(&%1(name=%2))").arg(s_groupFilter, nameValue);
}


QString CLdapUserGroupSynchronizerComp::GetDomainSearchPath(const QString& baseDn)
{
	static const QString s_ldapScheme = QStringLiteral("LDAP://");

	const QString searchPath = GetSearchPath(baseDn);
	if (searchPath.isEmpty()){
		return QString();
	}

	// An escaped slash or comma belongs to the value, an unescaped slash separates the server from the DN.
	const QString path = searchPath.mid(s_ldapScheme.size());
	QString serverPart;
	QString dn = path;
	for (int index = 0; index < path.size(); ++index){
		if (path.at(index) == QLatin1Char('\\')){
			++index;
		}
		else if (path.at(index) == QLatin1Char('/')){
			serverPart = path.left(index + 1);
			dn = path.mid(index + 1);

			break;
		}
	}

	QStringList rdns;
	QString rdn;
	for (int index = 0; index < dn.size(); ++index){
		const QChar character = dn.at(index);
		if (character == QLatin1Char('\\') && index + 1 < dn.size()){
			rdn += character;
			rdn += dn.at(++index);
		}
		else if (character == QLatin1Char(',')){
			rdns << rdn.trimmed();
			rdn.clear();
		}
		else{
			rdn += character;
		}
	}

	rdns << rdn.trimmed();

	QStringList domainRdns;
	for (int index = rdns.size() - 1; index >= 0; --index){
		if (!rdns[index].startsWith(QStringLiteral("DC="), Qt::CaseInsensitive)){
			break;
		}

		domainRdns.prepend(rdns[index]);
	}

	if (domainRdns.isEmpty()){
		return searchPath;
	}

	return s_ldapScheme + serverPart + domainRdns.join(QLatin1Char(','));
}


QStringList CLdapUserGroupSynchronizerComp::GetUserSearchFilters(const QStringList& groupDns, int maxGroupCount)
{
	static const QString s_userFilter = QStringLiteral("(objectCategory=person)(objectClass=user)");

	if (maxGroupCount < 1){
		maxGroupCount = 1;
	}

	QSet<QString> addedDns;
	QStringList memberFilters;
	for (const QString& groupDn : groupDns){
		if (groupDn.isEmpty() || addedDns.contains(groupDn.toLower())){
			continue;
		}

		addedDns.insert(groupDn.toLower());

		// The DN is an assertion value: all special characters of the filter syntax are escaped as defined by RFC 4515.
		QString dnValue;
		for (const QChar character : groupDn){
			if (character == QLatin1Char('\\')){
				dnValue += QStringLiteral("\\5c");
			}
			else if (character == QLatin1Char('*')){
				dnValue += QStringLiteral("\\2a");
			}
			else if (character == QLatin1Char('(')){
				dnValue += QStringLiteral("\\28");
			}
			else if (character == QLatin1Char(')')){
				dnValue += QStringLiteral("\\29");
			}
			else if (character.isNull()){
				dnValue += QStringLiteral("\\00");
			}
			else{
				dnValue += character;
			}
		}

		memberFilters << QStringLiteral("(memberOf=%1)").arg(dnValue);
	}

	QStringList filters;
	for (int index = 0; index < memberFilters.count(); index += maxGroupCount){
		const QStringList batch = memberFilters.mid(index, maxGroupCount);
		const QString memberFilter = (batch.count() == 1) ? batch.first() : QStringLiteral("(|%1)").arg(batch.join(QString()));

		filters << QStringLiteral("(&%1%2)").arg(s_userFilter, memberFilter);
	}

	return filters;
}


bool CLdapUserGroupSynchronizerComp::ReadLdapGroups(
			const QString& nameFilter,
			const QString& baseDn,
			LdapGroups& groups,
			const ReadingProgressFunction& progressFunction)
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

	IDirectorySearch* searchPtr = OpenDirectorySearch(GetSearchPath(baseDn));
	if (searchPtr != nullptr){
		ADS_SEARCHPREF_INFO searchPreferences[2];
		searchPreferences[0].dwSearchPref = ADS_SEARCHPREF_SEARCH_SCOPE;
		searchPreferences[0].vValue.dwType = ADSTYPE_INTEGER;
		searchPreferences[0].vValue.Integer = ADS_SCOPE_SUBTREE;
		searchPreferences[1].dwSearchPref = ADS_SEARCHPREF_PAGESIZE;
		searchPreferences[1].vValue.dwType = ADSTYPE_INTEGER;
		searchPreferences[1].vValue.Integer = 500;

		// Without paging the result is silently truncated, all preferences must be accepted.
		HRESULT hr = searchPtr->SetSearchPreference(searchPreferences, 2);

		// All groups are read, the name filter is applied locally: existing mirrors of groups no longer matching the filter
		// (renamed groups or a changed filter) and the nesting to them must remain part of the snapshot.
		std::wstring filter = GetSearchFilter(QString()).toStdWString();

		WCHAR nameAttrName[] = L"name";
		WCHAR descriptionAttrName[] = L"description";
		WCHAR sidAttrName[] = L"objectSid";
		WCHAR dnAttrName[] = L"distinguishedName";
		WCHAR memberOfAttrName[] = L"memberOf";
		LPWSTR searchAttrs[] = {nameAttrName, descriptionAttrName, sidAttrName, dnAttrName, memberOfAttrName};

		ADS_SEARCH_HANDLE searchHandle = nullptr;
		if (hr == S_OK && progressFunction && !progressFunction(0)){
			hr = E_ABORT;
		}

		if (hr == S_OK){
			hr = searchPtr->ExecuteSearch(&filter[0], searchAttrs, 5, &searchHandle);
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

				group.dn = dn;

				sidsByDn.insert(dn.toLower(), group.sid);
				parentDnsBySid.insert(group.sid, parentDns);
				foundGroups << group;

				if (progressFunction && !progressFunction(foundGroups.count())){
					areRowsComplete = false;

					break;
				}
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
	Q_UNUSED(baseDn)
	Q_UNUSED(progressFunction)

	return false;
#endif
}


bool CLdapUserGroupSynchronizerComp::ReadLdapUsers(const QString& baseDn, const LdapGroups& groups, LdapUsers& users)
{
	users.clear();

	QMap<QString, QByteArray> groupSidsByDn;
	QStringList groupDns;
	for (const LdapGroup& group : groups){
		if (!group.dn.isEmpty()){
			groupSidsByDn.insert(group.dn.toLower(), group.sid);
			groupDns << group.dn;
		}
	}

	if (groupSidsByDn.isEmpty()){
		return true;
	}

#ifdef Q_OS_WIN
	HRESULT hrCom = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	bool shouldUninit = SUCCEEDED(hrCom);
	if (!shouldUninit && hrCom != RPC_E_CHANGED_MODE){
		return false;
	}

	bool retVal = false;
	LdapUsers foundUsers;

	// The members of the groups can be located anywhere in the domain, not only below the base DN.
	IDirectorySearch* searchPtr = OpenDirectorySearch(GetDomainSearchPath(baseDn));
	if (searchPtr != nullptr){
		ADS_SEARCHPREF_INFO searchPreferences[2];
		searchPreferences[0].dwSearchPref = ADS_SEARCHPREF_SEARCH_SCOPE;
		searchPreferences[0].vValue.dwType = ADSTYPE_INTEGER;
		searchPreferences[0].vValue.Integer = ADS_SCOPE_SUBTREE;
		searchPreferences[1].dwSearchPref = ADS_SEARCHPREF_PAGESIZE;
		searchPreferences[1].vValue.dwType = ADSTYPE_INTEGER;
		searchPreferences[1].vValue.Integer = 500;

		// Without paging the result is silently truncated, all preferences must be accepted.
		HRESULT hr = searchPtr->SetSearchPreference(searchPreferences, 2);

		// Only the members of the read groups are transferred, not all users of the domain.
		const QStringList filters = GetUserSearchFilters(groupDns);

		WCHAR sidAttrName[] = L"objectSid";
		WCHAR accountNameAttrName[] = L"sAMAccountName";
		WCHAR principalNameAttrName[] = L"userPrincipalName";
		WCHAR displayNameAttrName[] = L"displayName";
		WCHAR descriptionAttrName[] = L"description";
		WCHAR mailAttrName[] = L"mail";
		WCHAR memberOfAttrName[] = L"memberOf";
		WCHAR accountControlAttrName[] = L"userAccountControl";
		LPWSTR searchAttrs[] = {
					sidAttrName,
					accountNameAttrName,
					principalNameAttrName,
					displayNameAttrName,
					descriptionAttrName,
					mailAttrName,
					memberOfAttrName,
					accountControlAttrName};

		// All filters must be read completely, a partial result would remove the members of the other groups.
		const bool arePreferencesSet = (hr == S_OK);
		QSet<QByteArray> foundUserSids;
		QMap<QByteArray, QString> domainNamesBySid;
		int readFilterCount = 0;
		for (const QString& filterText : filters){
			if (!arePreferencesSet){
				break;
			}

			std::wstring filter = filterText.toStdWString();

			ADS_SEARCH_HANDLE searchHandle = nullptr;
			hr = searchPtr->ExecuteSearch(&filter[0], searchAttrs, 8, &searchHandle);
			if (FAILED(hr)){
				break;
			}

			bool areRowsComplete = true;

			HRESULT rowResult = S_OK;
			while ((rowResult = searchPtr->GetNextRow(searchHandle)) == S_OK){
				LdapUser user;
				QString domainName;

				ADS_SEARCH_COLUMN column;
				if (SUCCEEDED(searchPtr->GetColumn(searchHandle, sidAttrName, &column))){
					if (column.dwADsType == ADSTYPE_OCTET_STRING && column.dwNumValues > 0){
						void* sidDataPtr = column.pADsValues->OctetString.lpValue;
						user.sid = ConvertSidToString(sidDataPtr, column.pADsValues->OctetString.dwLength);

						// The domain name is resolved once per domain SID (account SID without the relative ID).
						if (!user.sid.isEmpty()){
							const QByteArray domainSid = user.sid.left(user.sid.lastIndexOf('-'));
							if (!domainNamesBySid.contains(domainSid)){
								domainNamesBySid.insert(domainSid, LookupDomainName(sidDataPtr));
							}

							domainName = domainNamesBySid.value(domainSid);
						}
					}
					searchPtr->FreeColumn(&column);
				}

				QStringList values;
				if (ReadStringColumn(*searchPtr, searchHandle, accountNameAttrName, ADSTYPE_CASE_IGNORE_STRING, values) == S_OK && !values.isEmpty()){
					user.accountName = values.first().toUtf8();

					// Without the domain name no DOMAIN\account login is matched.
					if (!domainName.isEmpty()){
						user.domainAccountName = (domainName + QLatin1Char('\\') + values.first()).toUtf8();
					}
				}

				// Optional attributes have no column if they are not set, any other error is an incomplete read.
				QString principalName;
				HRESULT principalNameResult = ReadStringColumn(*searchPtr, searchHandle, principalNameAttrName, ADSTYPE_CASE_IGNORE_STRING, values);
				if (principalNameResult == S_OK && !values.isEmpty()){
					principalName = values.first();
				}

				HRESULT displayNameResult = ReadStringColumn(*searchPtr, searchHandle, displayNameAttrName, ADSTYPE_CASE_IGNORE_STRING, values);
				if (displayNameResult == S_OK && !values.isEmpty()){
					user.name = values.first();
				}

				HRESULT descriptionResult = ReadStringColumn(*searchPtr, searchHandle, descriptionAttrName, ADSTYPE_CASE_IGNORE_STRING, values);
				if (descriptionResult == S_OK && !values.isEmpty()){
					user.description = values.first();
				}

				HRESULT mailResult = ReadStringColumn(*searchPtr, searchHandle, mailAttrName, ADSTYPE_CASE_IGNORE_STRING, values);
				if (mailResult == S_OK && !values.isEmpty()){
					user.mail = values.first();
				}

				QStringList memberOfDns;
				HRESULT memberOfResult = ReadStringColumn(*searchPtr, searchHandle, memberOfAttrName, ADSTYPE_DN_STRING, memberOfDns);

				DWORD accountControl = 0;
				HRESULT accountControlResult = ReadIntegerColumn(*searchPtr, searchHandle, accountControlAttrName, accountControl);

				const auto isRead = [](HRESULT result){
					return (result == S_OK) || (result == E_ADS_COLUMN_NOT_SET);
				};

				if (		user.sid.isEmpty() ||
							user.accountName.isEmpty() ||
							!isRead(principalNameResult) ||
							!isRead(displayNameResult) ||
							!isRead(descriptionResult) ||
							!isRead(mailResult) ||
							!isRead(memberOfResult) ||
							!isRead(accountControlResult)){
					areRowsComplete = false;

					break;
				}

				// Disabled accounts cannot log in, they are not members of the mirrored groups.
				if ((accountControl & ADS_UF_ACCOUNTDISABLE) != 0){
					continue;
				}

				// Memberships in groups outside of the read groups (e.g. of other domains) are ignored.
				for (const QString& groupDn : std::as_const(memberOfDns)){
					const QByteArray groupSid = groupSidsByDn.value(groupDn.toLower());
					if (!groupSid.isEmpty() && !user.groupSids.contains(groupSid)){
						user.groupSids << groupSid;
					}
				}

				// A member of several groups can be found by several filters, the memberships are complete in each result.
				if (user.groupSids.isEmpty() || foundUserSids.contains(user.sid)){
					continue;
				}

				foundUserSids.insert(user.sid);

				user.login = principalName.isEmpty() ? user.accountName : principalName.toUtf8();

				foundUsers << user;
			}

			const bool isFilterRead = areRowsComplete && (rowResult == S_ADS_NOMORE_ROWS);

			searchPtr->CloseSearchHandle(searchHandle);

			if (!isFilterRead){
				break;
			}

			++readFilterCount;
		}

		retVal = !filters.isEmpty() && (readFilterCount == filters.count());

		searchPtr->Release();
	}

	if (shouldUninit){
		CoUninitialize();
	}

	if (!retVal){
		return false;
	}

	users = foundUsers;

	return true;
#else
	Q_UNUSED(baseDn)

	return false;
#endif
}


bool CLdapUserGroupSynchronizerComp::GetMirroredLdapGroups(const LdapGroups& ldapGroups, LdapGroups& mirroredGroups) const
{
	mirroredGroups.clear();

	if (!m_userGroupCollectionCompPtr.IsValid()){
		return false;
	}

	ilog::CMessageContainer readMessages;
	const imtbase::ICollectionInfo::Ids storedGroupIds = m_userGroupCollectionCompPtr->GetElementIds(0, -1, nullptr, &readMessages);
	if (readMessages.GetWorstCategory() >= istd::IInformationProvider::IC_ERROR){
		return false;
	}

	QSet<QByteArray> storedGroupIdSet;
	for (const QByteArray& groupId : storedGroupIds){
		storedGroupIdSet.insert(groupId);
	}

	for (const LdapGroup& ldapGroup : ldapGroups){
		if (ldapGroup.isMatching || storedGroupIdSet.contains(GetGroupObjectId(ldapGroup.sid))){
			mirroredGroups << ldapGroup;
		}
	}

	return true;
}


bool CLdapUserGroupSynchronizerComp::SynchronizeGroups(const LdapGroups& ldapGroups, const LdapUsers* ldapUsersPtr)
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

	if (ldapUsersPtr != nullptr){
		if (!m_userCollectionCompPtr.IsValid()){
			SendErrorMessage(0, QStringLiteral("Invalid component configuration: user collection missing"), s_messageSource);

			return false;
		}

		QSet<QByteArray> ldapUserSids;
		for (const LdapUser& ldapUser : *ldapUsersPtr){
			if (ldapUser.sid.isEmpty() || ldapUser.login.isEmpty() || ldapUserSids.contains(ldapUser.sid)){
				SendErrorMessage(0, QStringLiteral("Incomplete LDAP user snapshot, the groups are not synchronized"), s_messageSource);

				return false;
			}

			ldapUserSids.insert(ldapUser.sid);
		}
	}

	imtbase::IObjectCollection& groupCollection = *m_userGroupCollectionCompPtr;
	imtbase::IObjectCollection* userCollectionPtr = (ldapUsersPtr != nullptr) ? m_userCollectionCompPtr.GetPtr() : nullptr;

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

	// The members of the mirrored groups: existing users are only added to the groups, unknown users are created.
	struct UserChange
	{
		QByteArray userId;
		QString name;
		QString description;
		bool isNew = false;
		imtbase::IObjectCollection::DataPtr userPtr;

		// Changes of an existing user, applied to the current state of the user when it is written.
		QByteArray sid;
		QByteArrayList removedGroupIds;
		QByteArrayList addedGroupIds;
	};

	QList<UserChange> userChanges;
	QSet<QByteArray> managedUserIds;
	QMap<QByteArray, QByteArrayList> memberUserIdsMap;
	if (userCollectionPtr != nullptr){
		ilog::CMessageContainer userReadMessages;
		const imtbase::ICollectionInfo::Ids storedUserIds = userCollectionPtr->GetElementIds(0, -1, nullptr, &userReadMessages);
		if (userReadMessages.GetWorstCategory() >= istd::IInformationProvider::IC_ERROR){
			SendErrorMessage(0, QStringLiteral("Unable to read the users, the groups are not synchronized"), s_messageSource);

			return false;
		}

		QMap<QByteArray, imtbase::IObjectCollection::DataPtr> storedUserDataPtrs;
		QMap<QByteArray, QByteArray> storedUserIdsBySid;
		QMap<QString, QByteArrayList> storedUserIdsByLogin;
		QSet<QString> storedLogins;
		for (const QByteArray& userId : storedUserIds){
			imtbase::IObjectCollection::DataPtr dataPtr;
			const IUserInfo* userInfoPtr = nullptr;
			if (userCollectionPtr->GetObjectData(userId, dataPtr)){
				userInfoPtr = dynamic_cast<const IUserInfo*>(dataPtr.GetPtr());
			}

			if (userInfoPtr == nullptr){
				SendErrorMessage(0, QStringLiteral("Unable to read the user '%1', the groups are not synchronized").arg(QString::fromUtf8(userId)), s_messageSource);

				return false;
			}

			storedUserDataPtrs.insert(userId, dataPtr);

			const QString login = QString::fromUtf8(userInfoPtr->GetId()).toLower();
			storedLogins.insert(login);

			// Directory accounts (users with SID or created by LDAP) follow the membership of the directory.
			const QByteArray sid = userInfoPtr->GetSid();
			if (!sid.isEmpty()){
				storedUserIdsBySid.insert(sid, userId);
				managedUserIds.insert(userId);
			}
			else{
				storedUserIdsByLogin[login] << userId;
			}

			if (IsSystemUser(*userInfoPtr, ldapSystemId)){
				managedUserIds.insert(userId);
			}
		}

		QMap<QByteArray, QByteArrayList> userGroupIdsMap;
		QMap<QByteArray, QByteArray> ldapSidsByUserId;
		QSet<QByteArray> newUserIds;
		for (const LdapUser& ldapUser : *ldapUsersPtr){
			QByteArrayList groupIds;
			for (const QByteArray& groupSid : ldapUser.groupSids){
				const QByteArray groupId = objectIdsBySid.value(groupSid);
				if (!groupId.isEmpty() && !groupIds.contains(groupId)){
					groupIds << groupId;
				}
			}

			if (groupIds.isEmpty()){
				continue;
			}

			QByteArray userId = storedUserIdsBySid.value(ldapUser.sid);
			if (userId.isEmpty()){
				userId = FindUserIdByLogin(storedUserIdsByLogin, ldapUser);
			}

			if (userId.isEmpty()){
				userId = GetUserObjectId(ldapUser.sid);

				// The login is unique, a user with the same login and another SID is another account.
				if (storedUserDataPtrs.contains(userId) || storedLogins.contains(QString::fromUtf8(ldapUser.login).toLower())){
					SendWarningMessage(
								0,
								QStringLiteral("The LDAP user '%1' cannot be added, the login is already used by another user").arg(QString::fromUtf8(ldapUser.login)),
								s_messageSource);

					continue;
				}

				const QString name = ldapUser.name.isEmpty() ? QString::fromUtf8(ldapUser.accountName) : ldapUser.name;

				CIdentifiableUserInfo* userInfoPtr = new CIdentifiableUserInfo;
				userInfoPtr->SetObjectUuid(userId);
				userInfoPtr->SetId(ldapUser.login);
				userInfoPtr->SetName(name);
				userInfoPtr->SetDescription(ldapUser.description);
				userInfoPtr->SetMail(ldapUser.mail);
				userInfoPtr->SetSid(ldapUser.sid);
				userInfoPtr->AddToSystem(ldapSystemInfo);
				for (const QByteArray& groupId : groupIds){
					userInfoPtr->AddToGroup(groupId);
				}

				UserChange userChange;
				userChange.userId = userId;
				userChange.name = name;
				userChange.description = ldapUser.description;
				userChange.isNew = true;
				userChange.userPtr = imtbase::IObjectCollection::DataPtr::CreateFromUnique(istd::IChangeableUniquePtr(userInfoPtr));

				userChanges << userChange;
				newUserIds.insert(userId);
				storedLogins.insert(QString::fromUtf8(ldapUser.login).toLower());
			}
			else if (ldapSidsByUserId.contains(userId)){
				SendWarningMessage(
							0,
							QStringLiteral("The LDAP user '%1' is ignored, the user '%2' already belongs to another LDAP user").arg(QString::fromUtf8(ldapUser.login), QString::fromUtf8(userId)),
							s_messageSource);

				continue;
			}

			ldapSidsByUserId.insert(userId, ldapUser.sid);
			managedUserIds.insert(userId);
			userGroupIdsMap.insert(userId, groupIds);
			for (const QByteArray& groupId : groupIds){
				memberUserIdsMap[groupId] << userId;
			}
		}

		// Existing users only get the changed memberships, the other user data is managed by the user administration.
		for (const QByteArray& userId : storedUserIds){
			if (!managedUserIds.contains(userId)){
				continue;
			}

			const QByteArrayList desiredGroupIds = userGroupIdsMap.value(userId);
			const IUserInfo* storedUserInfoPtr = dynamic_cast<const IUserInfo*>(storedUserDataPtrs.value(userId).GetPtr());
			Q_ASSERT(storedUserInfoPtr != nullptr);

			UserChange userChange;
			userChange.userId = userId;
			userChange.name = storedUserInfoPtr->GetName();

			// A user found by the login is linked to the directory account, it follows the directory also when it leaves all mirrored groups.
			if (storedUserInfoPtr->GetSid().isEmpty() && ldapSidsByUserId.contains(userId)){
				userChange.sid = ldapSidsByUserId.value(userId);
			}

			const QByteArrayList storedGroupIdsOfUser = storedUserInfoPtr->GetGroups();
			for (const QByteArray& groupId : storedGroupIdsOfUser){
				if (storedLdapGroupIds.contains(groupId) && !desiredGroupIds.contains(groupId)){
					userChange.removedGroupIds << groupId;
				}
			}

			for (const QByteArray& groupId : desiredGroupIds){
				if (!storedGroupIdsOfUser.contains(groupId)){
					userChange.addedGroupIds << groupId;
				}
			}

			if (userChange.sid.isEmpty() && userChange.removedGroupIds.isEmpty() && userChange.addedGroupIds.isEmpty()){
				continue;
			}

			userChanges << userChange;
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

			if (userCollectionPtr != nullptr){
				groupInfoPtr->SetUsers(memberUserIdsMap.value(groupChange.groupId));
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

		// Members from LDAP follow the directory, other members assigned by an administrator are kept.
		if (userCollectionPtr != nullptr){
			const QByteArrayList memberUserIds = memberUserIdsMap.value(groupChange.groupId);
			const IUserGroupInfo::UserIds storedUserIds = GetOwnUserIds(*groupInfoPtr);

			IUserGroupInfo::UserIds userIds;
			for (const QByteArray& userId : storedUserIds){
				if (!managedUserIds.contains(userId) || memberUserIds.contains(userId)){
					userIds << userId;
				}
			}

			for (const QByteArray& userId : memberUserIds){
				if (!userIds.contains(userId)){
					userIds << userId;
				}
			}

			if (userIds != storedUserIds){
				groupInfoPtr->SetUsers(userIds);
				isChanged = true;
			}
		}

		if (isChanged){
			groupChanges << groupChange;
		}
	}

	if (groupChanges.isEmpty() && removedGroupIds.isEmpty() && userChanges.isEmpty()){
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
		istd::CChangeGroup userChangeGroup(m_userCollectionCompPtr.IsValid() ? m_userCollectionCompPtr.GetPtr() : nullptr);

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

		// Both sides of the membership are written, the users are written after their groups exist.
		if (retVal){
			for (const UserChange& userChange : userChanges){
				Q_ASSERT(userCollectionPtr != nullptr);

				if (userChange.isNew){
					retVal = !userCollectionPtr->InsertNewObject("User", userChange.name, userChange.description, userChange.userPtr.GetPtr(), userChange.userId).isEmpty();
				}
				else{
					// The user is read again, the changes made by the user administration since the snapshot are kept.
					imtbase::IObjectCollection::DataPtr currentDataPtr;
					istd::IChangeableUniquePtr userCopyPtr;
					if (userCollectionPtr->GetObjectData(userChange.userId, currentDataPtr) && currentDataPtr.IsValid()){
						userCopyPtr = currentDataPtr->CloneMe();
					}

					IUserInfo* userInfoPtr = dynamic_cast<IUserInfo*>(userCopyPtr.GetPtr());
					retVal = (userInfoPtr != nullptr);
					if (retVal){
						if (!userChange.sid.isEmpty() && userInfoPtr->GetSid().isEmpty()){
							userInfoPtr->SetSid(userChange.sid);
						}

						for (const QByteArray& groupId : userChange.removedGroupIds){
							userInfoPtr->RemoveFromGroup(groupId);
						}

						for (const QByteArray& groupId : userChange.addedGroupIds){
							userInfoPtr->AddToGroup(groupId);
						}

						retVal = userCollectionPtr->SetObjectData(userChange.userId, *userInfoPtr);
					}
				}

				if (!retVal){
					SendErrorMessage(0, QStringLiteral("Unable to write the user '%1'").arg(userChange.name), s_messageSource);

					break;
				}
			}
		}

		// The removal bypasses the group controller, the memberships of the removed groups are dropped in all users here,
		// also in the users not managed by LDAP and in cycles without a member snapshot.
		if (retVal && !removedGroupIds.isEmpty() && m_userCollectionCompPtr.IsValid()){
			retVal = RemoveGroupMemberships(*m_userCollectionCompPtr, removedGroupIds);
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


bool CLdapUserGroupSynchronizerComp::RemoveGroupMemberships(imtbase::IObjectCollection& userCollection, const imtbase::ICollectionInfo::Ids& groupIds) const
{
	static const QString s_messageSource = QStringLiteral("LDAP group synchronization");

	if (groupIds.isEmpty()){
		return true;
	}

	QByteArrayList groupIdList;
	for (const QByteArray& groupId : groupIds){
		groupIdList << groupId;
	}

	iprm::CParamsSet filterParams;

	istd::TUniqueInterfacePtr<CUserGroupFilter> groupFilterPtr = new CUserGroupFilter();
	groupFilterPtr->SetGroupIds(groupIdList);
	filterParams.SetEditableParameter(QByteArrayLiteral("GroupFilter"), groupFilterPtr.PopPtr(), true);

	// Only the current documents of the users are changed (default document state of the collection):
	// in a document database inactive documents are older revisions of a user, writing them back would revert the user.
	// Removed users cannot be written, they are not changed.
	istd::TUniqueInterfacePtr<imtbase::IObjectCollectionIterator> userIteratorPtr(
				userCollection.CreateObjectCollectionIterator(QByteArray(), 0, -1, &filterParams));
	if (!userIteratorPtr.IsValid()){
		SendErrorMessage(0, QStringLiteral("Unable to read the users, the memberships of the removed user groups cannot be removed"), s_messageSource);

		return false;
	}

	// All changed users are prepared before the first write, the iteration is not affected by the writes.
	QList<QPair<QByteArray, imtbase::IObjectCollection::DataPtr>> changedUsers;
	QSet<QByteArray> readUserIds;
	while (userIteratorPtr->Next()){
		const QByteArray userId = userIteratorPtr->GetObjectId();
		if (readUserIds.contains(userId)){
			continue;
		}

		readUserIds.insert(userId);

		imtbase::IObjectCollection::DataPtr userDataPtr;
		istd::IChangeableUniquePtr userCopyPtr;
		if (userIteratorPtr->GetObjectData(userDataPtr) && userDataPtr.IsValid()){
			userCopyPtr = userDataPtr->CloneMe();
		}

		IUserInfo* userInfoPtr = dynamic_cast<IUserInfo*>(userCopyPtr.GetPtr());
		if (userInfoPtr == nullptr){
			SendErrorMessage(0, QStringLiteral("Unable to read the user '%1', the memberships of the removed user groups cannot be removed").arg(QString::fromUtf8(userId)), s_messageSource);

			return false;
		}

		bool isChanged = false;
		for (const QByteArray& groupId : groupIdList){
			if (userInfoPtr->RemoveFromGroup(groupId)){
				isChanged = true;
			}
		}

		if (isChanged){
			changedUsers << qMakePair(userId, imtbase::IObjectCollection::DataPtr::CreateFromUnique(std::move(userCopyPtr)));
		}
	}

	for (const QPair<QByteArray, imtbase::IObjectCollection::DataPtr>& changedUser : changedUsers){
		if (!userCollection.SetObjectData(changedUser.first, *changedUser.second)){
			SendErrorMessage(0, QStringLiteral("Unable to remove the memberships of the removed user groups from the user '%1'").arg(QString::fromUtf8(changedUser.first)), s_messageSource);

			return false;
		}
	}

	return true;
}


QString CLdapUserGroupSynchronizerComp::GetBaseDn() const
{
	if (!m_baseDnParamCompPtr.IsValid()){
		return QString();
	}

	return m_baseDnParamCompPtr->GetText().trimmed();
}


QString CLdapUserGroupSynchronizerComp::GetGroupNameFilter() const
{
	if (m_groupNameFilterParamCompPtr.IsValid()){
		return m_groupNameFilterParamCompPtr->GetText();
	}

	return m_groupNameFilterAttrPtr.IsValid() ? *m_groupNameFilterAttrPtr : QStringLiteral("*");
}


bool CLdapUserGroupSynchronizerComp::IsSynchronizationEnabled() const
{
	const int paramsCount = m_enableableParamsCompPtr.GetCount();
	for (int index = 0; index < paramsCount; ++index){
		const iprm::IEnableableParam* paramPtr = m_enableableParamsCompPtr[index];
		if (paramPtr != nullptr && !paramPtr->IsEnabled()){
			return false;
		}
	}

	return true;
}


bool CLdapUserGroupSynchronizerComp::IsSystemFlagStored() const
{
	if (!m_versionInfoCompPtr.IsValid()){
		return true;
	}

	quint32 imtCoreVersion = 0;
	if (!m_versionInfoCompPtr->GetVersionNumber(imtcore::VI_IMTCORE, imtCoreVersion)){
		return false;
	}

	return imtCoreVersion >= s_minSystemInfoImtCoreVersion;
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
	bool isVersionErrorReported = false;
	bool isReadErrorReported = false;
	bool isUserReadErrorReported = false;

	while (!isInterruptionRequested()){
		if (m_parent.IsSynchronizationEnabled()){
			if (!m_parent.IsSystemFlagStored()){
				// The mirrored groups would be stored without the LDAP flag and could not be recognized again.
				if (!isVersionErrorReported){
					m_parent.SendErrorMessage(
								0,
								QStringLiteral("The user group storage does not support the system flag (ImtCore version %1 or later required), the LDAP groups are not synchronized").arg(s_minSystemInfoImtCoreVersion),
								QStringLiteral("LDAP group synchronization"));

					isVersionErrorReported = true;
				}
			}
			else{
				// Groups are only synchronized after a complete read, an unreachable directory must not remove them.
				// The base DN is read for every synchronization: a changed setting is used without restart.
				LdapGroups ldapGroups;
				if (ReadLdapGroups(m_parent.GetGroupNameFilter(), m_parent.GetBaseDn(), ldapGroups)){
					if (isReadErrorReported){
						m_parent.SendInfoMessage(
									0,
									QStringLiteral("The LDAP groups can be read again, the groups are synchronized"),
									QStringLiteral("LDAP group synchronization"));

						isReadErrorReported = false;
					}

					// Without a complete user snapshot the members of the groups are kept unchanged.
					if (m_parent.m_userCollectionCompPtr.IsValid()){
						// Only the members of the mirrored groups are queried, not of all groups read for the snapshot.
						LdapGroups mirroredGroups;
						LdapUsers ldapUsers;
						const bool areUsersRead =
									m_parent.GetMirroredLdapGroups(ldapGroups, mirroredGroups) &&
									ReadLdapUsers(m_parent.GetBaseDn(), mirroredGroups, ldapUsers);
						if (areUsersRead){
							isUserReadErrorReported = false;

							m_parent.SynchronizeGroups(ldapGroups, &ldapUsers);
						}
						else{
							if (!isUserReadErrorReported){
								m_parent.SendErrorMessage(
											0,
											QStringLiteral("Unable to read the LDAP users, the members of the user groups are not synchronized until the directory is available"),
											QStringLiteral("LDAP group synchronization"));

								isUserReadErrorReported = true;
							}

							m_parent.SynchronizeGroups(ldapGroups);
						}
					}
					else{
						m_parent.SynchronizeGroups(ldapGroups);
					}
				}
				else if (!isReadErrorReported){
					// Reported once per failure period: the stored groups are kept and may become outdated.
					m_parent.SendErrorMessage(
								0,
								QStringLiteral("Unable to read the LDAP groups, the user groups are not synchronized until the directory is available"),
								QStringLiteral("LDAP group synchronization"));

					isReadErrorReported = true;
				}
			}
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

