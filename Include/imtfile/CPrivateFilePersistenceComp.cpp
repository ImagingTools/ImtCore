// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtfile/CPrivateFilePersistenceComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QUuid>

#ifdef Q_OS_WIN
#include <windows.h>
#include <aclapi.h>
#include <sddl.h>
#include <vector>
#else
#include <cstdio>
#include <sys/stat.h>
#include <unistd.h>
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

		// The entries for the contained files and folders only, and the denials, do not give any access to this one
		if (((headerPtr->AceFlags & INHERIT_ONLY_ACE) != 0) || (headerPtr->AceType == ACCESS_DENIED_ACE_TYPE)){
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
	Create the folder with the access of the account of the process, the system and the administrators only: the access of the
	parent folder is not inherited.
*/
bool CreatePrivateFolder(const QString& folderPath)
{
	const std::vector<SidBuffer> sids = GetPrivateSids();

	std::vector<EXPLICIT_ACCESSW> entries;
	for (const SidBuffer& sid : sids){
		if (sid.empty()){
			return false;
		}

		EXPLICIT_ACCESSW entry = {};
		entry.grfAccessPermissions = FILE_ALL_ACCESS;
		entry.grfAccessMode = SET_ACCESS;
		entry.grfInheritance = SUB_CONTAINERS_AND_OBJECTS_INHERIT;
		entry.Trustee.TrusteeForm = TRUSTEE_IS_SID;
		entry.Trustee.TrusteeType = TRUSTEE_IS_UNKNOWN;
		entry.Trustee.ptstrName = reinterpret_cast<LPWSTR>(const_cast<BYTE*>(sid.data()));
		entries.push_back(entry);
	}

	PACL aclPtr = nullptr;
	if (::SetEntriesInAclW(ULONG(entries.size()), entries.data(), nullptr, &aclPtr) != ERROR_SUCCESS){
		return false;
	}

	SECURITY_DESCRIPTOR descriptor;
	bool retVal =
				::InitializeSecurityDescriptor(&descriptor, SECURITY_DESCRIPTOR_REVISION) &&
				::SetSecurityDescriptorDacl(&descriptor, TRUE, aclPtr, FALSE) &&
				::SetSecurityDescriptorControl(&descriptor, SE_DACL_PROTECTED, SE_DACL_PROTECTED);
	if (retVal){
		SECURITY_ATTRIBUTES attributes = {sizeof(SECURITY_ATTRIBUTES), &descriptor, FALSE};
		const std::wstring nativePath = QDir::toNativeSeparators(folderPath).toStdWString();
		retVal = ::CreateDirectoryW(nativePath.c_str(), &attributes) != 0;
	}

	::LocalFree(aclPtr);

	return retVal;
}


bool CheckFolderChain(const QString& folderPath, bool isPrivateFolder, QString& errorMessage)
{
	const QStringList chain = GetFolderChain(folderPath);
	for (int index = 0; index < chain.count(); ++index){
		if (!CheckSecurity(chain[index], isPrivateFolder && (index == chain.count() - 1), errorMessage)){
			return false;
		}
	}

	return true;
}


bool CheckFile(const QString& filePath, QString& errorMessage)
{
	return CheckSecurity(filePath, true, errorMessage);
}


/**
	Create a new empty file: it takes the access of its private folder.
*/
bool CreatePrivateFile(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::WriteOnly | QIODevice::NewOnly)){
		return false;
	}

	file.close();

	return true;
}


bool ReplaceFile(const QString& sourcePath, const QString& targetPath)
{
	const std::wstring nativeSourcePath = QDir::toNativeSeparators(sourcePath).toStdWString();
	const std::wstring nativeTargetPath = QDir::toNativeSeparators(targetPath).toStdWString();

	return ::MoveFileExW(nativeSourcePath.c_str(), nativeTargetPath.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
}


#else


bool IsTrustedOwner(uid_t ownerId)
{
	return (ownerId == 0) || (ownerId == ::geteuid());
}


/**
	On the way to the folder no other user can replace a folder or a link: the folders belong to this user or to the system, and the
	others cannot write into them or only with the sticky bit (/tmp). A private folder at the end belongs to this user and nobody else
	can open it.
*/
bool CheckFolderChain(const QString& folderPath, bool isPrivateFolder, QString& errorMessage)
{
	for (const QString& path : GetFolderChain(folderPath)){
		struct stat info;
		if (::lstat(QFile::encodeName(path).constData(), &info) != 0){
			errorMessage = QStringLiteral("The folder '%1' cannot be checked").arg(path);

			return false;
		}

		if (S_ISLNK(info.st_mode) && !IsTrustedOwner(info.st_uid)){
			errorMessage = QStringLiteral("The link '%1' belongs to another user").arg(path);

			return false;
		}
	}

	const QString canonicalPath = QFileInfo(folderPath).canonicalFilePath();
	if (canonicalPath.isEmpty()){
		errorMessage = QStringLiteral("The folder '%1' cannot be checked").arg(folderPath);

		return false;
	}

	const QStringList chain = GetFolderChain(canonicalPath);
	for (int index = 0; index < chain.count(); ++index){
		struct stat info;
		if ((::stat(QFile::encodeName(chain[index]).constData(), &info) != 0) || !S_ISDIR(info.st_mode)){
			errorMessage = QStringLiteral("'%1' is not a folder").arg(chain[index]);

			return false;
		}

		if (isPrivateFolder && (index == chain.count() - 1)){
			if (info.st_uid != ::geteuid()){
				errorMessage = QStringLiteral("The folder '%1' belongs to another user").arg(chain[index]);

				return false;
			}

			if ((info.st_mode & (S_IRWXG | S_IRWXO)) != 0){
				errorMessage = QStringLiteral("Other users can open the folder '%1': make it accessible to its owner only (chmod 700)").arg(chain[index]);

				return false;
			}
		}
		else{
			const bool isWritableByOthers = (info.st_mode & (S_IWGRP | S_IWOTH)) != 0;
			if (!IsTrustedOwner(info.st_uid) || (isWritableByOthers && ((info.st_mode & S_ISVTX) == 0))){
				errorMessage = QStringLiteral("Other users can replace what is in the folder '%1'").arg(chain[index]);

				return false;
			}
		}
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

	if ((info.st_mode & (S_IRWXG | S_IRWXO)) != 0){
		errorMessage = QStringLiteral("Other users can read the file '%1': make it readable by its owner only (chmod 600)").arg(filePath);

		return false;
	}

	return true;
}


/**
	Create a new empty file that only its owner can read and write. A file or a link with this name is not opened.
*/
bool CreatePrivateFile(const QString& filePath)
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


bool ReplaceFile(const QString& sourcePath, const QString& targetPath)
{
	return ::rename(QFile::encodeName(sourcePath).constData(), QFile::encodeName(targetPath).constData()) == 0;
}


#endif


/**
	Create the missing folders above the private folder: on Unix they are not writable by other users, whatever the umask is.
*/
bool CreateParentFolders(const QString& folderPath)
{
#if !defined(Q_OS_WIN) && (QT_VERSION >= QT_VERSION_CHECK(6, 3, 0))
	const QFileDevice::Permissions parentPermissions =
				QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
				QFileDevice::ReadGroup | QFileDevice::ExeGroup |
				QFileDevice::ReadOther | QFileDevice::ExeOther;

	for (const QString& path : GetFolderChain(folderPath)){
		if (!QFileInfo::exists(path) && !QDir().mkdir(path, parentPermissions)){
			return false;
		}
	}

	return true;
#else
	return QDir().mkpath(folderPath);
#endif
}


bool CreateFolder(const QString& folderPath)
{
#if defined(Q_OS_WIN)
	return CreatePrivateFolder(folderPath);
#elif QT_VERSION >= QT_VERSION_CHECK(6, 3, 0)
	return QDir().mkdir(folderPath, QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);
#else
	return QDir().mkdir(folderPath) && QFile::setPermissions(folderPath, QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);
#endif
}


} // namespace


namespace imtfile
{


// public static methods

bool CPrivateFilePersistenceComp::EnsurePrivateFolder(const QString& folderPath, QString& errorMessage)
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

		if (!CreateFolder(absolutePath)){
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


// reimplemented (ifile::IFilePersistence)

bool CPrivateFilePersistenceComp::IsOperationSupported(
			const istd::IChangeable* dataObjectPtr,
			const QString* filePathPtr,
			int flags,
			bool beQuiet) const
{
	return m_slavePersistenceCompPtr.IsValid() && m_slavePersistenceCompPtr->IsOperationSupported(dataObjectPtr, filePathPtr, flags, beQuiet);
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

	if (QFileInfo::exists(path) && (!CheckFolderChain(QFileInfo(path).absolutePath(), true, errorMessage) || !CheckFile(path, errorMessage))){
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
	if (!GetPrivatePath(filePath, path, errorMessage) || !EnsurePrivateFolder(QFileInfo(path).absolutePath(), errorMessage)){
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

	if (!CreatePrivateFile(workingPath)){
		SendErrorMessage(0, QStringLiteral("The data are not saved: the file '%1' cannot be created for its owner only").arg(workingPath));

		return OS_FAILED;
	}

	OperationState retVal = m_slavePersistenceCompPtr->SaveToFile(data, workingPath, progressManagerPtr);
	if (retVal == OS_OK){
		if (!CheckFile(workingPath, errorMessage)){
			retVal = OS_FAILED;
		}
		else if (!ReplaceFile(workingPath, path)){
			errorMessage = QStringLiteral("The file '%1' cannot be replaced").arg(path);

			retVal = OS_FAILED;
		}

		if (retVal != OS_OK){
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

	path = QDir::cleanPath(QFileInfo(filePath.isEmpty() ? targetPath : filePath).absoluteFilePath());

	// Another folder (a temporary one) is not checked: the data never leave the private folder
	if (!IsSameFolder(QFileInfo(path).absolutePath(), QFileInfo(targetPath).absolutePath())){
		errorMessage = QStringLiteral("'%1' is outside of the private folder of '%2'").arg(path, targetPath);

		return false;
	}

	return true;
}


} // namespace imtfile


