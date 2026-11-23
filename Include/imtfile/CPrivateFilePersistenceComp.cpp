// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CPrivateFilePersistenceComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMutexLocker>
#include <QtCore/QUuid>

// ImtCore includes
#include <imtfile/CFileTools.h>

#ifdef Q_OS_WIN
#include <QtCore/QElapsedTimer>
#include <QtCore/QThread>
#include <windows.h>
#include <aclapi.h>
#include <sddl.h>
#include <vector>
#else
#include <cerrno>
#include <climits>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#ifdef Q_OS_MACOS
#include <sys/acl.h>
#endif
#endif


namespace
{


/**
	The folder and its parents up to the root: the root first.
*/
QStringList GetFolderChain(const QString& folderPath)
{
	QStringList retVal;

	QString path = QDir::cleanPath(folderPath);
	while (true){
		retVal.prepend(path);

		const QString parentPath = QDir::cleanPath(QFileInfo(path).absolutePath());
		if (parentPath == path){
			break;
		}

		path = parentPath;
	}

	return retVal;
}


bool IsSameFolder(const QString& firstPath, const QString& secondPath)
{
#ifdef Q_OS_WIN
	const Qt::CaseSensitivity caseSensitivity = Qt::CaseInsensitive;
#else
	const Qt::CaseSensitivity caseSensitivity = Qt::CaseSensitive;
#endif

	return QDir::cleanPath(QFileInfo(firstPath).absoluteFilePath()).compare(QDir::cleanPath(QFileInfo(secondPath).absoluteFilePath()), caseSensitivity) == 0;
}


#ifdef Q_OS_WIN


typedef std::vector<BYTE> SidBuffer;


SidBuffer GetWellKnownSid(WELL_KNOWN_SID_TYPE sidType)
{
	DWORD size = SECURITY_MAX_SID_SIZE;
	SidBuffer retVal(size);
	if (!::CreateWellKnownSid(sidType, nullptr, retVal.data(), &size)){
		return SidBuffer();
	}

	retVal.resize(size);

	return retVal;
}


SidBuffer GetProcessUserSid()
{
	HANDLE token = nullptr;
	if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token)){
		return SidBuffer();
	}

	SidBuffer retVal;

	DWORD size = 0;
	::GetTokenInformation(token, TokenUser, nullptr, 0, &size);
	std::vector<BYTE> tokenUser(size);
	if ((size > 0) && ::GetTokenInformation(token, TokenUser, tokenUser.data(), size, &size)){
		const PSID sidPtr = reinterpret_cast<const TOKEN_USER*>(tokenUser.data())->User.Sid;
		const BYTE* sidBytesPtr = static_cast<const BYTE*>(sidPtr);
		retVal.assign(sidBytesPtr, sidBytesPtr + ::GetLengthSid(sidPtr));
	}

	::CloseHandle(token);

	return retVal;
}


/**
	The accounts that may have the private data: the account of the process, the system and the administrators of the computer.
*/
std::vector<SidBuffer> GetPrivateSids()
{
	return {GetProcessUserSid(), GetWellKnownSid(WinLocalSystemSid), GetWellKnownSid(WinBuiltinAdministratorsSid)};
}


/**
	The process has the rights of the administrators: it works as the system or as an administrator with the full rights (a program
	started "as administrator"). An administrator without them (the usual start under UAC) does not.
*/
bool IsAdministrator()
{
	const SidBuffer administratorsSid = GetWellKnownSid(WinBuiltinAdministratorsSid);
	BOOL isMember = FALSE;

	return !administratorsSid.empty() && ::CheckTokenMembership(nullptr, const_cast<BYTE*>(administratorsSid.data()), &isMember) && (isMember != FALSE);
}


/**
	The process works as the system.
*/
bool IsSystem()
{
	const SidBuffer userSid = GetProcessUserSid();
	const SidBuffer systemSid = GetWellKnownSid(WinLocalSystemSid);

	return !userSid.empty() && !systemSid.empty() && ::EqualSid(const_cast<BYTE*>(userSid.data()), const_cast<BYTE*>(systemSid.data()));
}


/**
	The accounts that get access to the new private folders and files. The data of the computer (\c areAdministratorsTrusted) that the
	system or an administrator creates are for the system and the administrators only: the same whoever of them creates them.
*/
std::vector<SidBuffer> GetCreatorSids(bool areAdministratorsTrusted)
{
	if (areAdministratorsTrusted && IsAdministrator()){
		return {GetWellKnownSid(WinLocalSystemSid), GetWellKnownSid(WinBuiltinAdministratorsSid)};
	}

	return GetPrivateSids();
}


/**
	The accounts that may own and change the folders of the system above the private folder: the private accounts and the installer
	of Windows.
*/
std::vector<SidBuffer> GetSystemSids()
{
	std::vector<SidBuffer> retVal = GetPrivateSids();

	PSID trustedInstallerSidPtr = nullptr;
	if (::ConvertStringSidToSidW(L"S-1-5-80-956008885-3418522649-1831038044-1853292631-2271478464", &trustedInstallerSidPtr)){
		const BYTE* sidBytesPtr = static_cast<const BYTE*>(trustedInstallerSidPtr);
		retVal.emplace_back(sidBytesPtr, sidBytesPtr + ::GetLengthSid(trustedInstallerSidPtr));

		::LocalFree(trustedInstallerSidPtr);
	}

	return retVal;
}


bool ContainsSid(const std::vector<SidBuffer>& sids, PSID sidPtr)
{
	for (const SidBuffer& sid : sids){
		if (!sid.empty() && ::EqualSid(const_cast<BYTE*>(sid.data()), sidPtr)){
			return true;
		}
	}

	return false;
}


/**
	Check the owner and the access list of a file or a folder. A private one: only the trusted accounts have any access. Another one
	(a folder above the private folder): the other accounts cannot delete, rename or replace what is in it, nor change its access.
*/
bool CheckSecurity(const QString& path, bool isPrivate, QString& errorMessage)
{
	const std::vector<SidBuffer> trustedSids = isPrivate ? GetPrivateSids() : GetSystemSids();
	const DWORD replacementRights = DELETE | WRITE_DAC | WRITE_OWNER | FILE_DELETE_CHILD | GENERIC_ALL;

	PSID ownerSidPtr = nullptr;
	PACL daclPtr = nullptr;
	PSECURITY_DESCRIPTOR descriptorPtr = nullptr;
	const std::wstring nativePath = QDir::toNativeSeparators(path).toStdWString();
	if (::GetNamedSecurityInfoW(
				nativePath.c_str(),
				SE_FILE_OBJECT,
				OWNER_SECURITY_INFORMATION | DACL_SECURITY_INFORMATION,
				&ownerSidPtr,
				nullptr,
				&daclPtr,
				nullptr,
				&descriptorPtr) != ERROR_SUCCESS){
		errorMessage = QStringLiteral("The access list of '%1' cannot be read").arg(path);

		return false;
	}

	bool retVal = true;

	if (!ContainsSid(trustedSids, ownerSidPtr)){
		errorMessage = QStringLiteral("'%1' belongs to an account that is neither this one, nor the system, nor the administrators").arg(path);

		retVal = false;
	}
	else if (daclPtr == nullptr){
		errorMessage = QStringLiteral("'%1' has no access list: everybody can open it").arg(path);

		retVal = false;
	}

	for (DWORD index = 0; retVal && (index < daclPtr->AceCount); ++index){
		ACE_HEADER* headerPtr = nullptr;
		if (!::GetAce(daclPtr, index, reinterpret_cast<LPVOID*>(&headerPtr))){
			errorMessage = QStringLiteral("The access list of '%1' cannot be read").arg(path);

			retVal = false;

			break;
		}

		// The denials do not give any access. The entries for the contained files and folders only do not give any access to a folder
		// above, but the new files of the private folder get them
		if ((!isPrivate && ((headerPtr->AceFlags & INHERIT_ONLY_ACE) != 0)) || (headerPtr->AceType == ACCESS_DENIED_ACE_TYPE)){
			continue;
		}

		// An entry of an unknown type is not trusted either
		bool isGrantedToOthers = true;
		if (headerPtr->AceType == ACCESS_ALLOWED_ACE_TYPE){
			const ACCESS_ALLOWED_ACE* acePtr = reinterpret_cast<const ACCESS_ALLOWED_ACE*>(headerPtr);
			isGrantedToOthers = !ContainsSid(trustedSids, const_cast<DWORD*>(&acePtr->SidStart)) && (isPrivate || ((acePtr->Mask & replacementRights) != 0));
		}

		if (isGrantedToOthers){
			errorMessage = isPrivate ?
						QStringLiteral("Other accounts have access to '%1'").arg(path) :
						QStringLiteral("Other accounts can replace what is in '%1'").arg(path);

			retVal = false;
		}
	}

	::LocalFree(descriptorPtr);

	return retVal;
}


/**
	The access list of the account of the process, the system and the administrators only (see GetCreatorSids()), \c nullptr if it
	cannot be made. Free it with LocalFree.
*/
PACL CreatePrivateAcl(DWORD inheritance, bool areAdministratorsTrusted)
{
	const std::vector<SidBuffer> sids = GetCreatorSids(areAdministratorsTrusted);

	std::vector<EXPLICIT_ACCESSW> entries;
	for (const SidBuffer& sid : sids){
		if (sid.empty()){
			return nullptr;
		}

		EXPLICIT_ACCESSW entry = {};
		entry.grfAccessPermissions = FILE_ALL_ACCESS;
		entry.grfAccessMode = SET_ACCESS;
		entry.grfInheritance = inheritance;
		entry.Trustee.TrusteeForm = TRUSTEE_IS_SID;
		entry.Trustee.TrusteeType = TRUSTEE_IS_UNKNOWN;
		entry.Trustee.ptstrName = reinterpret_cast<LPWSTR>(const_cast<BYTE*>(sid.data()));
		entries.push_back(entry);
	}

	PACL aclPtr = nullptr;
	if (::SetEntriesInAclW(ULONG(entries.size()), entries.data(), nullptr, &aclPtr) != ERROR_SUCCESS){
		return nullptr;
	}

	return aclPtr;
}


/**
	Create a folder or a new empty file with the given security.
*/
bool CreateEntry(const std::wstring& nativePath, bool isFolder, SECURITY_DESCRIPTOR& descriptor)
{
	SECURITY_ATTRIBUTES attributes = {sizeof(SECURITY_ATTRIBUTES), &descriptor, FALSE};
	if (isFolder){
		return ::CreateDirectoryW(nativePath.c_str(), &attributes) != 0;
	}

	const HANDLE fileHandle = ::CreateFileW(nativePath.c_str(), GENERIC_WRITE, 0, &attributes, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE){
		return false;
	}

	::CloseHandle(fileHandle);

	return true;
}


/**
	Create a folder (\c isFolder) or a new empty file with the private access list: the access of the parent folder is not inherited.
	A file or a link with this name is not opened. The data of the computer (\c areAdministratorsTrusted) that an administrator creates
	belong to the administrators: the other administrators and the system accept them.
*/
bool CreatePrivateEntry(const QString& path, bool isFolder, bool areAdministratorsTrusted)
{
	PACL aclPtr = CreatePrivateAcl(isFolder ? SUB_CONTAINERS_AND_OBJECTS_INHERIT : NO_INHERITANCE, areAdministratorsTrusted);
	if (aclPtr == nullptr){
		return false;
	}

	SECURITY_DESCRIPTOR descriptor;
	bool retVal =
				::InitializeSecurityDescriptor(&descriptor, SECURITY_DESCRIPTOR_REVISION) &&
				::SetSecurityDescriptorDacl(&descriptor, TRUE, aclPtr, FALSE) &&
				::SetSecurityDescriptorControl(&descriptor, SE_DACL_PROTECTED, SE_DACL_PROTECTED);
	if (retVal){
		const std::wstring nativePath = QDir::toNativeSeparators(path).toStdWString();

		// The administrators usually own what an administrator creates already, but the policy of the computer can make the account
		// the owner: then the others would refuse it. The system owns what it creates
		SidBuffer administratorsSid;
		if (areAdministratorsTrusted && IsAdministrator() && !IsSystem()){
			administratorsSid = GetWellKnownSid(WinBuiltinAdministratorsSid);
		}

		if (!administratorsSid.empty() && ::SetSecurityDescriptorOwner(&descriptor, administratorsSid.data(), FALSE)){
			retVal = CreateEntry(nativePath, isFolder, descriptor);

			// The account may be not allowed to give the administrators what it creates: the owner is chosen by the system then
			if (!retVal && (::GetLastError() == ERROR_INVALID_OWNER)){
				retVal = ::SetSecurityDescriptorOwner(&descriptor, nullptr, FALSE) && CreateEntry(nativePath, isFolder, descriptor);
			}
		}
		else{
			retVal = CreateEntry(nativePath, isFolder, descriptor);
		}
	}

	::LocalFree(aclPtr);

	return retVal;
}


/**
	A junction or a link (a reparse point) is not accepted: its access would be read through it, from where it leads to, and the
	folders on the way there would not be checked.
*/
bool CheckNoReparsePoint(const QString& path, QString& errorMessage)
{
	const std::wstring nativePath = QDir::toNativeSeparators(path).toStdWString();
	const DWORD attributes = ::GetFileAttributesW(nativePath.c_str());
	if (attributes == INVALID_FILE_ATTRIBUTES){
		errorMessage = QStringLiteral("'%1' cannot be checked").arg(path);

		return false;
	}

	if ((attributes & FILE_ATTRIBUTE_REPARSE_POINT) != 0){
		errorMessage = QStringLiteral("'%1' is a junction or a link: use the path of the folder that it leads to").arg(path);

		return false;
	}

	return true;
}


bool CheckFolderChain(const QString& folderPath, bool isPrivateFolder, QString& errorMessage)
{
	const QStringList chain = GetFolderChain(folderPath);
	for (int index = 0; index < chain.count(); ++index){
		if (!CheckNoReparsePoint(chain[index], errorMessage) || !CheckSecurity(chain[index], isPrivateFolder && (index == chain.count() - 1), errorMessage)){
			return false;
		}
	}

	return true;
}


bool CheckFile(const QString& filePath, QString& errorMessage)
{
	return CheckNoReparsePoint(filePath, errorMessage) && CheckSecurity(filePath, true, errorMessage);
}


/**
	Create a new empty file with its own private access list: what its folder would give to the new files is not taken.
*/
bool CreatePrivateFile(const QString& filePath, bool areAdministratorsTrusted)
{
	return CreatePrivateEntry(filePath, false, areAdministratorsTrusted);
}


bool CreateFolder(const QString& folderPath, bool areAdministratorsTrusted)
{
	return CreatePrivateEntry(folderPath, true, areAdministratorsTrusted);
}


/**
	Something is at the path: a file, a folder, a link (also one that leads nowhere) or an entry that cannot be checked.
*/
bool IsEntryPresent(const QString& path)
{
	const std::wstring nativePath = QDir::toNativeSeparators(path).toStdWString();
	if (::GetFileAttributesW(nativePath.c_str()) != INVALID_FILE_ATTRIBUTES){
		return true;
	}

	const DWORD errorCode = ::GetLastError();

	return (errorCode != ERROR_FILE_NOT_FOUND) && (errorCode != ERROR_PATH_NOT_FOUND);
}


/**
	The text of an error of the system for the log.
*/
QString GetErrorText(DWORD errorCode)
{
	LPWSTR textPtr = nullptr;
	const DWORD size = ::FormatMessageW(
				FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
				nullptr,
				errorCode,
				0,
				reinterpret_cast<LPWSTR>(&textPtr),
				0,
				nullptr);

	QString retVal = (size > 0) ? QString::fromWCharArray(textPtr, int(size)).trimmed() : QString();

	::LocalFree(textPtr);

	return QStringLiteral("%1 (error %2)").arg(retVal).arg(errorCode);
}


/**
	Write the data of the file from the cache to the disk.
*/
bool FlushFile(const QString& filePath, QString& errorMessage)
{
	const std::wstring nativePath = QDir::toNativeSeparators(filePath).toStdWString();
	const HANDLE fileHandle = ::CreateFileW(nativePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (fileHandle == INVALID_HANDLE_VALUE){
		errorMessage = QStringLiteral("The file '%1' cannot be written to the disk: %2").arg(filePath, GetErrorText(::GetLastError()));

		return false;
	}

	const bool retVal = ::FlushFileBuffers(fileHandle) != 0;
	if (!retVal){
		errorMessage = QStringLiteral("The file '%1' cannot be written to the disk: %2").arg(filePath, GetErrorText(::GetLastError()));
	}

	::CloseHandle(fileHandle);

	return retVal;
}


/**
	The replacement of a file is written to the disk by the replacement itself (MOVEFILE_WRITE_THROUGH).
*/
void FlushFolder(const QString& /*folderPath*/)
{
}


/**
	Replace the file by the new one. Another program can keep the file open for a moment and not allow to delete it, for example an
	antivirus or the indexer checks a file that has just been written: then the replacement is tried again for a while. A save waits
	for it at most \c maxWaitMs.
*/
bool ReplacePrivateFile(const QString& sourcePath, const QString& targetPath, QString& errorMessage)
{
	const qint64 maxWaitMs = 1000;
	const unsigned long retryDelayMs = 20;

	const std::wstring nativeSourcePath = QDir::toNativeSeparators(sourcePath).toStdWString();
	const std::wstring nativeTargetPath = QDir::toNativeSeparators(targetPath).toStdWString();

	QElapsedTimer timer;
	timer.start();
	while (true){
		if (::MoveFileExW(nativeSourcePath.c_str(), nativeTargetPath.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0){
			return true;
		}

		const DWORD errorCode = ::GetLastError();
		const bool isBusy = (errorCode == ERROR_ACCESS_DENIED) || (errorCode == ERROR_SHARING_VIOLATION) || (errorCode == ERROR_LOCK_VIOLATION);
		if (!isBusy || (timer.elapsed() >= maxWaitMs)){
			errorMessage = QStringLiteral("The file '%1' cannot be replaced: %2").arg(targetPath, GetErrorText(errorCode));

			return false;
		}

		QThread::msleep(retryDelayMs);
	}
}


#else


#ifdef Q_OS_MACOS
// The access list gives its rights before the mode: chmod does not remove it
const QString s_accessListHint = QStringLiteral(", chmod -N");
#else
const QString s_accessListHint;
#endif


bool IsTrustedOwner(uid_t ownerId)
{
	return (ownerId == 0) || (ownerId == ::geteuid());
}


#ifdef Q_OS_MACOS


/**
	The access list of macOS gives its rights before the mode is checked. A private file or folder may have no entry that allows
	anything, a folder above no entry that allows to replace what is in it. The denials do not give any access, the entries for the
	contained files and folders only do not give any access to a folder above. When in doubt (the list cannot be read, an entry of an
	unknown kind) the list allows.
*/
bool IsAllowedByAccessList(const QString& path, bool isPrivate)
{
	acl_t acl = ::acl_get_file(QFile::encodeName(path).constData(), ACL_TYPE_EXTENDED);
	if (acl == nullptr){
		// No list, or a file system without them
		return (errno != ENOENT) && (errno != ENOTSUP) && (errno != EOPNOTSUPP);
	}

	bool retVal = false;

	acl_entry_t entry = nullptr;
	for (int entryId = ACL_FIRST_ENTRY; !retVal && (::acl_get_entry(acl, entryId, &entry) == 0); entryId = ACL_NEXT_ENTRY){
		acl_tag_t tag = ACL_UNDEFINED_TAG;
		if ((::acl_get_tag_type(entry, &tag) == 0) && (tag == ACL_EXTENDED_DENY)){
			continue;
		}

		if (isPrivate || (tag != ACL_EXTENDED_ALLOW)){
			retVal = true;

			continue;
		}

		acl_flagset_t flags = nullptr;
		acl_permset_t permissions = nullptr;
		if ((::acl_get_flagset_np(entry, &flags) != 0) || (::acl_get_permset(entry, &permissions) != 0)){
			retVal = true;

			continue;
		}

		if (::acl_get_flag_np(flags, ACL_ENTRY_ONLY_INHERIT) == 1){
			continue;
		}

		retVal =
					(::acl_get_perm_np(permissions, ACL_DELETE) != 0) ||
					(::acl_get_perm_np(permissions, ACL_DELETE_CHILD) != 0) ||
					(::acl_get_perm_np(permissions, ACL_WRITE_SECURITY) != 0) ||
					(::acl_get_perm_np(permissions, ACL_CHANGE_OWNER) != 0);
	}

	::acl_free(acl);

	return retVal;
}


#endif


/**
	Other users can delete, rename or replace what is in the folder: it belongs to another user, the others can write into it without
	the sticky bit (/tmp), or its access list (macOS) allows it.
*/
bool IsReplaceableByOthers(const QString& folderPath, const struct stat& info)
{
	const bool isWritableByOthers = (info.st_mode & (S_IWGRP | S_IWOTH)) != 0;
	if (!IsTrustedOwner(info.st_uid) || (isWritableByOthers && ((info.st_mode & S_ISVTX) == 0))){
		return true;
	}

#ifdef Q_OS_MACOS
	return IsAllowedByAccessList(folderPath, false);
#else
	Q_UNUSED(folderPath);

	return false;
#endif
}


/**
	Other users have any access to the private file or folder: by its mode or by its access list (macOS).
*/
bool IsOpenToOthers(const QString& path, const struct stat& info)
{
	if ((info.st_mode & (S_IRWXG | S_IRWXO)) != 0){
		return true;
	}

#ifdef Q_OS_MACOS
	return IsAllowedByAccessList(path, true);
#else
	Q_UNUSED(path);

	return false;
#endif
}


/**
	The target of the link as it is written, not resolved: empty if it cannot be read.
*/
QString ReadLink(const QString& linkPath)
{
	QByteArray target(PATH_MAX, Qt::Uninitialized);
	const ssize_t size = ::readlink(QFile::encodeName(linkPath).constData(), target.data(), size_t(target.size()));
	if ((size <= 0) || (size >= target.size())){
		return {};
	}

	return QFile::decodeName(target.left(int(size)));
}


/**
	Resolve the folder as the system does, element by element, and check each element on the way: in a folder that the others can
	change they could put another entry in place of the next one, a link of another user could lead anywhere. A link leads on to its
	target, which is resolved and checked so as well, also the links that it leads to.
	\param resolvedPath	the folder without links.
*/
bool ResolveFolder(const QString& folderPath, QString& resolvedPath, QString& errorMessage)
{
	// The links that one path may lead through (SYMLOOP_MAX): a loop is not followed for ever
	const int maxLinkCount = 40;

	QString path = QStringLiteral("/");

	struct stat info;
	if ((::lstat("/", &info) != 0) || IsReplaceableByOthers(path, info)){
		errorMessage = QStringLiteral("Other users can replace what is in the folder '/'");

		return false;
	}

	QStringList names = QFileInfo(folderPath).absoluteFilePath().split(QLatin1Char('/'), Qt::SkipEmptyParts);
	int linkCount = 0;
	while (!names.isEmpty()){
		const QString name = names.takeFirst();
		if (name == QLatin1String(".")){
			continue;
		}

		// The path has no links: its parent is the folder on the disk, it is checked already
		if (name == QLatin1String("..")){
			path = QFileInfo(path).path();

			continue;
		}

		const QString nextPath = (path == QLatin1String("/")) ? (path + name) : (path + QLatin1Char('/') + name);
		if (::lstat(QFile::encodeName(nextPath).constData(), &info) != 0){
			errorMessage = QStringLiteral("The folder '%1' cannot be checked").arg(nextPath);

			return false;
		}

		if (S_ISLNK(info.st_mode)){
			if (!IsTrustedOwner(info.st_uid)){
				errorMessage = QStringLiteral("The link '%1' belongs to another user").arg(nextPath);

				return false;
			}

			const QString targetPath = ReadLink(nextPath);
			if (targetPath.isEmpty() || (++linkCount > maxLinkCount)){
				errorMessage = QStringLiteral("The link '%1' cannot be followed").arg(nextPath);

				return false;
			}

			// The target goes on from the folder of the link or from the root, the rest of the path after it
			names = targetPath.split(QLatin1Char('/'), Qt::SkipEmptyParts) + names;
			if (targetPath.startsWith(QLatin1Char('/'))){
				path = QStringLiteral("/");
			}

			continue;
		}

		if (!S_ISDIR(info.st_mode)){
			errorMessage = QStringLiteral("'%1' is not a folder").arg(nextPath);

			return false;
		}

		if (IsReplaceableByOthers(nextPath, info)){
			errorMessage = QStringLiteral("Other users can replace what is in the folder '%1'").arg(nextPath);

			return false;
		}

		path = nextPath;
	}

	resolvedPath = path;

	return true;
}


/**
	On the way to the folder no other user can replace a folder or a link: the folders belong to this user or to the system, and the
	others cannot write into them or only with the sticky bit (/tmp). A private folder at the end belongs to this user and nobody else
	can open it.
*/
bool CheckFolderChain(const QString& folderPath, bool isPrivateFolder, QString& errorMessage)
{
	QString resolvedPath;
	if (!ResolveFolder(folderPath, resolvedPath, errorMessage)){
		return false;
	}

	if (!isPrivateFolder){
		return true;
	}

	struct stat info;
	if (::stat(QFile::encodeName(resolvedPath).constData(), &info) != 0){
		errorMessage = QStringLiteral("The folder '%1' cannot be checked").arg(resolvedPath);

		return false;
	}

	if (info.st_uid != ::geteuid()){
		errorMessage = QStringLiteral("The folder '%1' belongs to another user").arg(resolvedPath);

		return false;
	}

	if (IsOpenToOthers(resolvedPath, info)){
		errorMessage = QStringLiteral("Other users can open the folder '%1': make it accessible to its owner only (chmod 700%2)").arg(resolvedPath, s_accessListHint);

		return false;
	}

	return true;
}


bool CheckFile(const QString& filePath, QString& errorMessage)
{
	struct stat info;
	if (::lstat(QFile::encodeName(filePath).constData(), &info) != 0){
		errorMessage = QStringLiteral("The file '%1' cannot be checked").arg(filePath);

		return false;
	}

	if (!S_ISREG(info.st_mode)){
		errorMessage = QStringLiteral("'%1' is not a file").arg(filePath);

		return false;
	}

	if (info.st_uid != ::geteuid()){
		errorMessage = QStringLiteral("The file '%1' belongs to another user").arg(filePath);

		return false;
	}

	if (IsOpenToOthers(filePath, info)){
		errorMessage = QStringLiteral("Other users can read the file '%1': make it readable by its owner only (chmod 600%2)").arg(filePath, s_accessListHint);

		return false;
	}

	return true;
}


/**
	Create a new empty file that only its owner can read and write. A file or a link with this name is not opened.
*/
bool CreatePrivateFile(const QString& filePath, bool /*areAdministratorsTrusted*/)
{
	const QFileDevice::Permissions ownerPermissions = QFileDevice::ReadOwner | QFileDevice::WriteOwner;

	QFile file(filePath);
#if QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
	if (!file.open(QIODevice::WriteOnly | QIODevice::NewOnly, ownerPermissions)){
		return false;
	}
#else
	if (!file.open(QIODevice::WriteOnly | QIODevice::NewOnly) || !file.setPermissions(ownerPermissions)){
		return false;
	}
#endif

	file.close();

	return true;
}


/**
	Create a folder with the mode at once: the umask can only take rights away. On macOS what the parent gives to the new folders (its
	inherited access list) is removed: the access list gives its rights before the mode.
*/
bool CreateFolderWithMode(const QString& folderPath, mode_t mode)
{
	const QByteArray nativePath = QFile::encodeName(folderPath);
	if (::mkdir(nativePath.constData(), mode) != 0){
		return false;
	}

#ifdef Q_OS_MACOS
	acl_t acl = ::acl_init(0);
	if (acl == nullptr){
		return false;
	}

	// A file system without the access lists has nothing to remove
	const bool retVal = (::acl_set_file(nativePath.constData(), ACL_TYPE_EXTENDED, acl) == 0) || (errno == ENOTSUP) || (errno == EOPNOTSUPP);

	::acl_free(acl);

	return retVal;
#else
	return true;
#endif
}


bool CreateFolder(const QString& folderPath, bool /*areAdministratorsTrusted*/)
{
	return CreateFolderWithMode(folderPath, 0700);
}


/**
	Something is at the path: a file, a folder, a link (also one that leads nowhere) or an entry that cannot be checked.
*/
bool IsEntryPresent(const QString& path)
{
	struct stat info;

	return (::lstat(QFile::encodeName(path).constData(), &info) == 0) || ((errno != ENOENT) && (errno != ENOTDIR));
}


/**
	Write the data of a file or the entries of a folder from the cache to the disk: on macOS fsync leaves them in the cache of the disk.
*/
bool Flush(const QString& path)
{
	const int descriptor = ::open(QFile::encodeName(path).constData(), O_RDONLY | O_CLOEXEC);
	if (descriptor < 0){
		return false;
	}

#ifdef Q_OS_MACOS
	const bool retVal = (::fcntl(descriptor, F_FULLFSYNC) == 0) || (::fsync(descriptor) == 0);
#else
	const bool retVal = ::fsync(descriptor) == 0;
#endif

	::close(descriptor);

	return retVal;
}


bool FlushFile(const QString& filePath, QString& errorMessage)
{
	if (!Flush(filePath)){
		const int errorCode = errno;

		errorMessage = QStringLiteral("The file '%1' cannot be written to the disk: %2 (error %3)").arg(filePath, QString::fromLocal8Bit(std::strerror(errorCode))).arg(errorCode);

		return false;
	}

	return true;
}


/**
	The new entry of the folder (the replaced file) is written to the disk. Some file systems cannot do it for a folder: the file is
	replaced anyway.
*/
void FlushFolder(const QString& folderPath)
{
	Flush(folderPath);
}


/**
	Replace the file by the new one: the file that other programs have open is replaced as well.
*/
bool ReplacePrivateFile(const QString& sourcePath, const QString& targetPath, QString& errorMessage)
{
	if (::rename(QFile::encodeName(sourcePath).constData(), QFile::encodeName(targetPath).constData()) != 0){
		const int errorCode = errno;

		errorMessage = QStringLiteral("The file '%1' cannot be replaced: %2 (error %3)").arg(targetPath, QString::fromLocal8Bit(std::strerror(errorCode))).arg(errorCode);

		return false;
	}

	return true;
}


#endif


/**
	Create the missing folders above the private folder: on Unix they are not writable by other users, whatever the umask is.
*/
bool CreateParentFolders(const QString& folderPath)
{
#ifdef Q_OS_WIN
	return QDir().mkpath(folderPath);
#else
	for (const QString& path : GetFolderChain(folderPath)){
		// Another program can create the folder meanwhile
		if (!QFileInfo::exists(path) && !CreateFolderWithMode(path, 0755) && !QFileInfo(path).isDir()){
			return false;
		}
	}

	return true;
#endif
}


/**
	The file and its folder are private, no other account can replace a folder on the way to them.
*/
bool CheckFileToLoad(const QString& filePath, QString& errorMessage)
{
	return CheckFolderChain(QFileInfo(filePath).absolutePath(), true, errorMessage) && CheckFile(filePath, errorMessage);
}


} // namespace


namespace imtfile
{


// public static methods

bool CPrivateFilePersistenceComp::EnsurePrivateFolder(const QString& folderPath, QString& errorMessage, bool areAdministratorsTrusted)
{
	const QString absolutePath = QDir::cleanPath(QFileInfo(folderPath).absoluteFilePath());
	if (!QFileInfo::exists(absolutePath)){
		const QString parentPath = QFileInfo(absolutePath).absolutePath();

		// The folders that exist are checked before anything is created in them: a refused folder gets nothing
		QString existingPath = parentPath;
		while (!QFileInfo::exists(existingPath)){
			const QString upperPath = QDir::cleanPath(QFileInfo(existingPath).absolutePath());
			if (upperPath == existingPath){
				break;
			}

			existingPath = upperPath;
		}

		if (!CheckFolderChain(existingPath, false, errorMessage)){
			return false;
		}

		if (!CreateParentFolders(parentPath)){
			errorMessage = QStringLiteral("The folder '%1' cannot be created").arg(parentPath);

			return false;
		}

		if (!CreateFolder(absolutePath, areAdministratorsTrusted)){
			errorMessage = QStringLiteral("The folder '%1' cannot be created for its owner only").arg(absolutePath);

			return false;
		}
	}

	return CheckFolderChain(absolutePath, true, errorMessage);
}


bool CPrivateFilePersistenceComp::IsPrivateFile(const QString& filePath, QString& errorMessage)
{
	return CheckFile(filePath, errorMessage);
}


// reimplemented (imtfile::ISavedFileDigestProvider)

QByteArray CPrivateFilePersistenceComp::GetSavedFileDigest(const QString& filePath) const
{
	QString path;
	QString errorMessage;
	if (!GetPrivatePath(filePath, path, errorMessage)){
		return {};
	}

	QMutexLocker locker(&m_savedDigestsMutex);

	return m_savedDigests.value(path);
}


// reimplemented (ifile::IFilePersistence)

bool CPrivateFilePersistenceComp::IsOperationSupported(
			const istd::IChangeable* dataObjectPtr,
			const QString* filePathPtr,
			int flags,
			bool beQuiet) const
{
	if (!m_slavePersistenceCompPtr.IsValid()){
		return false;
	}

	// The file to load is checked as LoadFromFile() checks it: the caller does not read a file that is not loaded
	if ((filePathPtr != nullptr) && ((flags & (QF_LOAD | QF_SAVE)) == QF_LOAD)){
		QString path;
		QString errorMessage;
		if (!GetPrivatePath(*filePathPtr, path, errorMessage) || !IsEntryPresent(path) || !CheckFileToLoad(path, errorMessage)){
			if (!beQuiet && !errorMessage.isEmpty()){
				SendErrorMessage(0, QStringLiteral("The file is not loaded: %1").arg(errorMessage));
			}

			return false;
		}
	}

	return m_slavePersistenceCompPtr->IsOperationSupported(dataObjectPtr, filePathPtr, flags, beQuiet);
}


ifile::IFilePersistence::OperationState CPrivateFilePersistenceComp::LoadFromFile(
			istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* progressManagerPtr) const
{
	QString path;
	QString errorMessage;
	if (!GetPrivatePath(filePath, path, errorMessage)){
		SendErrorMessage(0, QStringLiteral("The file is not loaded: %1").arg(errorMessage));

		return OS_FAILED;
	}

	// The slave does not get a path where nothing is: another account could put a file or a link there before the slave opens it
	if (!IsEntryPresent(path)){
		SendWarningMessage(0, QStringLiteral("The file '%1' does not exist").arg(path));

		return OS_FAILED;
	}

	if (!CheckFileToLoad(path, errorMessage)){
		SendErrorMessage(0, QStringLiteral("The file '%1' is not loaded: %2").arg(path, errorMessage));

		return OS_FAILED;
	}

	return m_slavePersistenceCompPtr->LoadFromFile(data, path, progressManagerPtr);
}


ifile::IFilePersistence::OperationState CPrivateFilePersistenceComp::SaveToFile(
			const istd::IChangeable& data,
			const QString& filePath,
			ibase::IProgressManager* progressManagerPtr) const
{
	QString path;
	QString errorMessage;
	const bool areAdministratorsTrusted = *m_trustAdministratorsAttrPtr;
	if (!GetPrivatePath(filePath, path, errorMessage) || !EnsurePrivateFolder(QFileInfo(path).absolutePath(), errorMessage, areAdministratorsTrusted)){
		SendErrorMessage(0, QStringLiteral("The data are not saved: %1").arg(errorMessage));

		return OS_FAILED;
	}

	// The slave writes into a new file of the owner next to the file: the file is replaced only by the complete data. The working file
	// keeps the extension, the serializers check it
	const QFileInfo fileInfo(path);
	QString workingPath = fileInfo.absolutePath() + QStringLiteral("/.") + fileInfo.completeBaseName() + QLatin1Char('.') +
				QUuid::createUuid().toString(QUuid::WithoutBraces);
	if (!fileInfo.suffix().isEmpty()){
		workingPath += QLatin1Char('.') + fileInfo.suffix();
	}

	if (!CreatePrivateFile(workingPath, areAdministratorsTrusted)){
		SendErrorMessage(0, QStringLiteral("The data are not saved: the file '%1' cannot be created for its owner only").arg(workingPath));

		return OS_FAILED;
	}

	// The new file is checked before the data are written into it: nobody else may have got any access to it
	if (!CheckFile(workingPath, errorMessage)){
		QFile::remove(workingPath);

		SendErrorMessage(0, QStringLiteral("The data are not saved: %1").arg(errorMessage));

		return OS_FAILED;
	}

	OperationState retVal = m_slavePersistenceCompPtr->SaveToFile(data, workingPath, progressManagerPtr);
	if (retVal == OS_OK){
		// The digest of the own contents is taken before the file is replaced: right after it another program can change the file
		const QByteArray digest = CheckFile(workingPath, errorMessage) ? CFileTools::GetFileDigest(workingPath) : QByteArray();
		if (digest.isEmpty()){
			if (errorMessage.isEmpty()){
				errorMessage = QStringLiteral("The file '%1' cannot be read").arg(workingPath);
			}

			retVal = OS_FAILED;
		}
		else if (!FlushFile(workingPath, errorMessage)){
			retVal = OS_FAILED;
		}
		else{
			// The replacement and its digest are published together: the digest of a concurrent save cannot be overwritten by an
			// older one
			QMutexLocker locker(&m_savedDigestsMutex);

			if (ReplacePrivateFile(workingPath, path, errorMessage)){
				m_savedDigests[path] = digest;
			}
			else{
				retVal = OS_FAILED;
			}
		}

		if (retVal == OS_OK){
			FlushFolder(fileInfo.absolutePath());
		}
		else{
			SendErrorMessage(0, QStringLiteral("The data are not saved: %1").arg(errorMessage));
		}
	}

	if (retVal != OS_OK){
		QFile::remove(workingPath);
	}

	return retVal;
}


// reimplemented (ifile::IFileTypeInfo)

bool CPrivateFilePersistenceComp::GetFileExtensions(QStringList& result, const istd::IChangeable* dataObjectPtr, int flags, bool doAppend) const
{
	return m_slavePersistenceCompPtr.IsValid() && m_slavePersistenceCompPtr->GetFileExtensions(result, dataObjectPtr, flags, doAppend);
}


QString CPrivateFilePersistenceComp::GetTypeDescription(const QString* extensionPtr) const
{
	return m_slavePersistenceCompPtr.IsValid() ? m_slavePersistenceCompPtr->GetTypeDescription(extensionPtr) : QString();
}


// private methods

bool CPrivateFilePersistenceComp::GetPrivatePath(const QString& filePath, QString& path, QString& errorMessage) const
{
	if (!m_slavePersistenceCompPtr.IsValid() || !m_filePathCompPtr.IsValid()){
		errorMessage = QStringLiteral("The persistence is not configured");

		return false;
	}

	const QString targetPath = m_filePathCompPtr->GetPath();
	if (targetPath.isEmpty()){
		errorMessage = QStringLiteral("The file has no path");

		return false;
	}

	// The private folder would be the folder above it, the file would be put next to it
	if (m_filePathCompPtr->GetPathType() == ifile::IFileNameParam::PT_DIRECTORY){
		errorMessage = QStringLiteral("'%1' is the path of a folder: the persistence needs the path of a file").arg(targetPath);

		return false;
	}

	path = QDir::cleanPath(QFileInfo(filePath.isEmpty() ? targetPath : filePath).absoluteFilePath());

	// Another folder (a temporary one) is not checked: the data never leave the private folder
	if (!IsSameFolder(QFileInfo(path).absolutePath(), QFileInfo(targetPath).absolutePath())){
		errorMessage = QStringLiteral("'%1' is outside of the private folder of '%2'").arg(path, targetPath);

		return false;
	}

	return true;
}


} // namespace imtfile


