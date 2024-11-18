#include "CSdlClassCodeGeneratorComp.h"


// C includes
#include <iostream>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QTextStream>

//Acf includes
#include <istd/CSystem.h>
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>
#include <ifile/CFileNameParam.h>

// ImtCore includes
#include <imtfile/CSimpleFileJoinerComp.h>
#include <imtsdl/CSdlType.h>


namespace imtsdlgenv2
{


int CSdlClassCodeGeneratorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_originalSchemaNamespaceCompPtr.IsValid());

	int retVal = TS_OK;

	if (!m_argumentParserCompPtr->IsCppEnabled()){
		return retVal;
	}

	if (!m_baseClassExtenderCompPtr.IsValid()){
		SendCriticalMessage(0, "BaseClassExtender is not set");
		I_CRITICAL();

		return TS_INVALID;
	}

	const QString outputDirectoryPath = QDir::cleanPath(m_argumentParserCompPtr->GetOutputDirectoryPath());
	if (outputDirectoryPath.isEmpty()){
		SendCriticalMessage(0, "Output path is not provided");
		I_CRITICAL();

		return TS_INVALID;
	}

	if (!istd::CSystem::EnsurePathExists(outputDirectoryPath)){
		SendErrorMessage(0, QString("Unable to create path '%1'").arg(outputDirectoryPath));
		I_CRITICAL();

		return TS_INVALID;
	}

	const QString defaultName = QFileInfo(m_argumentParserCompPtr->GetSchemaFilePath()).fileName();
	QMap<QString, QString> joinRules = m_argumentParserCompPtr->GetJoinRules();
	if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
		if (!m_customSchemaParamsCompPtr.IsValid()){
			SendErrorMessage(0, "Application is not configured with custom parameters. Auto join is not possible. Please specify paths to join explicitly(use -J option), or disable join.");

			return TS_INVALID;
		}

		joinRules = CalculateTargetCppFilesFromSchemaParams(*m_customSchemaParamsCompPtr, m_argumentParserCompPtr->GetOutputDirectoryPath(), defaultName);
	}
	const bool joinHeaders = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_headerFileType);
	const bool joinSources = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_sourceFileType);

	if (m_argumentParserCompPtr->IsDependenciesMode()){
		if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
			if (!m_customSchemaParamsCompPtr.IsValid()){
				SendErrorMessage(0, "Application is not configured with custom parameters. Auto join is not possible. Please specify paths to join explicitly(use -J option), or disable join.");

				return TS_INVALID;
			}

			QStringList autoJoinFilePaths = GetAutoJoinedCppFilePaths(*m_customSchemaParamsCompPtr, m_argumentParserCompPtr->GetOutputDirectoryPath(), defaultName);
			PrintFiles(std::cout, autoJoinFilePaths, m_argumentParserCompPtr->GetGeneratorType());
		}
		else {
			QStringList cumulatedFiles;
			imtsdl::SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes(true);
			for (const imtsdl::CSdlType& sdlType: sdlTypeList){
				if (!joinHeaders){
					cumulatedFiles << QString(outputDirectoryPath + "/C" + sdlType.GetName() + ".h");
				}
				if (!joinSources){
					cumulatedFiles << QString(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp");
				}
			}
			if (joinHeaders){
				cumulatedFiles << joinRules[imtsdl::ISdlProcessArgumentsParser::s_headerFileType];
			}
			if (joinSources){
				cumulatedFiles << joinRules[imtsdl::ISdlProcessArgumentsParser::s_sourceFileType];
			}

			PrintFiles(std::cout, cumulatedFiles, m_argumentParserCompPtr->GetGeneratorType());
		}

		return TS_OK;
	}

	imtsdl::SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes(true);

	// first create all files with basic mathods
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		m_headerFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".h"));
		m_sourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp"));

		const bool hasExtDeps = m_argumentParserCompPtr->GetAutoLinkLevel() != imtsdl::ISdlProcessArgumentsParser::ALL_NONE;

		if (!BeginClassFiles(sdlType, hasExtDeps || !joinHeaders, !joinSources)){
			SendErrorMessage(0, QString("Unable to begin files"));
			I_CRITICAL();

			return TS_INVALID;
		}

		// Close files so that extenders can make their own changes
		if (!CloseFiles()){
			SendErrorMessage(0, QString("Unable to close files"));
			I_CRITICAL();

			return TS_INVALID;
		}
	}

	QMap<QString, QString> baseClassList = m_argumentParserCompPtr->GetBaseClassList();
	if (!baseClassList.isEmpty()){

		for (const imtsdl::CSdlType& sdlType: sdlTypeList){
			iprm::CParamsSet paramsSet;

			const QString filePath = outputDirectoryPath + "/C" + sdlType.GetName() + ".h";
			ifile::CFileNameParam headerFileNameParam;
			headerFileNameParam.SetPath(filePath);
			paramsSet.SetEditableParameter(QByteArrayLiteral("HeaderFile"), &headerFileNameParam);

			iprm::COptionsManager baseClassDirectivesList;
			for (auto iter = baseClassList.cbegin(); iter != baseClassList.cend(); ++iter){
				baseClassDirectivesList.InsertOption(iter.value(), iter.key().toUtf8());
			}

			int extendResult = m_baseClassExtenderCompPtr->DoProcessing(&paramsSet, &baseClassDirectivesList, nullptr);
			if (extendResult != TS_OK){
				SendErrorMessage(0, QString("Unable to extend file: '%1'").arg(filePath));
				I_CRITICAL();

				return extendResult;
			}
		}
	}

	// Then let extenders to make changes
	const int extendersCount = m_codeGeneratorExtenderListCompPtr.GetCount();
	for (int i = 0; i < extendersCount; ++i){
		iproc::IProcessor* extenderPtr = m_codeGeneratorExtenderListCompPtr[i];
		Q_ASSERT(extenderPtr != nullptr);

		int extenderResult = extenderPtr->DoProcessing(paramsPtr, inputPtr, outputPtr, progressManagerPtr);
		if (extenderResult != TS_OK){
			return extenderResult;
		}
		retVal = qMax(retVal, extenderResult);
	}

	// Reopen files to complete processing
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		m_headerFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".h"));
		m_sourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp"));
		if (!ReOpenFiles()){
			SendErrorMessage(0, QString("Unable to reopen files"));
			I_CRITICAL();

			return TS_INVALID;
		}

		// And complete the processing
		EndClassFiles(sdlType);
	}

	// join files if required
	if (!joinRules.isEmpty()){
		if (m_filesJoinerCompPtr.IsValid()){
			iprm::CParamsSet inputParams;
			ifile::CFileNameParam sourceDirPathParam;
			sourceDirPathParam.SetPath(outputDirectoryPath);
			inputParams.SetEditableParameter(imtfile::CSimpleFileJoinerComp::s_sourceDirPathParamId, &sourceDirPathParam);
			ifile::CFileNameParam outputFileNameParam;
			inputParams.SetEditableParameter(imtfile::CSimpleFileJoinerComp::s_targetFilePathParamId, &outputFileNameParam);

			iprm::COptionsManager filterParams;

			if (joinHeaders){
				filterParams.ResetOptions();
				for (const imtsdl::CSdlType& sdlType: sdlTypeList){
					filterParams.InsertOption("C" + sdlType.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount()));
					SendVerboseMessage(QString("Add join file '%1', %2").arg("C" + sdlType.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount())));
				}

				outputFileNameParam.SetPath(joinRules[imtsdl::ISdlProcessArgumentsParser::s_headerFileType]);
				int joinProcessResult = m_filesJoinerCompPtr->DoProcessing(&inputParams, &filterParams, nullptr);
				if (joinProcessResult != TS_OK){
					SendCriticalMessage(0, "Unable to join header files");
					I_CRITICAL();

					return TS_INVALID;
				}

				// cleanup joined files
				for (const imtsdl::CSdlType& sdlType: sdlTypeList){
					QFile::remove(QString(outputDirectoryPath + "/C" + sdlType.GetName() + ".h"));
				}
			}
			if (joinSources){
				filterParams.ResetOptions();
				for (const imtsdl::CSdlType& sdlType: sdlTypeList){
					filterParams.InsertOption("C" + sdlType.GetName() + ".cpp", QByteArray::number(filterParams.GetOptionsCount()));
				}

				const QString sourceFilePath = joinRules[imtsdl::ISdlProcessArgumentsParser::s_sourceFileType];
				outputFileNameParam.SetPath(sourceFilePath);
				int joinProcessResult = m_filesJoinerCompPtr->DoProcessing(&inputParams, &filterParams, nullptr);
				if (joinProcessResult != TS_OK){
					SendCriticalMessage(0, "Unable to join cource  files");
					I_CRITICAL();

					return TS_INVALID;
				}

				// cleanup joined files
				for (const imtsdl::CSdlType& sdlType: sdlTypeList){
					QFile::remove(QString(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp"));
				}

				// add joined header include directive
				if (joinHeaders){
					QFile joinedSourceFile(sourceFilePath);
					if (!joinedSourceFile.open(QIODevice::ReadWrite)){
						SendCriticalMessage(0, QString("Unable to open joined filee '%1'").arg(sourceFilePath));
						I_CRITICAL();

						return TS_INVALID;
					}
					QFileInfo headerFileInfo(joinRules[imtsdl::ISdlProcessArgumentsParser::s_headerFileType]);
					QByteArray sourceReadData = joinedSourceFile.readAll();
					joinedSourceFile.seek(0);
					QByteArray includeDirective = QByteArrayLiteral("#include ");
					includeDirective.append('"').append(headerFileInfo.fileName().toUtf8()).append('"');
					includeDirective.append('\n').append('\n').append('\n');
					sourceReadData.prepend(includeDirective);
					joinedSourceFile.write(sourceReadData);
				}
			}
		}
	}

	return retVal;
}


// private methods

bool CSdlClassCodeGeneratorComp::ReOpenFiles()
{
	if (!m_headerFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)){
		SendCriticalMessage(0,
					QString("Unable to open file: '%1'. Error: %2")
						.arg(m_headerFilePtr->fileName(), m_headerFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	if (!m_sourceFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)){
		SendCriticalMessage(0,
						QString("Unable to open file: '%1'. Error: %2")
							.arg(m_sourceFilePtr->fileName(), m_sourceFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	return true;
}


bool CSdlClassCodeGeneratorComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_headerFilePtr->flush();
	retVal = m_sourceFilePtr->flush() && retVal;

	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	return retVal;
}


bool CSdlClassCodeGeneratorComp::BeginClassFiles(const imtsdl::CSdlType& sdlType, bool addDependenciesInclude, bool addSelfHeaderInclude)
{
	if (!m_headerFilePtr->open(QIODevice::WriteOnly)){
		SendCriticalMessage(0,
					QString("Unable to open file: '%1'. Error: %2")
						.arg(m_headerFilePtr->fileName(), m_headerFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	if (!m_sourceFilePtr->open(QIODevice::WriteOnly)){
		SendCriticalMessage(0,
						QString("Unable to open file: '%1'. Error: %2")
							.arg(m_sourceFilePtr->fileName(), m_sourceFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	bool retVal = true;
	retVal = retVal && BeginHeaderClassFile(sdlType, addDependenciesInclude);
	retVal = retVal && BeginSourceClassFile(sdlType, addSelfHeaderInclude);

	return retVal;
}


bool CSdlClassCodeGeneratorComp::BeginHeaderClassFile(const imtsdl::CSdlType& sdlType, bool addDependenciesInclude)
{
	QTextStream stream(m_headerFilePtr.GetPtr());

	stream << QStringLiteral("#pragma once");
	FeedStream(stream, 3, false);

	QSet<QString> complexTypeList;
	bool isQtCommentAdded = false;
	bool hasComplexTypes = IsTypeHasNonFundamentalTypes(sdlType, &complexTypeList);

	// add STD includes for unique_ptr
	stream << QStringLiteral("// STD includes");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("#include <memory>");
	FeedStream(stream, 2, false);

	if (hasComplexTypes){
		// Add Qt types
		if (complexTypeList.contains(QStringLiteral("QByteArray"))){
			if (!isQtCommentAdded){
				stream << QStringLiteral("// Qt includes");
				FeedStream(stream, 1, false);
				isQtCommentAdded = true;
			}
			stream << QStringLiteral("#include <QtCore/QByteArray>");
			FeedStream(stream, 1, false);
		}
		if (complexTypeList.contains(QStringLiteral("QString"))){
			if (!isQtCommentAdded){
				stream << QStringLiteral("// Qt includes");
				FeedStream(stream, 1, false);
				isQtCommentAdded = true;
			}
			stream << QStringLiteral("#include <QtCore/QString>");
			FeedStream(stream, 1, false);
		}
		if (complexTypeList.contains(QStringLiteral("QList"))){
			if (!isQtCommentAdded){
				stream << QStringLiteral("// Qt includes");
				FeedStream(stream, 1, false);
				isQtCommentAdded = true;
			}
			stream << QStringLiteral("#include <QtCore/QList>");
			FeedStream(stream, 1, false);
		}
		if (!isQtCommentAdded){
			stream << QStringLiteral("// Qt includes");
			FeedStream(stream, 1, false);
			isQtCommentAdded = true;
		}

		// add required types
		stream << QStringLiteral("#include <QtCore/QVariant>");
		FeedStream(stream, 1, false);
		stream << QStringLiteral("#include <QtCore/QVariantMap>");
		FeedStream(stream, 1, false);
		stream << QStringLiteral("#include <QtCore/QSet>");
		FeedStream(stream, 1, false);

		// remove qt types from list
		complexTypeList.remove(QStringLiteral("QByteArray"));
		complexTypeList.remove(QStringLiteral("QString"));
		complexTypeList.remove(QStringLiteral("QList"));
		if (!complexTypeList.isEmpty()){
			FeedStream(stream, 1, false);
		}

		// save already included files, to avoid duplicates
		QSet<QString> customIncluded;

		// Add user custom types
		if (addDependenciesInclude){
			imtsdl::SdlFieldList relatedFields = sdlType.GetFields();
			for (const imtsdl::CSdlField& field: relatedFields){
				bool isCustom = false;
				ConvertType(field, &isCustom);
				if (!isCustom){
					continue;
				}

				imtsdl::CSdlType foundType;
				bool isFound = GetSdlTypeForField(field, m_sdlTypeListCompPtr->GetSdlTypes(false), foundType);
				if (!isFound){
					SendCriticalMessage(0, QString("Unable to find type for %1 of %2").arg(field.GetId(), sdlType.GetName()));
					I_CRITICAL();

					return TS_INVALID;
				}

				if (foundType.IsExternal()){
					const QString relativeIncludePath = ResolveRelativeHeaderFileForType(foundType, m_argumentParserCompPtr->GetHeadersIncludePaths());
					if (!relativeIncludePath.isEmpty() && !customIncluded.contains(relativeIncludePath)){
						stream << QStringLiteral("#include <");
						stream << relativeIncludePath << '>';
						FeedStream(stream, 1, false);
						customIncluded << relativeIncludePath;
					}
				}
			}
		}
	}

	FeedStream(stream, 2, false);

	// namespace begin
	QString namespaceString;
	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlType.GetSchemaParams());
	if (!sdlNamespace.isEmpty()){
		namespaceString = QStringLiteral("namespace ");
		namespaceString+= sdlNamespace;
		namespaceString += QStringLiteral("\n{");
	}

	if (!namespaceString.isEmpty()){
		stream << namespaceString;
		FeedStream(stream, 3, false);
	}

	// class begin
	stream << QStringLiteral("class C") << sdlType.GetName() << '\n';
	stream << QStringLiteral("{");
	FeedStream(stream, 1, false);

	stream << QStringLiteral("public:");
	FeedStream(stream, 1, false);

	GenerateVersionStruct(stream, sdlType);

	stream.flush();

	return true;
}


bool CSdlClassCodeGeneratorComp::EndHeaderClassFile(const imtsdl::CSdlType& sdlType)
{
	QTextStream stream(m_headerFilePtr.GetPtr());

	FeedStream(stream, 1 , false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("// help methods");
	FeedStream(stream, 1 , false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("template <class T>");
	FeedStream(stream, 1 , false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("[[nodiscard]] static bool ComparePointers(const std::unique_ptr<T>& p1, const std::unique_ptr<T>& p2)");
	FeedStream(stream, 1 , false);

	FeedStreamHorizontally(stream);
	stream << '{';
	FeedStream(stream, 1 , false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("if (p1 == nullptr && p2 == nullptr){");
	FeedStream(stream, 1 , false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("return true;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << '}';
	FeedStream(stream, 1 , false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("else if (p1 != nullptr && p2 != nullptr){");
	FeedStream(stream, 1 , false);

	FeedStreamHorizontally(stream, 3);
	stream << QStringLiteral("return *p1 == *p2;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << '}';
	FeedStream(stream, 2 , false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << '}';
	FeedStream(stream, 1 , false);

	return true;
}


bool CSdlClassCodeGeneratorComp::BeginSourceClassFile(const imtsdl::CSdlType& sdlType, bool addSelfHeaderInclude)
{
	QTextStream stream(m_sourceFilePtr.GetPtr());

	// include section
	if (addSelfHeaderInclude){
		stream << QStringLiteral("#include \"C");
		stream << sdlType.GetName() << QStringLiteral(".h\"");
		FeedStream(stream, 3);
	}

	// namespace begin
	QString namespaceString;
	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlType.GetSchemaParams());
	if (!sdlNamespace.isEmpty()){
		namespaceString = QStringLiteral("namespace ");
		namespaceString+= sdlNamespace;
		namespaceString += QStringLiteral("\n{");
	}

	if (!namespaceString.isEmpty()){
		stream << namespaceString;
		FeedStream(stream, 3);
	}

	// Add compare operator
	const QString sdlVersion = GetTypeVerstion(sdlType);

	// a)declare
	stream << QStringLiteral("bool ");
	stream << 'C' << sdlType.GetName() << ':' << ':';
	stream << sdlVersion;
	stream << QStringLiteral("::operator==(const ");
	stream << 'C' << sdlType.GetName() << ':' << ':';
	stream << sdlVersion;
	stream << QStringLiteral("& other) const\n{");
	FeedStream(stream, 1, false);

	// b)implement
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("bool retVal = true;");
	FeedStream(stream, 1, false);
	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("retVal = retVal && ComparePointers(");
		stream << sdlField.GetId();
		stream << QStringLiteral(", other.");
		stream << sdlField.GetId();
		stream << QStringLiteral(");");
		FeedStream(stream, 1, false);
	}

	// c)complete
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return retVal;");
	FeedStream(stream, 1, false);
	stream << '}';
	FeedStream(stream, 3);

	return true;
}


bool CSdlClassCodeGeneratorComp::EndClassFiles(const imtsdl::CSdlType& sdlType)
{
	QString namespaceString;

	// finish header
	// end of namespace
	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlType.GetSchemaParams());
	if (!sdlNamespace.isEmpty()){
		namespaceString += QStringLiteral("} // namespace ");
		namespaceString += sdlNamespace;
	}

	EndHeaderClassFile(sdlType);
	QTextStream headerStream(m_headerFilePtr.GetPtr());
	headerStream << QStringLiteral("};");
	FeedStream(headerStream, 3, false);
	if (!namespaceString.isEmpty()){
		headerStream << namespaceString;
		FeedStream(headerStream, 2, false);
	}

	// add Q_DECLARE_METATYPE macro to use the type as a custom type in QVariant
	FeedStream(headerStream, 1, false);
	headerStream << QStringLiteral("Q_DECLARE_METATYPE(");
	if (sdlNamespace.length() > 0){
		headerStream << sdlNamespace;
		headerStream << ':' << ':';
	}
	headerStream << 'C' << sdlType.GetName();
	headerStream << ':' << ':';
	headerStream << GetTypeVerstion(sdlType);
	headerStream << QStringLiteral(");");

	FeedStream(headerStream, 3, true);

	// finish source
	QTextStream sourceStream(m_sourceFilePtr.GetPtr());
	if (!namespaceString.isEmpty()){
		sourceStream << namespaceString;
		FeedStream(sourceStream, 2);
	}
	sourceStream.flush();

	CloseFiles();

	return true;
}


void CSdlClassCodeGeneratorComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	I_CRITICAL();

	m_headerFilePtr->remove();
	m_sourceFilePtr->remove();
}


void CSdlClassCodeGeneratorComp::GenerateMetaInfo(
			QTextStream& stream,
			const imtsdl::CSdlType& sdlType,
			uint indents)
{
	FeedStreamHorizontally(stream, indents);
	// create struct to store field id list
	stream << QStringLiteral("struct ");
	stream << GetCapitalizedValue(sdlType.GetName());
	stream << QStringLiteral("Fields");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, indents);
	stream << '{';

	// create variables for all fields
	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, indents + 1);
		stream << QStringLiteral("static const inline QString ");
		stream << GetCapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(" = \"");
		stream << sdlField.GetId();
		stream << '"' << ';';
	}

	// finish struct
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, indents);
	stream << '}' << ';';
	FeedStream(stream, 1, false);
}


void CSdlClassCodeGeneratorComp::GenerateVersionStruct(
			QTextStream& stream,
			const imtsdl::CSdlType& sdlType,
			uint indents)
{
	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlType.GetSchemaParams());

	// main struct begin
	const QString sdlVersion = GetTypeVerstion(sdlType);
	FeedStreamHorizontally(stream, indents);
	stream << QStringLiteral("// ");
	stream << sdlVersion;
	stream << QStringLiteral(" struct");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, indents);
	stream << QStringLiteral("struct ");
	stream << sdlVersion;
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, indents);
	stream << '{';
	FeedStream(stream, 1, false);

	// add metainfo
	GenerateMetaInfo(stream, sdlType, indents + 1);
	FeedStream(stream, 1, false);

	// add types members
	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		const QString convertedType = OptListConvertTypeWithNamespaceStruct(field, sdlNamespace, *m_sdlTypeListCompPtr, true);
		FeedStreamHorizontally(stream, indents + 1);
		stream << QStringLiteral("std::unique_ptr<");
		stream << convertedType;
		stream << QStringLiteral("> ");
		stream << field.GetId() << ';';
		FeedStream(stream, 1, false);
	}
	FeedStream(stream, 1, false);

	// Add compare operator
	FeedStreamHorizontally(stream, indents + 1);
	stream << QStringLiteral("bool operator==(const ");
	stream << sdlVersion;
	stream << QStringLiteral("& other) const;");
	FeedStream(stream, 1, false);

	// Add invrted compare operator
	FeedStreamHorizontally(stream, indents + 1);
	stream << QStringLiteral("bool operator!=(const ");
	stream << sdlVersion;
	stream << QStringLiteral("& other) const { return !(operator==(other)); }");
	FeedStream(stream, 1, false);

	// end of struct
	FeedStreamHorizontally(stream, indents);
	stream << '}' << ';';
	FeedStream(stream, 1, false);
}


} // namespace imtsdlgenv2

