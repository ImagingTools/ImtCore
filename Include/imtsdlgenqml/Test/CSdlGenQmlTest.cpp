// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSdlGenQmlTest.h"


// Qt includes
#include <QtCore/QJsonDocument>
#include <QtTest/QTest>
#include <QtCore/QFile>
#include <QtCore/QTemporaryDir>

// ACF includes
#include <istd/CSystem.h>
#include <iproc/IProcessor.h>
#include <itest/CStandardTestExecutor.h>
#include <iprm/CParamsSet.h>
#include <iser/CJsonMemWriteArchive.h>
#include <iser/CJsonMemReadArchive.h>

// ImtCore includes
#include <imtsdlgenqml/CSdlQmlGenerationResult.h>
#include <imtsdlgenqml/CQmlGenTools.h>

// generated includes
#include <GeneratedFiles/ImtSdlGenQmlTest/CImtSdlGenQmlTest.h>


namespace imtsdlgenqmltest
{

// static variables
static const QString s_configFilePath = qEnvironmentVariable("IMTCOREDIR") + "/Config/SDL/CommonCXX.cfg";
static const QString s_testDataDirectoryPath = qEnvironmentVariable("IMTCOREDIR") + "/Include/imtsdlgenqml/Test/TestData";
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


void GetPrecessorAndExec(CImtSdlGenQmlTest& testSuite, const QByteArray& processorName, iprm::IParamsSet* processorParamsPtr = nullptr)
{
	auto* processorPtr = testSuite.GetInterface<iproc::IProcessor>(processorName);
	QCOMPARE_NE(processorPtr, nullptr);

	iproc::IProcessor::TaskState execResult = processorPtr->DoProcessing(processorParamsPtr, nullptr, nullptr);
	QCOMPARE(execResult, iproc::IProcessor::TS_OK);
}


void PrepareSuite(CImtSdlGenQmlTest& testSuite, const QDir& outputDir)
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
	CImtSdlGenQmlTest& testSuite,
	const QString& schemaFileName,
	const QString& referenceDataDirName = QString(),
	const QString& outputDirName = QString())
{
	QCOMPARE_NE(schemaFileName.length(), 0);
	const QString schemaFilePath = s_testDataDirectoryPath + '/' + schemaFileName;
	QVERIFY(QFileInfo::exists(schemaFilePath));

	auto* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	QCOMPARE_NE(argParserPtr, nullptr);

	argParserPtr->SetSchemaFilePath(schemaFilePath);

	// set a temp dir for all processors
	QTemporaryDir tempOutputDir;
	const QString tempOutputDirPath = tempOutputDir.path();
	bool isOutputDirExists = istd::CSystem::EnsurePathExists(tempOutputDirPath);
	if (!isOutputDirExists){
		qCritical() <<  QString("Unable to create temp dir at '%1'").arg(tempOutputDirPath);
	}
	QVERIFY(isOutputDirExists);

	// first parse the schema
	GetPrecessorAndExec(testSuite, "GqlSchemaParser");

	// do processing for QML
	GetPrecessorAndExec(testSuite, "QmlCodeGenFramework");
	if (!referenceDataDirName.isEmpty() && !outputDirName.isEmpty()){
		const QString referenceDataAbsolutePath = s_testReferenceDataDirectoryPath + '/' + referenceDataDirName;
		const QString outputDirFullPath = argParserPtr->GetOutputDirectoryPath() + "/1.0/QML/" + outputDirName;
		bool isDataEqual = CompareDirectories(outputDirFullPath, referenceDataAbsolutePath);

		if (!isDataEqual){
			qWarning() << outputDirFullPath << "and" << referenceDataAbsolutePath << "are not equal";
		}
		QVERIFY(isDataEqual);
	}
}


// prevate slots (i.e. test cases)

void CSdlGenQmlTest::initTestCase()
{
	m_isAllTestsPassed = true;
	m_tempOutputDir.setPath("SdlGenQmlTest");
	if (m_tempOutputDir.exists()){
		QVERIFY(m_tempOutputDir.removeRecursively());
	}
}


void CSdlGenQmlTest::TestComplexCollectionFilter()
{
	CImtSdlGenQmlTest testSuite;
	PrepareSuite(testSuite, m_tempOutputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled(false);
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled();
	ExecuteTest(testSuite, "ComplexCollectionFilter.sdl", "ComplexCollectionFilter", "imtbaseComplexCollectionFilterSdl");
}


void CSdlGenQmlTest::TestGenerationResultSerialization()
{
	// Create a generation result object
	imtsdlgenqml::CSdlQmlGenerationResult result;
	result.SetCreatedAt(QDateTime::currentDateTimeUtc());
	result.SetGeneratorVersion("1.0");
	QSet<QString> folders;
	folders << "/path/to/folder1" << "/path/to/folder2";
	result.SetCreatedFolders(folders);

	// Test serialization
	iser::CJsonMemWriteArchive writeArchive(nullptr, false);
	QVERIFY(result.Serialize(writeArchive));
	QByteArray buffer = writeArchive.GetData();
	QVERIFY(!buffer.isEmpty());

	// Test deserialization
	imtsdlgenqml::CSdlQmlGenerationResult loadedResult;
	iser::CJsonMemReadArchive readArchive(buffer, false);
	QVERIFY(loadedResult.Serialize(readArchive));

	// Verify data
	QCOMPARE(loadedResult.GetGeneratorVersion(), result.GetGeneratorVersion());
	QCOMPARE(loadedResult.GetCreatedFolders(), result.GetCreatedFolders());
	QCOMPARE(loadedResult.GetCreatedAt().toMSecsSinceEpoch(), result.GetCreatedAt().toMSecsSinceEpoch());
}


void CSdlGenQmlTest::TestGenerationResultFileOperations()
{
	QTemporaryDir tempDir;
	QVERIFY(tempDir.isValid());

	const QString testFilePath = tempDir.path() + "/generation_info.json";

	// Create test data
	imtsdlgenqml::CSdlQmlGenerationResult result;
	result.SetCreatedAt(QDateTime::currentDateTimeUtc());
	result.SetGeneratorVersion("1.0");
	QSet<QString> folders;
	folders << tempDir.path() + "/folder1" << tempDir.path() + "/folder2";
	result.SetCreatedFolders(folders);

	// Test write
	QVERIFY(imtsdlgenqml::CQmlGenTools::WriteGenerationResultFile(result, testFilePath));
	QVERIFY(QFile::exists(testFilePath));

	// Test read
	imtsdlgenqml::CSdlQmlGenerationResult loadedResult;
	QVERIFY(imtsdlgenqml::CQmlGenTools::ReadGenerationResultFile(loadedResult, testFilePath));

	// Verify data
	QCOMPARE(loadedResult.GetGeneratorVersion(), result.GetGeneratorVersion());
	QCOMPARE(loadedResult.GetCreatedFolders(), result.GetCreatedFolders());
}


void CSdlGenQmlTest::TestAppendFoldersWithAutomaticTimestamp()
{
	QTemporaryDir tempDir;
	QVERIFY(tempDir.isValid());

	const QString testFilePath = tempDir.path() + "/generation_info.json";

	// Create initial file with some folders
	static QString generatorVersion = "1.0";
	imtsdlgenqml::CSdlQmlGenerationResult initialResult;
	initialResult.SetCreatedAt(QDateTime::currentDateTimeUtc());
	initialResult.SetGeneratorVersion(generatorVersion);
	QSet<QString> initialFolders;
	initialFolders << tempDir.path() + "/folder1" << tempDir.path() + "/folder2";
	initialResult.SetCreatedFolders(initialFolders);
	QVERIFY(imtsdlgenqml::CQmlGenTools::WriteGenerationResultFile(initialResult, testFilePath));

	// Prepare update data with new folders
	imtsdlgenqml::CSdlQmlGenerationResult updateData;
	QSet<QString> additionalFolders;
	additionalFolders << tempDir.path() + "/folder3" << tempDir.path() + "/folder2"; // folder2 is duplicate
	updateData.SetCreatedFolders(additionalFolders);
	// Not setting createdAt, so current time should be used
	
	QVERIFY(imtsdlgenqml::CQmlGenTools::UpdateGenerationResult(testFilePath, updateData));

	// Read back and verify
	imtsdlgenqml::CSdlQmlGenerationResult loadedResult;
	QVERIFY(imtsdlgenqml::CQmlGenTools::ReadGenerationResultFile(loadedResult, testFilePath));

	// Should have all 3 folders (folder2 should not be duplicated)
	QSet<QString> expectedFolders = initialFolders | additionalFolders;
	QCOMPARE(loadedResult.GetCreatedFolders(), expectedFolders);
	QCOMPARE(loadedResult.GetGeneratorVersion(), generatorVersion);

	// Verify timestamp was updated (should be recent)
	QDateTime now = QDateTime::currentDateTimeUtc();
	qint64 timeDiffSecs = qAbs(loadedResult.GetCreatedAt().secsTo(now));
	QVERIFY2(timeDiffSecs < 5, "Timestamp should be updated to current time");
}


void CSdlGenQmlTest::TestAppendFoldersWithSpecificTimestamp()
{
	QTemporaryDir tempDir;
	QVERIFY(tempDir.isValid());

	const QString testFilePath = tempDir.path() + "/generation_info.json";

	// Create initial file with some folders
	static QString generatorVersion = "1.0";
	imtsdlgenqml::CSdlQmlGenerationResult initialResult;
	initialResult.SetCreatedAt(QDateTime::currentDateTimeUtc());
	initialResult.SetGeneratorVersion(generatorVersion);
	QSet<QString> initialFolders;
	initialFolders << tempDir.path() + "/folder1" << tempDir.path() + "/folder2";
	initialResult.SetCreatedFolders(initialFolders);
	QVERIFY(imtsdlgenqml::CQmlGenTools::WriteGenerationResultFile(initialResult, testFilePath));

	// Prepare update data with specific timestamp
	imtsdlgenqml::CSdlQmlGenerationResult updateData;
	QSet<QString> additionalFolders;
	additionalFolders << tempDir.path() + "/folder3";
	updateData.SetCreatedFolders(additionalFolders);
	QDateTime specificTime = QDateTime::fromString("2024-06-15T14:20:00.000Z", Qt::ISODateWithMs);
	updateData.SetCreatedAt(specificTime);
	
	QVERIFY(imtsdlgenqml::CQmlGenTools::UpdateGenerationResult(testFilePath, updateData));
	
	// Read back and verify specific timestamp was used
	imtsdlgenqml::CSdlQmlGenerationResult loadedResult;
	QVERIFY(imtsdlgenqml::CQmlGenTools::ReadGenerationResultFile(loadedResult, testFilePath));
	
	// Should have all 3 folders now
	QSet<QString> expectedFolders = initialFolders | additionalFolders;
	QCOMPARE(loadedResult.GetCreatedFolders(), expectedFolders);
	QCOMPARE(loadedResult.GetGeneratorVersion(), generatorVersion);
	
	// Verify the specific timestamp was used
	QCOMPARE(loadedResult.GetCreatedAt().toMSecsSinceEpoch(), specificTime.toMSecsSinceEpoch());
}


void CSdlGenQmlTest::TestGenerationResultJsonFormat()
{
	// Create test data with known values
	imtsdlgenqml::CSdlQmlGenerationResult result;
	result.SetCreatedAt(QDateTime::fromString("2024-01-15T10:30:00.000Z", Qt::ISODateWithMs));
	result.SetGeneratorVersion("1.0");
	QSet<QString> folders;
	folders << "/path/to/folder1" << "/path/to/folder2" << "/path/to/folder3";
	result.SetCreatedFolders(folders);

	// Serialize to JSON
	iser::CJsonMemWriteArchive writeArchive(nullptr, false);
	QVERIFY(result.Serialize(writeArchive));
	QByteArray actualJson = writeArchive.GetData();
	QVERIFY(!actualJson.isEmpty());

	// Parse to QJsonDocument and write in indented format (non-compact)
	QJsonParseError actualParseError;
	QJsonDocument actualDoc = QJsonDocument::fromJson(actualJson, &actualParseError);
	QVERIFY2(actualParseError.error == QJsonParseError::NoError, 
			qPrintable(QString("Failed to parse actual JSON: %1 at offset %2")
					.arg(actualParseError.errorString())
					.arg(actualParseError.offset)));
	QByteArray actualJsonIndented = actualDoc.toJson(QJsonDocument::Indented);

	// Write actual JSON to file in temp output directory
	const QString actualFilePath = m_tempOutputDir.path() + "/ActualGenerationResult.json";
	QFile actualFile(actualFilePath);
	QVERIFY(actualFile.open(QIODevice::WriteOnly | QIODevice::Text));
	QCOMPARE(actualFile.write(actualJsonIndented), static_cast<qint64>(actualJsonIndented.size()));
	actualFile.close();

	// Load expected JSON from reference file
	const QString referenceFilePath = s_testReferenceDataDirectoryPath + "/ExpectedGenerationResult.json";
	QVERIFY(QFile::exists(referenceFilePath));

	QFile referenceFile(referenceFilePath);
	QVERIFY(referenceFile.open(QIODevice::ReadOnly | QIODevice::Text));
	QByteArray expectedJson = referenceFile.readAll();
	referenceFile.close();
	QVERIFY(!expectedJson.isEmpty());

	iser::CJsonMemReadArchive readArchive(expectedJson, false);
	imtsdlgenqml::CSdlQmlGenerationResult expectedResult;
	QVERIFY(expectedResult.Serialize(readArchive));

	// Compare actual and expected results
	QVERIFY2(result.IsEqual(expectedResult), "Actual generation result does not match expected result");
}


void CSdlGenQmlTest::cleanup() 
{
	m_isAllTestsPassed = m_isAllTestsPassed && !QTest::currentTestFailed();
}


void CSdlGenQmlTest::cleanupTestCase()
{
	if (m_tempOutputDir.exists()){
		if (m_isAllTestsPassed) {
			QVERIFY(m_tempOutputDir.removeRecursively());
		}
	}
}


// add tests

I_ADD_TEST(CSdlGenQmlTest);


} //namespace imtsdlgenqmltest
