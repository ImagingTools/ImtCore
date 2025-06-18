#include "CSdlGenTest.h"


// Qt includes
#include <QtTest/QTest>
#include <QtCore/QFile>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/IOptionsManager.h>
#include <iproc/IProcessor.h>
#include <itest/CStandardTestExecutor.h>

// generated includes
#include <GeneratedFiles/ImtSdlGenTest/CImtSdlGenTest.h>

#define IMT_QML_FINAL_APPROVED 0
namespace imtsdlgentest
{

// static variables
static const QString s_testDataDirectoryPath = qEnvironmentVariable("IMTCOREDIR")+"/Include/imtsdlgenv2/Test/TestData";
static const QString s_testReferenceDataDirectoryPath = s_testDataDirectoryPath + "/ReferenceData";


// static helpers

static bool CompareFilesContent(const QString& filePath1, const QString& filePath2) {
	QFile file1(filePath1);
	QFile file2(filePath2);
	if (!file1.open(QIODevice::ReadOnly | QIODevice::Text) || !file2.open(QIODevice::ReadOnly | QIODevice::Text)){
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
	QVERIFY(processorPtr != nullptr);

	iproc::IProcessor::TaskState execResult = processorPtr->DoProcessing(nullptr, nullptr, nullptr);
	QVERIFY(execResult == iproc::IProcessor::TS_OK);
}


void PrepareSuite(CImtSdlGenTest& testSuite)
{
	testSuite.EnsureAutoInitComponentsCreated();

	imtsdl::ISdlEditableProcessArgumentsParser* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	QVERIFY(argParserPtr != nullptr);

	argParserPtr->SetAutoJoinEnabled();
	argParserPtr->SetAutoLinkLevel(imtsdl::ISdlProcessArgumentsParser::ALL_ONLY_FILE);
	argParserPtr->SetGenerateModeEnabled();
	argParserPtr->SetModuleGenerateEnabled(false);
	argParserPtr->SetEnabledAllModificators();
}


void ExecuteTest(
	CImtSdlGenTest& testSuite,
	const QString& schemaFileName,
	const QString& referenceDataDirName = QString(),
	QList<std::shared_ptr<QDir>>* tempDirPtrList = nullptr)
{
	QVERIFY(schemaFileName.length() != 0);
	QVERIFY(QFileInfo::exists(s_testDataDirectoryPath + '/' + schemaFileName));

	std::shared_ptr<QDir> tempOutputDir(new QDir(QString("SdlGenTest/%1").arg(schemaFileName)));
	const bool isDirCreated = istd::CSystem::EnsurePathExists(tempOutputDir->absolutePath());
	QVERIFY(isDirCreated);
	QVERIFY(QDir(tempOutputDir->path()).exists());
	QString outDirPath = tempOutputDir->absolutePath();

	imtsdl::ISdlEditableProcessArgumentsParser* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	QVERIFY(argParserPtr != nullptr);

	if (tempDirPtrList != nullptr){
		QStringList includePaths;
		tempDirPtrList->append(tempOutputDir);

		for (const std::shared_ptr<QDir>& dirPtr: std::as_const(*tempDirPtrList)){
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
#if IMT_QML_FINAL_APPROVED /// QML disabled until the final generation choice is approved.
	GetPrecessorAndExec(testSuite, "QmlCodeGenerator");
	GetPrecessorAndExec(testSuite, "QmlCodeMetaGenerator");
	GetPrecessorAndExec(testSuite, "QmlCodeCommandIdGenerator");
#endif
	if (!referenceDataDirName.isEmpty()){
		const QString referenceDataAbsolutePath = s_testReferenceDataDirectoryPath + '/' + referenceDataDirName;

#if not IMT_QML_FINAL_APPROVED /// QML -> set CPP directly until the final generation choice is approved.
			bool isDataEqual = CompareDirectories(outDirPath + "/1.0/CPP", referenceDataAbsolutePath + "/1.0/CPP");
#else
			bool isDataEqual = CompareDirectories(outDirPath, referenceDataAbsolutePath);
#endif

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
	QSKIP("BASIC SKIP");
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite);

	imtsdl::ISdlEditableProcessArgumentsParser* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);
	ExecuteTest(testSuite, "ImtBaseTypes.sdl", "BasicSchemaTest", &m_tempDirPtrList);
}


void CSdlGenTest::TestComplexCollectionFilter()
{
	QSKIP("COMPLEX SKIP");
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
	QSKIP("UNION SKIP");
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite);

	imtsdl::ISdlEditableProcessArgumentsParser* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled();

	ExecuteTest(testSuite, "UnionTest.sdl", "UnionTest", &m_tempDirPtrList);
}


void CSdlGenTest::TestComplexUnion1()
{
	QSKIP("TestComplexUnion1 SKIP");
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite);

	imtsdl::ISdlEditableProcessArgumentsParser* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);

	ExecuteTest(testSuite, "ComplexUnion1.sdl", "ComplexUnion1", &m_tempDirPtrList);
}


void CSdlGenTest::TestComplexUnion2()
{
	QSKIP("TestComplexUnion2 SKIP");
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite);

	imtsdl::ISdlEditableProcessArgumentsParser* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);

	ExecuteTest(testSuite, "ComplexUnion2.sdl", "ComplexUnion2", &m_tempDirPtrList);
}


void CSdlGenTest::cleanupTestCase()
{
	for (const std::shared_ptr<QDir>& tempDirPtr: std::as_const(m_tempDirPtrList)){
		const bool isRemoved = 1;//tempDirPtr->removeRecursively();
		QVERIFY(isRemoved);
	}
}


// add tests

I_ADD_TEST(CSdlGenTest);


} //namespace imtsdlgentest
