// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CPrivateFilePersistenceTest.h"


// Qt includes
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QStandardPaths>
#include <QtCore/QTemporaryDir>

// ACF includes
#include <icomp/CComponentBase.h>
#include <icomp/TSimComponentWrap.h>
#include <ifile/CFileNameParamComp.h>

// ImtCore includes
#include <imtfile/CPrivateFilePersistenceComp.h>
#include <imtfile/CSystemConfigLocationComp.h>


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
	Persistence of the text as a serializer of files does it: the file is opened for writing (truncated) and read whole.
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

		dynamic_cast<CTextData&>(data).text = file.readAll();

		return OS_OK;
	}

	OperationState SaveToFile(const istd::IChangeable& data, const QString& filePath, ibase::IProgressManager* /*progressManagerPtr*/) const override
	{
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


// The data are written to a temporary file first (as the automatic persistence does): it is readable by its owner only too
void CPrivateFilePersistenceTest::SaveToTemporaryFileTest()
{
	QTemporaryDir root;
	QVERIFY(root.isValid());
	const QString filePath = root.filePath(QStringLiteral("Private/Secrets.xml"));
	const QString temporaryPath = root.filePath(QStringLiteral("Secrets.xml"));
	QVERIFY(WriteShared(temporaryPath, "left from before"));

	SPersistence persistence(filePath);
	CTextData data;
	data.text = "secret";
	QCOMPARE(persistence.persistence.SaveToFile(data, temporaryPath), ifile::IFilePersistence::OS_OK);

	QVERIFY(IsOwnerOnly(temporaryPath));
	QVERIFY(IsOwnerOnly(root.filePath(QStringLiteral("Private"))));

	QFile file(temporaryPath);
	QVERIFY(file.open(QIODevice::ReadOnly));
	QCOMPARE(file.readAll(), QByteArray("secret"));
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


