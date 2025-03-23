#include "CGqlWrapClassCodeGeneratorComp.h"
#include "imtsdl/CSdlField.h"


// C includes
#include <QtCore/qstring.h>
#include <iostream>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QTextStream>

// Acf includes
#include <istd/CSystem.h>
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>
#include <iprm/CEnableableParam.h>
#include <ifile/CFileNameParam.h>

// ImtCore includes
#include <imtfile/CSimpleFileJoinerComp.h>
#include <imtsdl/CSdlRequest.h>


namespace imtsdlgenv2
{


int CGqlWrapClassCodeGeneratorComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlRequestListCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());

	if (!m_argumentParserCompPtr->IsGqlEnabled()){
		return TS_OK;
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
		joinRules = CalculateTargetCppFilesFromSchemaParams(*m_customSchemaParamsCompPtr, m_argumentParserCompPtr->GetOutputDirectoryPath(), defaultName);
	}
	const bool joinHeaders = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_headerFileType);
	const bool joinSources = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_sourceFileType);

	imtsdl::SdlRequestList sdlRequestList = m_sdlRequestListCompPtr->GetRequests(true);
	if (m_argumentParserCompPtr->IsDependenciesMode() || !m_argumentParserCompPtr->GetDepFilePath().isEmpty()){
		if (!m_argumentParserCompPtr->IsAutoJoinEnabled()){
			QStringList cumulatedFiles;
			for (const imtsdl::CSdlRequest& sdlRequest: sdlRequestList){
				if (!joinHeaders){
					cumulatedFiles << QString(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.h");
				}
				if (!joinSources){
					cumulatedFiles << QString(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.cpp");
				}
			}
			PrintFiles(m_argumentParserCompPtr->GetDepFilePath(), cumulatedFiles, *m_dependentSchemaListCompPtr);
			PrintFiles(std::cout, cumulatedFiles, m_argumentParserCompPtr->GetGeneratorType());
		}
		if (m_argumentParserCompPtr->IsDependenciesMode()){
			return TS_OK;
		}
	}

	for (const imtsdl::CSdlRequest& sdlRequest: sdlRequestList){
		m_headerFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.h"));
		m_sourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.cpp"));

		if (!ProcessFiles(sdlRequest, !joinHeaders, !joinSources)){
			SendErrorMessage(0, QString("Unable to process files"));

			return TS_INVALID;
		}

		if (!CloseFiles()){
			SendErrorMessage(0, QString("Unable to finalize files"));

			return TS_INVALID;
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
			iprm::CEnableableParam appendEnableParam;
			appendEnableParam.SetEnabled(true);
			inputParams.SetEditableParameter(imtfile::CSimpleFileJoinerComp::s_appendModeParamId, &appendEnableParam);

			iprm::COptionsManager filterParams;

			if (joinHeaders){
				filterParams.ResetOptions();
				for (const imtsdl::CSdlRequest& sdlRequest: sdlRequestList){
					filterParams.InsertOption("C" + sdlRequest.GetName() + "GqlRequest.h", QByteArray::number(filterParams.GetOptionsCount()));
				}

				outputFileNameParam.SetPath(joinRules[imtsdl::ISdlProcessArgumentsParser::s_headerFileType]);
				int joinProcessResult = m_filesJoinerCompPtr->DoProcessing(&inputParams, &filterParams, nullptr);
				if (joinProcessResult != TS_OK){
					SendCriticalMessage(0, "Unable to join header files");
					I_CRITICAL();

					return TS_INVALID;
				}

				// cleanup joined files
				for (const imtsdl::CSdlRequest& sdlRequest: sdlRequestList){
					QFile::remove(QString(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.h"));
				}
			}
			if (joinSources){
				filterParams.ResetOptions();
				for (const imtsdl::CSdlRequest& sdlRequest: sdlRequestList){
					filterParams.InsertOption("C" + sdlRequest.GetName() + "GqlRequest.cpp", QByteArray::number(filterParams.GetOptionsCount()));
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
				for (const imtsdl::CSdlRequest& sdlRequest: sdlRequestList){
					QFile::remove(QString(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.cpp"));
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


// private methods

bool CGqlWrapClassCodeGeneratorComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_headerFilePtr->flush();
	retVal = m_sourceFilePtr->flush() && retVal;

	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	return retVal;
}


bool CGqlWrapClassCodeGeneratorComp::ProcessFiles(const imtsdl::CSdlRequest& sdlRequest, bool addDependenciesInclude, bool addSelfHeaderInclude)
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
	retVal = retVal && ProcessHeaderClassFile(sdlRequest, addDependenciesInclude);
	retVal = retVal && ProcessSourceClassFile(sdlRequest, addSelfHeaderInclude);

	return retVal;
}


bool CGqlWrapClassCodeGeneratorComp::ProcessHeaderClassFile(const imtsdl::CSdlRequest& sdlRequest, bool addDependenciesInclude)
{
	QTextStream ifStream(m_headerFilePtr.GetPtr());

	// preprocessor's section
	ifStream << QStringLiteral("#pragma once");
	FeedStream(ifStream, 3, false);

	AddRequiredIncludesToHeaderFile(ifStream, sdlRequest, addDependenciesInclude);

	// namespace begin
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
				m_customSchemaParamsCompPtr,
				m_argumentParserCompPtr,
				false);

	ifStream << QStringLiteral("namespace ");
	ifStream <<  sdlNamespace;
	FeedStream(ifStream, 1, false);
	ifStream <<  QStringLiteral("{");
	FeedStream(ifStream, 2, false);

	// RequestInfo struct Begin
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("struct ");
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestInfo");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);

	imtsdl::CSdlField outputArgument = sdlRequest.GetOutputArgument();
	// RequestInfo struct props
	GenerateFieldRequestInfo(ifStream, outputArgument);

	// RequestInfo struct End
	ifStream << '}' << ';';
	FeedStream(ifStream, 1, false);

	// RequestArguments struct Begin
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("struct ");
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestArguments");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);

	// RequestArguments struct props
	imtsdl::SdlFieldList requestArguments = sdlRequest.GetInputArguments();
	for (const imtsdl::CSdlField& sdlField: requestArguments){
		CStructNamespaceConverter structNameConverter(sdlField, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, false);

		FeedStreamHorizontally(ifStream, 1);
		ifStream << structNameConverter.GetString();
		ifStream << ' ' << sdlField.GetId() << ';';
		FeedStream(ifStream, 1, false);
	}

	// RequestArguments struct End
	ifStream << '}' << ';';
	FeedStream(ifStream, 1, false);


	// main GQL-wrap class begin
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("class C") << sdlRequest.GetName() << QStringLiteral("GqlRequest");
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("{");
	FeedStream(ifStream, 1, false);

	// public section
	ifStream << QStringLiteral("public:");
	FeedStream(ifStream, 1, false);

	AddMethodDeclarations(ifStream, sdlRequest);

	// private section
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("private:");
	FeedStream(ifStream, 1, false);

	AddClassProperties(ifStream, sdlRequest);

	// main GQL-wrap end of class
	ifStream << QStringLiteral("};");
	FeedStream(ifStream, 3, false);

	// end of namespace
	ifStream << QStringLiteral("} // namespace ");
	ifStream << sdlNamespace;
	FeedStream(ifStream, 3, true);


	return true;
}


bool CGqlWrapClassCodeGeneratorComp::ProcessSourceClassFile(const imtsdl::CSdlRequest& sdlRequest, bool addSelfHeaderInclude)
{
	QTextStream ifStream(m_sourceFilePtr.GetPtr());

	// include section
	if (addSelfHeaderInclude){
		ifStream << QStringLiteral("#include \"C");
		ifStream << sdlRequest.GetName() << QStringLiteral("GqlRequest.h\"");
		FeedStream(ifStream, 2);
	}
	FeedStream(ifStream, 1);

	// namespace begin
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
		m_customSchemaParamsCompPtr,
		m_argumentParserCompPtr,
		false);
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("namespace ");
		ifStream << sdlNamespace;
		FeedStream(ifStream, 1, false);
		ifStream << '{';
		FeedStream(ifStream, 1, false);
	}

	const QString className = 'C' + sdlRequest.GetName() + QStringLiteral("GqlRequest");
	// implementation of methods

	// CommandId method
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("QByteArray ");
	ifStream << className << ':' << ':';
	ifStream << QStringLiteral("GetCommandId()");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("return QByteArrayLiteral(\"");
	ifStream << sdlRequest.GetName();
	ifStream << '"' << ')' << ';';
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// setup GQL method
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("bool ");
	ifStream << className << ':' << ':';
	ifStream << QStringLiteral("SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const ");
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestArguments& requestArguments, const ");
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestInfo& /*requestInfo*/)");
	FeedStream(ifStream, 1, false);

	ifStream << '{';
	FeedStream(ifStream, 1, false);

	GenerateRequestSetup(ifStream, sdlRequest);
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// MAIN CONSTRUCTOR + PARSING
	FeedStream(ifStream, 2, false);
	ifStream << className << ':' << ':' << className;
	ifStream << QStringLiteral("(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral(": m_isValid(true)");
	FeedStream(ifStream, 1, false);

	ifStream << '{';
	FeedStream(ifStream, 1, false);

	if (sdlRequest.GetInputArguments().isEmpty()){
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("Q_UNUSED(optRead)");
		FeedStream(ifStream, 1, false);

		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("m_isValid = true;");
		FeedStream(ifStream, 1, false);
	}

	GenerateRequestParsing(ifStream, sdlRequest, 1);

	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// IsValid
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("bool ");
	ifStream << className << ':' << ':' ;
	ifStream << QStringLiteral("IsValid() const");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream);
	ifStream << "return m_isValid;";
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// GQL context
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("const ::imtgql::IGqlContext* ");
	ifStream << className << ':' << ':' ;
	ifStream << QStringLiteral("GetRequestContext() const");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream);
	ifStream << "return m_gqlContextPtr;";
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// GetRequestedArguments
	FeedStream(ifStream, 2, false);
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestArguments ");
	ifStream << className << ':' << ':' ;
	ifStream << QStringLiteral("GetRequestedArguments() const");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream);
	ifStream << "return m_requestedArguments;";
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// GetRequestInfo
	FeedStream(ifStream, 2, false);
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestInfo ");
	ifStream << className << ':' << ':' ;
	ifStream << QStringLiteral("GetRequestInfo() const");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream);
	ifStream << "return m_requestInfo;";
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// end of namespace
	FeedStream(ifStream, 2, false);
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("} // namespace ");
		ifStream << sdlNamespace;
	}
	FeedStream(ifStream, 1, true);

	return true;
}


void CGqlWrapClassCodeGeneratorComp::GenerateFieldRequestInfo(
			QTextStream& stream,
			const imtsdl::CSdlField& sdlField,
			uint hIndents,
			bool createStructDefinition)
{
	imtsdl::CSdlType sdlType;
	[[maybe_unused]]bool isTypeFound = GetSdlTypeForField(sdlField, m_sdlTypeListCompPtr->GetSdlTypes(false), sdlType);
	Q_ASSERT(isTypeFound);

	if (createStructDefinition ){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("struct ");
		stream << sdlField.GetId();
		stream << QStringLiteral("RequestInfo");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, hIndents);
		stream << '{';
		FeedStream(stream, 1, false);
	}

	// then add general props
	imtsdl::SdlFieldList customTypes;
	for (const imtsdl::CSdlField& fieldFromType: sdlType.GetFields()){
		FeedStreamHorizontally(stream, hIndents + createStructDefinition);
		stream << QStringLiteral("bool is") << GetCapitalizedValue(fieldFromType.GetId()) << QStringLiteral("Requested = ");
		stream << QStringLiteral("true");
		stream << ';';
		FeedStream(stream, 1, false);

		// add custom types for nested structs creation
		bool isCustom = false;
		bool isEnum = false;
		ConvertTypeOrEnum(fieldFromType, m_sdlEnumListCompPtr->GetEnums(false), &isCustom, nullptr, nullptr, &isEnum);
		if (isCustom && !isEnum){
			customTypes << fieldFromType;
		}
	}

	// and finally create all custom types;
	for (const imtsdl::CSdlField& customType: customTypes){
		if (customType.GetType() != sdlField.GetType()){
			GenerateFieldRequestInfo(stream, customType, hIndents + 1, true);
		}
	}

	if (createStructDefinition ){
		FeedStreamHorizontally(stream, hIndents);
		stream << '}' << ' ';
		stream << sdlField.GetId() << ';';
		FeedStream(stream, 1, false);
	}
}


void CGqlWrapClassCodeGeneratorComp::GenerateRequestParsing(
			QTextStream& stream,
			const imtsdl::CSdlRequest& sdlRequest,
			uint hIndents)
{
	// Get context
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("m_gqlContextPtr = gqlRequest.GetRequestContext();");
	FeedStream(stream, 2, false);

	// declare and read gql protocol version
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const QByteArray protocolVersion = gqlRequest.GetProtocolVersion();");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// reading input arguments");
	FeedStream(stream, 1, false);

	for (const imtsdl::CSdlField& field: sdlRequest.GetInputArguments()){
		AddFieldReadFromRequestCode(stream, field);
	}
	FeedStream(stream, 1, false);

	// fill requested fields
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// reading requested fields");
	FeedStream(stream, 1, false);

	// Get requested fields from request

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const imtgql::CGqlObject* requestedFieldsObjectPtr = nullptr;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){");
	FeedStream(stream, 1, false);

	// get top-level request. Usually it always one => we need only first
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("requestedFieldsObjectPtr = gqlRequest.GetFields().GetFieldArgumentObjectPtr(gqlRequest.GetFields().GetFieldIds().constFirst());");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (requestedFieldsObjectPtr != nullptr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("const QByteArrayList requestedIds = requestedFieldsObjectPtr->GetFieldIds();");
	FeedStream(stream, 1, false);

	GenerateRequestedFieldsParsing(
				stream,
				sdlRequest.GetOutputArgument(),
				QStringLiteral("requestedIds"),
				QStringLiteral("requestedFieldsObjectPtr"),
				QString(),
				hIndents + 2);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

}


void CGqlWrapClassCodeGeneratorComp::GenerateRequestedFieldsParsing(
			QTextStream& stream,
			const imtsdl::CSdlField& sdlField,
			const QString& idListContainerParamName,
			const QString& gqlObjectVarName,
			const QString& complexFieldName,
			uint hIndents)
{
	bool isCustom = false;
	bool isEnum = false;
	ConvertTypeOrEnum(sdlField, m_sdlEnumListCompPtr->GetEnums(false), &isCustom, nullptr, nullptr, &isEnum);
	if (!isCustom || isEnum){
		return;
	}

	std::shared_ptr<imtsdl::CSdlEntryBase> foundEntry = GetSdlTypeOrEnumForField(sdlField, m_sdlTypeListCompPtr->GetSdlTypes(false), m_sdlEnumListCompPtr->GetEnums(false));
	const imtsdl::CSdlType* sdlTypePtr = dynamic_cast<imtsdl::CSdlType*>(foundEntry.get());
	if(sdlTypePtr == nullptr){
		I_CRITICAL();

		return;
	}

	GenerateRequestedFieldsParsing(stream, *sdlTypePtr, idListContainerParamName, gqlObjectVarName, complexFieldName, hIndents);
}


void CGqlWrapClassCodeGeneratorComp::GenerateRequestedFieldsParsing(
			QTextStream& stream,
			const imtsdl::CSdlType& sdlType,
			const QString& idListContainerParamName,
			const QString& gqlObjectVarName,
			const QString& complexFieldName,
			uint hIndents)
{
	const imtsdl::SdlFieldList typeFieldList = sdlType.GetFields();
	if (typeFieldList.isEmpty()){
		I_CRITICAL();

		return;
	}

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!");
	stream << idListContainerParamName;
	stream << QStringLiteral(".isEmpty()){");
	FeedStream(stream, 1, false);

	// Initialize variables with values from the specified list, if they are present.
	for (const imtsdl::CSdlField& typeField: typeFieldList){
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("m_requestInfo.");
		if (!complexFieldName.isEmpty()){
			stream << complexFieldName;
			if (!complexFieldName.endsWith('.')){
				stream << '.';
			}
		}
		stream << QStringLiteral("is");
		stream << GetCapitalizedValue(typeField.GetId());
		stream << QStringLiteral("Requested = ");
		if (typeField.IsRequired()){
			stream << QStringLiteral("true;");
		}
		else {
			stream << idListContainerParamName;
			stream << QStringLiteral(".contains(\"");
			stream << typeField.GetId();
			stream << '"' << ')' << ';';
		}
		FeedStream(stream, 1, false);
	}

	// Initialize variables for complex fields of this
	for (const imtsdl::CSdlField& typeField: typeFieldList){
		bool isCustom = false;
		bool isEnum = false;
		ConvertTypeOrEnum(typeField, m_sdlEnumListCompPtr->GetEnums(false), &isCustom, nullptr, nullptr, &isEnum);
		if (!isCustom || isEnum){
			continue;
		}

		std::shared_ptr<imtsdl::CSdlEntryBase> foundEntry = GetSdlTypeOrEnumForField(typeField, m_sdlTypeListCompPtr->GetSdlTypes(false), m_sdlEnumListCompPtr->GetEnums(false));
		const imtsdl::CSdlType* sdlTypePtr = dynamic_cast<imtsdl::CSdlType*>(foundEntry.get());
		if(sdlTypePtr == nullptr){
			I_CRITICAL();

			return;
		}

		// check if a type contains itself
		QStringList processedIdList = complexFieldName.split('.');
		if (processedIdList.contains(typeField.GetId())){

			FeedStreamHorizontally(stream, hIndents);
			stream << '}';
			FeedStream(stream, 1, false);

			return;
		}

		QString newComplexFieldName = complexFieldName;
		if (!newComplexFieldName.isEmpty()){
			newComplexFieldName += '.';
		}
		newComplexFieldName += typeField.GetId();

		const QString newGqlContainerVarName = GetDecapitalizedValue(typeField.GetId()) + QStringLiteral("RequestedFieldsPtr");
		const QString newIdListContainerVarName = GetDecapitalizedValue(typeField.GetId()) + QStringLiteral("RequestedIds");

		// first create a GQL-info object
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("const imtgql::CGqlObject* ");
		stream << newGqlContainerVarName;
		stream << ' ' << '=' << ' ';
		stream << gqlObjectVarName;
		stream << QStringLiteral("->GetFieldArgumentObjectPtr(\"");
		stream << typeField.GetId();
		stream << '"' << ')' << ';';
		FeedStream(stream, 1, false);

		// ensure is not null
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("if (");
		stream << newGqlContainerVarName;
		stream << QStringLiteral(" != nullptr){");
		FeedStream(stream, 1, false);

		// get requested id list
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("const QByteArrayList ");
		stream << newIdListContainerVarName;
		stream << ' ' << '=' << ' ';
		stream << newGqlContainerVarName;
		stream << QStringLiteral("->GetFieldIds();");
		FeedStream(stream, 1, true);

		// then generate field info (self nested is not allowed)
		if (sdlType != *sdlTypePtr){
			GenerateRequestedFieldsParsing(
				stream,
				*sdlTypePtr,
				newIdListContainerVarName,
				newGqlContainerVarName,
				newComplexFieldName,
				hIndents + 2);
		}

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::GenerateRequestSetup(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, uint hIndents)
{
	// set commandID
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("gqlRequest.SetCommandId(GetCommandId());");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// writting input arguments");
	FeedStream(stream, 1, false);
	for (const imtsdl::CSdlField& field: sdlRequest.GetInputArguments()){
		AddFieldWriteToRequestCode(stream, field);
	}
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("return true;");
}


void CGqlWrapClassCodeGeneratorComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	I_CRITICAL();

	m_headerFilePtr->remove();
	m_sourceFilePtr->remove();
}


void CGqlWrapClassCodeGeneratorComp::AddRequiredIncludesToHeaderFile(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, bool addDependenciesInclude) const
{
	QSet<QString> complexTypeList;
	bool hasComplexTypes = IsTypeHasNonFundamentalTypes(sdlRequest, &complexTypeList);

	if (hasComplexTypes){
		bool isQtCommentAdded = false;
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
		// if variant map is enabled we need to add QVariant and QVariantMap
		if (!isQtCommentAdded){
			stream << QStringLiteral("// Qt includes");
			FeedStream(stream, 1, false);
		}
		stream << QStringLiteral("#include <QtCore/QVariant>");
		FeedStream(stream, 1, false);
		stream << QStringLiteral("#include <QtCore/QVariantMap>");
		FeedStream(stream, 1, false);

		// remove qt types from list
		complexTypeList.remove(QStringLiteral("QByteArray"));
		complexTypeList.remove(QStringLiteral("QString"));
		complexTypeList.remove(QStringLiteral("QList"));
		if (!complexTypeList.isEmpty()){
			FeedStream(stream, 1, false);
		}

		/// \todo add it also for GQL Base handler and GQL CollectionHandler
		QList<imtsdl::IncludeDirective> includeDirectivesList;
		QSet<QString> customIncluded;
		QList<imtsdl::CSdlField> requestFields = sdlRequest.GetInputArguments();
		requestFields << sdlRequest.GetOutputArgument();
		for (const imtsdl::CSdlField& field: requestFields){
			std::shared_ptr<imtsdl::CSdlEntryBase> foundEntryPtr = GetSdlTypeOrEnumForField(
				field,
				m_sdlTypeListCompPtr->GetSdlTypes(false),
				m_sdlEnumListCompPtr->GetEnums(false));

			if (!foundEntryPtr){
				SendCriticalMessage(0, QString("Unable to find type for %1 of %2").arg(field.GetId(), sdlRequest.GetName()));
				I_CRITICAL();

				return;
			}

			if (!foundEntryPtr->IsExternal()){
				continue;
			}

			QString resolvedPath = ResolveRelativeHeaderFileForType(*foundEntryPtr, m_argumentParserCompPtr->GetHeadersIncludePaths(), false);
			if (resolvedPath.isEmpty()){
				SendErrorMessage(0, QString("Unable to find header for %1 of %2").arg(field.GetId(), sdlRequest.GetName()));

				return;
			}

			const QString relativeIncludePath = '<' + resolvedPath + '>';
			if (!resolvedPath.isEmpty() && !customIncluded.contains(relativeIncludePath)){
				includeDirectivesList << CreateCustomDirective(relativeIncludePath);
				customIncluded << relativeIncludePath;
			}
		}

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


		/// \fallback for V1 ? \todo inspect and remove it
		// Add user's custom types
		if (addDependenciesInclude){
			// first add include comment
			if (!complexTypeList.isEmpty()){
				stream << QStringLiteral("// ") << GetNamespaceFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr, false) << QStringLiteral(" includes");
				FeedStream(stream, 1, false);
			}

			// then add inclides
			for (QSet<QString>::const_iterator complexIter = complexTypeList.cbegin(); complexIter != complexTypeList.cend(); ++complexIter){
				const QString& complexTypeName = *complexIter;
				stream << QStringLiteral("#include \"") << complexTypeName << QStringLiteral(".h\"");
				FeedStream(stream, 1, false);
			}
		}
	}

	// add imtgql includes
	FeedStream(stream, 1, false);
	stream << QStringLiteral("// imtgql includes");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("#include <imtgql/IGqlContext.h>");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("#include <imtgql/CGqlRequest.h>");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("#include <imtgql/CGqlObject.h>");
	FeedStream(stream, 3, false);
}


void CGqlWrapClassCodeGeneratorComp::AddMethodDeclarations(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest) const
{
	// CommandId method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("static QByteArray GetCommandId();");
	FeedStream(stream, 2, false);

	// client-side method (setup GQL request)
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("[[nodiscard]] static bool SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const ");
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("RequestArguments& requestArguments, const ");
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("RequestInfo& requestInfo = ");
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("RequestInfo()");
	stream << ')' << ';';
	FeedStream(stream, 2, false);

	// default constructor with GraphQL request
	FeedStreamHorizontally(stream);
	stream << 'C' << sdlRequest.GetName() << QStringLiteral("GqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead);");
	FeedStream(stream, 1, false);

	// validation method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("bool IsValid() const;");
	FeedStream(stream, 1, false);

	// GetRequestedArguments method
	FeedStreamHorizontally(stream);
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("RequestArguments GetRequestedArguments() const;");
	FeedStream(stream, 1, false);

	// GetRequestInfo method
	FeedStreamHorizontally(stream);
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("RequestInfo GetRequestInfo() const;");
	FeedStream(stream, 1, false);

	// Get GQL context
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("const ::imtgql::IGqlContext* GetRequestContext() const;");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddClassProperties(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest) const
{
	// validation property
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("bool m_isValid;");
	FeedStream(stream, 1, false);

	// Arguments property
	FeedStreamHorizontally(stream);
	stream << GetCapitalizedValue(sdlRequest.GetName()) << QStringLiteral("RequestArguments m_requestedArguments;");
	FeedStream(stream, 1, false);

	// Info property
	FeedStreamHorizontally(stream);
	stream << GetCapitalizedValue(sdlRequest.GetName()) << QStringLiteral("RequestInfo m_requestInfo;");
	FeedStream(stream, 1, false);

	// Gql context
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("const ::imtgql::IGqlContext* m_gqlContextPtr;");
	FeedStream(stream, 1, false);

}


// read methods

bool CGqlWrapClassCodeGeneratorComp::AddFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	bool isCustom = false;
	bool isArray = false;
	ConvertType(field, &isCustom, nullptr, &isArray);

	if (isArray){
		QString errorString = QString("Arrays is not allowed in request arguments! FieldID = '%1', FieldType = '%2'").arg(field.GetId(), field.GetType());
		SendCriticalMessage(0, errorString);
		Q_ASSERT_X(false, __func__, errorString.toLocal8Bit());

		return false;
	}
	if (isCustom){
		AddCustomFieldReadFromRequestCode(stream, field);
	}
	else {
		AddScalarFieldReadFromRequestCode(stream, field);
	}

	return true;
}


void CGqlWrapClassCodeGeneratorComp::AddScalarFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	AddExtractValueFromRequestCode(stream, field);
	FeedStreamHorizontally(stream);

	if (field.IsRequired()){
		AddDataCheckRequiredValueCode(stream, field);
		FeedStreamHorizontally(stream);

		AddSetValueToObjectCode(stream, field);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (!") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.isNull()){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		AddSetValueToObjectCode(stream, field);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CGqlWrapClassCodeGeneratorComp::AddCustomFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	AddExtractCustomValueFromRequestCode(stream, field);
	FeedStreamHorizontally(stream);

	if (field.IsRequired()){
		AddCheckCustomRequiredValueCode(stream, field);
		FeedStreamHorizontally(stream);

		AddSetCustomValueToObjectCode(stream, field);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr != nullptr){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		AddSetCustomValueToObjectCode(stream, field, 2);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


// write methods

bool CGqlWrapClassCodeGeneratorComp::AddFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	bool isCustom = false;
	bool isArray = false;
	ConvertType(field, &isCustom, nullptr, &isArray);

	if (isArray){
		QString errorString = QString("Arrays is not allowed in request arguments! FieldID = '%1', FieldType = '%2'").arg(field.GetId(), field.GetType());
		SendCriticalMessage(0, errorString);
		Q_ASSERT_X(false, __func__, errorString.toLocal8Bit());

		return false;
	}

	if (isCustom){
		AddCustomFieldWriteToRequestCode(stream, field);
	}
	else {
		AddScalarFieldWriteToRequestCode(stream, field);
	}

	return true;
}


void CGqlWrapClassCodeGeneratorComp::AddScalarFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	QString tempListVarName;
	GenerateListTempValueCode(stream, field, tempListVarName, hIndents);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("request.InsertField(");
	if (field.IsArray()){
		stream << tempListVarName;
	}
	else {
		stream << QStringLiteral(", m_") << GetDecapitalizedValue(field.GetId());
	}
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddCustomFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, false);

	// declare temp GQL object
	FeedStreamHorizontally(stream, hIndents);
	const QString dataObjectVariableName = field.GetId() + QStringLiteral("DataObject");
	stream << QStringLiteral("::imtgql::CGqlObject ") << dataObjectVariableName << ';';
	FeedStream(stream, 1, false);

	// add me to temp object and checks
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!");
	stream << QStringLiteral("requestArguments.");
	stream << field.GetId();
	stream << (".WriteToGraphQlObject(");
	stream << dataObjectVariableName;
	stream << QStringLiteral(")){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// insert temp GQL object
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("gqlRequest.AddParam(");
	stream << '"' << field.GetId() << '"';
	stream << ',' << ' ';
	stream << dataObjectVariableName;
	stream << ')' << ';';
	FeedStream(stream, 1, false);
}


// help methods


// general help methods for scalar

void CGqlWrapClassCodeGeneratorComp::AddExtractValueFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, quint32 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("QVariant ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data = gqlRequest.GetParamObject(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddDataCheckRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, quint32 hIndents)
{
	stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.isNull()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("m_isValid = false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("m_isValid = true;");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddSetValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	stream << QStringLiteral("m_requestedArguments.") << field.GetId() << QStringLiteral(" = ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.");
	stream << GetFromVariantConversionString(field) << ';';
}


// general help methods for custom

void CGqlWrapClassCodeGeneratorComp::AddExtractCustomValueFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const ::imtgql::CGqlObject* ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr = gqlRequest.GetParamObject(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddCheckCustomRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr == nullptr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("m_isValid = false;");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("m_isValid = true;");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddSetCustomValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	// check if a protocol is acceptable
	stream << QStringLiteral("if (!protocolVersion.isEmpty()){");
	FeedStream(stream, 1, false);

	imtsdl::CSdlType foundType;
	[[maybe_unused]]const bool isTypeFound = GetSdlTypeForField(
				field,
				m_sdlTypeListCompPtr->GetSdlTypes(false),
				foundType);
	Q_ASSERT(isTypeFound);

	/// \todo check all versions
	const QString fieldVersion = GetTypeVersion(foundType);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (protocolVersion == \"");
	stream << fieldVersion;
	stream << QStringLiteral("\"){");
	FeedStream(stream, 1, false);

	/// \todo do this for all versions
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(
				foundType,
				sdlNamespace,
				*m_sdlTypeListCompPtr,
				*m_sdlEnumListCompPtr,
				false);

	QString typeVersion = structNameConverter.GetString();
	typeVersion += QStringLiteral("::PV_");
	typeVersion += GetSdlEntryVersion(foundType, false);
	AddSetCustomValueToObjectCodeImpl(stream, field, typeVersion, hIndents + 1);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("else {");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("qWarning() << QString(\"Bad request. Version %1 is not supported\").arg(protocolVersion);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("m_isValid = false;");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("else {");
	FeedStream(stream, 1, false);

	AddSetCustomValueToObjectCodeImpl(stream, field, QString(), hIndents);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
}

void CGqlWrapClassCodeGeneratorComp::AddSetCustomValueToObjectCodeImpl(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const QString& typeVersion,
			uint hIndents)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, false);

	// declare bool variable and read data in private property
	const QString readVariableName = QStringLiteral("is") + GetCapitalizedValue(field.GetId()) + QStringLiteral("Read");

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("bool ");
	stream << readVariableName << ';';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (optRead){");
	FeedStream(stream, 1, false);
	
	AddReadFromRequestCode(stream, field, typeVersion, readVariableName, true, hIndents + 2);
	
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("else {");
	FeedStream(stream, 1, false);

	AddReadFromRequestCode(stream, field, typeVersion, readVariableName, false, hIndents + 2);
	
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	// update validation status
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("m_isValid = ") << readVariableName << ';';
	FeedStream(stream, 1, false);

	// check the result of reading...
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!") << readVariableName << QStringLiteral("){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddReadFromRequestCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const QString& typeVersion,
			const QString& readVariableName,
			bool optRead,
			uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << readVariableName;
	stream << QStringLiteral(" = ");
	stream << QStringLiteral("m_requestedArguments.") << field.GetId();
	stream << '.';
	if (optRead){
		stream << QStringLiteral("Opt");
	}
	stream <<  QStringLiteral("ReadFromGraphQlObject(*");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr");
	if (!typeVersion.isEmpty()){
		stream << ',' << ' '<< typeVersion;
	}
	stream << ')' << ';';
	FeedStream(stream, 1, false);
}


} // namespace imtsdlgenv2

