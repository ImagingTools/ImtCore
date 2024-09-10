#include "CGqlWrapClassCodeGeneratorComp.h"


// C includes
#include <iostream>

// Qt includes
#include <QtCore/QDir>

//Acf includes
#include <istd/CSystem.h>
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>
#include <iprm/CEnableableParam.h>
#include <ifile/CFileNameParam.h>

// ImtCore includes
#include <imtfile/CSimpleFileJoinerComp.h>
#include <imtsdl/CSdlRequest.h>


namespace imtsdl
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

	TaskState retVal = TS_OK;

	if (!m_argumentParserCompPtr->IsGqlEnabled()){
		return retVal;
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

	const QMap<QString, QString> joinRules = m_argumentParserCompPtr->GetJoinRules();
	const bool joinHeaders = joinRules.contains(ISdlProcessArgumentsParser::s_headerFileType);
	const bool joinSources = joinRules.contains(ISdlProcessArgumentsParser::s_sourceFileType);

	SdlRequestList sdlRequestList = m_sdlRequestListCompPtr->GetRequests();
	if (m_argumentParserCompPtr->IsDependenciesMode()){
		if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
			if (!m_customSchemaParamsCompPtr.IsValid()){
				SendErrorMessage(0, "Application is not configured with custom parameters. Auto join is not possible. Please specify paths to join explicitly(use -J option), or disable join.");

				return TS_INVALID;
			}

			const QString defaultName = QFileInfo(m_argumentParserCompPtr->GetSchemaFilePath()).fileName();
			QStringList autoJoinFilePaths = GetAutoJoinedCppFilePaths(*m_customSchemaParamsCompPtr, m_argumentParserCompPtr->GetOutputDirectoryPath(), defaultName);
			PrintFiles(std::cout, autoJoinFilePaths, m_argumentParserCompPtr->GetGeneratorType());
		}
		else {
			QStringList cumulatedFiles;
			for (const CSdlRequest& sdlRequest: sdlRequestList){
				if (!joinHeaders){
					cumulatedFiles << QString(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.h");
				}
				if (!joinSources){
					cumulatedFiles << QString(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.cpp");
				}
			}
			if (joinHeaders){
				cumulatedFiles << joinRules[ISdlProcessArgumentsParser::s_headerFileType];
			}
			if (joinSources){
				cumulatedFiles << joinRules[ISdlProcessArgumentsParser::s_sourceFileType];
			}

			PrintFiles(std::cout, cumulatedFiles, m_argumentParserCompPtr->GetGeneratorType());
		}

		return TS_OK;
	}

	for (const CSdlRequest& sdlRequest: sdlRequestList){
		m_headerFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.h"));
		m_sourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.cpp"));

		if (!ProcessFiles(sdlRequest, !joinHeaders, !joinSources)){
			SendErrorMessage(0, QString("Unable to begin files"));
			I_CRITICAL();

			return TS_INVALID;
		}

		if (!CloseFiles()){
			SendErrorMessage(0, QString("Unable to finalize files"));
			I_CRITICAL();

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
				for (const CSdlRequest& sdlRequest: sdlRequestList){
					filterParams.InsertOption("C" + sdlRequest.GetName() + "GqlRequest.h", QByteArray::number(filterParams.GetOptionsCount()));
				}

				outputFileNameParam.SetPath(joinRules[ISdlProcessArgumentsParser::s_headerFileType]);
				int joinProcessResult = m_filesJoinerCompPtr->DoProcessing(&inputParams, &filterParams, nullptr);
				if (joinProcessResult != TS_OK){
					SendCriticalMessage(0, "Unable to join header files");
					I_CRITICAL();

					return TS_INVALID;
				}

				// cleanup joined files
				for (const CSdlRequest& sdlRequest: sdlRequestList){
					QFile::remove(QString(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.h"));
				}
			}
			if (joinSources){
				filterParams.ResetOptions();
				for (const CSdlRequest& sdlRequest: sdlRequestList){
					filterParams.InsertOption("C" + sdlRequest.GetName() + "GqlRequest.cpp", QByteArray::number(filterParams.GetOptionsCount()));
				}

				const QString sourceFilePath = joinRules[ISdlProcessArgumentsParser::s_sourceFileType];
				outputFileNameParam.SetPath(sourceFilePath);
				int joinProcessResult = m_filesJoinerCompPtr->DoProcessing(&inputParams, &filterParams, nullptr);
				if (joinProcessResult != TS_OK){
					SendCriticalMessage(0, "Unable to join cource  files");
					I_CRITICAL();

					return TS_INVALID;
				}

				// cleanup joined files
				for (const CSdlRequest& sdlRequest: sdlRequestList){
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
					QFileInfo headerFileInfo(joinRules[ISdlProcessArgumentsParser::s_headerFileType]);
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

bool CGqlWrapClassCodeGeneratorComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_headerFilePtr->flush();
	retVal = m_sourceFilePtr->flush() && retVal;

	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	return retVal;
}


bool CGqlWrapClassCodeGeneratorComp::ProcessFiles(const CSdlRequest& sdlRequest, bool addDependenciesInclude, bool addSelfHeaderInclude)
{
	if (!m_headerFilePtr->open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
					QString("Unable to open file: '%1'. Error: %2")
						.arg(m_headerFilePtr->fileName(), m_headerFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	if (!m_sourceFilePtr->open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
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


bool CGqlWrapClassCodeGeneratorComp::ProcessHeaderClassFile(const CSdlRequest& sdlRequest, bool addDependenciesInclude)
{
	QTextStream ifStream(m_headerFilePtr.GetPtr());

	// preprocessor's section
	ifStream << QStringLiteral("#pragma once");
	FeedStream(ifStream, 3, false);

	AddRequiredIncludesToHeaderFile(ifStream, sdlRequest, addDependenciesInclude);

	// namespace begin
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr);
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("namespace ");
		ifStream <<  sdlNamespace;
		FeedStream(ifStream, 1, false);
		ifStream <<  QStringLiteral("{");
		FeedStream(ifStream, 1, false);
	}

	// RequestInfo struct Begin
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("struct ");
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestInfo");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);

	// RequestInfo struct props
	GenerateFieldRequestInfo(ifStream, sdlRequest.GetOutputArgument());

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
	SdlFieldList requestArguments = sdlRequest.GetInputArguments();
	for (const CSdlField& sdlField: requestArguments){
		FeedStreamHorizontally(ifStream, 1);
		ifStream << ConvertType(sdlField) << ' ' << sdlField.GetId() << ';';
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


bool CGqlWrapClassCodeGeneratorComp::ProcessSourceClassFile(const CSdlRequest& sdlRequest, bool addSelfHeaderInclude)
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
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr);
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

	// MAIN CONSTRUCTOR + PARSING
	FeedStream(ifStream, 2, false);
	ifStream << className << ':' << ':' << className;
	ifStream << QStringLiteral("(const imtgql::CGqlRequest& gqlRequest)");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);
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
	ifStream << QStringLiteral("const imtgql::IGqlContext* ");
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
			const CSdlField& sdlField,
			uint hIndents,
			bool createStructDefinition)
{
	CSdlType sdlType;
	[[maybe_unused]]bool isTypeFound = GetSdlTypeForField(sdlField, m_sdlTypeListCompPtr->GetSdlTypes(), sdlType);
	Q_ASSERT(isTypeFound);

	if (createStructDefinition ){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("struct ");
		stream << sdlField.GetType();
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, hIndents);
		stream << '{';
		FeedStream(stream, 1, false);
	}
	// first add general prop
	FeedStreamHorizontally(stream, hIndents + createStructDefinition);
	stream << QStringLiteral("bool isRequested = true;");
	FeedStream(stream, 1, false);

	// then add general props
	SdlFieldList customTypes;
	for (const CSdlField& fieldFromType: sdlType.GetFields()){
		FeedStreamHorizontally(stream, hIndents + createStructDefinition);
		stream << QStringLiteral("bool is") << GetCapitalizedValue(fieldFromType.GetId()) << QStringLiteral("Requested = ");
/// \todo fix it in \c GenerateRequestParsing
#if SDL__PARSING_OF_REQUESTED_FIELDS_FIXED
		if (fieldFromType.IsRequired()){
			stream << QStringLiteral("true");
		}
		else {
			stream << QStringLiteral("false");
		}
#else
		stream << QStringLiteral("true");
#endif
		stream << ';';
		FeedStream(stream, 1, false);

		// add custom types for nested structs creation
		bool isCustom = false;
		ConvertType(fieldFromType, &isCustom);
		if (isCustom){
			customTypes << fieldFromType;
		}
	}

	// and finally create all custom types;
	for (const CSdlField& customType: customTypes){
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
			const CSdlRequest& sdlRequest,
			uint hIndents)
{
	// Get context
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("m_gqlContextPtr = gqlRequest.GetRequestContext();");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// reading input arguments");
	FeedStream(stream, 1, false);
	for (const CSdlField& field: sdlRequest.GetInputArguments()){
		AddFieldReadFromRequestCode(stream, field);
	}

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// reading requested fields");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("/// implemenatation will be in new version");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddRequiredIncludesToHeaderFile(QTextStream& stream, const CSdlRequest& sdlRequest, bool addDependenciesInclude) const
{

	QSet<QString> complexTypeList;
	bool hasComplexTypes = m_argumentParserCompPtr->IsModificatorEnabled(s_variantMapModificatorArgumentName);
	hasComplexTypes = IsTypeHasNonFundamentalTypes(sdlRequest, &complexTypeList) || hasComplexTypes;

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
		if (m_argumentParserCompPtr->IsModificatorEnabled(s_variantMapModificatorArgumentName)){
			if (!isQtCommentAdded){
				stream << QStringLiteral("// Qt includes");
				FeedStream(stream, 1, false);
			}
			stream << QStringLiteral("#include <QtCore/QVariant>");
			FeedStream(stream, 1, false);
			stream << QStringLiteral("#include <QtCore/QVariantMap>");
			FeedStream(stream, 1, false);
		}

		// remove qt types from list
		complexTypeList.remove(QStringLiteral("QByteArray"));
		complexTypeList.remove(QStringLiteral("QString"));
		complexTypeList.remove(QStringLiteral("QList"));
		if (!complexTypeList.isEmpty()){
			FeedStream(stream, 1, false);
		}

		// Add user's custom types
		if (addDependenciesInclude){
			// first add include comment
			if (!complexTypeList.isEmpty()){
				stream << QStringLiteral("// ") << GetNamespaceFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr) << QStringLiteral(" includes");
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


void CGqlWrapClassCodeGeneratorComp::AddMethodDeclarations(QTextStream& stream, const CSdlRequest& sdlRequest) const
{
	// CommandId method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("static QByteArray GetCommandId();");
	FeedStream(stream, 2, false);

	// default constructor with GraphQL request
	FeedStreamHorizontally(stream);
	stream << 'C' << sdlRequest.GetName() << QStringLiteral("GqlRequest (const imtgql::CGqlRequest& gqlRequest);");
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
	stream << QStringLiteral("const imtgql::IGqlContext* GetRequestContext() const;");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddClassProperties(QTextStream& stream, const CSdlRequest& sdlRequest) const
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
	stream << QStringLiteral("const imtgql::IGqlContext* m_gqlContextPtr;");
	FeedStream(stream, 1, false);

}


// read methods

bool CGqlWrapClassCodeGeneratorComp::AddFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field)
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


void CGqlWrapClassCodeGeneratorComp::AddScalarFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field)
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


void CGqlWrapClassCodeGeneratorComp::AddCustomFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field)
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

bool CGqlWrapClassCodeGeneratorComp::AddFieldWriteToRequestCode(QTextStream& stream, const CSdlField& field)
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

	const bool isFieldRequired = field.IsRequired();
	if (isFieldRequired){
		AddSelfCheckRequiredValueCode(stream, field);
	}
	else {
		AddBeginSelfCheckNonRequiredValueCode(stream, field);
	}
	const int hIndents = (isFieldRequired ? 1 : 2);
	if (isCustom){
		AddCustomFieldWriteToRequestCode(stream, field, hIndents);
	}
	else {
		AddScalarFieldWriteToRequestCode(stream, field, hIndents);
	}

	if (!isFieldRequired){
		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	return true;
}


void CGqlWrapClassCodeGeneratorComp::AddScalarFieldWriteToRequestCode(QTextStream& stream, const CSdlField& field, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("request.InsertField(");
	stream << '"' << field.GetId() << '"';
	stream << ',' << ' ' << FromVariantMapAccessString(field);
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddCustomFieldWriteToRequestCode(QTextStream& stream, const CSdlField& field, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	// declare temp GQL object
	const QString dataObjectVariableName = field.GetId() + QStringLiteral("DataObject");
	stream << QStringLiteral("imtgql::CGqlObject ") << dataObjectVariableName << ';';
	FeedStream(stream, 1, false);

	// add me to temt object and checks
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!m_");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral(".WriteToGraphQlObject(");
	stream << dataObjectVariableName;
	stream << QStringLiteral(")){");
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

	// insert temp GQL object
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("request.InsertField(");
	stream << '"' << field.GetId() << '"';
	stream << ',' << ' ';
	stream << dataObjectVariableName;
	stream << ')' << ';';
	FeedStream(stream, 1, false);
}


// help methods


// general help methods for scalar

void CGqlWrapClassCodeGeneratorComp::AddExtractValueFromRequestCode(QTextStream& stream, const CSdlField& field, quint32 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("QVariant ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data = gqlRequest.GetParamObject(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddDataCheckRequiredValueCode(QTextStream& stream, const CSdlField& field, quint32 hIndents)
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
}


void CGqlWrapClassCodeGeneratorComp::AddSetValueToObjectCode(QTextStream& stream, const CSdlField& field)
{
	stream << QStringLiteral("m_requestedArguments.") << field.GetId() << QStringLiteral(" = ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.");
	stream << GetFromVariantConversionString(field) << ';';
}


// general help methods for custom

void CGqlWrapClassCodeGeneratorComp::AddExtractCustomValueFromRequestCode(QTextStream& stream, const CSdlField& field, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const imtgql::CGqlObject* ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr = gqlRequest.GetParamObject(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddCheckCustomRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents)
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
}


void CGqlWrapClassCodeGeneratorComp::AddSetCustomValueToObjectCode(QTextStream& stream, const CSdlField& field, uint hIndents)
{
	// declare bool variable and read data in private property
	stream << QStringLiteral("const bool is") << GetCapitalizedValue(field.GetId()) << QStringLiteral("Read = ");
	stream << 'C' << GetCapitalizedValue(field.GetType()) << QStringLiteral("::ReadFromGraphQlObject(");
	stream << QStringLiteral("m_requestedArguments.") << field.GetId() << QStringLiteral(", *");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);

	// check the result of reading...
	stream << QStringLiteral("if (!is") << GetCapitalizedValue(field.GetId()) << QStringLiteral("Read){");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);

	// ...and exit if it fail
	stream << QStringLiteral("m_isValid = false;");
	FeedStream(stream, 2, false);
	FeedStreamHorizontally(stream, hIndents + 1);

	stream << QStringLiteral("return;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
}


void CGqlWrapClassCodeGeneratorComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	I_CRITICAL();

	m_headerFilePtr->remove();
	m_sourceFilePtr->remove();
}


} // namespace imtsdl

