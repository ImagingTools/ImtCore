// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtauth/CLdapGroupDirectory.h>


#ifdef Q_OS_WIN
#include <windows.h>
#include <sddl.h>
#include <ActiveDS.h>
#include <Adshlp.h>
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "activeds.lib")
#pragma comment(lib, "adsiid.lib")
#endif


namespace imtauth
{


#ifdef Q_OS_WIN
namespace
{


class ComScope
{
public:
	ComScope()
		:m_result(CoInitializeEx(nullptr, COINIT_MULTITHREADED))
	{
	}

	~ComScope()
	{
		if (SUCCEEDED(m_result)){
			CoUninitialize();
		}
	}

	bool IsValid() const
	{
		return SUCCEEDED(m_result) || m_result == RPC_E_CHANGED_MODE;
	}

private:
	HRESULT m_result;
};


QByteArray SidString(void* data, DWORD size)
{
	if (data == nullptr || size < 8){
		return QByteArray();
	}
	const SID* sid = static_cast<const SID*>(data);
	if (size < 8u + 4u * sid->SubAuthorityCount || !IsValidSid(data)){
		return QByteArray();
	}
	LPWSTR text = nullptr;
	if (!ConvertSidToStringSidW(data, &text)){
		return QByteArray();
	}
	QByteArray result = QString::fromWCharArray(text).toUtf8();
	LocalFree(text);
	return result;
}


bool ReadSidArray(const VARIANT& value, QByteArray& sid)
{
	if (value.vt != (VT_ARRAY | VT_UI1) || value.parray == nullptr ||
		SafeArrayGetDim(value.parray) != 1){
		return false;
	}
	LONG first = 0;
	LONG last = -1;
	if (FAILED(SafeArrayGetLBound(value.parray, 1, &first)) ||
		FAILED(SafeArrayGetUBound(value.parray, 1, &last)) || last < first){
		return false;
	}
	void* data = nullptr;
	if (FAILED(SafeArrayAccessData(value.parray, &data))){
		return false;
	}
	sid = SidString(data, DWORD(last - first + 1));
	SafeArrayUnaccessData(value.parray);
	return !sid.isEmpty();
}


}
#endif


bool CLdapGroupDirectory::DiscoverGroups(const QString& pattern, QMap<QByteArray, QString>& groups)
{
	groups.clear();
	if (pattern.isEmpty() || pattern.size() > 256){
		return false;
	}
#ifdef Q_OS_WIN
	ComScope com;
	if (!com.IsValid()){
		return false;
	}
	IADs* root = nullptr;
	if (FAILED(ADsOpenObject(L"LDAP://rootDSE", nullptr, nullptr,
		ADS_SECURE_AUTHENTICATION | ADS_USE_SIGNING | ADS_USE_SEALING, IID_IADs, reinterpret_cast<void**>(&root)))){
		return false;
	}
	VARIANT namingContext;
	VariantInit(&namingContext);
	BSTR attribute = SysAllocString(L"defaultNamingContext");
	HRESULT result = root->Get(attribute, &namingContext);
	SysFreeString(attribute);
	root->Release();
	QString path;
	if (SUCCEEDED(result) && namingContext.vt == VT_BSTR){
		path = QStringLiteral("LDAP://") + QString::fromWCharArray(namingContext.bstrVal);
	}
	VariantClear(&namingContext);
	if (path.isEmpty()){
		return false;
	}
	IDirectorySearch* search = nullptr;
	if (FAILED(ADsOpenObject(reinterpret_cast<LPCWSTR>(path.utf16()), nullptr, nullptr,
		ADS_SECURE_AUTHENTICATION | ADS_USE_SIGNING | ADS_USE_SEALING, IID_IDirectorySearch, reinterpret_cast<void**>(&search)))){
		return false;
	}
	ADS_SEARCHPREF_INFO preferences[2] = {};
	preferences[0].dwSearchPref = ADS_SEARCHPREF_SEARCH_SCOPE;
	preferences[0].vValue.dwType = ADSTYPE_INTEGER;
	preferences[0].vValue.Integer = ADS_SCOPE_SUBTREE;
	preferences[1].dwSearchPref = ADS_SEARCHPREF_PAGESIZE;
	preferences[1].vValue.dwType = ADSTYPE_INTEGER;
	preferences[1].vValue.Integer = 500;
	result = search->SetSearchPreference(preferences, 2);
	bool ok = SUCCEEDED(result) && preferences[0].dwStatus == ADS_STATUS_S_OK && preferences[1].dwStatus == ADS_STATUS_S_OK;
	// Only '*' is a wildcard. Every other LDAP filter metacharacter is literal.
	QString escaped = pattern;
	escaped.replace("\\", "\\5c");
	escaped.replace("(", "\\28");
	escaped.replace(")", "\\29");
	escaped.replace(QChar('\0'), "\\00");
	QString filter = QStringLiteral("(&(objectCategory=group)(groupType:1.2.840.113556.1.4.803:=2147483648)(name=%1))").arg(escaped);
	std::wstring filterText = filter.toStdWString();
	WCHAR nameAttribute[] = L"name";
	WCHAR sidAttribute[] = L"objectSid";
	LPWSTR attributes[] = {nameAttribute, sidAttribute};
	ADS_SEARCH_HANDLE handle = nullptr;
	if (ok){
		ok = SUCCEEDED(search->ExecuteSearch(filterText.data(), attributes, 2, &handle));
	}
	QMap<QByteArray, QString> found;
	if (ok){
		HRESULT row;
		while ((row = search->GetNextRow(handle)) == S_OK){
			ADS_SEARCH_COLUMN nameColumn = {};
			ADS_SEARCH_COLUMN sidColumn = {};
			bool hasName = SUCCEEDED(search->GetColumn(handle, nameAttribute, &nameColumn));
			bool hasSid = SUCCEEDED(search->GetColumn(handle, sidAttribute, &sidColumn));
			QByteArray sid;
			QString name;
			if (hasName && nameColumn.pADsValues != nullptr && nameColumn.dwNumValues == 1 &&
				nameColumn.dwADsType == ADSTYPE_CASE_IGNORE_STRING){
				name = QString::fromWCharArray(nameColumn.pADsValues[0].CaseIgnoreString);
			}
			if (hasSid && sidColumn.pADsValues != nullptr && sidColumn.dwNumValues == 1 &&
				sidColumn.dwADsType == ADSTYPE_OCTET_STRING){
				ADS_OCTET_STRING value = sidColumn.pADsValues[0].OctetString;
				sid = SidString(value.lpValue, value.dwLength);
			}
			if (hasName){
				search->FreeColumn(&nameColumn);
			}
			if (hasSid){
				search->FreeColumn(&sidColumn);
			}
			if (sid.isEmpty() || name.isEmpty() || found.size() >= 10000){
				ok = false;
				break;
			}
			found.insert(sid, name);
		}
		ok = ok && row == S_ADS_NOMORE_ROWS;
	}
	if (handle != nullptr){
		search->CloseSearchHandle(handle);
	}
	search->Release();
	if (ok){
		groups = found;
	}
	return ok;
#else
	return false;
#endif
}


bool CLdapGroupDirectory::GetSecurityGroups(const QByteArray& userSid, QByteArrayList& groupSids)
{
	groupSids.clear();
#ifdef Q_OS_WIN
	ComScope com;
	if (!com.IsValid()){
		return false;
	}
	PSID binarySid = nullptr;
	if (userSid.isEmpty() || !ConvertStringSidToSidW(qUtf16Printable(userSid), &binarySid)){
		return false;
	}
	QByteArray hexSid(static_cast<const char*>(binarySid), GetLengthSid(binarySid));
	LocalFree(binarySid);
	QString path = QStringLiteral("LDAP://<SID=%1>").arg(QString::fromLatin1(hexSid.toHex()));
	IADs* user = nullptr;
	if (FAILED(ADsOpenObject(reinterpret_cast<LPCWSTR>(path.utf16()), nullptr, nullptr,
		ADS_SECURE_AUTHENTICATION | ADS_USE_SIGNING | ADS_USE_SEALING, IID_IADs, reinterpret_cast<void**>(&user)))){
		return false;
	}
	// tokenGroups includes transitive security membership and the primary group.
	// AD requires a reachable global catalog to compute it; any failure denies login.
	BSTR attribute = SysAllocString(L"tokenGroups");
	VARIANT requestedAttributes;
	VariantInit(&requestedAttributes);
	requestedAttributes.vt = VT_ARRAY | VT_VARIANT;
	requestedAttributes.parray = SafeArrayCreateVector(VT_VARIANT, 0, 1);
	VARIANT attributeValue;
	VariantInit(&attributeValue);
	attributeValue.vt = VT_BSTR;
	attributeValue.bstrVal = SysAllocString(L"tokenGroups");
	LONG attributeIndex = 0;
	HRESULT result = requestedAttributes.parray != nullptr
		? SafeArrayPutElement(requestedAttributes.parray, &attributeIndex, &attributeValue)
		: E_OUTOFMEMORY;
	VariantClear(&attributeValue);
	if (SUCCEEDED(result)){
		result = user->GetInfoEx(requestedAttributes, 0);
	}
	VariantClear(&requestedAttributes);
	VARIANT memberships;
	VariantInit(&memberships);
	if (SUCCEEDED(result)){
		result = user->GetEx(attribute, &memberships);
	}
	SysFreeString(attribute);
	user->Release();
	bool ok = SUCCEEDED(result) && memberships.vt == (VT_ARRAY | VT_VARIANT) &&
		memberships.parray != nullptr && SafeArrayGetDim(memberships.parray) == 1;
	LONG first = 0;
	LONG last = -1;
	if (ok){
		ok = SUCCEEDED(SafeArrayGetLBound(memberships.parray, 1, &first)) &&
			SUCCEEDED(SafeArrayGetUBound(memberships.parray, 1, &last)) && last >= first && qint64(last) - first < 10000;
	}
	QByteArrayList found;
	for (qint64 position = first; ok && position <= last; ++position){
		LONG index = LONG(position);
		VARIANT value;
		VariantInit(&value);
		QByteArray groupSid;
		ok = SUCCEEDED(SafeArrayGetElement(memberships.parray, &index, &value)) && ReadSidArray(value, groupSid);
		VariantClear(&value);
		if (ok && !found.contains(groupSid)){
			found.append(groupSid);
		}
	}
	VariantClear(&memberships);
	if (ok){
		groupSids = found;
	}
	return ok;
#else
	Q_UNUSED(userSid)
	return false;
#endif
}


} // namespace imtauth
