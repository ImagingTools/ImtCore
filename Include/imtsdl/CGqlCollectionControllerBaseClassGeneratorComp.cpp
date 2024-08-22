#include "CGqlCollectionControllerBaseClassGeneratorComp.h"


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

// imtsdl includes
#include <imtfile/CSimpleFileJoinerComp.h>
#include <imtsdl/CSdlRequest.h>


namespace imtsdl
{


int CGqlCollectionControllerBaseClassGeneratorComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlRequestListCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_baseClassExtenderCompPtr.IsValid());
	Q_ASSERT(m_sdlDocumentListCompPtr.IsValid());

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

	const QMap<QString, QString> joinRules = m_argumentParserCompPtr->GetJoinRules();
	const bool joinHeaders = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_headerFileType);
	const bool joinSources = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_sourceFileType);

	SdlDocumentTypeList sdlDocumentTypeList = m_sdlDocumentListCompPtr->GetDocumentTypes();
	if (m_argumentParserCompPtr->IsDependenciesMode()){
		for (const CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
			if (!joinHeaders) {
				std::cout << WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h"), outputDirectoryPath).toStdString() << std::endl;
			}
			if (!joinSources){
				std::cout << WrapFileName(sdlDocumentType.GetName(), QStringLiteral("cpp"), outputDirectoryPath).toStdString() << std::endl;
			}
		}
		if (joinHeaders){
			std::cout << joinRules[imtsdl::ISdlProcessArgumentsParser::s_headerFileType].toStdString() << std::endl;
		}
		if (joinSources){
			std::cout << joinRules[imtsdl::ISdlProcessArgumentsParser::s_sourceFileType].toStdString() << std::endl;
		}

		return TS_OK;
	}

	for (const CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
		m_headerFilePtr.SetPtr(new QFile(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h"), outputDirectoryPath)));
		m_sourceFilePtr.SetPtr(new QFile(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("cpp"), outputDirectoryPath)));

		if (!ProcessFiles(sdlDocumentType, !joinHeaders, !joinSources)){
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
		baseClassDirectivesList.InsertOption(QStringLiteral("imtgql/CObjectCollectionControllerCompBase.h"), QByteArrayLiteral("imtgql::CObjectCollectionControllerCompBase"));

		int extendResult = m_baseClassExtenderCompPtr->DoProcessing(&paramsSet, &baseClassDirectivesList, nullptr);
		if (extendResult != TS_OK){
			SendErrorMessage(0, QString("Unable to extend file'%1'").arg(m_headerFilePtr->fileName()));
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
				for (const CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
					filterParams.InsertOption(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h")), QByteArray::number(filterParams.GetOptionsCount()));
				}

				outputFileNameParam.SetPath(joinRules[imtsdl::ISdlProcessArgumentsParser::s_headerFileType]);
				int joinProcessResult = m_filesJoinerCompPtr->DoProcessing(&inputParams, &filterParams, nullptr);
				if (joinProcessResult != TS_OK){
					SendCriticalMessage(0, "Unable to join header files");
					I_CRITICAL();

					return TS_INVALID;
				}

				// cleanup joined files
				for (const CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
					QFile::remove(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h"), outputDirectoryPath));
				}
			}
			if (joinSources){
				filterParams.ResetOptions();
				for (const CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
					filterParams.InsertOption(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("cpp")), QByteArray::number(filterParams.GetOptionsCount()));
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
				for (const CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
					QFile::remove(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("cpp"), outputDirectoryPath));
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


// private static methods

QString CGqlCollectionControllerBaseClassGeneratorComp::WrapFileName(const QString& baseName, const QString& ext, const QString& directoryPath)
{
	QString retVal;
	Q_ASSERT_X(!ext.isEmpty(), __func__, "Extension for file MUST be specified");

	if (!directoryPath.isEmpty()){
		retVal.append(directoryPath);
		if (!retVal.endsWith('/') || !retVal.endsWith(QDir::separator())){
			retVal.append('/');
		}
	}
	retVal += 'C';
	retVal.append(baseName);
	retVal.append(QStringLiteral("CollectionControllerCompBase."));
	retVal.append(ext);

	return retVal;
}


// private methods

bool CGqlCollectionControllerBaseClassGeneratorComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_headerFilePtr->flush();
	Q_ASSERT(retVal);

	retVal = m_sourceFilePtr->flush() && retVal;
	Q_ASSERT(retVal);

	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	return retVal;
}


bool CGqlCollectionControllerBaseClassGeneratorComp::ProcessFiles(const CSdlDocumentType& sdlDocumentType, bool addDependenciesInclude, bool addSelfHeaderInclude)
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
	retVal = retVal && ProcessHeaderClassFile(sdlDocumentType, addDependenciesInclude);
	retVal = retVal && ProcessSourceClassFile(sdlDocumentType, addSelfHeaderInclude);

	return retVal;
}


bool CGqlCollectionControllerBaseClassGeneratorComp::ProcessHeaderClassFile(const CSdlDocumentType& sdlDocumentType, bool addDependenciesInclude)
{
	QTextStream ifStream(m_headerFilePtr.GetPtr());

	// preprocessor's section
	ifStream << QStringLiteral("#pragma once");
	FeedStream(ifStream, 3, false);

	const SdlDocumentTypeList subtypesList = sdlDocumentType.GetSubtypes();

	// add generated includes
	if (addDependenciesInclude){
		AddRequiredIncludesForDocument(ifStream, sdlDocumentType);
		FeedStream(ifStream, 1, false);
		for (const CSdlDocumentType& subtype: subtypesList){
			AddRequiredIncludesForDocument(ifStream, subtype);
			FeedStream(ifStream, 1, false);
		}
		FeedStream(ifStream, 1, false);
	}

	// namespace begin
	const QString sdlNamespace = m_argumentParserCompPtr->GetNamespace();
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("namespace ");
		ifStream <<  sdlNamespace;
		FeedStream(ifStream, 1, false);
		ifStream <<  QStringLiteral("{");
		FeedStream(ifStream, 1, false);
	}

	// class begin
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("class C") << sdlDocumentType.GetName() << QStringLiteral("CollectionControllerCompBase");
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("{");
	FeedStream(ifStream, 1, false);

	// public section
	/// \bug if no public section, base class macro will not be added
	/// \todo fix it
	ifStream << QStringLiteral("public:");
	FeedStream(ifStream, 1, false);

	// protected section (base)
	ifStream << QStringLiteral("protected:");
	FeedStream(ifStream, 1, false);

	// base class methods override definition
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("// reimplemented (imtgql::CObjectCollectionControllerCompBase)");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("virtual bool SetupGqlItem(const imtgql::CGqlRequest& gqlRequest, imtbase::CTreeItemModel& model, int itemIndex,const imtbase::IObjectCollectionIterator* objectCollectionIterator, QString& errorMessage) const override;");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("virtual bool CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const imtgql::CGqlRequest& gqlRequest, imtbase::CTreeItemModel& dataModel) const override;");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("virtual istd::IChangeable* CreateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& name, QString& description, QString& errorMessage) const override;");
	FeedStream(ifStream, 1, false);

	// protected section
	// definition of pure virtual methods (to be reimplemented)
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("protected:");
	FeedStream(ifStream, 1, false);

	AddMethodsForDocument(ifStream, sdlDocumentType, 1);
	FeedStream(ifStream, 1, false);
	for (const CSdlDocumentType& subtype: subtypesList){
		AddMethodsForDocument(ifStream, subtype, 1);
		FeedStream(ifStream, 1, false);
	}

	// class end
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


bool CGqlCollectionControllerBaseClassGeneratorComp::ProcessSourceClassFile(const CSdlDocumentType& sdlDocumentType, bool addSelfHeaderInclude)
{
	QTextStream ifStream(m_sourceFilePtr.GetPtr());

	// include section
	if (addSelfHeaderInclude){
		ifStream << QStringLiteral("#include \"");
		ifStream << WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h")) << '"';
		FeedStream(ifStream, 2);
	}
	FeedStream(ifStream, 1);

	// namespace begin
	const QString sdlNamespace = m_argumentParserCompPtr->GetNamespace();
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("namespace ");
		ifStream << sdlNamespace;
		FeedStream(ifStream, 1, false);
		ifStream << '{';
		FeedStream(ifStream, 3, false);
	}

	AddCollectionMethodsImplForDocument(ifStream, sdlDocumentType);

	// end of namespace
	FeedStream(ifStream, 2, false);
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("} // namespace ");
		ifStream << sdlNamespace;
	}
	FeedStream(ifStream, 1, true);

	return true;
}


void CGqlCollectionControllerBaseClassGeneratorComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	I_CRITICAL();

	m_headerFilePtr->remove();
	m_sourceFilePtr->remove();
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddRequiredIncludesForDocument(QTextStream& stream, const CSdlDocumentType& sdlDocumentType, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// ");
	stream << sdlDocumentType.GetName();
	stream << QStringLiteral(" includes");
	FeedStream(stream, 1, false);

	const QMap<CSdlDocumentType::OperationType, CSdlRequest> operationsList = sdlDocumentType.GetOperationsList();
	for (auto operationIter = operationsList.cbegin(); operationIter != operationsList.cend(); ++operationIter){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("#include \"C");
		stream << GetCapitalizedValue(operationIter->GetName());
		stream << QStringLiteral("GqlRequest.h\"");

		FeedStream(stream, 1, false);
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddMethodsForDocument(QTextStream& stream, const CSdlDocumentType& sdlDocumentType, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// ");
	stream << sdlDocumentType.GetName();
	stream << QStringLiteral(" methods");
	FeedStream(stream, 1, false);

	const QString documentClassName = sdlDocumentType.GetReferenceType().GetName();

	const QMap<CSdlDocumentType::OperationType, CSdlRequest> operationsList = sdlDocumentType.GetOperationsList();

	if (	operationsList.contains(CSdlDocumentType::OT_UPDATE) ||
			operationsList.contains(CSdlDocumentType::OT_INSERT))
	{
		AddMethodForDocument(stream, CSdlRequest(), CSdlDocumentType::OT_UPDATE, documentClassName, hIndents);
	}
	QList<CSdlRequest> implementedGetRequests;
	QMapIterator operationsIter(operationsList);
	while (operationsIter.hasNext()){
		auto operation = operationsIter.next();
		CSdlRequest sdlRequest = operation.value();
		CSdlDocumentType::OperationType operationType = operation.key();
		if (operationType == CSdlDocumentType::OT_GET || operationType == CSdlDocumentType::OT_LIST){
			if (!implementedGetRequests.contains(sdlRequest)){
				AddMethodForDocument(stream, operation.value(), operationType, documentClassName, hIndents);
				implementedGetRequests << sdlRequest;
			}
		}
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddMethodForDocument(QTextStream& stream, const CSdlRequest& sdlRequest, CSdlDocumentType::OperationType operationType, const QString& itemClassName, uint hIndents)
{
	if (operationType == CSdlDocumentType::OT_GET ||
		operationType == CSdlDocumentType::OT_LIST)
	{
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("virtual bool CreateRepresentationFromObject(const ");

		if (operationType == CSdlDocumentType::OT_GET){
			stream << QStringLiteral("istd::IChangeable& data");
		}
		else {
			stream << QStringLiteral("imtbase::IObjectCollectionIterator& objectCollectionIterator");
		}
		stream << QStringLiteral(", const C");
		stream << GetCapitalizedValue(sdlRequest.GetName());
		stream << QStringLiteral("GqlRequest& ");
		stream << GetDecapitalizedValue(sdlRequest.GetName());
		stream << QStringLiteral("Request, C");
		stream << itemClassName;
		stream << QStringLiteral("& representationObject) const = 0;");
		FeedStream(stream, 1, false);
	}
	else if (operationType == CSdlDocumentType::OT_UPDATE){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("virtual istd::IChangeable* CreateObjectFromRepresentation(const C");
		stream << itemClassName;
		stream << QStringLiteral("& representationObject, QByteArray& newObjectId, QString& name, QString& description, QString& errorMessage) const = 0;");
		FeedStream(stream, 1, false);
	}
	else {
		I_CRITICAL();
	}
}

void CGqlCollectionControllerBaseClassGeneratorComp::AddCollectionMethodsImplForDocument(QTextStream& stream, const CSdlDocumentType& sdlDocumentType)
{
	const QString className = 'C' + sdlDocumentType.GetName() + QStringLiteral("CollectionControllerCompBase");
	QMultiMap<CSdlDocumentType::OperationType, CSdlRequest> requestMultiMap(sdlDocumentType.GetOperationsList());
	for (const CSdlDocumentType& documentType: sdlDocumentType.GetSubtypes()){
		requestMultiMap += QMultiMap<CSdlDocumentType::OperationType, CSdlRequest>(documentType.GetOperationsList());
	}
	QList<CSdlDocumentType::OperationType> remainingOperations({
				CSdlDocumentType::OT_GET,
				CSdlDocumentType::OT_INSERT,
				CSdlDocumentType::OT_UPDATE,
				CSdlDocumentType::OT_LIST});

	// join update and insert. it has same code
	if (requestMultiMap.contains(CSdlDocumentType::OT_INSERT) && requestMultiMap.contains(CSdlDocumentType::OT_UPDATE)) {
		remainingOperations.removeAll(CSdlDocumentType::OT_UPDATE);
		const SdlRequestList requestList = requestMultiMap.values(CSdlDocumentType::OT_UPDATE);
		for (const CSdlRequest& request: requestList){
			requestMultiMap.insert(CSdlDocumentType::OT_INSERT, request);
		}
		requestMultiMap.remove(CSdlDocumentType::OT_UPDATE);
	}

	for (CSdlDocumentType::OperationType operationType: requestMultiMap.uniqueKeys()){
		SdlRequestList requestList = requestMultiMap.values(operationType);
		AddImplCodeForRequests(stream, operationType, requestList, className);
		remainingOperations.removeAll(operationType);
	}
}

void CGqlCollectionControllerBaseClassGeneratorComp::AddImplCodeForRequests(
			QTextStream& stream,
			CSdlDocumentType::OperationType operationType,
			const SdlRequestList& requestList,
			const QString& className,
			uint hIndents)
{
	// declare method
	FeedStreamHorizontally(stream, hIndents);

	// a) type
	switch (operationType) {
	case CSdlDocumentType::OT_GET:
	case CSdlDocumentType::OT_LIST:
		stream << QStringLiteral("bool ");
		break;
	case CSdlDocumentType::OT_UPDATE:
	case CSdlDocumentType::OT_INSERT:
		stream << QStringLiteral("istd::IChangeable* ");
		break;
	case CSdlDocumentType::OT_DELETE:
		// nothing todo
		break;
	default:
		SendCriticalMessage(0, QString("Unexpected type: %1").arg(QString::number(operationType)));
		I_CRITICAL();
		break;
	}

	stream << className << ':' << ':';

	// b) name and argset
	switch (operationType) {
	case CSdlDocumentType::OT_LIST:
		stream << QStringLiteral("SetupGqlItem(const imtgql::CGqlRequest& gqlRequest, imtbase::CTreeItemModel& model, int itemIndex,const imtbase::IObjectCollectionIterator* objectCollectionIterator, QString& errorMessage) const");
		break;
	case CSdlDocumentType::OT_GET:
		stream << QStringLiteral("CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const imtgql::CGqlRequest& gqlRequest, imtbase::CTreeItemModel& dataModel) const");
		break;
	case CSdlDocumentType::OT_UPDATE:
	case CSdlDocumentType::OT_INSERT:
		stream << QStringLiteral("CreateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& name, QString& description, QString& errorMessage) const");
		break;
	default:
		break;
	}

	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '{';
	FeedStream(stream, 1, false);



	// method's body
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("const QByteArray commandId = gqlRequest.GetCommandId();");
	FeedStream(stream, 2, false);

	// create sections for expected command IDs
	for (const CSdlRequest& sdlRequest: requestList){
//		AddImplCodeForRequest(stream, sdlRequest, operationType, hIndents + 1);
	}

	FeedStreamHorizontally(stream, hIndents + 1);
	switch (operationType) {
	case CSdlDocumentType::OT_LIST:
	case CSdlDocumentType::OT_GET:
		stream << QStringLiteral("return false;");
		break;
	case CSdlDocumentType::OT_UPDATE:
	case CSdlDocumentType::OT_INSERT:
		stream << QStringLiteral("return nullptr;");
		break;
	default:
		break;
	}
	FeedStream(stream, 1, false);

	// end of method
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 3, false);

}

void CGqlCollectionControllerBaseClassGeneratorComp::AddImplCodeForRequest(QTextStream& stream, const CSdlRequest& sdlRequest, CSdlDocumentType::OperationType operationType, uint hIndents)
{
	if (	operationType == CSdlDocumentType::OT_LIST ||
			operationType == CSdlDocumentType::OT_GET){
		return;
	}
	FeedStreamHorizontally(stream, hIndents);
	stream << '/' << '/' << sdlRequest.GetName();
	FeedStream(stream, 1, false);

	const QString requestClassName = sdlRequest.GetName() + QStringLiteral("GqlRequest");

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
	stream << QStringLiteral("(gqlRequest);");
	FeedStream(stream, 1, false);

	// [1->2] SDL request validate
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!");
	stream << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral(".IsValid()){");
	FeedStream(stream, 1, false);

	// [2] add error message if SDL request is not valid
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected request for command id '%1'\").arg(commandId);");
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
	FeedStream(stream, 1, false);

	// [1] create payload variable by calling reimplemented method
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << 'C' << sdlRequest.GetOutputArgument().GetType();
	stream << QStringLiteral(" replyPayload = On");
	stream << sdlRequest.GetName() << '(';
	stream << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral(", gqlRequest, errorMessage);");
	FeedStream(stream, 1, false);

	// [1] write payload variable in model and create variable, to check if it success
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("const bool isModelCreated = replyPayload.WriteToModel(*dataModelPtr);");
	FeedStream(stream, 1, false);

	// [1->2] check if payload write to TreeModel is failed
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!isModelCreated){");
	FeedStream(stream, 1, false);

	// [2] set error message
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("errorMessage = QString(\"Internal error. Unable to create response for command id '%1'\").arg(commandId);");
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


	// create default section
	// add error message
	FeedStreamHorizontally(stream, hIndents + 1);
	//	stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected command id '%1'\").arg(commandId);");
	FeedStream(stream, 1, false);

	// send log message
	FeedStreamHorizontally(stream, hIndents + 1);
	//	stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
	FeedStream(stream, 2, false);
}


} // namespace imtsdl

