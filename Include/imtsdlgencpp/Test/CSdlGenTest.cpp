// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSdlGenTest.h"


// Qt includes
#include <QtTest/QTest>
#include <QtCore/QBuffer>
#include <QtCore/QDirIterator>
#include <QtCore/QProcess>
#include <QtCore/QStandardPaths>
#include <QtCore/QTemporaryDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>

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
#include <imtsdl/ISdlTypeListProvider.h>
#include <imtsdl/CSdlType.h>
#include <imtsdlgencpp/ICxxFilePostProcessor.h>


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


/**
	Parses the schema and runs the code generation.
	Unlike \c ExecuteTest, it doesn't require the successful result of the generation, so it can be used to check the errors.
*/
static iproc::IProcessor::TaskState GenerateSchema(CImtSdlGenTest& testSuite, const QString& schemaFileName)
{
	auto* argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	Q_ASSERT(argParserPtr != nullptr);

	argParserPtr->SetSchemaFilePath(s_testDataDirectoryPath + '/' + schemaFileName);

	auto* parserPtr = testSuite.GetInterface<iproc::IProcessor>("GqlSchemaParser");
	Q_ASSERT(parserPtr != nullptr);
	if (parserPtr->DoProcessing(nullptr, nullptr, nullptr, nullptr, nullptr) != iproc::IProcessor::TS_OK){
		return iproc::IProcessor::TS_INVALID;
	}

	auto* generatorPtr = testSuite.GetInterface<iproc::IProcessor>("CppCodeGeneratorFramework");
	Q_ASSERT(generatorPtr != nullptr);

	return generatorPtr->DoProcessing(nullptr, nullptr, nullptr, nullptr, nullptr);
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

	// the objects header extends the main one, its name is the main header name with the objects suffix
	const QStringList objectHeaders = FindFiles(outputDir.absolutePath(), "ArraySerializationObjects.h");
	QCOMPARE(objectHeaders.size(), 1);
	QVERIFY(ReadFileContent(objectHeaders.first()).contains("#include \"ArraySerialization.h\""));
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


void CSdlGenTest::TestTargetHeaderPathsOfLocalTypes()
{
	struct Paths
	{
		QString header;
		QString objectHeader;
	};

	const auto getPaths = [this](bool isObjectsEnabled, const QString& subDirName) -> Paths {
		CImtSdlGenTest testSuite;
		PrepareSuite(testSuite, QDir(m_tempOutputDir.absoluteFilePath(subDirName)));

		auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
		argParserPtr->SetCppEnabled();
		argParserPtr->SetGqlEnabled();
		argParserPtr->SetQmlEnabled(false);
		argParserPtr->SetObjectsGenerationEnabled(isObjectsEnabled);
		argParserPtr->SetSchemaFilePath(s_testDataDirectoryPath + "/ArraySerialization.sdl");

		auto* parserPtr = testSuite.GetInterface<iproc::IProcessor>("GqlSchemaParser");
		if (parserPtr == nullptr || parserPtr->DoProcessing(nullptr, nullptr, nullptr, nullptr, nullptr) != iproc::IProcessor::TS_OK){
			return Paths();
		}

		auto* typeListPtr = testSuite.GetInterface<imtsdl::ISdlTypeListProvider>();
		if (typeListPtr == nullptr){
			return Paths();
		}

		for (const imtsdl::CSdlType& sdlType: typeListPtr->GetSdlTypes(true)){
			if (sdlType.GetName() == QStringLiteral("Attribute")){
				return Paths{sdlType.GetTargetHeaderFilePath(), sdlType.GetObjectTargetHeaderFilePath()};
			}
		}

		return Paths();
	};

	const Paths withObjects = getPaths(true, "TargetPathsWithObjects");
	const Paths withoutObjects = getPaths(false, "TargetPathsWithoutObjects");

	// the main header path of a local type is the same, regardless of the objects
	QVERIFY(!withObjects.header.isEmpty());
	QVERIFY(QDir::isAbsolutePath(withObjects.header));
	QVERIFY(withObjects.header.endsWith(".h"));
	QVERIFY(!withObjects.header.endsWith("Objects.h"));
	QVERIFY(withObjects.header.contains(QStringLiteral("TargetPathsWithObjects")));
	QVERIFY(withoutObjects.header.contains(QStringLiteral("TargetPathsWithoutObjects")));
	QCOMPARE(QFileInfo(withObjects.header).fileName(), QFileInfo(withoutObjects.header).fileName());
	QCOMPARE(QFileInfo(withObjects.header).dir().relativeFilePath(withObjects.header), QFileInfo(withoutObjects.header).dir().relativeFilePath(withoutObjects.header));

	// the objects header exists only if the objects are generated
	QVERIFY(withObjects.objectHeader.endsWith(QStringLiteral("Objects.h")));
	QVERIFY(QDir::isAbsolutePath(withObjects.objectHeader));
	QVERIFY(withoutObjects.objectHeader.isEmpty());
}


void CSdlGenTest::TestImportedObjectsIncludeIsGuarded()
{
	const QDir outputDir(m_tempOutputDir.absoluteFilePath("ImportedObjectsGuard"));

	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, outputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);
	argParserPtr->SetObjectsGenerationEnabled(true);

	ExecuteTest(testSuite, "ImportedEnumArray.sdl");

	const QStringList objectHeaders = FindFiles(outputDir.absolutePath(), "ImportedEnumArrayObjects.h");
	QCOMPARE(objectHeaders.size(), 1);
	const QByteArray content = ReadFileContent(objectHeaders.first());

	// the imported schema may be generated without the objects, it is checked by the compiler
	const QByteArray importedHeader = "<externalenumsdl/SDL/1.0/CPP/ImportedEnumObjects.h>";
	QVERIFY(content.contains("#if __has_include(" + importedHeader + ")\n#include " + importedHeader + "\n#else\n#error The objects header " + importedHeader));
	QVERIFY(content.contains("NO_OBJECTS"));

	// the includes of the own libraries are always required
	QVERIFY(content.contains("#include <imtbase/CItemModelBase.h>"));
	QVERIFY(!content.contains("#if __has_include(<imtbase/"));
}


void CSdlGenTest::TestImportedObjectsGuardReportsMissingHeader()
{
	const QString compilerPath = []() -> QString {
		for (const QString& name: {QStringLiteral("c++"), QStringLiteral("clang++"), QStringLiteral("g++")}){
			const QString path = QStandardPaths::findExecutable(name);
			if (!path.isEmpty()){
				return path;
			}
		}

		return QString();
	}();
	if (compilerPath.isEmpty()){
		QSKIP("A C++ compiler is not found");
	}

	// the generated guard, checked by the preprocessor of the compiler
	const QDir outputDir(m_tempOutputDir.absoluteFilePath("ImportedObjectsGuardCompiler"));

	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, outputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);
	argParserPtr->SetObjectsGenerationEnabled(true);

	ExecuteTest(testSuite, "ImportedEnumArray.sdl");

	const QStringList objectHeaders = FindFiles(outputDir.absolutePath(), "ImportedEnumArrayObjects.h");
	QCOMPARE(objectHeaders.size(), 1);
	const QByteArray content = ReadFileContent(objectHeaders.first());
	const int guardBegin = content.indexOf("#if __has_include(");
	const int guardEnd = content.indexOf("#endif", guardBegin);
	QVERIFY(guardBegin >= 0 && guardEnd > guardBegin);

	QTemporaryDir workDir;
	QVERIFY(workDir.isValid());
	QFile snippet(workDir.filePath("guard.cpp"));
	QVERIFY(snippet.open(QIODevice::WriteOnly));
	snippet.write(content.mid(guardBegin, guardEnd + 6 - guardBegin));
	snippet.close();

	const auto preprocess = [&](const QString& includeDirPath, QString& errorOutput) -> bool {
		QProcess process;
		process.start(compilerPath, {QStringLiteral("-E"), QStringLiteral("-x"), QStringLiteral("c++"), QStringLiteral("-I"), includeDirPath, snippet.fileName()});
		if (!process.waitForFinished(60000)){
			return false;
		}
		errorOutput = QString::fromLocal8Bit(process.readAllStandardError());

		return process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0;
	};

	// there is no objects header (the imported schema is generated without objects): the clear error
	QTemporaryDir emptyIncludeDir;
	QVERIFY(emptyIncludeDir.isValid());
	QString errorOutput;
	QVERIFY(!preprocess(emptyIncludeDir.path(), errorOutput));
	QVERIFY2(errorOutput.contains("NO_OBJECTS"), qPrintable(errorOutput));
	QVERIFY2(errorOutput.contains("ImportedEnumObjects.h"), qPrintable(errorOutput));

	// the objects header exists: the guard is passed
	QTemporaryDir includeDir;
	QVERIFY(includeDir.isValid());
	QVERIFY(QDir().mkpath(includeDir.filePath("externalenumsdl/SDL/1.0/CPP")));
	QFile header(includeDir.filePath("externalenumsdl/SDL/1.0/CPP/ImportedEnumObjects.h"));
	QVERIFY(header.open(QIODevice::WriteOnly));
	header.close();
	QVERIFY2(preprocess(includeDir.path(), errorOutput), qPrintable(errorOutput));
}


void CSdlGenTest::TestNoObjectsSchemaImportsSchema()
{
	// the schema without the objects imports another schema: no objects header is required
	const QDir outputDir(m_tempOutputDir.absoluteFilePath("NoObjectsImportsSchema"));

	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, outputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);
	argParserPtr->SetObjectsGenerationEnabled(false);

	QCOMPARE(GenerateSchema(testSuite, "ImportedEnumArray.sdl"), iproc::IProcessor::TS_OK);
	QCOMPARE(FindFiles(outputDir.absolutePath(), "*Objects*").size(), 0);
	QCOMPARE(FindFiles(outputDir.absolutePath(), "ImportedEnumArray.cpp").size(), 1);

	// the main header of the imported schema is still required
	const QStringList headers = FindFiles(outputDir.absolutePath(), "ImportedEnumArray.h");
	QCOMPARE(headers.size(), 1);
	QVERIFY(ReadFileContent(headers.first()).contains("ImportedEnum.h"));
}


void CSdlGenTest::TestIntermediateFileIsRemovedAfterFailure()
{
	// The implementation-only mode is a failing one (the objects header is created in the header mode),
	// the intermediate file is already created at this moment. In any case it must not stay in the output directory
	const QDir outputDir(m_tempOutputDir.absoluteFilePath("IntermediateFileAfterFailure"));

	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, outputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);
	argParserPtr->SetObjectsGenerationEnabled(true);
	argParserPtr->SetCppGenerationMode(imtsdl::ISdlProcessArgumentsParser::CGM_IMPLEMENTATION_ONLY);

	[[maybe_unused]] const iproc::IProcessor::TaskState result = GenerateSchema(testSuite, "ArraySerialization.sdl");

	QCOMPARE(FindFiles(outputDir.absolutePath(), "*Objects.cpp*").size(), 0);
}


void CSdlGenTest::TestDepFileDoesNotContainIntermediateFile()
{
	const QDir outputDir(m_tempOutputDir.absoluteFilePath("DepFileOutputs"));

	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, outputDir);

	const QString depFilePath = outputDir.absoluteFilePath("schema.depfile");

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetCppEnabled();
	argParserPtr->SetGqlEnabled();
	argParserPtr->SetQmlEnabled(false);
	argParserPtr->SetObjectsGenerationEnabled(true);
	argParserPtr->SetDepFilePath(depFilePath);

	ExecuteTest(testSuite, "ArraySerialization.sdl");

	const QByteArray depFile = ReadFileContent(depFilePath);
	QVERIFY(!depFile.isEmpty());

	// the outputs
	QVERIFY(depFile.contains("ArraySerialization.cpp"));
	QVERIFY(depFile.contains("ArraySerialization.h"));
	QVERIFY(depFile.contains("ArraySerialization_fwd.h"));
	QVERIFY(depFile.contains("ArraySerializationObjects.h"));

	// the intermediate file is removed after the processing, so it is not an output
	QVERIFY(!depFile.contains("ArraySerializationObjects.cpp"));
}


void CSdlGenTest::TestSourceTailPostProcessor()
{
	const QDir outputDir(m_tempOutputDir.absoluteFilePath("SourceTailPostProcessor"));
	QVERIFY(QDir().mkpath(outputDir.absolutePath()));

	CImtSdlGenTest testSuite;
	PrepareSuite(testSuite, outputDir);

	auto argParserPtr = testSuite.GetInterface<imtsdl::ISdlEditableProcessArgumentsParser>();
	argParserPtr->SetMocIncludeEnabled(true);

	auto* postProcessorPtr = testSuite.GetInterface<imtsdlgencpp::ICxxFilePostProcessor>("CxxSourceTailPostProcessor");
	QVERIFY(postProcessorPtr != nullptr);

	const auto createFile = [&](const QString& name, const QByteArray& content, QIODevice::OpenMode mode) -> imtsdlgencpp::CxxEntryFileInfo::FilePtr {
		auto filePtr = std::make_shared<QFile>(outputDir.absoluteFilePath(name));
		if (!filePtr->open(QIODevice::WriteOnly | QIODevice::Truncate)){
			return nullptr;
		}
		filePtr->write(content);
		filePtr->close();

		return filePtr->open(mode) ? filePtr : nullptr;
	};

	// the objects source is appended to the main one, with the moc includes
	{
		imtsdlgencpp::CxxEntryFileInfo files;
		files.sourceFile = createFile("Foo.cpp", "// main source\n", QIODevice::ReadWrite | QIODevice::Append);
		files.objectSourceFile = createFile("FooObjects.cpp", "// objects source\n", QIODevice::ReadOnly);
		QVERIFY(files.sourceFile != nullptr);
		QVERIFY(files.objectSourceFile != nullptr);

		QVERIFY(postProcessorPtr->ProcessFiles(files));
		files.sourceFile->close();

		const QByteArray source = ReadFileContent(outputDir.absoluteFilePath("Foo.cpp"));
		QVERIFY(source.startsWith("// main source"));
		QVERIFY(source.contains("// objects source"));
		QVERIFY(source.indexOf("// main source") < source.indexOf("// objects source"));
		QVERIFY(source.contains("#include \"moc_Foo.cpp\""));
		QVERIFY(source.contains("#include \"moc_FooObjects.cpp\""));
	}

	// the failed copying must fail the post processing: the objects source is removed after it, and the main one would be incomplete
	{
		imtsdlgencpp::CxxEntryFileInfo files;
		// the main source is opened only for reading, so the writing fails
		files.sourceFile = createFile("Bar.cpp", "// main source\n", QIODevice::ReadOnly);
		files.objectSourceFile = createFile("BarObjects.cpp", "// objects source\n", QIODevice::ReadOnly);
		QVERIFY(files.sourceFile != nullptr);
		QVERIFY(files.objectSourceFile != nullptr);

		QVERIFY(!postProcessorPtr->ProcessFiles(files));
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
