// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSdlGenTest.h"


// Qt includes
#include <QtTest/QTest>
#include <QtCore/QBuffer>
#include <QtCore/QDirIterator>
#include <QtCore/QFile>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/IOptionsManager.h>
#include <iproc/IProcessor.h>
#include <itest/CStandardTestExecutor.h>
#include <iprm/CParamsSet.h>

// ImtCore includes
#include <GeneratedFiles/ImtSdlGenTest/CImtSdlGenTest.h>
#include <imtbase/CTreeItemModel.h>
#include <imtsdl/CGqlSchemaParser.h>


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


static QStringList FindFiles(const QString& dirPath, const QString& nameFilter)
{
	QStringList retVal;

	QDirIterator iterator(dirPath, QStringList() << nameFilter, QDir::Files, QDirIterator::Subdirectories);
	while (iterator.hasNext()){
		retVal << iterator.next();
	}

	return retVal;
}


static QByteArray ReadFileContent(const QString& filePath)
{
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
		return QByteArray();
	}

	return file.readAll();
}


void GetPrecessorAndExec(CImtSdlGenTest& testSuite, const QByteArray& processorName, iprm::IParamsSet* processorParamsPtr = nullptr)
{
	auto* processorPtr = testSuite.GetInterface<iproc::IProcessor>(processorName);
	QCOMPARE_NE(processorPtr, nullptr);

	iproc::IProcessor::TaskState execResult = processorPtr->DoProcessing(processorParamsPtr, nullptr, nullptr, nullptr, nullptr);
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
		qCritical() <<  QStringLiteral("Unable to create temp dir at '%1'").arg(tempOutputDirPath);

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


void CSdlGenTest::TestArrayNullabilityParsing()
{
	QByteArray schema = R"(
		type Attribute {
			Value: String
		}

		type Entity {
			RequiredList: [Attribute!]!
			OptionalListWithRequiredElements: [Attribute!]
			OptionalListWithOptionalElements: [Attribute]
		}
	)";
	QBuffer schemaBuffer(&schema);
	QVERIFY(schemaBuffer.open(QIODevice::ReadOnly));

	imtsdl::CGqlSchemaParser parser(schemaBuffer);
	QVERIFY(parser.ParseGqlSchema());

	const imtsdl::SdlFieldList fields = parser.GetFields(QStringLiteral("Entity"));
	QCOMPARE(fields.size(), 3);

	QCOMPARE(fields[0].GetId(), QStringLiteral("RequiredList"));
	QVERIFY(fields[0].IsArray());
	QVERIFY(fields[0].IsRequired());
	QVERIFY(fields[0].AreArrayElementsRequired());

	QCOMPARE(fields[1].GetId(), QStringLiteral("OptionalListWithRequiredElements"));
	QVERIFY(fields[1].IsArray());
	QVERIFY(!fields[1].IsRequired());
	QVERIFY(fields[1].AreArrayElementsRequired());

	QCOMPARE(fields[2].GetId(), QStringLiteral("OptionalListWithOptionalElements"));
	QVERIFY(fields[2].IsArray());
	QVERIFY(!fields[2].IsRequired());
	QVERIFY(!fields[2].AreArrayElementsRequired());
}


void CSdlGenTest::TestImportedEnumArrayQObjectSetter()
{
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, m_tempOutputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled();

	ExecuteTest(testSuite, "ImportedEnumArray.sdl");

	QFile generatedSource(m_tempOutputDir.absoluteFilePath(
		"enumconsumersdl/SDL/1.0/CPP/ImportedEnumArray.cpp"));
	QVERIFY(generatedSource.open(QIODevice::ReadOnly | QIODevice::Text));

	const QByteArray source = generatedSource.readAll();
	QVERIFY(source.contains(
		"istd::TNullableValue<sdl::V1_0::externalenum::ImportedState> tempItem("
		"static_cast<sdl::V1_0::externalenum::ImportedState>(key));"));
}


void CSdlGenTest::TestTreeModelExplicitNullKey()
{
	imtbase::CTreeItemModel model;
	QVERIFY(!model.ContainsKey("Value"));
	QVERIFY(model.SetData("Value", QVariant()));
	QVERIFY(model.ContainsKey("Value"));
	QVERIFY(!model.GetData("Value").isValid());

	QVERIFY(model.AddTreeModel("Value") != nullptr);
	QVERIFY(model.GetTreeItemModel("Value") != nullptr);
	QVERIFY(model.SetData("Value", QVariant()));
	QVERIFY(model.ContainsKey("Value"));
	QVERIFY(model.GetTreeItemModel("Value") == nullptr);
}


void CSdlGenTest::TestNestedFieldNameCollision()
{
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, m_tempOutputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);

	ExecuteTest(testSuite, "NestedFieldNameCollision.sdl");
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


void CSdlGenTest::SubstrateSpecifications()
{
	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, m_tempOutputDir);
	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();

	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);

	ExecuteTest(testSuite, "SubstrateSpecifications.sdl", "Substrates", "substratesdl");
}


void CSdlGenTest::TestObjectsMocInclude()
{
	const QDir outputDir(m_tempOutputDir.absoluteFilePath("ObjectsMocInclude"));

	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, outputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);
	argParserPtr->SetObjectsGenerationEnabled(true);
	argParserPtr->SetMocIncludeEnabled(true);

	ExecuteTest(testSuite, "ArraySerialization.sdl");

	// the objects header is generated, the objects source is an intermediate file, joined with the main one and removed
	QCOMPARE(FindFiles(outputDir.absolutePath(), "ArraySerializationObjects.h").size(), 1);
	QCOMPARE(FindFiles(outputDir.absolutePath(), "ArraySerializationObjects.cpp*").size(), 0);

	const QStringList sourceFiles = FindFiles(outputDir.absolutePath(), "ArraySerialization.cpp");
	QCOMPARE(sourceFiles.size(), 1);

	// both moc outputs are included, with the exact names, that are created by the build system
	const QByteArray source = ReadFileContent(sourceFiles.first());
	QVERIFY(source.contains("#include \"moc_ArraySerialization.cpp\""));
	QVERIFY(source.contains("#include \"moc_ArraySerializationObjects.cpp\""));
}


void CSdlGenTest::TestNoObjectsMocInclude()
{
	const QDir outputDir(m_tempOutputDir.absoluteFilePath("NoObjectsMocInclude"));

	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, outputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);
	argParserPtr->SetObjectsGenerationEnabled(false);
	argParserPtr->SetMocIncludeEnabled(true);

	ExecuteTest(testSuite, "ArraySerialization.sdl");

	// nothing of objects is generated
	QCOMPARE(FindFiles(outputDir.absolutePath(), "*Objects*").size(), 0);

	const QStringList sourceFiles = FindFiles(outputDir.absolutePath(), "ArraySerialization.cpp");
	QCOMPARE(sourceFiles.size(), 1);

	// the main header still has the meta object (namespace, enums), and the build system doesn't compile its moc output separately
	const QByteArray source = ReadFileContent(sourceFiles.first());
	QVERIFY(source.contains("#include \"moc_ArraySerialization.cpp\""));
	QVERIFY(!source.contains("Objects.cpp"));
}


void CSdlGenTest::TestNoObjectsWithoutTemplate()
{
	const QDir outputDir(m_tempOutputDir.absoluteFilePath("NoObjectsWithoutTemplate"));

	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, outputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);
	argParserPtr->SetTemplateEnabled(false);
	argParserPtr->SetObjectsGenerationEnabled(false);

	ExecuteTest(testSuite, "ArraySerialization.sdl");

	QCOMPARE(FindFiles(outputDir.absolutePath(), "ArraySerialization.h").size(), 1);
	QCOMPARE(FindFiles(outputDir.absolutePath(), "ArraySerialization.cpp").size(), 1);
	QCOMPARE(FindFiles(outputDir.absolutePath(), "*Objects*").size(), 0);
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
