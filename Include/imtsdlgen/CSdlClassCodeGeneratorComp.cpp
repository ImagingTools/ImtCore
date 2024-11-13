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


namespace imtsdlgen
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

			ifile::CFileNameParam headerFileNameParam;
			headerFileNameParam.SetPath(outputDirectoryPath + "/C" + sdlType.GetName() + ".h");
			paramsSet.SetEditableParameter(QByteArrayLiteral("HeaderFile"), &headerFileNameParam);

			iprm::COptionsManager baseClassDirectivesList;
			for (auto iter = baseClassList.cbegin(); iter != baseClassList.cend(); ++iter){
				baseClassDirectivesList.InsertOption(iter.value(), iter.key().toUtf8());
			}

			int extendResult = m_baseClassExtenderCompPtr->DoProcessing(&paramsSet, &baseClassDirectivesList, nullptr);
			if (extendResult != TS_OK){
				SendErrorMessage(0, "Unable to extend");
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
	QTextStream ifStream(m_headerFilePtr.GetPtr());

	ifStream << QStringLiteral("#pragma once");
	FeedStream(ifStream, 3, false);

	QSet<QString> complexTypeList;
	bool isQtCommentAdded = false;
	bool hasComplexTypes = IsTypeHasNonFundamentalTypes(sdlType, &complexTypeList);

	if (hasComplexTypes){
		// Add Qt types
		if (complexTypeList.contains(QStringLiteral("QByteArray"))){
			if (!isQtCommentAdded){
				ifStream << QStringLiteral("// Qt includes");
				FeedStream(ifStream, 1, false);
				isQtCommentAdded = true;
			}
			ifStream << QStringLiteral("#include <QtCore/QByteArray>");
			FeedStream(ifStream, 1, false);
		}
		if (complexTypeList.contains(QStringLiteral("QString"))){
			if (!isQtCommentAdded){
				ifStream << QStringLiteral("// Qt includes");
				FeedStream(ifStream, 1, false);
				isQtCommentAdded = true;
			}
			ifStream << QStringLiteral("#include <QtCore/QString>");
			FeedStream(ifStream, 1, false);
		}
		if (complexTypeList.contains(QStringLiteral("QList"))){
			if (!isQtCommentAdded){
				ifStream << QStringLiteral("// Qt includes");
				FeedStream(ifStream, 1, false);
				isQtCommentAdded = true;
			}
			ifStream << QStringLiteral("#include <QtCore/QList>");
			FeedStream(ifStream, 1, false);
		}
		if (!isQtCommentAdded){
			ifStream << QStringLiteral("// Qt includes");
			FeedStream(ifStream, 1, false);
			isQtCommentAdded = true;
		}

		// add required types
		ifStream << QStringLiteral("#include <QtCore/QVariant>");
		FeedStream(ifStream, 1, false);
		ifStream << QStringLiteral("#include <QtCore/QVariantMap>");
		FeedStream(ifStream, 1, false);
		ifStream << QStringLiteral("#include <QtCore/QSet>");
		FeedStream(ifStream, 1, false);

		// remove qt types from list
		complexTypeList.remove(QStringLiteral("QByteArray"));
		complexTypeList.remove(QStringLiteral("QString"));
		complexTypeList.remove(QStringLiteral("QList"));
		if (!complexTypeList.isEmpty()){
			FeedStream(ifStream, 1, false);
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
						ifStream << QStringLiteral("#include <");
						ifStream << relativeIncludePath << '>';
						FeedStream(ifStream, 1, false);
						customIncluded << relativeIncludePath;
					}
				}
			}
		}
	}

	FeedStream(ifStream, 2, false);

	// namespace begin
	QString namespaceString;
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr);
	if (!sdlNamespace.isEmpty()){
		namespaceString = QStringLiteral("namespace ");
		namespaceString+= sdlNamespace;
		namespaceString += QStringLiteral("\n{");
	}

	if (!namespaceString.isEmpty()){
		ifStream << namespaceString;
		FeedStream(ifStream, 3, false);
	}

	// class begin
	ifStream << QStringLiteral("class C") << sdlType.GetName() << '\n';
	ifStream << QStringLiteral("{");
	FeedStream(ifStream, 1, false);

	ifStream << QStringLiteral("public:");
	FeedStream(ifStream, 1, false);

	// add metainfo
	GenerateMetaInfo(ifStream, sdlType);
	FeedStream(ifStream, 1);

	// default constructor for defining primitive types
	if (IsTypeHasFundamentalTypes(sdlType)){
		ifStream << QStringLiteral("\tC") << sdlType.GetName() << QStringLiteral("();");
		FeedStream(ifStream, 2);
	}

	// defining member's access methods
	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
		ifStream << GenerateAccessMethods(sdlField);
		FeedStream(ifStream);
	}
	ifStream.flush();

	return true;
}


bool CSdlClassCodeGeneratorComp::EndHeaderClassFile(const imtsdl::CSdlType& sdlType)
{
	QTextStream ifStream(m_headerFilePtr.GetPtr());

	// Add compare operator
	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("bool operator==(const ");
	ifStream << 'C' << sdlType.GetName();
	ifStream << QStringLiteral("& other) const;");
	FeedStream(ifStream, 1, false);

	// Add invrted compare operator
	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("bool operator!=(const ");
	ifStream << 'C' << sdlType.GetName();
	ifStream << QStringLiteral("& other) const { return !(operator==(other)); }");
	FeedStream(ifStream, 2, false);

	// defining class members
	ifStream << QStringLiteral("private:");
	FeedStream(ifStream, 1, false);

	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
		ifStream << '\t' << ConvertTypeWithNamespace(sdlField, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr);
		ifStream << " m_" << GetDecapitalizedValue(sdlField.GetId()) << ";";
		FeedStream(ifStream, 1, false);
	}

	ifStream << QStringLiteral("\tQSet<QByteArray/*FieldId*/> ") << s_variantMapClassMemberName << ';';

	FeedStream(ifStream);

	return true;
}


bool CSdlClassCodeGeneratorComp::BeginSourceClassFile(const imtsdl::CSdlType& sdlType, bool addSelfHeaderInclude)
{
	QTextStream ifStream(m_sourceFilePtr.GetPtr());

	// include section
	if (addSelfHeaderInclude){
		ifStream << QStringLiteral("#include \"C");
		ifStream << sdlType.GetName() << QStringLiteral(".h\"");
		FeedStream(ifStream, 3);
	}

	// namespace begin
	QString namespaceString;
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr);
	if (!sdlNamespace.isEmpty()){
		namespaceString = QStringLiteral("namespace ");
		namespaceString+= sdlNamespace;
		namespaceString += QStringLiteral("\n{");
	}

	if (!namespaceString.isEmpty()){
		ifStream << namespaceString;
		FeedStream(ifStream, 3);
	}

	// default constructor implementation
	if (IsTypeHasFundamentalTypes(sdlType)){
		ifStream << ConvertType(sdlType.GetName()) << QStringLiteral("::") << ConvertType(sdlType.GetName());
		ifStream << QStringLiteral("()\n:");
		bool isAdded = false;

		imtsdl::SdlFieldList sdlFieldList = sdlType.GetFields();

		for (int i = 0; i < sdlFieldList.count(); ++i){
			imtsdl::CSdlField& sdlField = sdlFieldList[i];
			bool isComplex = true;
			bool isArray = true;
			const QString convertedType = ConvertType(sdlField, nullptr, &isComplex, &isArray);
			if (isComplex || isArray){
				// we need to initialize only fundamental types
				continue;
			}

			if (isAdded){
				ifStream << ',';
				FeedStream(ifStream, 1, false);
			}

			ifStream << '\t' << "m_" << GetDecapitalizedValue(sdlField.GetId()) << '(';

			if (convertedType == QStringLiteral("int") ||
				convertedType == QStringLiteral("long") ||
				convertedType == QStringLiteral("long long"))
			{
				ifStream << QStringLiteral("0");
			}
			else if (convertedType == QStringLiteral("float")){
				ifStream << QStringLiteral("0.00f");
			}
			else if (convertedType == QStringLiteral("double")){
				ifStream << QStringLiteral("0.00");
			}
			else if (convertedType == QStringLiteral("bool")){
				ifStream << QStringLiteral("false");
			}
			ifStream << (')');
			isAdded = true;
		}

		ifStream << QStringLiteral("\n{\n}");
		FeedStream(ifStream, 2);
	}

	// access methods implementation
	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
		GenerateAccessMethodsImpl(ifStream, sdlType.GetName(), sdlField);
	}

	return true;
}


bool CSdlClassCodeGeneratorComp::EndSourceClassFile(const imtsdl::CSdlType& sdlType)
{
	QTextStream ifStream(m_sourceFilePtr.GetPtr());

	// Add compare operator
	// a)declare
	ifStream << QStringLiteral("bool ");
	ifStream << 'C' << sdlType.GetName();
	ifStream << QStringLiteral("::operator==(const ");
	ifStream << 'C' << sdlType.GetName();
	ifStream << QStringLiteral("& other) const\n{");
	FeedStream(ifStream, 1, false);

	// b)implement
	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("bool retVal = true;");
	FeedStream(ifStream, 1, false);
	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
		FeedStreamHorizontally(ifStream);
		ifStream << QStringLiteral("retVal = retVal && (m_");
		ifStream << GetDecapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral(" == other.m_");
		ifStream << GetDecapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral(");");
		FeedStream(ifStream, 1, false);
	}

	// c)complete
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("return retVal;");
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 3);

	return true;
}


bool CSdlClassCodeGeneratorComp::EndClassFiles(const imtsdl::CSdlType& sdlType)
{
	QString namespaceString;

	// finish header
	// end of namespace
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr);
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
	headerStream << QStringLiteral(");");

	FeedStream(headerStream, 3, true);

	// finish source
	EndSourceClassFile(sdlType);
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


QString CSdlClassCodeGeneratorComp::GenerateAccessMethods(
			const imtsdl::CSdlField& sdlField,
			uint indents,
			bool generateGetter,
			bool generateSetter,
			bool generateExistenceChecks)
{
	QString retVal;

	if (generateGetter){
		FeedLineHorizontally(retVal, indents);

		retVal += QStringLiteral("[[nodiscard]] ");
		retVal += ConvertTypeWithNamespace(sdlField, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr);;
		retVal += QStringLiteral(" Get") + GetCapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral("() const;\n");
	}

	if (generateSetter){
		FeedLineHorizontally(retVal, indents);

		retVal += QStringLiteral("void Set") + GetCapitalizedValue(sdlField.GetId());
		retVal += '(';

		bool isCustom = false;
		const QString originalSchemaNamespace = m_originalSchemaNamespaceCompPtr->GetText();
		const QString convertedType = ConvertTypeWithNamespace(
					sdlField,
					originalSchemaNamespace,
					*m_sdlTypeListCompPtr,
					&isCustom);

		bool isComplex = sdlField.IsArray() ||
					sdlField.GetType() == QStringLiteral("String") ||
					sdlField.GetType() == QStringLiteral("ID") ||
					isCustom;

		if (isComplex){
			retVal += QStringLiteral("const ");
		}

		retVal += convertedType;

		if (isComplex){
			retVal += '&';
		}

		retVal += ' ';
		retVal += GetDecapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral(");\n");

		// declare list insertion method
		if (sdlField.IsArray()){
			bool isCustom = false;
			QString convertedArrayType = ConvertType(sdlField.GetType(), &isCustom);
			if (!originalSchemaNamespace.isEmpty()){
				if (isCustom){
					imtsdl::CSdlType typeForField;
					const bool isFound = GetSdlTypeForField(sdlField, m_sdlTypeListCompPtr->GetSdlTypes(false), typeForField);
					Q_ASSERT(isFound);
					const QString typeNamespace = typeForField.GetNamespace();
					if (typeNamespace != originalSchemaNamespace){
						convertedArrayType.prepend(typeNamespace);
						// use global namespace
						if (!convertedArrayType.startsWith(QStringLiteral("::"))){
							convertedArrayType.prepend(QStringLiteral("::"));
						}
					}
				}
			}

			bool isComplex = sdlField.GetType() == QStringLiteral("String") ||
							 sdlField.GetType() == QStringLiteral("ID") ||
							 isCustom;

			FeedLineHorizontally(retVal, indents);
			retVal += QStringLiteral("void Add");
			retVal += GetCapitalizedValue(sdlField.GetId());
			retVal += QStringLiteral("Element(");

			if (isComplex){
				retVal += QStringLiteral("const ");
			}

			retVal += convertedArrayType;

			if (isComplex){
				retVal += '&';
			}

			retVal += QStringLiteral(" element);\n");
		}

		// declare reset metod
		FeedLineHorizontally(retVal, indents);
		retVal += QStringLiteral("void Reset");
		retVal += GetCapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral("();\n");
	}

	if (generateExistenceChecks){
		FeedLineHorizontally(retVal, indents);
		retVal += QStringLiteral("[[nodiscard]] bool Has");
		retVal += GetCapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral("() const;\n");
	}

	return retVal;
}


void CSdlClassCodeGeneratorComp::GenerateAccessMethodsImpl(
			QTextStream& stream,
			const QString className,
			const imtsdl::CSdlField& sdlField,
			uint indents,
			bool generateGetter,
			bool generateSetter,
			bool generateExistenceChecks)
{
	if (generateGetter){
		FeedStreamHorizontally(stream, indents);

		stream << ConvertTypeWithNamespace(
					sdlField,
					m_originalSchemaNamespaceCompPtr->GetText(),
					*m_sdlTypeListCompPtr);

		stream << QStringLiteral(" C") + className + QStringLiteral("::");
		stream << QStringLiteral("Get") + GetCapitalizedValue(sdlField.GetId());
		stream << QStringLiteral("() const\n{");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, indents + 1);
		stream << QStringLiteral("return m_");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(";\n}");
		FeedStream(stream, 3, false);
	}

	if (generateSetter){
		// name of method
		FeedStreamHorizontally(stream, indents);
		stream << QStringLiteral("void");
		stream << QStringLiteral(" C") + className + QStringLiteral("::");
		stream << QStringLiteral("Set") + GetCapitalizedValue(sdlField.GetId());
		stream << '(';

		bool isCustom = false;
		const QString convertedType = ConvertTypeWithNamespace(
			sdlField,
			m_originalSchemaNamespaceCompPtr->GetText(),
			*m_sdlTypeListCompPtr,
			&isCustom);

		bool isComplex = sdlField.IsArray() ||
					sdlField.GetType() == QStringLiteral("String") ||
					sdlField.GetType() == QStringLiteral("ID") ||
					isCustom;

		if (isComplex){
			stream << QStringLiteral("const ");
		}

		stream << convertedType;

		if (isComplex){
			stream << '&';
		}

		stream << ' ';
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(")\n{");
		FeedStream(stream, 1, false);

		// impl of method
		// a) check
		FeedStreamHorizontally(stream, indents + 1);
		stream << QStringLiteral("if (");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(" != m_");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(" || !");
		stream << FromInternalMapCheckString(sdlField);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, indents + 2);
		// b) set value
		stream << QStringLiteral("m_");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(" = ");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(";");
		FeedStream(stream, 1, false);
		// c) set status (isSet)
		FeedStreamHorizontally(stream, indents + 2);
		stream << s_variantMapClassMemberName << QStringLiteral(" << ") << '"' << sdlField.GetId() << '"' << ';';
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, indents + 1);
		stream << '}';
		FeedStream(stream, 1, false);

		stream << '}';
		FeedStream(stream, 3);

		// add spectal list insertion method
		if (sdlField.IsArray()){
			bool isCustom = false;
			QString convertedArrayType = ConvertType(sdlField.GetType(), &isCustom);
			const QString originalSchemaNamespace = m_originalSchemaNamespaceCompPtr->GetText();
			if (!originalSchemaNamespace.isEmpty()){
				if (isCustom){
					imtsdl::CSdlType typeForField;
					const bool isFound = GetSdlTypeForField(sdlField, m_sdlTypeListCompPtr->GetSdlTypes(false), typeForField);
					Q_ASSERT(isFound);
					const QString typeNamespace = typeForField.GetNamespace();
					if (typeNamespace != originalSchemaNamespace){
						convertedArrayType.prepend(typeNamespace);
						// use global namespace
						if (!convertedArrayType.startsWith(QStringLiteral("::"))){
							convertedArrayType.prepend(QStringLiteral("::"));
						}
					}
				}
			}

			bool isComplex = sdlField.GetType() == QStringLiteral("String") ||
							 sdlField.GetType() == QStringLiteral("ID") ||
							 isCustom;

			FeedStreamHorizontally(stream, indents);
			stream << QStringLiteral("void");
			stream << QStringLiteral(" C") + className + QStringLiteral("::");
			stream << QStringLiteral("Add");
			stream << GetCapitalizedValue(sdlField.GetId());
			stream << QStringLiteral("Element(");

			if (isComplex){
				stream << QStringLiteral("const ");
			}

			stream << convertedArrayType;

			if (isComplex){
				stream << '&';
			}

			stream << QStringLiteral(" element)");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, indents);
			stream << '{';
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, indents + 1);
			stream << QStringLiteral("m_");
			stream << GetDecapitalizedValue(sdlField.GetId());
			stream << QStringLiteral(" << element;");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, indents);
			stream << '}';
			FeedStream(stream, 3, false);
		}

		GenerateResetMethodImpl(stream, className, sdlField, indents);
	}

	if (generateExistenceChecks){
		FeedStreamHorizontally(stream, indents);
		stream << QStringLiteral("bool");
		stream << QStringLiteral(" C") + className + QStringLiteral("::");
		stream << QStringLiteral("Has");
		stream << GetCapitalizedValue(sdlField.GetId());
		stream << QStringLiteral("() const");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, indents);
		stream << '{';
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, indents + 1);
		stream << QStringLiteral("return ");
		stream << FromInternalMapCheckString(sdlField);
		stream << QStringLiteral(";");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, indents);
		stream << '}';
		FeedStream(stream, 3, false);
	}
}



void CSdlClassCodeGeneratorComp::GenerateResetMethodImpl(
			QTextStream& stream,
			const QString className,
			const imtsdl::CSdlField& sdlField,
			uint indents)
{
	FeedStreamHorizontally(stream, indents);
	stream << QStringLiteral("void C");
	stream << className;
	stream << QStringLiteral("::Reset") << GetCapitalizedValue(sdlField.GetId());
	stream << QStringLiteral("()");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, indents);
	stream << '{';
	FeedStream(stream, 1, false);

	// first remove value from used values
	FeedStreamHorizontally(stream, indents + 1);
	stream << s_variantMapClassMemberName << QStringLiteral(".remove(") << '"' << sdlField.GetId() << '"' << ')' << ';';
	FeedStream(stream, 1, false);

	// and reset a value
	FeedStreamHorizontally(stream, indents + 1);
	bool isCustom = false;
	bool isComplex = false;
	bool isArray = false;
	const QString originalSchemaNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	const QString convertedType = ConvertTypeWithNamespace(
					sdlField,
					originalSchemaNamespace,
					*m_sdlTypeListCompPtr,
					&isCustom,
					&isComplex,
					&isArray);
	// list, string, byte has a 'clear' method
	if (isArray || convertedType == QStringLiteral("QString") || convertedType == QStringLiteral("QByteArray")){
		stream << QStringLiteral("m_");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(".clear();");
	}
	// if suctom - just reset with a defaul constructor
	else if (isCustom){
		stream << QStringLiteral("m_");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(" = ");
		stream << convertedType;
		stream << QStringLiteral("();");
	}
	// set default value to scalar types
	else if (!isComplex){
		stream << QStringLiteral("m_");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(" = ");
		if (convertedType == QStringLiteral("int") ||
			convertedType == QStringLiteral("long") ||
			convertedType == QStringLiteral("long long"))
		{
			stream << QStringLiteral("0");
		}
		else if (convertedType == QStringLiteral("float")){
			stream << QStringLiteral("0.00f");
		}
		else if (convertedType == QStringLiteral("double")){
			stream << QStringLiteral("0.00");
		}
		else if (convertedType == QStringLiteral("bool")){
			stream << QStringLiteral("false");
		}
		else {
			I_CRITICAL();
		}
		stream << ';';
	}
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, indents);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStream(stream, 3, false);
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


} // namespace imtsdlgen

