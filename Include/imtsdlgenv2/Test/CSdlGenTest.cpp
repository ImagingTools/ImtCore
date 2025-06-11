#include "CSdlGenTest.h"


// Qt includes
#include <QtTest/QTest>
#include <QtCore/QFile>

// ACF includes
#include <QtTest/qtestcase.h>
#include <itest/CStandardTestExecutor.h>
#include <iprm/IOptionsManager.h>
#include <iproc/IProcessor.h>

// generated includes
#include <GeneratedFiles/ImtSdlGenTest/CImtSdlGenTest.h>


namespace imtsdlgentest
{

// static variables
static const QString s_testDataDirectoryPath = qEnvironmentVariable("IMTCOREDIR")+"/Include/imtsdlgenv2/Test/TestData";
static const QString s_testReferenceDataDirectoryPath = s_testDataDirectoryPath + "/ReferenceData";


// static helpers

static bool CompareFilesContent(const QString& filePath1, const QString& filePath2) {
	QFile file1(filePath1);
	QFile file2(filePath2);
	if (!file1.open(QIODevice::ReadOnly) || !file2.open(QIODevice::ReadOnly)){
		return false;
	}

	return file1.readAll() == file2.readAll();
}


static bool CompareDirectories(const QString& dir1, const QString& dir2) {
	QDir directory1(dir1);
	QDir directory2(dir2);

	const QStringList files1 = directory1.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
	for (const QString &entry : files1) {
		const QString path1 = directory1.absoluteFilePath(entry);
		const QString path2 = directory2.absoluteFilePath(entry);

		if (QFileInfo(path1).isDir()) {
			bool isSubDirsEqual = CompareDirectories(path1, path2);
			if (!isSubDirsEqual){
				return false;
			}
		}
		else {
			if (!QFileInfo::exists(path2)) {
				qWarning() << path2 << "doesn't exists";

				return false;
			}
			if (!CompareFilesContent(path1, path2)) {
				qWarning() << path1 << "and" << path2 << "are have not equal data";

				return false;
			}
		}
	}

	return true;
}



void GetPrecessorAndExec(CImtSdlGenTest& testSuite, const QByteArray& processorName)
{
	iproc::IProcessor* processorPtr = testSuite.GetInterface<iproc::IProcessor>(processorName);
	QCOMPARE_NE(processorPtr, nullptr);

	iproc::IProcessor::TaskState execResult = processorPtr->DoProcessing(nullptr, nullptr, nullptr);
	QCOMPARE_EQ(execResult, iproc::IProcessor::TS_OK);
}


void PrepareSuite(CImtSdlGenTest& testSuite)
{
	testSuite.EnsureAutoInitComponentsCreated();

	imtsdl::ISdlEditableProcessArgumentsParser* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	QCOMPARE_NE(argParserPtr, nullptr);

	argParserPtr->SetAutoJoinEnabled();
	argParserPtr->SetAutoLinkLevel(imtsdl::ISdlProcessArgumentsParser::ALL_ONLY_FILE);
	argParserPtr->SetGenerateModeEnabled();
	argParserPtr->SetModuleGenerateEnabled(false);
}


void ExecuteTest(CImtSdlGenTest& testSuite, const QString& schemaFileName, const QString& referenceDataDirName = QString(), QList<std::shared_ptr<QTemporaryDir>>* tempDirPtrList = nullptr)
{
	QCOMPARE_NE(schemaFileName.length(), 0);
	QVERIFY(QFileInfo::exists(s_testDataDirectoryPath + '/' + schemaFileName));

	std::shared_ptr<QTemporaryDir> tempOutputDir(new QTemporaryDir("SdlGenTest"));
	QVERIFY(tempOutputDir->isValid());
	QVERIFY(QDir(tempOutputDir->path()).exists());
	QString outDirPath = QDir(tempOutputDir->path()).absolutePath();

	imtsdl::ISdlEditableProcessArgumentsParser* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	QCOMPARE_NE(argParserPtr, nullptr);

	if (tempDirPtrList != nullptr){
		QStringList includePaths;
		tempDirPtrList->append(tempOutputDir);

		for (const std::shared_ptr<QTemporaryDir>& dirPtr: std::as_const(*tempDirPtrList)){
			QFileInfo tempDirPathInfo = QFileInfo(tempOutputDir->path());
			QVERIFY(tempDirPathInfo.exists());
			QVERIFY(tempDirPathInfo.isDir());
			includePaths << tempDirPathInfo.absoluteFilePath();
		}
		argParserPtr->SetHeadersIncludePaths(includePaths);
	}

	argParserPtr->SetOutputDirectoryPath(outDirPath);
	argParserPtr->SetSchemaFilePath(s_testDataDirectoryPath + '/' + schemaFileName);

	// first parse the schema
	GetPrecessorAndExec(testSuite, "GqlSchemaParser");

	// do processing
	// CXX
	GetPrecessorAndExec(testSuite, "SdlEnumGenerator");
	GetPrecessorAndExec(testSuite, "SdlUnionGenerator");
	GetPrecessorAndExec(testSuite, "SdlClassCodeGenerator");
	GetPrecessorAndExec(testSuite, "GqlWrapClassCodeGenerator");
	GetPrecessorAndExec(testSuite, "GqlHandlerBaseClassGenerator");
	GetPrecessorAndExec(testSuite, "GqlCollectionControllerBaseClassGenerator");
	// QML
	GetPrecessorAndExec(testSuite, "QmlCodeGenerator");
	GetPrecessorAndExec(testSuite, "QmlCodeMetaGenerator");
	GetPrecessorAndExec(testSuite, "QmlCodeCommandIdGenerator");

	if (!referenceDataDirName.isEmpty()){
		const QString referenceDataAbsolutePath = s_testReferenceDataDirectoryPath + '/' + referenceDataDirName;
		bool isDataEqual = CompareDirectories(outDirPath, referenceDataAbsolutePath);
		if (!isDataEqual){
			qWarning() << outDirPath << "and" << referenceDataAbsolutePath << "are not equal";
		}
		QVERIFY(isDataEqual);
	}
}


// prevate slots (i.e. test cases)

void CSdlGenTest::initTestCase()
{
#if 0 ///\todo inspect it and don't forget to fix partutura
	CImtSdlGenTest testSuite;
	iprm::IOptionsManager* dependentSchemaList = testSuite.GetInterface<iprm::IOptionsManager>("DependentSchemaList");
	QCOMPARE_NE(dependentSchemaList, nullptr);
	qDebug() << dependentSchemaList->GetOptionsCount();
	qDebug() << __func__ << "end";
#endif
}


void CSdlGenTest::TestBasicSchema()
{
	QSKIP("[BASIC] The test will be disabled until the generator code is synchronized.");
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite);

	imtsdl::ISdlEditableProcessArgumentsParser* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled(false);
	argParserPtr->SetQmlEnabled(false);
	ExecuteTest(testSuite, "ImtBaseTypes.sdl", "BasicSchemaTest", &m_tempDirPtrList);
}


void CSdlGenTest::TestComplexCollectionFilter()
{
	QSKIP("[COMPLEX] The test will be disabled until the generator code is synchronized.");
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite);

	imtsdl::ISdlEditableProcessArgumentsParser* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled();
	ExecuteTest(testSuite, "ComplexCollectionFilter.sdl", "ComplexCollectionFilterTest", &m_tempDirPtrList);
}


void CSdlGenTest::TestUnion()
{
	QSKIP("[ENUM] The test will be disabled until the generator code is synchronized.");
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite);

	imtsdl::ISdlEditableProcessArgumentsParser* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled(false);
	argParserPtr->SetQmlEnabled();

	ExecuteTest(testSuite, "UnionTest.sdl", "UnionTest", &m_tempDirPtrList);
}


void CSdlGenTest::cleanupTestCase()
{
	for (std::shared_ptr<QTemporaryDir> tempDirPtr: std::as_const(m_tempDirPtrList)){
		tempDirPtr->setAutoRemove(true);
	}
}


// add tests

I_ADD_TEST(CSdlGenTest);


} //namespace imtsdlgentest
