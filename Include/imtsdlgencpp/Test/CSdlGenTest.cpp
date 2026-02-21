// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSdlGenTest.h"


// Qt includes
#include <QtTest/QTest>
#include <QtCore/QFile>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/IOptionsManager.h>
#include <iproc/IProcessor.h>
#include <itest/CStandardTestExecutor.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <GeneratedFiles/ImtSdlGenTest/CImtSdlGenTest.h>


namespace imtsdlgentest
{

// static variables
static const QString s_configFilePath = qEnvironmentVariable("IMTCOREDIR") + "/Config/SDL/CommonCXX.cfg";
static const QString s_testDataDirectoryPath = qEnvironmentVariable("IMTCOREDIR") + "/Include/imtsdlgencpp/Test/TestData";
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

	QStringList files1 = directory1.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
	QStringList files2 = directory2.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
	if (files1 != files2){
		QListIterator dirIter1(files1);
		while(dirIter1.hasNext()){
			files2.removeAll(dirIter1.next());
		}

		QListIterator dir2Iter(files2);
		while(dir2Iter.hasNext()){
			files1.removeAll(dir2Iter.next());
		}

		if (!files1.isEmpty()){
			qWarning() << directory2.absolutePath() << "Doesn't contains expected files: " << files1.join(" | ");
		}

		if (!files2.isEmpty()){
			qWarning() << directory1.absolutePath() << "Doesn't contains expected files: " << files2.join(" | ");
		}

		return false;
	}

	for (const QString &entry : std::as_const(files1)){
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


void GetPrecessorAndExec(CImtSdlGenTest& testSuite, const QByteArray& processorName, iprm::IParamsSet* processorParamsPtr = nullptr)
{
	auto* processorPtr = testSuite.GetInterface<iproc::IProcessor>(processorName);
	QCOMPARE_NE(processorPtr, nullptr);

	iproc::IProcessor::TaskState execResult = processorPtr->DoProcessing(processorParamsPtr, nullptr, nullptr);
	QCOMPARE(execResult, iproc::IProcessor::TS_OK);
}


void PrepareSuite(CImtSdlGenTest& testSuite, const QDir& outputDir)
{
	testSuite.EnsureAutoInitComponentsCreated();

	auto* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	QVERIFY(argParserPtr != nullptr);


	const bool isDirCreated = istd::CSystem::EnsurePathExists(outputDir.absolutePath());
	QVERIFY(isDirCreated);
	QVERIFY(QDir(outputDir.path()).exists());
	QString outDirPath = outputDir.absolutePath();
	argParserPtr->SetOutputDirectoryPath(outDirPath);

	const bool settingsIsValid = argParserPtr->ReadFromSettings(s_configFilePath);
	QVERIFY(settingsIsValid);

	argParserPtr->SetGenerateModeEnabled();
	argParserPtr->SetEnabledAllModificators();
}


void ExecuteTest(
	CImtSdlGenTest& testSuite,
	const QString& schemaFileName,
	const QString& referenceDataDirName = QString(),
	const QString& outputDirName = QString())
{
	QCOMPARE_NE(schemaFileName.length(), 0);
	QVERIFY(QFileInfo::exists(s_testDataDirectoryPath + '/' + schemaFileName));


	auto* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	QCOMPARE_NE(argParserPtr, nullptr);

	argParserPtr->SetSchemaFilePath(s_testDataDirectoryPath + '/' + schemaFileName);

	// set a temp dir for all processors
	QTemporaryDir tempOutputDir;
	const QString tempOutputDirPath = tempOutputDir.path();
	if (!istd::CSystem::EnsurePathExists(tempOutputDirPath)){
		qCritical() <<  QString("Unable to create temp dir at '%1'").arg(tempOutputDirPath);

		return;
	}

	// first parse the schema
	GetPrecessorAndExec(testSuite, "GqlSchemaParser");

	// do processing
	GetPrecessorAndExec(testSuite, "CppCodeGeneratorFramework");
	if (!referenceDataDirName.isEmpty() && !outputDirName.isEmpty()){
		/// compare only CPP dirs \todo remove it when QML will be acceptable
		const QString referenceDataAbsolutePath = s_testReferenceDataDirectoryPath + '/' + referenceDataDirName;
		bool isDataEqual = CompareDirectories(argParserPtr->GetOutputDirectoryPath() + '/' + outputDirName, referenceDataAbsolutePath);

		if (!isDataEqual){
			qWarning() << argParserPtr->GetOutputDirectoryPath() + '/' + outputDirName << "and" << referenceDataAbsolutePath << "are not equal";
		}
		QVERIFY(isDataEqual);
	}
}


// prevate slots (i.e. test cases)

void CSdlGenTest::initTestCase()
{
	m_isAllTestsPassed = true;
	m_tempOutputDir.setPath("SdlGenTest");
	if (m_tempOutputDir.exists()){
		QVERIFY(m_tempOutputDir.removeRecursively());
	}

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
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, m_tempOutputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);
	ExecuteTest(testSuite, "ImtBaseTypes.sdl");
}


void CSdlGenTest::TestComplexCollectionFilter()
{
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, m_tempOutputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled();
	ExecuteTest(testSuite, "ComplexCollectionFilter.sdl", "ComplexCollectionFilterTest", "imtbasesdl");
}


void CSdlGenTest::TestUnion()
{
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, m_tempOutputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled();

	ExecuteTest(testSuite, "UnionTest.sdl", "ComplexUnion");
}


void CSdlGenTest::TestComplexUnion()
{
	// p1
		CImtSdlGenTest testSuite;
		PrepareSuite(testSuite, m_tempOutputDir);

		auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
		argParserPtr->SetCppEnabled();
		argParserPtr->SetGqlEnabled();
		argParserPtr->SetQmlEnabled(false);

		ExecuteTest(testSuite, "ComplexUnion1.sdl");

	// p2
	{
		CImtSdlGenTest testSuite;
		PrepareSuite(testSuite, m_tempOutputDir);

		auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
		argParserPtr->SetCppEnabled();
		argParserPtr->SetGqlEnabled();
		argParserPtr->SetQmlEnabled(false);

		ExecuteTest(testSuite, "ComplexUnion2.sdl", "ComplexUnion", "complextestsdl");
	}
}


void CSdlGenTest::PrinterTest()
{
	// printer test
	{
		CImtSdlGenTest testSuite;
		PrepareSuite(testSuite, m_tempOutputDir);

		auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
		argParserPtr->SetCppEnabled();
		argParserPtr->SetGqlEnabled();
		argParserPtr->SetQmlEnabled(false);

		ExecuteTest(testSuite, "PrinterBase.sdl", "PrinterBase");
	}
	// digital printer
	{
		CImtSdlGenTest testSuite;
		PrepareSuite(testSuite, m_tempOutputDir);

		auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
		argParserPtr->SetCppEnabled();
		argParserPtr->SetGqlEnabled();
		argParserPtr->SetQmlEnabled(false);

		ExecuteTest(testSuite, "DigitalPrinter.sdl", "Printer", "modsdlsdl");
	}
}


void CSdlGenTest::cleanup() 
{
	m_isAllTestsPassed = m_isAllTestsPassed && !QTest::currentTestFailed();
}


void CSdlGenTest::cleanupTestCase()
{
	if (m_tempOutputDir.exists()){
		if (m_isAllTestsPassed) {
			QVERIFY(m_tempOutputDir.removeRecursively());
		}
	}
}


// add tests

I_ADD_TEST(CSdlGenTest);


} //namespace imtsdlgentest
