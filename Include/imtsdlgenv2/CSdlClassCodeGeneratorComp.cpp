#include <imtsdlgenv2/CSdlClassCodeGeneratorComp.h>


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


iproc::IProcessor::TaskState CSdlClassCodeGeneratorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_originalSchemaNamespaceCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());

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

	if (m_argumentParserCompPtr->IsDependenciesMode() || !m_argumentParserCompPtr->GetDepFilePath().isEmpty()){
		if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
			if (!m_customSchemaParamsCompPtr.IsValid()){
				SendErrorMessage(0, "Application is not configured with custom parameters. Auto join is not possible. Please specify paths to join explicitly(use -J option), or disable join.");

				return TS_INVALID;
			}
			QStringList autoJoinFilePaths = GetAutoJoinedCppFilePaths(*m_customSchemaParamsCompPtr, m_argumentParserCompPtr->GetOutputDirectoryPath(), defaultName);
			PrintFiles(std::cout, autoJoinFilePaths, m_argumentParserCompPtr->GetGeneratorType());
			PrintFiles(m_argumentParserCompPtr->GetDepFilePath(), autoJoinFilePaths, *m_dependentSchemaListCompPtr);
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
			PrintFiles(m_argumentParserCompPtr->GetDepFilePath(), cumulatedFiles, *m_dependentSchemaListCompPtr);
			PrintFiles(std::cout, cumulatedFiles, m_argumentParserCompPtr->GetGeneratorType());
		}

		if (m_argumentParserCompPtr->IsDependenciesMode()){
			return TS_OK;
		}
	}

	imtsdl::SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes(true);
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		m_headerFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".h"));
		m_sourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp"));
		const bool hasExtDeps = m_argumentParserCompPtr->GetAutoLinkLevel() != imtsdl::ISdlProcessArgumentsParser::ALL_NONE;

		// First create all files with basic methods
		if (!BeginClassFiles(sdlType, hasExtDeps || !joinHeaders, !joinSources)){
			SendErrorMessage(0, QString("Unable to process files"));

			return TS_INVALID;
		}

		// Then let extenders to make changes. Add new transformation methods (JSON, GQL, ...)
		const int extendersCount = m_codeGeneratorExtenderListCompPtr.GetCount();
		for (int i = 0; i < extendersCount; ++i){
			imtsdlgen::ICxxFileProcessor* extenderPtr = m_codeGeneratorExtenderListCompPtr[i];
			Q_ASSERT(extenderPtr != nullptr);

			m_headerFilePtr->write(QByteArrayLiteral("\n"));
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

			TaskState extendResult = m_baseClassExtenderCompPtr->DoProcessing(&paramsSet, &baseClassDirectivesList, nullptr);
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
				const imtsdl::SdlEnumList enumList = m_sdlEnumListCompPtr->GetEnums(true);
				for (const imtsdl::CSdlEnum& sdlEnum: enumList){
					filterParams.InsertOption(sdlEnum.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount()));
					SendVerboseMessage(QString("Add join enum file '%1. Total: %2").arg(sdlEnum.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount())));
				}

				// then join unions forward declarations
				const imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(true);
				for (const imtsdl::CSdlUnion& sdlUnion : unionList){
					filterParams.InsertOption(sdlUnion.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount()));
					SendVerboseMessage(QString("Add join enum file '%1. Total: %2").arg(sdlUnion.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount())));
				}

				// then join types
				for (const imtsdl::CSdlType& sdlType: sdlTypeList){
					filterParams.InsertOption("C" + sdlType.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount()));
					SendVerboseMessage(QString("Add join file '%1'. Total: %2").arg("C" + sdlType.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount())));
				}

				// then join unions class definitions
				for (const imtsdl::CSdlUnion& sdlUnion : unionList){
					filterParams.InsertOption(sdlUnion.GetName() + "_ClassDef.h", QByteArray::number(filterParams.GetOptionsCount()));
					SendVerboseMessage(QString("Add join enum file '%1. Total: %2").arg(sdlUnion.GetName() + ".h", QByteArray::number(filterParams.GetOptionsCount())));
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

				std::shared_ptr<imtsdl::CSdlEntryBase> foundType = GetSdlTypeOrEnumOrUnionForField(field, m_sdlTypeListCompPtr->GetSdlTypes(false), m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false));
				if (!foundType){
					SendCriticalMessage(0, QString("Unable to find type for %1 of %2").arg(field.GetId(), sdlType.GetName()));
					I_CRITICAL();

					return false;
				}

				if (foundType->IsExternal()){
					QString resolvedPath = ResolveRelativeHeaderFileForType(*foundType, m_argumentParserCompPtr->GetHeadersIncludePaths());
					if (resolvedPath.isEmpty()){
						QString foundTypeName;
						imtsdl::CSdlType* typePtr = dynamic_cast<imtsdl::CSdlType*>(foundType.get());
						if (typePtr != nullptr){
							foundTypeName = typePtr->GetName();
						}
						else {
							imtsdl::CSdlEnum* enumPtr = dynamic_cast<imtsdl::CSdlEnum*>(foundType.get());
							if (enumPtr !=nullptr){
								foundTypeName = enumPtr->GetName();
							}
						}
						SendErrorMessage(0, QString("Unable to find header file for type '%1' of '%2' in '%3' ").arg(foundTypeName, field.GetId(), sdlType.GetName()));

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

	// version enum
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("enum ProtocolVersion");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("PV_") << GetSdlEntryVersion(sdlType, false) << ',';
	FeedStream(stream, 1, false);

	/// \todo later add other versions here

	// add last alias
	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("PV_LAST = PV_") << GetSdlEntryVersion(sdlType, false);
	FeedStream(stream, 1, false);

	// complete protocol version enum
	FeedStreamHorizontally(stream);
	stream << '}' << ';';
	FeedStream(stream, 2, false);

	GenerateVersionStruct(stream, sdlType);

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

	//implement GetVersionId method
	stream << QStringLiteral("QByteArray C");
	stream << sdlType.GetName();
	stream << ':' << ':';
	stream << GetSdlEntryVersion(sdlType);
	stream << QStringLiteral("::GetVersionId()");
	FeedStream(stream, 1, false);

	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return QByteArrayLiteral(\"");
	stream << GetTypeVersion(sdlType);
	stream << QStringLiteral("\");");
	FeedStream(stream, 1, false);

	stream << '}';
	FeedStream(stream, 3, false);

	// implement coparation operator
	stream << QStringLiteral("bool C");
	stream << sdlType.GetName();
	stream << ':' << ':';
	stream << GetSdlEntryVersion(sdlType);
	stream << QStringLiteral("::operator==(const ");
	stream << GetSdlEntryVersion(sdlType);
	stream << QStringLiteral("& other) const");
	FeedStream(stream, 1, false);

	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return ");
	FeedStream(stream, 1, true);

	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
		FeedStreamHorizontally(stream, 4);
		const QString fieldType = sdlField.GetType();

		// use fuzzy compare for double and float
		if (	!sdlField.IsArray() &&(
					fieldType == QStringLiteral("Double") ||
					fieldType == QStringLiteral("Float")))
		{
			stream << sdlField.GetId();
			stream << QStringLiteral(".has_value() == other.");
			stream << sdlField.GetId();
			stream << QStringLiteral(".has_value() &&");
			FeedStream(stream);

			FeedStreamHorizontally(stream, 4);
			stream << '(' << '(';
			stream << sdlField.GetId();
			stream << QStringLiteral(".has_value() && other.");
			stream << sdlField.GetId();
			stream << QStringLiteral(".has_value()) ?");
			FeedStream(stream);

			FeedStreamHorizontally(stream, 5);
			stream << QStringLiteral("qFuzzyCompare(*");
			stream << sdlField.GetId();
			stream << QStringLiteral(", *other.");
			stream << sdlField.GetId();
			stream << QStringLiteral(") : true)");
		}
		else {
			stream << sdlField.GetId();
			stream << QStringLiteral(" == other.");
			stream << sdlField.GetId();
		}

		stream << QStringLiteral(" &&");
		FeedStream(stream, 1, true);
	}

	stream.seek(stream.pos() - 4); /// < remove last ' &&'
	stream << ';';
	FeedStream(stream, 1, false);

	stream << '}';
	FeedStream(stream, 3, false);

	return true;
}


bool CSdlClassCodeGeneratorComp::EndClassFiles(const imtsdl::CSdlType& sdlType)
{
	// finish header
	QTextStream headerStream(m_headerFilePtr.GetPtr());

	// end of struct
	FeedStreamHorizontally(headerStream, 1);
	headerStream << '}' << ';';
	FeedStream(headerStream, 2, false);

	// member versions
	FeedStreamHorizontally(headerStream);
	headerStream << QStringLiteral("// available version members");
	FeedStream(headerStream, 1, false);

	/// \todo add here ALL versions
	FeedStreamHorizontally(headerStream);
	GenerateVersionMemberDeclaration(headerStream, sdlType, true);
	headerStream << ';';
	FeedStream(headerStream, 2, false);


	// add version-independend read write methods
	const int modifiersCount = m_modifierListCompPtr.GetCount();
	if (modifiersCount > 0){
		FeedStreamHorizontally(headerStream);
		headerStream << QStringLiteral("// serialize methods");
	}
	for (int modifierIndex = 0; modifierIndex < modifiersCount; ++modifierIndex){
		imtsdlgen::ICxxModifier* modifierPtr = m_modifierListCompPtr[modifierIndex];
		Q_ASSERT(modifierPtr != nullptr);

		FeedStream(headerStream, 1, false);

		FeedStreamHorizontally(headerStream);
		GenerateMethodDefinition(headerStream, sdlType, MT_WRITE, *modifierPtr, true);

		FeedStreamHorizontally(headerStream);
		GenerateMethodDefinition(headerStream, sdlType, MT_READ, *modifierPtr, true);

		FeedStreamHorizontally(headerStream);
		GenerateMethodDefinition(headerStream, sdlType, MT_OPT_READ, *modifierPtr, true);
	}

	// end of class
	headerStream << QStringLiteral("};");

	// end of namespace
	QString namespaceString;
	const QString sdlNamespace = GetNamespaceFromSchemaParams(sdlType.GetSchemaParams());
	if (!sdlNamespace.isEmpty()){
		namespaceString += QStringLiteral("} // namespace ");
		namespaceString += sdlNamespace;
	}
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
	QTextStream sourceStream(m_sourceFilePtr.GetPtr());

	if (modifiersCount > 0){
		sourceStream << QStringLiteral("// serialize methods");
		FeedStream(sourceStream, 2, false);
	}
	for (int modifierIndex = 0; modifierIndex < modifiersCount; ++modifierIndex){
		imtsdlgen::ICxxModifier* modifierPtr = m_modifierListCompPtr[modifierIndex];
		Q_ASSERT(modifierPtr != nullptr);

		GenerateMethodDefinition(sourceStream, sdlType, MT_WRITE, *modifierPtr, false);
		GenerateMethodImplementation(sourceStream, sdlType, MT_WRITE, *modifierPtr);
		FeedStream(sourceStream, 2, false);

		GenerateMethodDefinition(sourceStream, sdlType, MT_READ, *modifierPtr, false);
		GenerateMethodImplementation(sourceStream, sdlType, MT_READ, *modifierPtr);
		FeedStream(sourceStream, 2, false);

		GenerateMethodDefinition(sourceStream, sdlType, MT_OPT_READ, *modifierPtr, false);
		GenerateMethodImplementation(sourceStream, sdlType, MT_OPT_READ, *modifierPtr);
		FeedStream(sourceStream, 2, false);
	}

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
			*m_sdlUnionListCompPtr,
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

	// add GetVersionId method
	FeedStreamHorizontally(stream, indents + 1);
	stream << QStringLiteral("static QByteArray GetVersionId();");
	FeedStream(stream, 2, false);

	// add coparation operators
	FeedStreamHorizontally(stream, indents + 1);
	stream << QStringLiteral("[[nodiscard]] bool operator==(const ");
	stream << GetSdlEntryVersion(sdlType);
	stream << QStringLiteral("& other) const;");
	FeedStream(stream, 1, false);

	// add inverted coparation operator
	FeedStreamHorizontally(stream, indents + 1);
	stream << QStringLiteral("[[nodiscard]] bool operator!=(const ");
	stream << GetSdlEntryVersion(sdlType);
	stream << QStringLiteral("& other) const {return !(operator==(other));}");
	FeedStream(stream, 1, false);
}


void CSdlClassCodeGeneratorComp::GenerateMethodDefinition(
			QTextStream& stream,
			const imtsdl::CSdlType& sdlType,
			MetdodType methodType,
			imtsdlgen::ICxxModifier& modifier,
			bool forHeader)
{
	// type
	if (forHeader){
		stream << QStringLiteral("[[nodiscard]] ");
	}
	stream << QStringLiteral("bool ");

	if (!forHeader){
		stream << 'C' << GetCapitalizedValue(sdlType.GetName()) << ':' << ':';
	}

	// optional?
	switch (methodType) {
	case MT_OPT_READ:
	case MT_OPT_WRITE:
		stream << QStringLiteral("Opt");
		break;
	default:
		break;
	}

	// name
	switch (methodType) {
	case MT_OPT_READ:
	case MT_READ:
		stream << modifier.GetReadMethodName();
		break;
	case MT_OPT_WRITE:
	case MT_WRITE:
		stream << modifier.GetWriteMethodName();
		break;
	default:
		break;
	}

	stream << '(';
	// arguments
	imtsdlgen::ICxxModifier::ArgumentList argumentList = modifier.GetArguments();
	Q_ASSERT(!argumentList.isEmpty());

	// container argument
	switch (methodType) {
	case MT_OPT_READ:
	case MT_READ:
		stream << QStringLiteral("const ");
		break;
	default:
		break;
	}

	imtsdlgen::ICxxModifier::Argument containerArgument = argumentList.takeFirst();
	stream << containerArgument.Type << '&' << ' ' << containerArgument.Name;

	// other arguments
	for (const imtsdlgen::ICxxModifier::Argument& argument: std::as_const(argumentList)){
		stream << ',' << ' ' << argument.Type << ' ' << argument.Name;
		if (forHeader && !argument.DefaultValue.isNull()){
			stream << ' ' << '=' << ' ' << argument.DefaultValue;
		}
	}

	stream << QStringLiteral(", ProtocolVersion version");
	if (forHeader){
		switch (methodType) {
			case MT_WRITE:
			case MT_OPT_WRITE:
				stream << QStringLiteral(" = PV_AUTO");
			break;
			default:
				stream << QStringLiteral(" = PV_LAST");
			break;
		}
	}
	stream << ')';

	// const modifier
	switch (methodType) {
	case MT_WRITE:
	case MT_OPT_WRITE:
		stream << QStringLiteral(" const");
	break;
	default:
	break;
	}

	// finalize
	if (forHeader){
		stream << ';';
	}

	FeedStream(stream, 1, false);
}


QString CSdlClassCodeGeneratorComp::GetVersionMemberVariableName(
			const imtsdl::CSdlType& sdlType,
			int /*versionIndex*/) const
{
	return QStringLiteral("Version_") + GetSdlEntryVersion(sdlType, false);
}


void CSdlClassCodeGeneratorComp::GenerateVersionMemberDeclaration(
			QTextStream& stream,
			const imtsdl::CSdlType& sdlType,
			bool optWrap,
			int versionIndex)
{
	if (optWrap){
		stream << QStringLiteral("std::optional<");
	}
	stream << GetSdlEntryVersion(sdlType);
	if (optWrap){
		stream << '>';
	}
	stream << ' ' << GetVersionMemberVariableName(sdlType, versionIndex);
}


void CSdlClassCodeGeneratorComp::GenerateMethodImplementation(
			QTextStream& stream,
			const imtsdl::CSdlType& sdlType,
			MetdodType methodType,
			imtsdlgen::ICxxModifier& modifier)
{
	stream << '{';
	FeedStream(stream, 1, false);

	// auto version check
	switch (methodType) {
		case MT_WRITE:
		case MT_OPT_WRITE:
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("if (version == PV_AUTO){");
			FeedStream(stream, 1, false);

			/// \todo add all versions
			// check if version member is valid
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("if (");
			stream << GetVersionMemberVariableName(sdlType);
			stream << QStringLiteral("){");
			FeedStream(stream, 1, false);

			// call method of  version member
			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("return ");
			stream << GetVersionMemberVariableName(sdlType);
			stream << QStringLiteral("->");
			GenerateMethodCall(stream, sdlType, methodType, modifier);
			stream << ';';
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << '}';
			FeedStream(stream, 1, false);

			// if all versions is invalid, return
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("else {");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("return false;");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << '}';
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << '}';
			FeedStream(stream, 1, false);
		break;

		// if AUTO protocol is not allowed
		default:
			FeedStreamHorizontally(stream);
			stream << QStringLiteral("if (version == PV_AUTO){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("qCritical() << \"AUTO protocol is NOT supported for read methods!\";");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("Q_ASSERT_X(false, __func__, \"AUTO protocol is NOT supported for read methods!\");");
			FeedStream(stream, 2, false);

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("return false;");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream);
			stream << '}';
			FeedStream(stream, 1, false);
		break;
	}

	/// \todo add all versions
	// specific versions
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("else if (version == PV_");
	stream << GetSdlEntryVersion(sdlType, false);
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// auto-init(for read) and checks(for write)
	switch (methodType) {
		case MT_READ:
		case MT_OPT_READ:
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("if (!");
			stream << GetVersionMemberVariableName(sdlType);
			stream << QStringLiteral("){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 3);
			stream << GetVersionMemberVariableName(sdlType);
			stream << QStringLiteral(" = ");
			stream << GetSdlEntryVersion(sdlType);
			stream << QStringLiteral("();");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << '}';
			FeedStream(stream, 2, false);
		break;
		default:
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("if (!");
			stream << GetVersionMemberVariableName(sdlType);
			stream << QStringLiteral("){");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("qCritical() << \"Uninitialized version member\";");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("Q_ASSERT_X(false, __func__, \"Uninitialized version member\");");
			FeedStream(stream, 2, false);

			FeedStreamHorizontally(stream, 3);
			stream << QStringLiteral("return false;");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, 2);
			stream << '}';
			FeedStream(stream, 2, false);
		break;
	}

	FeedStreamHorizontally(stream, 2);
	stream << QStringLiteral("return ");
	stream << GetVersionMemberVariableName(sdlType);
	stream << QStringLiteral("->");
	GenerateMethodCall(stream, sdlType, methodType, modifier);
	stream << ';';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << '}';
	FeedStream(stream, 2, false);

	// default error if invalid version and return
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("qCritical() << \"Invalid version\";");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("Q_ASSERT_X(false, __func__, \"Invalid version\");");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	// end of method
	stream << '}';
	FeedStream(stream, 1, false);
}


void CSdlClassCodeGeneratorComp::GenerateMethodCall(
			QTextStream& stream,
			const imtsdl::CSdlType& sdlType,
			MetdodType methodType,
			imtsdlgen::ICxxModifier& modifier)
{
	// optional?
	switch (methodType) {
	case MT_OPT_READ:
	case MT_OPT_WRITE:
		stream << QStringLiteral("Opt");
		break;
	default:
		break;
	}

	// name
	switch (methodType) {
	case MT_OPT_READ:
	case MT_READ:
		stream << modifier.GetReadMethodName();
		break;
	case MT_OPT_WRITE:
	case MT_WRITE:
		stream << modifier.GetWriteMethodName();
		break;
	default:
		break;
	}

	stream << '(';
	// arguments
	imtsdlgen::ICxxModifier::ArgumentList argumentList = modifier.GetArguments();
	Q_ASSERT(!argumentList.isEmpty());

	imtsdlgen::ICxxModifier::Argument containerArgument = argumentList.takeFirst();
	stream << containerArgument.Name;

	// other arguments
	for (const imtsdlgen::ICxxModifier::Argument& argument: std::as_const(argumentList)){
		stream << ',' << ' ' << argument.Name;
	}
	stream << ')';
}


} // namespace imtsdlgenv2

