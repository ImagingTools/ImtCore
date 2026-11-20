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

#ifndef Q_OS_WIN
#include <sys/stat.h>
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
	SAutoPersistence(const QString& path, bool autoReload)
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
		persistence.SetBoolAttr("StoreOnBegin", true);
		persistence.SetBoolAttr("AutoReload", autoReload);
		persistence.InitComponent();
	}

	void SetText(const QByteArray& text)
	{
		istd::CChangeNotifier notifier(&*object);
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


#ifndef Q_OS_WIN
const QFileDevice::Permissions s_sharedPermissions =
			QFileDevice::ReadGroup | QFileDevice::WriteGroup | QFileDevice::ExeGroup |
			QFileDevice::ReadOther | QFileDevice::WriteOther | QFileDevice::ExeOther;
#endif


bool IsOwnerOnly(const QString& path)
{
#ifndef Q_OS_WIN
	return (QFile::permissions(path) & s_sharedPermissions) == 0;
#else
	return QFileInfo::exists(path);
#endif
}


bool WriteShared(const QString& path, const QByteArray& text)
{
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly)){
		return false;
	}

	file.write(text);
	file.close();

	return file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ReadGroup | QFileDevice::ReadOther);
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
#ifdef Q_OS_WIN
	QSKIP("The permissions of other users are the access list on Windows");
#endif
	QTemporaryDir root;
	QVERIFY(root.isValid());
	QVERIFY(QFile::setPermissions(root.path(), QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner));
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


// Another user could replace the private folder in a folder above it that everybody can write into: nothing is saved there.
// With the sticky bit (as /tmp has) only the owner of a folder can replace it
void CPrivateFilePersistenceTest::ReplaceableParentFolderIsRefusedTest()
{
#ifdef Q_OS_WIN
	QSKIP("The rights of other accounts are the access list on Windows");
#else
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString sharedPath = root.filePath(QStringLiteral("Shared"));
	QVERIFY(QDir().mkdir(sharedPath));
	const QByteArray nativeSharedPath = QFile::encodeName(sharedPath);
	QCOMPARE(::chmod(nativeSharedPath.constData(), 0777), 0);

	SPersistence persistence(sharedPath + QStringLiteral("/Company/Private/Secrets.xml"));
	CTextData data;
	data.text = "secret";
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_FAILED);

	// The folders that exist are checked before the missing ones are created: nothing is left in a refused folder
	QVERIFY(!QFileInfo::exists(sharedPath + QStringLiteral("/Company")));

	QCOMPARE(::chmod(nativeSharedPath.constData(), 01777), 0);
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);

	QCOMPARE(::chmod(nativeSharedPath.constData(), 0755), 0);
	QCOMPARE(persistence.persistence.SaveToFile(data), ifile::IFilePersistence::OS_OK);
#endif
}


// A file that other users can read is not loaded: its secrets are not secret any more
void CPrivateFilePersistenceTest::LoadRefusesSharedFileTest()
{
#ifdef Q_OS_WIN
	QSKIP("The permissions of other users are the access list on Windows");
#endif
	QTemporaryDir root;
	QVERIFY(root.isValid());
	QVERIFY(QFile::setPermissions(root.path(), QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner));
	const QString filePath = root.filePath(QStringLiteral("Secrets.xml"));
	QVERIFY(WriteShared(filePath, "secret"));

	SPersistence persistence(filePath);
	CTextData loaded;
	QCOMPARE(persistence.persistence.LoadFromFile(loaded), ifile::IFilePersistence::OS_FAILED);
	QVERIFY(loaded.text.isEmpty());

	QVERIFY(QFile::setPermissions(filePath, QFileDevice::ReadOwner | QFileDevice::WriteOwner));
	QCOMPARE(persistence.persistence.LoadFromFile(loaded), ifile::IFilePersistence::OS_OK);
	QCOMPARE(loaded.text, QByteArray("secret"));
}


// A folder that other users can open is not trusted either: the file is neither loaded nor saved
void CPrivateFilePersistenceTest::LoadRefusesSharedFolderTest()
{
#ifdef Q_OS_WIN
	QSKIP("The permissions of other users are the access list on Windows");
#endif
	QTemporaryDir root;
	QVERIFY(root.isValid());
	QVERIFY(QFile::setPermissions(root.path(), QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
				QFileDevice::ReadOther | QFileDevice::ExeOther));
	const QString filePath = root.filePath(QStringLiteral("Secrets.xml"));
	QVERIFY(WriteShared(filePath, "secret"));
	QVERIFY(QFile::setPermissions(filePath, QFileDevice::ReadOwner | QFileDevice::WriteOwner));

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


