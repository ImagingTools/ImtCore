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

	if (!m_argumentParserCompPtr->IsCppEnabled()){
		return TS_OK;
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

	imtsdl::SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes(true);	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		m_headerFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".h"));
		m_sourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp"));
		const bool hasExtDeps = m_argumentParserCompPtr->GetAutoLinkLevel() != imtsdl::ISdlProcessArgumentsParser::ALL_NONE;

		// First create all files with basic methods
		if (!BeginClassFiles(sdlType, hasExtDeps || !joinHeaders, !joinSources)){
			SendErrorMessage(0, QString("Unable to begin files"));
			I_CRITICAL();

			return TS_INVALID;
		}

		// Then let extenders to make changes. Add new transformation methods (JSON, GQL, ...)
		const int extendersCount = m_codeGeneratorExtenderListCompPtr.GetCount();
		for (int i = 0; i < extendersCount; ++i){
			imtsdlgen::ICxxFileProcessor* extenderPtr = m_codeGeneratorExtenderListCompPtr[i];
			Q_ASSERT(extenderPtr != nullptr);

			bool extendSuccess = extenderPtr->ProcessType(sdlType, m_headerFilePtr.GetPtr(), m_sourceFilePtr.GetPtr());
			if (!extendSuccess){
				return TS_INVALID;
			}
		}

		// And complete the processing
		EndClassFiles(sdlType);

		// Extend with base class if required
		QMap<QString, QString> baseClassList = m_argumentParserCompPtr->GetBaseClassList();
		if (!baseClassList.isEmpty()){
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
				// first join enums
				const imtsdl::SdlEnumList enumList = m_sdlEnumListCompPtr->GetEnums(false);
				for (const imtsdl::CSdlEnum& sdlEnum: enumList){
					filterParams.InsertOption(sdlEnum.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount()));
					SendVerboseMessage(QString("Add join enum file '%1. Total: %2").arg(sdlEnum.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount())));
				}

				// then join types
				for (const imtsdl::CSdlType& sdlType: sdlTypeList){
					filterParams.InsertOption("C" + sdlType.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount()));
					SendVerboseMessage(QString("Add join file '%1'. Total: %2").arg("C" + sdlType.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount())));
				}

				outputFileNameParam.SetPath(joinRules[imtsdl::ISdlProcessArgumentsParser::s_headerFileType]);
				int joinProcessResult = m_filesJoinerCompPtr->DoProcessing(&inputParams, &filterParams, nullptr);
				if (joinProcessResult != TS_OK){
					SendCriticalMessage(0, "Unable to join header files");
					I_CRITICAL();

					return TS_INVALID;
				}

				// cleanup joined files
				for (const imtsdl::CSdlEnum& sdlEnum: enumList){
					QFile::remove(outputDirectoryPath + '/' + sdlEnum.GetName() + ".h");
				}
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

	return TS_OK;
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

	QList<imtsdl::IncludeDirective> includeDirectivesList;
	for (int i = 0; i < m_includeDirectivesProviderListCompPtr.GetCount(); ++i){
		imtsdlgen::IIncludeDirectivesProvider* providerPtr = m_includeDirectivesProviderListCompPtr[i];
		if (providerPtr != nullptr){
			includeDirectivesList << providerPtr->GetIncludeDirectives();
		}
	}

	// add CXX includes for optional
	static imtsdl::IncludeDirective memoryIncludeDirective = CreateCxxDirective(QStringLiteral("<optional>"));
	includeDirectivesList << memoryIncludeDirective;

	QSet<QString> complexTypeList;
	bool hasComplexTypes = IsTypeHasNonFundamentalTypes(sdlType, &complexTypeList);
	if (hasComplexTypes){
		// Add Qt types
		if (complexTypeList.contains(QStringLiteral("QByteArray"))){
			static imtsdl::IncludeDirective byteArrayDirective = CreateQtDirective(QStringLiteral("<QtCore/QByteArray>"));
			includeDirectivesList << byteArrayDirective;
		}
		if (complexTypeList.contains(QStringLiteral("QString"))){
			static imtsdl::IncludeDirective stringDirective = CreateQtDirective(QStringLiteral("<QtCore/QString>"));
			includeDirectivesList << stringDirective;
		}
		if (complexTypeList.contains(QStringLiteral("QList"))){
			static imtsdl::IncludeDirective listDirective = CreateQtDirective(QStringLiteral("<QtCore/QList>"));
			includeDirectivesList << listDirective;
		}

		static imtsdl::IncludeDirective variantDirective = CreateQtDirective(QStringLiteral("<QtCore/QVariant>"));
		includeDirectivesList << variantDirective;
		static imtsdl::IncludeDirective variantMapDirective = CreateQtDirective(QStringLiteral("<QtCore/QVariantMap>"));
		includeDirectivesList << variantMapDirective;
		static imtsdl::IncludeDirective setDirective = CreateQtDirective(QStringLiteral("<QtCore/QSet>"));
		includeDirectivesList << setDirective;

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

				std::shared_ptr<imtsdl::CSdlEntryBase> foundType = GetSdlTypeOrEnumForField(field, m_sdlTypeListCompPtr->GetSdlTypes(false), m_sdlEnumListCompPtr->GetEnums(false));
				if (!foundType){
					SendCriticalMessage(0, QString("Unable to find type for %1 of %2").arg(field.GetId(), sdlType.GetName()));
					I_CRITICAL();

					return false;
				}

				if (foundType->IsExternal()){
					QString resolvedPath = ResolveRelativeHeaderFileForType(*foundType, m_argumentParserCompPtr->GetHeadersIncludePaths(), false);
					if (resolvedPath.isEmpty()){
						SendErrorMessage(0, QString("Unable to find header file for type of '%1' in ").arg(field.GetId(), sdlType.GetName()));

						return false;
					}

					const QString relativeIncludePath = '<' + resolvedPath + '>';
					if (!resolvedPath.isEmpty() && !customIncluded.contains(relativeIncludePath)){
						includeDirectivesList << CreateCustomDirective(relativeIncludePath);
						customIncluded << relativeIncludePath;
					}
				}
			}
		}
	}


	// add all required includes
	QList<imtsdl::Priority> orderList = {
		imtsdl::P_C,
		imtsdl::P_OS_API,
		imtsdl::P_QT,
		imtsdl::P_ACF,
		imtsdl::P_IMT,
		imtsdl::P_CUSTOM
	};

	QMutableListIterator includeIter(includeDirectivesList);
	while(!orderList.isEmpty()){
		imtsdl::Priority currentPriority = orderList.takeFirst();
		bool addRemark = true;
		bool isAdded = false;
		while(includeIter.hasNext()){
			imtsdl::IncludeDirective directive = includeIter.next();
			if (directive.priority == currentPriority){
				isAdded = true;
				if (addRemark){
					if (!directive.remark.startsWith(QStringLiteral("//"))){
						stream << QStringLiteral("// ");
					}
					stream << directive.remark;
					FeedStream(stream, 1, false);
					addRemark = false;
				}
				stream << QStringLiteral("#include ");
				stream << directive.path;
				FeedStream(stream, 1, false);

				includeIter.remove();
			}
		}
		if (isAdded){
			FeedStream(stream, 1, false);
		}
		includeIter.toFront();
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
	FeedStream(stream, 1, false);

	stream.flush();

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

	QTextStream headerStream(m_headerFilePtr.GetPtr());
	// end of struct
	FeedStreamHorizontally(headerStream, 1);
	headerStream << '}' << ';';
	FeedStream(headerStream, 1, false);

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
	headerStream << GetSdlEntryVersion(sdlType);
	headerStream << QStringLiteral(");");

	FeedStream(headerStream, 3, true);

	// finish source
	QTextStream sourceStream(m_sourceFilePtr.GetPtr());
	if (!namespaceString.isEmpty()){
		sourceStream << namespaceString;
		FeedStream(sourceStream, 2);
	}
	sourceStream.flush();

	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	m_headerFilePtr.Reset();
	m_sourceFilePtr.Reset();

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
	const QString sdlVersion = GetSdlEntryVersion(sdlType);
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
		const QString convertedType = OptListConvertTypeWithNamespaceStruct(
			field,
			sdlNamespace,
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			true,
			nullptr,
			nullptr,
			nullptr,
			nullptr);
		FeedStreamHorizontally(stream, indents + 1);
		stream << QStringLiteral("std::optional<");
		stream << convertedType;
		stream << QStringLiteral("> ");
		stream << field.GetId() << ';';
		FeedStream(stream, 1, false);
	}
	FeedStream(stream, 1, false);
}


} // namespace imtsdlgenv2

