#include "CGqlHandlerBaseClassGeneratorComp.h"


// C includes
#include <iostream>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QTextStream>

//Acf includes
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


int CGqlHandlerBaseClassGeneratorComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlRequestListCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_baseClassExtenderCompPtr.IsValid());

	if (!m_argumentParserCompPtr->IsGqlEnabled()){
		return TS_OK;
	}

	const QString outputDirectoryPath = QDir::cleanPath(m_argumentParserCompPtr->GetOutputDirectoryPath());
	if (outputDirectoryPath.isEmpty()){
		SendErrorMessage(0, "Output path is not provided");

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
	if (m_argumentParserCompPtr->IsDependenciesMode()){
		if (!m_argumentParserCompPtr->IsAutoJoinEnabled()){
			QStringList cumulatedFiles;
			if (!joinHeaders){
				cumulatedFiles << WrapFileName(QStringLiteral("h"), outputDirectoryPath);
			}
			if (!joinSources){
				cumulatedFiles << WrapFileName(QStringLiteral("cpp"), outputDirectoryPath);
			}

			PrintFiles(std::cout, cumulatedFiles, m_argumentParserCompPtr->GetGeneratorType());
		}

		return TS_OK;
	}

	imtsdl::SdlRequestList requests = m_sdlRequestListCompPtr->GetRequests();
	if (requests.isEmpty()){
		SendVerboseMessage("Requests is empty. Nothing todo");

		return TS_OK;
	}

	m_headerFilePtr.SetPtr(new QFile(WrapFileName(QStringLiteral("h"), outputDirectoryPath)));
	m_sourceFilePtr.SetPtr(new QFile(WrapFileName(QStringLiteral("cpp"), outputDirectoryPath)));

	if (!ProcessFiles(!joinHeaders, !joinSources)){
		SendErrorMessage(0, QString("Unable to begin files"));
		I_CRITICAL();

		return TS_INVALID;
	}

	if (!CloseFiles()){
		SendErrorMessage(0, QString("Unable to finalize files"));
		I_CRITICAL();

		return TS_INVALID;
	}

	// add extending for base class
	iprm::CParamsSet paramsSet;
	ifile::CFileNameParam headerFileNameParam;
	headerFileNameParam.SetPath(m_headerFilePtr->fileName());
	paramsSet.SetEditableParameter(QByteArrayLiteral("HeaderFile"), &headerFileNameParam);
	iprm::CEnableableParam enableCompMacroParam(true);
	paramsSet.SetEditableParameter(QByteArrayLiteral("AddBaseComponentMacro"), &enableCompMacroParam);

	iprm::COptionsManager baseClassDirectivesList;
	baseClassDirectivesList.InsertOption(QStringLiteral("imtservergql/CPermissibleGqlRequestHandlerComp.h"), QByteArrayLiteral("::imtservergql::CPermissibleGqlRequestHandlerComp"));

	int extendResult = m_baseClassExtenderCompPtr->DoProcessing(&paramsSet, &baseClassDirectivesList, nullptr);
	if (extendResult != TS_OK){
		SendErrorMessage(0, QString("Unable to extend file'%1'").arg(m_headerFilePtr->fileName()));
		I_CRITICAL();

		return TS_INVALID;
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
				filterParams.InsertOption(WrapFileName(QStringLiteral("h")), QByteArray::number(filterParams.GetOptionsCount()));

				outputFileNameParam.SetPath(joinRules[imtsdl::ISdlProcessArgumentsParser::s_headerFileType]);
				int joinProcessResult = m_filesJoinerCompPtr->DoProcessing(&inputParams, &filterParams, nullptr);
				if (joinProcessResult != TS_OK){
					SendCriticalMessage(0, "Unable to join header files");
					I_CRITICAL();

					return TS_INVALID;
				}

				// cleanup joined files
				QFile::remove(WrapFileName(QStringLiteral("h"), outputDirectoryPath));
			}

			if (joinSources){
				filterParams.ResetOptions();
				filterParams.InsertOption(WrapFileName(QStringLiteral("cpp")), QByteArray::number(filterParams.GetOptionsCount()));

				const QString sourceFilePath = joinRules[imtsdl::ISdlProcessArgumentsParser::s_sourceFileType];
				outputFileNameParam.SetPath(sourceFilePath);
				int joinProcessResult = m_filesJoinerCompPtr->DoProcessing(&inputParams, &filterParams, nullptr);
				if (joinProcessResult != TS_OK){
					SendCriticalMessage(0, "Unable to join source  files");
					I_CRITICAL();

					return TS_INVALID;
				}

				// cleanup joined files
				QFile::remove(WrapFileName(QStringLiteral("cpp"), outputDirectoryPath));

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


// private static methods

QString CGqlHandlerBaseClassGeneratorComp::WrapFileName(const QString& ext, const QString& directoryPath)
{
	QString retVal;
	Q_ASSERT_X(!ext.isEmpty(), __func__, "Extension for file MUST be specified");

	if (!directoryPath.isEmpty()){
		retVal.append(directoryPath);
		if (!retVal.endsWith('/') || !retVal.endsWith(QDir::separator())){
			retVal.append('/');
		}
	}
	retVal.append(QStringLiteral("CGraphQlHandlerCompBase."));
	retVal.append(ext);

	return retVal;
}


// private methods

bool CGqlHandlerBaseClassGeneratorComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_headerFilePtr->flush();
	retVal = m_sourceFilePtr->flush() && retVal;

	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	return retVal;
}


bool CGqlHandlerBaseClassGeneratorComp::ProcessFiles(bool addDependenciesInclude, bool addSelfHeaderInclude)
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
	retVal = retVal && ProcessHeaderClassFile(addDependenciesInclude);
	retVal = retVal && ProcessSourceClassFile(addSelfHeaderInclude);

	return retVal;
}


bool CGqlHandlerBaseClassGeneratorComp::ProcessHeaderClassFile(bool addDependenciesInclude)
{
	QTextStream ifStream(m_headerFilePtr.GetPtr());

	// preprocessor's section
	ifStream << QStringLiteral("#pragma once");
	FeedStream(ifStream, 3, false);

	// add generated includes
	if (addDependenciesInclude){
		AddRequiredIncludesForDocument(ifStream);
		FeedStream(ifStream, 1, false);
	}

	// namespace begin

	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
				m_customSchemaParamsCompPtr,
				m_argumentParserCompPtr,
				false);
	ifStream << QStringLiteral("namespace ");
	ifStream <<  sdlNamespace;
	FeedStream(ifStream, 1, false);

	ifStream <<  QStringLiteral("{");
	FeedStream(ifStream, 3, false);

	// class begin
	ifStream << QStringLiteral("class CGraphQlHandlerCompBase");
	FeedStream(ifStream, 1, false);

	ifStream << QStringLiteral("{");
	FeedStream(ifStream, 1, false);

	// public section
	ifStream << QStringLiteral("public:");
	FeedStream(ifStream, 1, false);

	// base class methods override definition
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("virtual ::imtbase::CTreeItemModel* CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;");
	FeedStream(ifStream, 2, false);

	// protected section
	// definition of pure virtual methods (to be reimplemented)
	ifStream << QStringLiteral("protected:");
	FeedStream(ifStream, 1, false);

	AddMethodsForDocument(ifStream, 1);

	// class end
	ifStream << QStringLiteral("};");
	FeedStream(ifStream, 3, false);

	// end of namespace
	ifStream << QStringLiteral("} // namespace ");
	ifStream << sdlNamespace;
	FeedStream(ifStream, 1, true);

	return true;
}


bool CGqlHandlerBaseClassGeneratorComp::ProcessSourceClassFile(bool addSelfHeaderInclude)
{
	QTextStream ifStream(m_sourceFilePtr.GetPtr());

	// include section
	if (addSelfHeaderInclude){
		ifStream << QStringLiteral("#include \"");
		ifStream << WrapFileName(QStringLiteral("h")) << '"';
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
		FeedStream(ifStream, 3, false);
	}

	AddCollectionMethodsImplForDocument(ifStream);

	// end of namespace
	FeedStream(ifStream, 2, false);
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("} // namespace ");
		ifStream << sdlNamespace;
	}
	FeedStream(ifStream, 1, true);

	return true;
}


void CGqlHandlerBaseClassGeneratorComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	I_CRITICAL();

	m_headerFilePtr->remove();
	m_sourceFilePtr->remove();
}


void CGqlHandlerBaseClassGeneratorComp::AddRequiredIncludesForDocument(QTextStream& stream, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// generated SDL includes");
	FeedStream(stream, 1, false);

	const imtsdl::SdlRequestList requestList = m_sdlRequestListCompPtr->GetRequests();
	for (const imtsdl::CSdlRequest& sdlRequest: requestList){
		stream << QStringLiteral("#include \"C");
		stream << sdlRequest.GetName();
		stream << QStringLiteral("GqlRequest.h\"");
		FeedStream(stream, 1, false);
	}
	FeedStream(stream, 1, false);
}


void CGqlHandlerBaseClassGeneratorComp::AddMethodsForDocument(QTextStream& stream, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// abstract methods");
	FeedStream(stream, 1, false);

	const imtsdl::SdlRequestList requestList = m_sdlRequestListCompPtr->GetRequests();
	for (const imtsdl::CSdlRequest& sdlRequest: requestList){
		AddMethodForDocument(stream, sdlRequest, hIndents);
	}
}


void CGqlHandlerBaseClassGeneratorComp::AddMethodForDocument(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, uint hIndents)
{
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
				m_customSchemaParamsCompPtr,
				m_argumentParserCompPtr,
				false);

	imtsdl::CSdlField outputArgument = sdlRequest.GetOutputArgument();
	CStructNamespaceConverter structNameConverter(
			outputArgument,
			sdlNamespace, 
			*m_sdlTypeListCompPtr, 
			*m_sdlEnumListCompPtr,
			false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("virtual ");
	stream << structNameConverter.GetString();
	stream << QStringLiteral(" On");
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("(const C");
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("GqlRequest& ");
	stream << GetDecapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("Request, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;");

	FeedStream(stream, 1, false);
}

void CGqlHandlerBaseClassGeneratorComp::AddCollectionMethodsImplForDocument(QTextStream& stream, uint hIndents)
{
	const QString className = QStringLiteral("CGraphQlHandlerCompBase");
	const imtsdl::SdlRequestList requestList = m_sdlRequestListCompPtr->GetRequests();

	// declare method
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("::imtbase::CTreeItemModel* ");
	stream << className;
	stream << ':' << ':';
	stream << QStringLiteral("CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const");
	FeedStream(stream, 1, false);

	// method's body
	FeedStreamHorizontally(stream, hIndents);
	stream << '{';
	FeedStream(stream, 1, false);
	if (requestList.isEmpty()){
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return BaseClass::CreateInternalResponse");
		stream << QStringLiteral("(gqlRequest, errorMessage);");
		FeedStream(stream, 1, false);
	}
	else {
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("const QByteArray commandId = gqlRequest.GetCommandId();");
		FeedStream(stream, 1, false);

		/// create a carrier model GQL spec	\link https://spec.graphql.org/draft/#sec-Response-Format
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("istd::TDelPtr<::imtbase::CTreeItemModel> modelPtr(new ::imtbase::CTreeItemModel);");
		FeedStream(stream, 1, false);

		/// \todo add errors model and don't forget to fill it	\link https://spec.graphql.org/draft/#sec-Errors

		// create dataModel - child of a carrier model
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("::imtbase::CTreeItemModel* dataModelPtr = modelPtr->AddTreeModel(\"data\");");
		FeedStream(stream, 2, false);

		// create sections for expected command IDs
		for (const imtsdl::CSdlRequest& sdlRequest: requestList){
			AddImplCodeForRequest(stream, sdlRequest, hIndents + 1);
		}

		// create default section
		// add error message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected command-ID: '%1'\").arg(qPrintable(commandId));");
		FeedStream(stream, 1, false);

		// send log message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
		FeedStream(stream, 2, false);

		// return
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return nullptr;");
		FeedStream(stream, 1, false);


		// end of method
		FeedStreamHorizontally(stream, hIndents);
		stream << '}';
		FeedStream(stream, 3, false);
	}
}


void CGqlHandlerBaseClassGeneratorComp::AddImplCodeForRequest(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << '/' << '/' << ' ' << sdlRequest.GetName();
	FeedStream(stream, 1, false);

	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
		m_customSchemaParamsCompPtr,
		m_argumentParserCompPtr,
		false);
	const QString requestClassName = sdlRequest.GetName() + QStringLiteral("GqlRequest");
	imtsdl::CSdlField outputArgument = sdlRequest.GetOutputArgument();
	CStructNamespaceConverter structNameConverter(
		outputArgument,
		sdlNamespace,
		*m_sdlTypeListCompPtr,
		*m_sdlEnumListCompPtr,
		false);

	// [1] command ID check
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (commandId == C");
	stream << requestClassName;
	stream << QStringLiteral("::GetCommandId()){");
	FeedStream(stream, 1, false);

	// [1] create SDL request variable
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << 'C' <<  requestClassName;
	stream << ' ' << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral("(gqlRequest, false);");
	FeedStream(stream, 1, false);

	// [1->2] SDL request validate
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!");
	stream << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral(".IsValid()){");
	FeedStream(stream, 1, false);

	// [2] add error message if SDL request is not valid
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected request for command-ID: '%1'\").arg(qPrintable(commandId));");
	FeedStream(stream, 1, false);

	// [2] add log message
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
	FeedStream(stream, 2, false);

	// [2] return
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream, 1, false);

	// [2->1] end of SDL request validate
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 2, false);

	// [1] create payload variable by calling reimplemented method
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << structNameConverter.GetString();
	stream << QStringLiteral(" replyPayload = On");
	stream << sdlRequest.GetName() << '(';
	stream << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral(", gqlRequest, errorMessage);");
	FeedStream(stream, 1, false);

	// [1->2] ensure, the derived call didn't return an error
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!errorMessage.isEmpty()){");
	FeedStream(stream, 1, false);

	// [2] send error message
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("SendErrorMessage(0, QString(\"The derived call [");
	stream << QStringLiteral("On") << sdlRequest.GetName();
	stream << QStringLiteral("] returned an error: %1\").arg(errorMessage));");
	FeedStream(stream, 2, false);

	// [2] return
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream, 1, false);

	// [2->1] end of derived error checks
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 2, false);

	// [1] write payload variable in model and create variable, to check if it success
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("const bool isModelCreated = ");
	stream << QStringLiteral("replyPayload.WriteToModel(*dataModelPtr);");
	FeedStream(stream, 1, false);

	// [1->2] check if payload write to TreeModel is failed
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!isModelCreated){");
	FeedStream(stream, 1, false);

	// [2] set error message
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("errorMessage = QString(\"Internal error. Unable to create response for command-ID: '%1'\").arg(qPrintable(commandId));");
	FeedStream(stream, 1, false);

	// [2] add log message
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("SendCriticalMessage(0, errorMessage);");
	FeedStream(stream, 2, false);

	// [2] return
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream, 1, false);

	// [2->1] end of payload write to TreeModel checks
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 2, false);

	// [1] return pop ptr
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return modelPtr.PopPtr();");
	FeedStream(stream, 1, false);

	// end of section
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 2, false);
}


} // namespace imtsdlgenv2

