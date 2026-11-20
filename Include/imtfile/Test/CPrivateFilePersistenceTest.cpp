// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CPrivateFilePersistenceTest.h"


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>
#include <QtCore/QTemporaryDir>
#include <QtCore/QUuid>

// ACF includes
#include <icomp/CComponentBase.h>
#include <icomp/TModelCompWrap.h>
#include <icomp/TSimComponentWrap.h>
#include <ifile/CFileNameParamComp.h>
#include <istd/CChangeNotifier.h>

// ImtCore includes
#include <imtfile/CPrivateAutoPersistenceComp.h>
#include <imtfile/CPrivateFilePersistenceComp.h>
#include <imtfile/CSystemConfigLocationComp.h>

#ifdef Q_OS_WIN
#include <windows.h>
#include <aclapi.h>
#include <sddl.h>
#include <vector>
#else
#include <QtCore/QProcess>
#include <sys/stat.h>
#ifdef Q_OS_MACOS
#include <sys/acl.h>
#endif
#endif


namespace
{


/**
	The data of the test: a text.
*/
class CTextData: virtual public istd::IChangeable
{
public:
	QByteArray text;
};


/**
	The text as the object of the automatic persistence.
*/
class CTextDataComp:
			public icomp::CComponentBase,
			public CTextData
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTextDataComp);
		I_REGISTER_INTERFACE(istd::IChangeable);
	I_END_COMPONENT;
};


/**
	Persistence of the text as a serializer of files does it: the file is opened for writing (truncated) and read whole, the loaded
	object tells its observers about the change.
*/
class CTextPersistenceComp:
			public icomp::CComponentBase,
			virtual public ifile::IFilePersistence
{
public:
	typedef icomp::CComponentBase BaseClass;

	I_BEGIN_COMPONENT(CTextPersistenceComp);
		I_REGISTER_INTERFACE(ifile::IFilePersistence);
	I_END_COMPONENT;

	/**
		The serializer rejects the data before it writes anything.
	*/
	bool isFailing = false;

	mutable int saveCount = 0;

	// reimplemented (ifile::IFilePersistence)
	bool IsOperationSupported(const istd::IChangeable* /*dataObjectPtr*/, const QString* /*filePathPtr*/, int /*flags*/, bool /*beQuiet*/) const override
	{
		return true;
	}

	OperationState LoadFromFile(istd::IChangeable& data, const QString& filePath, ibase::IProgressManager* /*progressManagerPtr*/) const override
	{
		QFile file(filePath);
		if (!file.open(QIODevice::ReadOnly)){
			return OS_FAILED;
		}

		istd::CChangeNotifier notifier(&data);
		dynamic_cast<CTextData&>(data).text = file.readAll();

		return OS_OK;
	}

	OperationState SaveToFile(const istd::IChangeable& data, const QString& filePath, ibase::IProgressManager* /*progressManagerPtr*/) const override
	{
		// The serializers of files check the extension
		if (isFailing || (QFileInfo(filePath).suffix() != QStringLiteral("xml"))){
			return OS_FAILED;
		}

		++saveCount;

		QFile file(filePath);
		if (!file.open(QIODevice::WriteOnly)){
			return OS_FAILED;
		}

		file.write(dynamic_cast<const CTextData&>(data).text);

		return OS_OK;
	}

	// reimplemented (ifile::IFileTypeInfo)
	bool GetFileExtensions(QStringList& result, const istd::IChangeable* /*dataObjectPtr*/, int /*flags*/, bool doAppend) const override
	{
		if (!doAppend){
			result.clear();
		}

		result << QStringLiteral("txt");

		return true;
	}

	QString GetTypeDescription(const QString* /*extensionPtr*/) const override
	{
		return QStringLiteral("Text");
	}
};


typedef icomp::TSimSharedComponentPtr<CTextPersistenceComp> TextPersistence;
typedef icomp::TSimSharedComponentPtr<ifile::CFileNameParamComp> FilePath;
typedef icomp::TSimComponentWrap<imtfile::CPrivateFilePersistenceComp> PrivatePersistence;


/**
	The private persistence of a file in a folder of the test.
*/
struct SPersistence
{
	explicit SPersistence(const QString& path)
	{
		slave->InitComponent();

		filePath->InitComponent();
		filePath->SetPath(path);

		persistence.SetRef("SlavePersistence", slave);
		persistence.SetRef("FilePath", filePath);
		persistence.InitComponent();
	}

	TextPersistence slave;
	FilePath filePath;
	PrivatePersistence persistence;
};


typedef icomp::TSimSharedComponentPtr<icomp::TModelCompWrap<CTextDataComp>> TextObject;
typedef icomp::TSimSharedComponentPtr<imtfile::CPrivateFilePersistenceComp> PrivatePersistencePtr;
typedef icomp::TSimComponentWrap<imtfile::CPrivateAutoPersistenceComp> PrivateAutoPersistence;


/**
	The text kept in a private file by the automatic persistence, as the settings of an application are.
*/
struct SAutoPersistence
{
	SAutoPersistence(const QString& path, bool autoReload, bool storeOnBegin = true)
	{
		slave->InitComponent();

		filePath->InitComponent();
		filePath->SetPath(path);

		loader->SetRef("SlavePersistence", slave);
		loader->SetRef("FilePath", filePath);
		loader->InitComponent();

		object->InitComponent();

		persistence.SetRef("Object", object);
		persistence.SetRef("FileLoader", loader);
		persistence.SetRef("FilePath", filePath);
		persistence.SetBoolAttr("StoreOnBegin", storeOnBegin);
		persistence.SetBoolAttr("AutoReload", autoReload);
		persistence.InitComponent();
	}

	void SetText(const QByteArray& text, const istd::IChangeable::ChangeSet& changeSet = istd::IChangeable::GetAnyChange())
	{
		istd::CChangeNotifier notifier(&*object, &changeSet);
		object->text = text;
	}

	TextPersistence slave;
	FilePath filePath;
	PrivatePersistencePtr loader;
	TextObject object;
	PrivateAutoPersistence persistence;
};


QByteArray ReadFile(const QString& path)
{
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)){
		return QByteArray();
	}

	return file.readAll();
}


/**
	The access of the other accounts to a file or a folder of the test.
*/
enum Access
{
	/**
		Unix: mode 700 (600 for a file); Windows: this account, the system and the administrators only, nothing inherited.
	*/
	A_OWNER_ONLY,

	/**
		The others can read (and open a folder), not change. Unix: mode 755 (644); Windows: Everyone may read and execute.
	*/
	A_READABLE_BY_OTHERS,

	/**
		A folder that the others can change, also delete and rename what is in it. Unix: mode 777; Windows: Everyone has full
		control.
	*/
	A_REPLACEABLE_BY_OTHERS,

	/**
		A folder of the owner only, but the new files in it are readable by the others. macOS: the inherited entry of the access
		list; Windows: the entry for the files only. Linux has no such access: the mode of a new file limits its access list.
	*/
	A_FILES_READABLE_BY_OTHERS
};


#ifdef Q_OS_WIN


/**
	The account of the process as SDDL.
*/
QString GetUserSidString()
{
	HANDLE token = nullptr;
	if (!::OpenProcessToken(::GetCurrentProcess(), TOKEN_QUERY, &token)){
		return QString();
	}

	QString retVal;

	DWORD size = 0;
	::GetTokenInformation(token, TokenUser, nullptr, 0, &size);
	std::vector<BYTE> tokenUser(size);
	LPWSTR sidStringPtr = nullptr;
	if (		(size > 0) &&
				::GetTokenInformation(token, TokenUser, tokenUser.data(), size, &size) &&
				::ConvertSidToStringSidW(reinterpret_cast<const TOKEN_USER*>(tokenUser.data())->User.Sid, &sidStringPtr)){
		retVal = QString::fromWCharArray(sidStringPtr);

		::LocalFree(sidStringPtr);
	}

	::CloseHandle(token);

	return retVal;
}


bool SetAccess(const QString& path, Access access)
{
	const QString userSid = GetUserSidString();
	if (userSid.isEmpty()){
		return false;
	}

	const bool isFolder = QFileInfo(path).isDir();
	const QString inheritance = isFolder ? QStringLiteral("OICI") : QString();

	// The protected list (P): nothing is inherited from the parent folder
	QString sddl = QStringLiteral("D:P(A;%1;FA;;;%2)(A;%1;FA;;;SY)(A;%1;FA;;;BA)").arg(inheritance, userSid);
	switch (access){
	case A_OWNER_ONLY:
		break;

	case A_READABLE_BY_OTHERS:
		sddl += QStringLiteral("(A;%1;FRFX;;;WD)").arg(inheritance);
		break;

	case A_REPLACEABLE_BY_OTHERS:
		sddl += QStringLiteral("(A;%1;FA;;;WD)").arg(inheritance);
		break;

	case A_FILES_READABLE_BY_OTHERS:
		sddl += QStringLiteral("(A;OIIO;FR;;;WD)");
		break;
	}

	PSECURITY_DESCRIPTOR descriptorPtr = nullptr;
	if (!::ConvertStringSecurityDescriptorToSecurityDescriptorW(reinterpret_cast<LPCWSTR>(sddl.utf16()), SDDL_REVISION_1, &descriptorPtr, nullptr)){
		return false;
	}

	BOOL isPresent = FALSE;
	BOOL isDefaulted = FALSE;
	PACL daclPtr = nullptr;
	const std::wstring nativePath = QDir::toNativeSeparators(path).toStdWString();
	const bool retVal =
				::GetSecurityDescriptorDacl(descriptorPtr, &isPresent, &daclPtr, &isDefaulted) &&
				(::SetNamedSecurityInfoW(
							const_cast<LPWSTR>(nativePath.c_str()),
							SE_FILE_OBJECT,
							DACL_SECURITY_INFORMATION | PROTECTED_DACL_SECURITY_INFORMATION,
							nullptr,
							nullptr,
							daclPtr,
							nullptr) == ERROR_SUCCESS);

	::LocalFree(descriptorPtr);

	return retVal;
}


/**
	Only this account, the system and the administrators have any access, also to the new files of a folder, and nothing is inherited
	from the parent folder.
*/
bool IsOwnerOnly(const QString& path)
{
	const QStringList privateSids = {GetUserSidString(), QStringLiteral("S-1-5-18"), QStringLiteral("S-1-5-32-544")};

	PACL daclPtr = nullptr;
	PSECURITY_DESCRIPTOR descriptorPtr = nullptr;
	const std::wstring nativePath = QDir::toNativeSeparators(path).toStdWString();
	if (::GetNamedSecurityInfoW(nativePath.c_str(), SE_FILE_OBJECT, DACL_SECURITY_INFORMATION, nullptr, nullptr, &daclPtr, nullptr, &descriptorPtr) != ERROR_SUCCESS){
		return false;
	}

	SECURITY_DESCRIPTOR_CONTROL control = 0;
	DWORD revision = 0;
	bool retVal =
				(daclPtr != nullptr) &&
				::GetSecurityDescriptorControl(descriptorPtr, &control, &revision) &&
				((control & SE_DACL_PROTECTED) != 0);

	for (DWORD index = 0; retVal && (index < daclPtr->AceCount); ++index){
		ACE_HEADER* headerPtr = nullptr;
		LPWSTR sidStringPtr = nullptr;
		retVal =
					::GetAce(daclPtr, index, reinterpret_cast<LPVOID*>(&headerPtr)) &&
					(headerPtr->AceType == ACCESS_ALLOWED_ACE_TYPE) &&
					::ConvertSidToStringSidW(&reinterpret_cast<ACCESS_ALLOWED_ACE*>(headerPtr)->SidStart, &sidStringPtr);
		if (retVal){
			retVal = privateSids.contains(QString::fromWCharArray(sidStringPtr));

			::LocalFree(sidStringPtr);
		}
	}

	::LocalFree(descriptorPtr);

	return retVal;
}


#else


bool SetAccess(const QString& path, Access access)
{
	const bool isFolder = QFileInfo(path).isDir();

	mode_t mode = isFolder ? 0700 : 0600;
	switch (access){
	case A_OWNER_ONLY:
	case A_FILES_READABLE_BY_OTHERS:
		break;

	case A_READABLE_BY_OTHERS:
		mode = isFolder ? 0755 : 0644;
		break;

	case A_REPLACEABLE_BY_OTHERS:
		mode = 0777;
		break;
	}

	if (::chmod(QFile::encodeName(path).constData(), mode) != 0){
		return false;
	}

#ifdef Q_OS_MACOS
	// The access list gives its rights before the mode: an empty one removes them
	acl_t acl = ::acl_init(0);
	const bool isAclRemoved = (acl != nullptr) && (::acl_set_file(QFile::encodeName(path).constData(), ACL_TYPE_EXTENDED, acl) == 0);
	::acl_free(acl);
	if (!isAclRemoved){
		return false;
	}
#endif

	if (access != A_FILES_READABLE_BY_OTHERS){
		return true;
	}

#ifdef Q_OS_MACOS
	return QProcess::execute(QStringLiteral("/bin/chmod"), {QStringLiteral("+a"), QStringLiteral("everyone allow read,file_inherit,only_inherit"), path}) == 0;
#else
	return false;
#endif
}


/**
	Only the owner has any access: by the mode and (macOS) by the access list.
*/
bool IsOwnerOnly(const QString& path)
{
	const QFileDevice::Permissions sharedPermissions =
				QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ExeGroup |
				QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther;
	if ((QFile::permissions(path) & sharedPermissions) != 0){
		return false;
	}

#ifdef Q_OS_MACOS
	acl_t acl = ::acl_get_file(QFile::encodeName(path).constData(), ACL_TYPE_EXTENDED);
	if (acl != nullptr){
		::acl_free(acl);

		return false;
	}
#endif

	return true;
}


#endif


bool WriteFile(const QString& path, const QByteArray& text)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly)){
		return false;
	}

	return file.write(text) == text.size();
}


/**
	Write a file that other accounts can read.
*/
bool WriteShared(const QString& path, const QByteArray& text)
{
	return WriteFile(path, text) && SetAccess(path, A_READABLE_BY_OTHERS);
}


bool MakeFolder(const QString& path, Access access)
{
	return QDir().mkpath(path) && SetAccess(path, access);
}


} // namespace


// The folder that does not exist is created for its owner only, the file is readable by its owner only
void CPrivateFilePersistenceTest::SaveCreatesPrivateFolderAndFileTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString folderPath = root.filePath(QStringLiteral("Company/Application"));
	const QString filePath = folderPath + QStringLiteral("/Secrets.xml");

	SPersistence persistence(filePath);
	CTextData data;
	data.text = "secret";
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);

	QVERIFY(IsOwnerOnly(folderPath));
	QVERIFY(IsOwnerOnly(filePath));

	CTextData loaded;
	QCOMPARE(persistence.persistence.LoadFromFile(loaded), ifile::IFilePersistence::OS_OK);
	QCOMPARE(loaded.text, QByteArray("secret"));
}


// A file that other users could read is replaced by a new file of the owner, not written in place
void CPrivateFilePersistenceTest::SaveReplacesSharedFileTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	QVERIFY(SetAccess(root.path(), A_OWNER_ONLY));
	const QString filePath = root.filePath(QStringLiteral("Secrets.xml"));
	QVERIFY(WriteShared(filePath, "old"));
	QVERIFY(!IsOwnerOnly(filePath));

	SPersistence persistence(filePath);
	CTextData data;
	data.text = "new";
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);

	QVERIFY(IsOwnerOnly(filePath));
	CTextData loaded;
	QCOMPARE(persistence.persistence.LoadFromFile(loaded), ifile::IFilePersistence::OS_OK);
	QCOMPARE(loaded.text, QByteArray("new"));
}


// The serializer rejects the data: the file keeps the data of the last save, nothing else is left in the folder
void CPrivateFilePersistenceTest::SaveKeepsFileOnFailureTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString folderPath = root.filePath(QStringLiteral("Private"));
	const QString filePath = folderPath + QStringLiteral("/Secrets.xml");

	SPersistence persistence(filePath);
	CTextData data;
	data.text = "old";
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);

	persistence.slave->isFailing = true;
	data.text = "new";
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_FAILED);

	QCOMPARE(ReadFile(filePath), QByteArray("old"));
	QCOMPARE(QDir(folderPath).entryList(QDir::Files | QDir::Hidden), QStringList{QStringLiteral("Secrets.xml")});
}


// A file in another folder (the temporary folder of ifile::CAutoPersistenceComp) is neither written nor read
void CPrivateFilePersistenceTest::OtherFolderIsRefusedTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString filePath = root.filePath(QStringLiteral("Private/Secrets.xml"));
	const QString otherPath = root.filePath(QStringLiteral("Secrets.xml"));

	SPersistence persistence(filePath);
	CTextData data;
	data.text = "secret";
	QCOMPARE(persistence.persistence.SaveToFile(data, otherPath), ifile::IFilePersistence::OS_FAILED);
	QVERIFY(!QFileInfo::exists(otherPath));

	QVERIFY(WriteShared(otherPath, "planted"));
	CTextData loaded;
	QCOMPARE(persistence.persistence.LoadFromFile(loaded, otherPath), ifile::IFilePersistence::OS_FAILED);
	QVERIFY(loaded.text.isEmpty());
}


// Another account could replace the private folder in a folder above it that everybody can change: nothing is saved there. The
// folder above that the others can only read is accepted
void CPrivateFilePersistenceTest::ReplaceableParentFolderIsRefusedTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString sharedPath = root.filePath(QStringLiteral("Shared"));
	QVERIFY(MakeFolder(sharedPath, A_REPLACEABLE_BY_OTHERS));

	SPersistence persistence(sharedPath + QStringLiteral("/Company/Private/Secrets.xml"));
	CTextData data;
	data.text = "secret";
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_FAILED);

	// The folders that exist are checked before the missing ones are created: nothing is left in a refused folder
	QVERIFY(!QFileInfo::exists(sharedPath + QStringLiteral("/Company")));

#ifndef Q_OS_WIN
	// With the sticky bit (as /tmp has) only the owner of an entry can delete or rename it
	QCOMPARE(::chmod(QFile::encodeName(sharedPath).constData(), 01777), 0);
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);
#endif

	QVERIFY(SetAccess(sharedPath, A_READABLE_BY_OTHERS));
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);
	QVERIFY(IsOwnerOnly(sharedPath + QStringLiteral("/Company/Private")));
	QVERIFY(IsOwnerOnly(sharedPath + QStringLiteral("/Company/Private/Secrets.xml")));
}


// A link to the private folder in a folder that everybody can change could be replaced by another account (with a link to its own
// folder): the folders are checked on the way to the link as well, not only on the way to the folder it points to
void CPrivateFilePersistenceTest::LinkInReplaceableFolderIsRefusedTest()
{
#ifdef Q_OS_WIN
	QSKIP("The links of Unix are tested");
#else
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString privatePath = root.filePath(QStringLiteral("Real/Private"));
	QVERIFY(MakeFolder(privatePath, A_OWNER_ONLY));
	const QString sharedPath = root.filePath(QStringLiteral("Shared"));
	QVERIFY(MakeFolder(sharedPath, A_REPLACEABLE_BY_OTHERS));
	const QString linkPath = sharedPath + QStringLiteral("/Link");
	QVERIFY(QFile::link(privatePath, linkPath));

	SPersistence persistence(linkPath + QStringLiteral("/Secrets.xml"));
	CTextData data;
	data.text = "secret";
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_FAILED);
	QVERIFY(QDir(privatePath).entryList(QDir::Files | QDir::Hidden).isEmpty());

	QVERIFY(WriteFile(privatePath + QStringLiteral("/Secrets.xml"), "secret"));
	QVERIFY(SetAccess(privatePath + QStringLiteral("/Secrets.xml"), A_OWNER_ONLY));
	CTextData loaded;
	QCOMPARE(persistence.persistence.LoadFromFile(loaded), ifile::IFilePersistence::OS_FAILED);
	QVERIFY(loaded.text.isEmpty());

	// With the sticky bit only the owner of the link can replace it
	QCOMPARE(::chmod(QFile::encodeName(sharedPath).constData(), 01777), 0);
	QCOMPARE(persistence.persistence.LoadFromFile(loaded), ifile::IFilePersistence::OS_OK);
	QCOMPARE(loaded.text, QByteArray("secret"));

	QVERIFY(SetAccess(sharedPath, A_READABLE_BY_OTHERS));
	data.text = "new";
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);
	QCOMPARE(ReadFile(privatePath + QStringLiteral("/Secrets.xml")), QByteArray("new"));
#endif
}


// The private folder gives the others the access to its new files (an inherited entry of its access list): the folder is refused
// before any file is created in it, the data are not written
void CPrivateFilePersistenceTest::InheritedAccessIsRefusedTest()
{
#if !defined(Q_OS_WIN) && !defined(Q_OS_MACOS)
	QSKIP("The mode of a new file limits its access list on Linux");
#else
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString folderPath = root.filePath(QStringLiteral("Private"));
	QVERIFY(MakeFolder(folderPath, A_FILES_READABLE_BY_OTHERS));

	SPersistence persistence(folderPath + QStringLiteral("/Secrets.xml"));
	CTextData data;
	data.text = "secret";
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_FAILED);
	QCOMPARE(persistence.slave->saveCount, 0);
	QVERIFY(QDir(folderPath).entryList(QDir::Files | QDir::Hidden).isEmpty());

	QVERIFY(SetAccess(folderPath, A_OWNER_ONLY));
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);
	QVERIFY(IsOwnerOnly(folderPath + QStringLiteral("/Secrets.xml")));
#endif
}


// A file that other users can read is not loaded: its secrets are not secret any more
void CPrivateFilePersistenceTest::LoadRefusesSharedFileTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	QVERIFY(SetAccess(root.path(), A_OWNER_ONLY));
	const QString filePath = root.filePath(QStringLiteral("Secrets.xml"));
	QVERIFY(WriteShared(filePath, "secret"));

	SPersistence persistence(filePath);
	CTextData loaded;
	QCOMPARE(persistence.persistence.LoadFromFile(loaded), ifile::IFilePersistence::OS_FAILED);
	QVERIFY(loaded.text.isEmpty());

	QVERIFY(SetAccess(filePath, A_OWNER_ONLY));
	QCOMPARE(persistence.persistence.LoadFromFile(loaded), ifile::IFilePersistence::OS_OK);
	QCOMPARE(loaded.text, QByteArray("secret"));
}


// A folder that other users can open is not trusted either: the file is neither loaded nor saved
void CPrivateFilePersistenceTest::LoadRefusesSharedFolderTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	QVERIFY(SetAccess(root.path(), A_READABLE_BY_OTHERS));
	const QString filePath = root.filePath(QStringLiteral("Secrets.xml"));
	QVERIFY(WriteFile(filePath, "secret"));
	QVERIFY(SetAccess(filePath, A_OWNER_ONLY));

	SPersistence persistence(filePath);
	CTextData loaded;
	QCOMPARE(persistence.persistence.LoadFromFile(loaded), ifile::IFilePersistence::OS_FAILED);
	QVERIFY(loaded.text.isEmpty());

	CTextData data;
	data.text = "new";
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_FAILED);
}


// The object is stored into the file itself: no copy in the temporary folder of the system, no working file left
void CPrivateFilePersistenceTest::AutoPersistenceStoresInPlaceTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString folderPath = root.filePath(QStringLiteral("Private"));
	const QString fileName = QStringLiteral("Secrets-%1.xml").arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
	const QString filePath = folderPath + QLatin1Char('/') + fileName;

	SAutoPersistence persistence(filePath, false);
	QVERIFY(QFileInfo::exists(filePath));
	QVERIFY(IsOwnerOnly(folderPath));
	QVERIFY(IsOwnerOnly(filePath));

	persistence.SetText("secret");
	QCOMPARE(ReadFile(filePath), QByteArray("secret"));
	QVERIFY(IsOwnerOnly(filePath));

	QVERIFY(!QFileInfo::exists(QDir::tempPath() + QLatin1Char('/') + fileName));
	QCOMPARE(QDir(folderPath).entryList(QDir::Files | QDir::Hidden), QStringList{fileName});
}


// Another program changes the file (it is replaced): the object takes the new data and does not store them back. The file replaced
// again is watched again
void CPrivateFilePersistenceTest::AutoPersistenceReloadTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString filePath = root.filePath(QStringLiteral("Private/Secrets.xml"));

	SPersistence otherProgram(filePath);
	CTextData data;
	data.text = "first";
	QCOMPARE(otherProgram.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);

	SAutoPersistence persistence(filePath, true);
	QCOMPARE(persistence.object->text, QByteArray("first"));
	QCOMPARE(persistence.slave->saveCount, 0);

	data.text = "second";
	QCOMPARE(otherProgram.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);
	QTRY_COMPARE(persistence.object->text, QByteArray("second"));

	data.text = "third";
	QCOMPARE(otherProgram.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);
	QTRY_COMPARE(persistence.object->text, QByteArray("third"));

	// The own save is not loaded again
	persistence.SetText("fourth");
	QCOMPARE(persistence.slave->saveCount, 1);
	QTest::qWait(200);
	QCOMPARE(persistence.object->text, QByteArray("fourth"));
	QCOMPARE(persistence.slave->saveCount, 1);
	QCOMPARE(ReadFile(filePath), QByteArray("fourth"));
}


// The change of the whole object (CF_ALL_DATA, as CopyFrom makes it) is stored as well. Only the change explicitly without undo is
// not stored
void CPrivateFilePersistenceTest::AutoPersistenceStoresWholeObjectChangeTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString filePath = root.filePath(QStringLiteral("Private/Secrets.xml"));

	SAutoPersistence persistence(filePath, false);

	persistence.SetText("whole", istd::IChangeable::GetAllChanges());
	QCOMPARE(ReadFile(filePath), QByteArray("whole"));

	persistence.SetText("temporary", istd::IChangeable::ChangeSet(istd::IChangeable::CF_NO_UNDO));
	QCOMPARE(ReadFile(filePath), QByteArray("whole"));
}


// The folder of the file does not exist on begin, the first change creates it: from now on the changes of another program are
// loaded
void CPrivateFilePersistenceTest::AutoPersistenceWatchesCreatedFolderTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString filePath = root.filePath(QStringLiteral("Private/Secrets.xml"));

	SAutoPersistence persistence(filePath, true, false);
	QVERIFY(!QFileInfo::exists(filePath));

	persistence.SetText("own");
	QCOMPARE(ReadFile(filePath), QByteArray("own"));

	SPersistence otherProgram(filePath);
	CTextData data;
	data.text = "other";
	QCOMPARE(otherProgram.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);
	QTRY_COMPARE(persistence.object->text, QByteArray("other"));
}


// The configuration folder of the application for the whole computer, not the one of the user and not a shared data folder
void CPrivateFilePersistenceTest::SystemConfigPathTest()
{
	const QString organizationName = QCoreApplication::organizationName();
	const QString applicationName = QCoreApplication::applicationName();
	QCoreApplication::setOrganizationName(QStringLiteral("TestCompany"));
	QCoreApplication::setApplicationName(QStringLiteral("Test Application"));

	const QString path = imtfile::CSystemConfigLocationComp::GetSystemConfigPath();
	const QString userConfigPath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
	const QString userDataPath = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));

	QCoreApplication::setOrganizationName(organizationName);
	QCoreApplication::setApplicationName(applicationName);

	QVERIFY(path != userConfigPath && path != userDataPath);
	QVERIFY(!path.contains(QStringLiteral("/Users/Public"), Qt::CaseInsensitive));
	QVERIFY(!path.contains(QStringLiteral("/Users/Shared")));
#if defined(Q_OS_WIN)
	// The folder is not always C:/ProgramData (another system drive, a moved folder): Windows tells it in the environment
	const QString programDataPath = QDir::cleanPath(QDir::fromNativeSeparators(qEnvironmentVariable("ProgramData")));
	QVERIFY(!programDataPath.isEmpty());
	QCOMPARE(path, programDataPath + QStringLiteral("/TestCompany/Test Application"));
#elif defined(Q_OS_MACOS)
	QCOMPARE(path, QStringLiteral("/Library/Application Support/TestCompany/Test Application"));
#else
	// The folder of services in kebab-case, the same in any environment of a session (XDG_CONFIG_DIRS is not used)
	qputenv("XDG_CONFIG_DIRS", "/etc/xdg/desktop:/etc/xdg");
	QCoreApplication::setOrganizationName(QStringLiteral("TestCompany"));
	QCoreApplication::setApplicationName(QStringLiteral(" Test  Application_Server "));
	const QString servicePath = imtfile::CSystemConfigLocationComp::GetSystemConfigPath();
	QCoreApplication::setOrganizationName(organizationName);
	QCoreApplication::setApplicationName(applicationName);
	qunsetenv("XDG_CONFIG_DIRS");

	QCOMPARE(path, QStringLiteral("/etc/testcompany/test-application"));
	QCOMPARE(servicePath, QStringLiteral("/etc/testcompany/test-application-server"));
#endif
}


I_ADD_TEST(CPrivateFilePersistenceTest);


