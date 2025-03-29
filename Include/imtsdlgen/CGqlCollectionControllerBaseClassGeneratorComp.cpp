#include "CGqlCollectionControllerBaseClassGeneratorComp.h"


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


namespace imtsdlgen
{


// static variables
const QMap<imtsdl::CSdlDocumentType::OperationType, QString> CGqlCollectionControllerBaseClassGeneratorComp::s_nonTrivialOperationMethodsMap = {
	std::make_pair(imtsdl::CSdlDocumentType::OT_UPDATE_COLLECTION,		"UpdateCollection"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_RENAME,					"RenameObject"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_SET_DESCRIPTION,		"SetObjectDescription"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_HEADERS,				"GetHeaders"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_INFO,					"GetInfo"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_METAINFO,				"GetMetaInfo"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_DATAMETAINFO,			"GetDataMetaInfo"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_ELEMENT_HISTORY,		"GetObjectHistory"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_IMPORT,					"ImportObject"),
	std::make_pair(imtsdl::CSdlDocumentType::OT_EXPORT,					"ExportObject")
};


// public methods

// reimplemented(iproc::IProcessor)

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

	const QString defaultName = QFileInfo(m_argumentParserCompPtr->GetSchemaFilePath()).fileName();
	QMap<QString, QString> joinRules = m_argumentParserCompPtr->GetJoinRules();
	if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
		joinRules = CalculateTargetCppFilesFromSchemaParams(*m_customSchemaParamsCompPtr, m_argumentParserCompPtr->GetOutputDirectoryPath(), defaultName);
	}
	const bool joinHeaders = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_headerFileType);
	const bool joinSources = joinRules.contains(imtsdl::ISdlProcessArgumentsParser::s_sourceFileType);

	imtsdl::SdlDocumentTypeList sdlDocumentTypeList = m_sdlDocumentListCompPtr->GetDocumentTypes(false);
	if (m_argumentParserCompPtr->IsDependenciesMode()){
		if (!m_argumentParserCompPtr->IsAutoJoinEnabled()){
			QStringList cumulatedFiles;
			for (const imtsdl::CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
				if (!joinHeaders){
					cumulatedFiles << WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h"), outputDirectoryPath);
				}
				if (!joinSources){
					cumulatedFiles << WrapFileName(sdlDocumentType.GetName(), QStringLiteral("cpp"), outputDirectoryPath);
				}
			}

			PrintFiles(std::cout, cumulatedFiles, m_argumentParserCompPtr->GetGeneratorType());
		}

		return TS_OK;
	}

	for (const imtsdl::CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
		m_headerFilePtr.SetPtr(new QFile(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h"), outputDirectoryPath)));
		m_sourceFilePtr.SetPtr(new QFile(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("cpp"), outputDirectoryPath)));

		if (!ProcessFiles(sdlDocumentType, !joinHeaders, !joinSources)){
			SendErrorMessage(0, QString("Unable to process files"));

			return TS_INVALID;
		}

		if (!CloseFiles()){
			SendErrorMessage(0, QString("Unable to finalize files"));

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
		baseClassDirectivesList.InsertOption(QStringLiteral("imtservergql/CObjectCollectionControllerCompBase.h"), QByteArrayLiteral("::imtservergql::CObjectCollectionControllerCompBase"));

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
				for (const imtsdl::CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
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
				for (const imtsdl::CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
					QFile::remove(WrapFileName(sdlDocumentType.GetName(), QStringLiteral("h"), outputDirectoryPath));
				}
			}
			if (joinSources){
				filterParams.ResetOptions();
				for (const imtsdl::CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
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
				for (const imtsdl::CSdlDocumentType& sdlDocumentType: sdlDocumentTypeList){
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

	retVal = m_sourceFilePtr->flush() && retVal;

	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	return retVal;
}


bool CGqlCollectionControllerBaseClassGeneratorComp::ProcessFiles(const imtsdl::CSdlDocumentType& sdlDocumentType, bool addDependenciesInclude, bool addSelfHeaderInclude)
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
	retVal = retVal && ProcessHeaderClassFile(sdlDocumentType, addDependenciesInclude);
	retVal = retVal && ProcessSourceClassFile(sdlDocumentType, addSelfHeaderInclude);

	return retVal;
}


bool CGqlCollectionControllerBaseClassGeneratorComp::ProcessHeaderClassFile(const imtsdl::CSdlDocumentType& sdlDocumentType, bool addDependenciesInclude)
{
	QTextStream ifStream(m_headerFilePtr.GetPtr());

	// preprocessor's section
	ifStream << QStringLiteral("#pragma once");
	FeedStream(ifStream, 3, false);

	const imtsdl::SdlDocumentTypeList subtypesList = sdlDocumentType.GetSubtypes();

	// add generated includes
	if (addDependenciesInclude){
		AddRequiredIncludesForDocument(ifStream, sdlDocumentType);
		FeedStream(ifStream, 1, false);
		for (const imtsdl::CSdlDocumentType& subtype: subtypesList){
			AddRequiredIncludesForDocument(ifStream, subtype);
			FeedStream(ifStream, 1, false);
		}
		FeedStream(ifStream, 1, false);
	}

	// namespace begin
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr);
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

	QList<imtsdl::CSdlDocumentType::OperationType> operationsList = sdlDocumentType.GetOperationsList().keys();
	for (const imtsdl::CSdlDocumentType& subtype : sdlDocumentType.GetSubtypes()){
		operationsList << subtype.GetOperationsList().keys();
	}

	// base class methods override definition
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("// reimplemented (::imtservergql::CObjectCollectionControllerCompBase)");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("virtual bool GetOperationFromRequest(const ::imtgql::CGqlRequest& gqlRequest, ::imtgql::CGqlObject& gqlObject, QString& errorMessage, int& operationType) const override;");
	FeedStream(ifStream, 1, false);

	if (operationsList.contains(imtsdl::CSdlDocumentType::OT_LIST)){
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("virtual bool SetupGqlItem(const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, int itemIndex,const ::imtbase::IObjectCollectionIterator* objectCollectionIterator, QString& errorMessage) const override;");
		FeedStream(ifStream, 1, false);
		operationsList.removeAll(imtsdl::CSdlDocumentType::OT_LIST);
	}

	if (operationsList.contains(imtsdl::CSdlDocumentType::OT_GET)){
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("virtual bool CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, QString& errorMessage) const override;");
		FeedStream(ifStream, 1, false);
		operationsList.removeAll(imtsdl::CSdlDocumentType::OT_GET);
	}

	if (operationsList.contains(imtsdl::CSdlDocumentType::OT_UPDATE))
	{
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("virtual bool UpdateObjectFromRequest(const ::imtgql::CGqlRequest& gqlRequest, istd::IChangeable& object, QString& errorMessage) const override;");
		FeedStream(ifStream, 1, false);
		operationsList.removeAll(imtsdl::CSdlDocumentType::OT_UPDATE);
	}

	if (operationsList.contains(imtsdl::CSdlDocumentType::OT_INSERT))
	{
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("virtual istd::IChangeable* CreateObjectFromRequest(const ::imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const override;");
		FeedStream(ifStream, 1, false);
		operationsList.removeAll(imtsdl::CSdlDocumentType::OT_INSERT);
	}

	// remove all collection trtivial types
	operationsList.removeAll(imtsdl::CSdlDocumentType::OT_DELETE);
	operationsList.removeAll(imtsdl::CSdlDocumentType::OT_ELEMENT_IDS);
	operationsList.removeAll(imtsdl::CSdlDocumentType::OT_ELEMENTS_COUNT);

	// add methods for special cases (non-trivial collection methods)
	for (const imtsdl::CSdlDocumentType::OperationType& operationType: operationsList){
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("virtual ");
		AddBaseMethodDeclarationForOperationType(ifStream, operationType);
		ifStream << QStringLiteral(" override;");
		FeedStream(ifStream, 1, false);
	}


	// protected section
	// definition of pure virtual methods (to be reimplemented)
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("protected:");
	FeedStream(ifStream, 1, false);

	AddMethodsForDocument(ifStream, sdlDocumentType, 1);
	FeedStream(ifStream, 1, false);

	for (const imtsdl::CSdlDocumentType& subtype: subtypesList){
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


bool CGqlCollectionControllerBaseClassGeneratorComp::ProcessSourceClassFile(const imtsdl::CSdlDocumentType& sdlDocumentType, bool addSelfHeaderInclude)
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
	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr);
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("namespace ");
		ifStream << sdlNamespace;
		FeedStream(ifStream, 1, false);
		ifStream << '{';
		FeedStream(ifStream, 3, false);
	}

	AddOperationRequestMethodImplForDocument(ifStream, sdlDocumentType);

	AddCollectionMethodsImplForDocument(ifStream, sdlDocumentType);

	AddSpecialMethodImplCodeForDocument(ifStream, sdlDocumentType);


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


void CGqlCollectionControllerBaseClassGeneratorComp::AddMethodDeclarationForOperationType(QTextStream& stream, imtsdl::CSdlDocumentType::OperationType operationType, const imtsdl::CSdlRequest& sdlRequest)
{
	if (!s_nonTrivialOperationMethodsMap.contains(operationType)){
		SendCriticalMessage(0, "Unexpected type");
		I_CRITICAL();

		return;
	}

	QString requestCalssName = sdlRequest.GetName();
	requestCalssName[0] = requestCalssName[0].toUpper();
	QString functionName = s_nonTrivialOperationMethodsMap[operationType];
	functionName.prepend(QStringLiteral(" On"));

	stream << 'C' << GetCapitalizedValue(sdlRequest.GetOutputArgument().GetType());
	stream << functionName;

	stream << QStringLiteral("(const C");
	stream << requestCalssName;
	stream << QStringLiteral("GqlRequest& ");
	stream << GetDecapitalizedValue(requestCalssName);
	stream << QStringLiteral("GqlRequest, QString& errorMessage) const");
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddBaseMethodDeclarationForOperationType(QTextStream& stream, imtsdl::CSdlDocumentType::OperationType operationType)
{
	if (!s_nonTrivialOperationMethodsMap.contains(operationType)){
		SendCriticalMessage(0, "Unexpected type");
		I_CRITICAL();

		return;
	}

	stream << QStringLiteral("::imtbase::CTreeItemModel* ");
	stream << s_nonTrivialOperationMethodsMap[operationType];
	stream << QStringLiteral("(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const");
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddRequiredIncludesForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// ");
	stream << sdlDocumentType.GetName();
	stream << QStringLiteral(" includes");
	FeedStream(stream, 1, false);

	const QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> operationsList = sdlDocumentType.GetOperationsList();
	for (auto operationIter = operationsList.cbegin(); operationIter != operationsList.cend(); ++operationIter){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("#include \"C");
		stream << GetCapitalizedValue(operationIter->GetName());
		stream << QStringLiteral("GqlRequest.h\"");

		FeedStream(stream, 1, false);
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddMethodsForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// ");
	stream << sdlDocumentType.GetName();
	stream << QStringLiteral(" methods");
	FeedStream(stream, 1, false);

	const QString documentClassName = sdlDocumentType.GetReferenceType().GetName();

	const QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> operationsList = sdlDocumentType.GetOperationsList();

	QList<imtsdl::CSdlRequest> implementedGetRequests;
	QMultiMapIterator operationsIter(operationsList);
	while (operationsIter.hasNext()){
		auto operation = operationsIter.next();
		imtsdl::CSdlRequest sdlRequest = operation.value();
		imtsdl::CSdlDocumentType::OperationType operationType = operation.key();

		if (	operationType == imtsdl::CSdlDocumentType::OT_GET ||
				operationType == imtsdl::CSdlDocumentType::OT_LIST ||
				operationType == imtsdl::CSdlDocumentType::OT_UPDATE ||
				operationType == imtsdl::CSdlDocumentType::OT_INSERT)
		{
			if (!implementedGetRequests.contains(sdlRequest)){
				AddMethodForDocument(stream, operation.value(), operationType, documentClassName, hIndents);
				implementedGetRequests << sdlRequest;
			}
		}
		else if (	operationType != imtsdl::CSdlDocumentType::OT_UPDATE &&
					operationType != imtsdl::CSdlDocumentType::OT_INSERT &&
					operationType != imtsdl::CSdlDocumentType::OT_GET &&
					operationType != imtsdl::CSdlDocumentType::OT_LIST &&
					operationType != imtsdl::CSdlDocumentType::OT_DELETE &&
					operationType != imtsdl::CSdlDocumentType::OT_ELEMENT_IDS &&
					operationType != imtsdl::CSdlDocumentType::OT_ELEMENTS_COUNT)
		{
			AddMethodForDocument(stream, operation.value(), operationType, documentClassName, hIndents);
		}
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddMethodForDocument(
			QTextStream& stream,
			const imtsdl::CSdlRequest& sdlRequest,
			imtsdl::CSdlDocumentType::OperationType operationType,
			const QString& itemClassName,
			uint hIndents)
{
	if (operationType == imtsdl::CSdlDocumentType::OT_GET ||
		operationType == imtsdl::CSdlDocumentType::OT_LIST)
	{
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("virtual bool CreateRepresentationFromObject(");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("const ");

		if (operationType == imtsdl::CSdlDocumentType::OT_GET){
			stream << QStringLiteral("istd::IChangeable& data,");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 3);
			stream << QStringLiteral("const C");
			stream << GetCapitalizedValue(sdlRequest.GetName());
			stream << QStringLiteral("GqlRequest& ");
			stream << GetDecapitalizedValue(sdlRequest.GetName());
			stream << QStringLiteral("Request,");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 3);
			stream << 'C';
			stream << GetCapitalizedValue(sdlRequest.GetOutputArgument().GetType());
			stream << QStringLiteral("& representationPayload");
		}
		else {
			stream << QStringLiteral("::imtbase::IObjectCollectionIterator& objectCollectionIterator,");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 3);
			stream << QStringLiteral("const C");
			stream << GetCapitalizedValue(sdlRequest.GetName());
			stream << QStringLiteral("GqlRequest& ");
			stream << GetDecapitalizedValue(sdlRequest.GetName());
			stream << QStringLiteral("Request,");
			FeedStream(stream, 1, false);

			FeedStreamHorizontally(stream, hIndents + 3);
			stream << 'C';
			stream << itemClassName;
			stream << QStringLiteral("& representationObject");
		}
		stream << ',';
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("QString& errorMessage) const = 0;");
		FeedStream(stream, 1, false);
	}
	else if (operationType == imtsdl::CSdlDocumentType::OT_INSERT){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("virtual istd::IChangeable* CreateObjectFromRepresentation(");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("const C");
		stream << itemClassName;
		stream << QStringLiteral("& ");
		stream << GetDecapitalizedValue(itemClassName);
		stream << QStringLiteral("Representation,");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("QByteArray& newObjectId,");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("QString& errorMessage) const = 0;");
		FeedStream(stream, 1, false);
	}
	else if (operationType == imtsdl::CSdlDocumentType::OT_UPDATE){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("virtual bool UpdateObjectFromRepresentationRequest(");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("const ::imtgql::CGqlRequest& rawGqlRequest,");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("const C");
		stream << GetCapitalizedValue(sdlRequest.GetName());
		stream << QStringLiteral("GqlRequest& ");
		stream << GetDecapitalizedValue(sdlRequest.GetName());
		stream << QStringLiteral("Request,");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("istd::IChangeable& object,");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 3);
		stream << QStringLiteral("QString& errorMessage) const = 0;");
		FeedStream(stream, 1, false);
	}
	else if (		operationType == imtsdl::CSdlDocumentType::OT_UPDATE_COLLECTION ||
					operationType == imtsdl::CSdlDocumentType::OT_RENAME ||
					operationType == imtsdl::CSdlDocumentType::OT_SET_DESCRIPTION ||
					operationType == imtsdl::CSdlDocumentType::OT_HEADERS ||
					operationType == imtsdl::CSdlDocumentType::OT_INFO ||
					operationType == imtsdl::CSdlDocumentType::OT_METAINFO ||
					operationType == imtsdl::CSdlDocumentType::OT_DATAMETAINFO ||
					operationType == imtsdl::CSdlDocumentType::OT_ELEMENT_HISTORY ||
					operationType == imtsdl::CSdlDocumentType::OT_IMPORT ||
					operationType == imtsdl::CSdlDocumentType::OT_EXPORT)
	{
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("virtual ");
		AddMethodDeclarationForOperationType(stream, operationType, sdlRequest);
		stream << QStringLiteral(" = 0;");
		FeedStream(stream, 1, false);
	}

	else {
		SendCriticalMessage(0, QString("Unexpected request method: %1").arg(qPrintable(imtsdl::CSdlDocumentType::ToString(operationType))));
		I_CRITICAL();
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddImplCodeForSpecialRequest(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, imtsdl::CSdlDocumentType::OperationType operationType, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << '/' << '/' << ' ' << s_nonTrivialOperationMethodsMap[operationType];
	FeedStream(stream, 1, false);

	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr);
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
	stream << QStringLiteral("(gqlRequest, ");
	if (operationType == imtsdl::CSdlDocumentType::OT_UPDATE){
		stream << QStringLiteral("true");
	}
	else {
		stream << QStringLiteral("false");
	}
	stream << QStringLiteral(");");
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
	stream << OptListConvertTypeWithNamespace(sdlRequest.GetOutputArgument(), sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);
	stream << QStringLiteral(" replyPayload = On");
	stream << s_nonTrivialOperationMethodsMap[operationType] << '(';
	stream << GetDecapitalizedValue(requestClassName);
	stream << QStringLiteral(", errorMessage);");
	FeedStream(stream, 1, false);

	// [1->2] ensure, the derived call didn't return an error
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!errorMessage.isEmpty()){");
	FeedStream(stream, 1, false);

	// [2] send error message
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("SendErrorMessage(0, QString(\"The derived call [");
	stream << QStringLiteral("On") << s_nonTrivialOperationMethodsMap[operationType];
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
	stream << QStringLiteral("const bool isModelCreated = replyPayload.WriteToModel(*dataModelPtr);");
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


void CGqlCollectionControllerBaseClassGeneratorComp::AddOperationRequestMethodImplForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType)
{
	const QString className = 'C' + sdlDocumentType.GetName() + QStringLiteral("CollectionControllerCompBase");
	stream << QStringLiteral("bool ");
	stream << className << ':' << ':';
	stream << QStringLiteral("GetOperationFromRequest(const ::imtgql::CGqlRequest& gqlRequest, ::imtgql::CGqlObject& gqlObject, QString& errorMessage, int& operationType) const");
	FeedStream(stream, 1, false);

	stream << '{';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("const QByteArray commandId = gqlRequest.GetCommandId();");
	FeedStream(stream, 2, false);

	AddOperationRequestCheck(stream, sdlDocumentType);

	imtsdl::SdlDocumentTypeList subtypes = sdlDocumentType.GetSubtypes();
	for (const imtsdl::CSdlDocumentType& documentSubtype: subtypes){
		AddOperationRequestCheck(stream, documentSubtype);
	}

	FeedStreamHorizontally(stream);
	stream << QStringLiteral("return BaseClass::GetOperationFromRequest(gqlRequest, gqlObject, errorMessage, operationType);");
	FeedStream(stream, 1, false);

	stream << '}';
	FeedStream(stream, 3, false);
}



void CGqlCollectionControllerBaseClassGeneratorComp::AddOperationRequestCheck(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType)
{
	static QMap<imtsdl::CSdlDocumentType::OperationType, QString> operationsALiasList({
		std::make_pair(imtsdl::CSdlDocumentType::OT_GET,					QStringLiteral("OT_GET")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_INSERT,					QStringLiteral("OT_NEW")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_UPDATE,					QStringLiteral("OT_UPDATE")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_LIST,					QStringLiteral("OT_LIST")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_UPDATE_COLLECTION,		QStringLiteral("OT_UPDATE_COLLECTION")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_RENAME,					QStringLiteral("OT_RENAME")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_SET_DESCRIPTION,		QStringLiteral("OT_SET_DESCRIPTION")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_HEADERS,				QStringLiteral("OT_HEADERS")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_INFO,					QStringLiteral("OT_INFO")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_METAINFO,				QStringLiteral("OT_METAINFO")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_DATAMETAINFO,			QStringLiteral("OT_DATAMETAINFO")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_ELEMENTS_COUNT,			QStringLiteral("OT_ELEMENTS_COUNT")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_ELEMENT_IDS,			QStringLiteral("OT_ELEMENT_IDS")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_ELEMENT_HISTORY,		QStringLiteral("OT_ELEMENT_HISTORY")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_IMPORT,					QStringLiteral("OT_IMPORT")),
		std::make_pair(imtsdl::CSdlDocumentType::OT_EXPORT,					QStringLiteral("OT_EXPORT"))
	});

	QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> operations = sdlDocumentType.GetOperationsList();

	for (auto operationsIter = operations.cbegin(); operationsIter != operations.cend(); ++operationsIter){
		const QString requestClassName = operationsIter->GetName() + QStringLiteral("GqlRequest");

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (commandId == C");
		stream << requestClassName;
		stream << QStringLiteral("::GetCommandId()){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("operationType =  ");
		stream << operationsALiasList[operationsIter.key()];
		stream << ';';
		FeedStream(stream, 2, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return true;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 2, false);
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddCollectionMethodsImplForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType)
{
	const QString className = 'C' + sdlDocumentType.GetName() + QStringLiteral("CollectionControllerCompBase");

	QMultiMap<imtsdl::CSdlDocumentType::OperationType, ImplGenerationInfo> requestInfoMultiMap;
	QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> requestMap(sdlDocumentType.GetOperationsList());
	const QString typeClassName = sdlDocumentType.GetReferenceType().GetName();
	for (auto mapIter = requestMap.cbegin(); mapIter != requestMap.cend(); ++mapIter){
		requestInfoMultiMap.insert(mapIter.key(), ImplGenerationInfo(mapIter.value(), typeClassName));
	}

	for (const imtsdl::CSdlDocumentType& documentType: sdlDocumentType.GetSubtypes()){
		QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> subtypeRequestMap(documentType.GetOperationsList());
		const QString subtypeClassName = documentType.GetReferenceType().GetName();
		for (auto mapIter = subtypeRequestMap.cbegin(); mapIter != subtypeRequestMap.cend(); ++mapIter){
			requestInfoMultiMap.insert(mapIter.key(), ImplGenerationInfo(mapIter.value(), subtypeClassName));
		}
	}
	QList<imtsdl::CSdlDocumentType::OperationType> remainingOperations({
				imtsdl::CSdlDocumentType::OT_GET,
				imtsdl::CSdlDocumentType::OT_INSERT,
				imtsdl::CSdlDocumentType::OT_UPDATE,
				imtsdl::CSdlDocumentType::OT_LIST});

	for (imtsdl::CSdlDocumentType::OperationType operationType: requestInfoMultiMap.uniqueKeys()){
		const QList<ImplGenerationInfo> requestList = requestInfoMultiMap.values(operationType);
		AddImplCodeForRequests(stream, operationType, requestList, className);
		remainingOperations.removeAll(operationType);
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddImplCodeForRequests(
			QTextStream& stream,
			imtsdl::CSdlDocumentType::OperationType operationType,
			const QList<ImplGenerationInfo>& requestList,
			const QString& className,
			uint hIndents)
{
	// declare method

	// a) type
	switch (operationType){
	case imtsdl::CSdlDocumentType::OT_GET:
	case imtsdl::CSdlDocumentType::OT_LIST:
	case imtsdl::CSdlDocumentType::OT_UPDATE:
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("bool ");
		break;
	case imtsdl::CSdlDocumentType::OT_INSERT:
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("istd::IChangeable* ");
		break;
	case imtsdl::CSdlDocumentType::OT_DELETE:
	case imtsdl::CSdlDocumentType::OT_ELEMENT_IDS:
	case imtsdl::CSdlDocumentType::OT_ELEMENTS_COUNT:
		// nothing todo (only basic implementation)
	case imtsdl::CSdlDocumentType::OT_UPDATE_COLLECTION:
	case imtsdl::CSdlDocumentType::OT_RENAME:
	case imtsdl::CSdlDocumentType::OT_SET_DESCRIPTION:
	case imtsdl::CSdlDocumentType::OT_HEADERS:
	case imtsdl::CSdlDocumentType::OT_INFO:
	case imtsdl::CSdlDocumentType::OT_METAINFO:
	case imtsdl::CSdlDocumentType::OT_DATAMETAINFO:
	case imtsdl::CSdlDocumentType::OT_ELEMENT_HISTORY:
	case imtsdl::CSdlDocumentType::OT_IMPORT:
	case imtsdl::CSdlDocumentType::OT_EXPORT:
		// special non-trivial methods
		return;
		break;
	default:
		SendCriticalMessage(0, QString("Unexpected type: %1").arg(QString::number(operationType)));
		I_CRITICAL();
		break;
	}

	stream << className << ':' << ':';

	// b) name and argset
	switch (operationType){
	case imtsdl::CSdlDocumentType::OT_LIST:
		stream << QStringLiteral("SetupGqlItem(const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, int itemIndex,const ::imtbase::IObjectCollectionIterator* objectCollectionIterator, QString& errorMessage) const");
		break;
	case imtsdl::CSdlDocumentType::OT_GET:
		stream << QStringLiteral("CreateRepresentationFromObject(const istd::IChangeable& data, const QByteArray& objectTypeId, const ::imtgql::CGqlRequest& gqlRequest, ::imtbase::CTreeItemModel& dataModel, QString& errorMessage) const");
		break;
	case imtsdl::CSdlDocumentType::OT_INSERT:
		stream << QStringLiteral("CreateObjectFromRequest(const ::imtgql::CGqlRequest& gqlRequest, QByteArray& newObjectId, QString& errorMessage) const");
		break;
	case imtsdl::CSdlDocumentType::OT_UPDATE:
		stream << QStringLiteral("UpdateObjectFromRequest(const imtgql::CGqlRequest& gqlRequest, istd::IChangeable& object, QString& errorMessage) const");
		break;
	default:
		SendCriticalMessage(0, QString("Unexpected type: %1").arg(QString::number(operationType)));
		I_CRITICAL();
		break;
	}

	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '{';
	FeedStream(stream, 1, false);



	// method's body
	// for list we must ensure, the iterator is not null
	if (operationType == imtsdl::CSdlDocumentType::OT_LIST){
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("if (objectCollectionIterator == nullptr){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("}");
		FeedStream(stream, 2, false);
	}

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("const QByteArray commandId = gqlRequest.GetCommandId();");
	FeedStream(stream, 2, false);

	// create sections for expected command IDs
	for (const ImplGenerationInfo& sdlRequest: requestList){
		AddImplCodeForRequest(stream, sdlRequest, operationType, hIndents + 1);
	}

	switch (operationType){
	case imtsdl::CSdlDocumentType::OT_LIST:
	case imtsdl::CSdlDocumentType::OT_GET:
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return true;");
		break;
	case imtsdl::CSdlDocumentType::OT_INSERT:
		// create default section
		// add error message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected command-ID: '%1'\").arg(qPrintable(commandId));");
		FeedStream(stream, 1, false);

		// send log message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
		FeedStream(stream, 2, false);
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return nullptr;");
		break;
	case imtsdl::CSdlDocumentType::OT_UPDATE:
		// unexpected section
		// add error message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected command-ID: '%1'\").arg(qPrintable(commandId));");
		FeedStream(stream, 1, false);

		// send log message
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
		FeedStream(stream, 2, false);

		// return after error report
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return false;");
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



void CGqlCollectionControllerBaseClassGeneratorComp::AddSpecialMethodImplCodeForDocument(QTextStream& stream, const imtsdl::CSdlDocumentType& sdlDocumentType)
{
	const QString className = 'C' + sdlDocumentType.GetName() + QStringLiteral("CollectionControllerCompBase");

	QMultiMap<imtsdl::CSdlDocumentType::OperationType, ImplGenerationInfo> requestInfoMultiMap;
	QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> requestMap(sdlDocumentType.GetOperationsList());
	const QString typeClassName = sdlDocumentType.GetReferenceType().GetName();
	for (auto mapIter = requestMap.cbegin(); mapIter != requestMap.cend(); ++mapIter){
		if (s_nonTrivialOperationMethodsMap.contains(mapIter.key())){
			requestInfoMultiMap.insert(mapIter.key(), ImplGenerationInfo(mapIter.value(), typeClassName));
		}
	}

	for (const imtsdl::CSdlDocumentType& documentType: sdlDocumentType.GetSubtypes()){
		QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> subtypeRequestMap(documentType.GetOperationsList());
		const QString subtypeClassName = documentType.GetReferenceType().GetName();
		for (auto mapIter = subtypeRequestMap.cbegin(); mapIter != subtypeRequestMap.cend(); ++mapIter){
			if (s_nonTrivialOperationMethodsMap.contains(mapIter.key())){
				requestInfoMultiMap.insert(mapIter.key(), ImplGenerationInfo(mapIter.value(), subtypeClassName));
			}
		}
	}

	QList<imtsdl::CSdlDocumentType::OperationType> remainingOperations(requestInfoMultiMap.uniqueKeys());
	QMutableListIterator remainingOperationsIter(remainingOperations);
	while(remainingOperationsIter.hasNext()){
		imtsdl::CSdlDocumentType::OperationType operationType = remainingOperationsIter.next();
		const QList<ImplGenerationInfo> requestList = requestInfoMultiMap.values(operationType);
		AddSpecialMethodImplCode(stream, operationType, requestList, className);
		remainingOperationsIter.remove();
	}
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddSpecialMethodImplCode(QTextStream& stream, imtsdl::CSdlDocumentType::OperationType operationType, const QList<ImplGenerationInfo>& requestList, const QString& className, uint hIndents)
{
	// declare method
	AddBaseMethodDeclarationForOperationType(stream, operationType);
	FeedStream(stream, 1, false);

	stream << '{';
	FeedStream(stream, 1, false);

	for (const ImplGenerationInfo& requestInfo: requestList){
		/// create a carrier model GQL spec	\link https://spec.graphql.org/draft/#sec-Response-Format
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("istd::TDelPtr<::imtbase::CTreeItemModel> modelPtr(new ::imtbase::CTreeItemModel);");
		FeedStream(stream, 1, false);

		/// \todo add errors model and don't forget to fill it	\link https://spec.graphql.org/draft/#sec-Errors

		// create dataModel - child of a carrier model
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("::imtbase::CTreeItemModel* dataModelPtr = modelPtr->AddTreeModel(\"data\");");
		FeedStream(stream, 2, false);

		AddImplCodeForSpecialRequest(stream, requestInfo.request, operationType, hIndents);
	}

	// create default section
	// add error message
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("errorMessage = QString(\"Bad request. Unexpected command-ID: '%1'\").arg(qPrintable(commandId));");
	FeedStream(stream, 1, false);

	// send log message
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("SendErrorMessage(0, errorMessage);");
	FeedStream(stream, 2, false);

	// return
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("return nullptr;");
	FeedStream(stream, 1, false);

	stream << '}';
	FeedStream(stream, 3, false);
}


void CGqlCollectionControllerBaseClassGeneratorComp::AddImplCodeForRequest(
		QTextStream& stream,
		const ImplGenerationInfo& sdlRequestInfo,
		imtsdl::CSdlDocumentType::OperationType operationType,
		uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << '/' << '/' << sdlRequestInfo.request.GetName();
	FeedStream(stream, 1, false);

	const QString requestClassName = sdlRequestInfo.request.GetName() + QStringLiteral("GqlRequest");

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
	stream << QStringLiteral("(gqlRequest, ");
	if (operationType == imtsdl::CSdlDocumentType::OT_UPDATE){
		stream << QStringLiteral("true");
	}
	else {
		stream << QStringLiteral("false");
	}
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);

	// GET
	if (	operationType == imtsdl::CSdlDocumentType::OT_GET ||
			operationType == imtsdl::CSdlDocumentType::OT_LIST)
	{
		FeedStreamHorizontally(stream, hIndents + 1);
		if (operationType == imtsdl::CSdlDocumentType::OT_GET){
			stream << 'C' << sdlRequestInfo.request.GetOutputArgument().GetType();
			stream << QStringLiteral(" representationObject;");
		}
		else {
			stream << 'C' << sdlRequestInfo.containerClassName;
			stream << QStringLiteral(" representationObject;");
		}

		FeedStream(stream, 1, false);

		// [1] create check variable
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("const bool isRepresentationCreated = CreateRepresentationFromObject(");
		if (operationType == imtsdl::CSdlDocumentType::OT_GET){
			stream << QStringLiteral("data, ");
		}
		else {
			stream << QStringLiteral("*objectCollectionIterator, ");
		}
		stream << GetDecapitalizedValue(sdlRequestInfo.request.GetName());
		stream << QStringLiteral("GqlRequest, representationObject, errorMessage);");
		FeedStream(stream, 1, false);

		// [1->2] checks validate
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("if (!isRepresentationCreated){");
		FeedStream(stream, 1, false);

		// [2] return
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		// [2->1] end of checks validate
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << '}';
		FeedStream(stream, 2, false);

		// [1] create write check variable
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("const bool isRepresentationWritten = representationObject.WriteToModel(dataModel");
		// [-||-] add index for list
		if (operationType == imtsdl::CSdlDocumentType::OT_LIST){
			stream << QStringLiteral(", itemIndex");
		}
		stream << QStringLiteral(");");
		FeedStream(stream, 1, false);

		// [1->2] checks write validate
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("if (!isRepresentationWritten){");
		FeedStream(stream, 1, false);

		// [2] return
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		// [2->1] end of write validate
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	// UPDATE
	else if (operationType == imtsdl::CSdlDocumentType::OT_UPDATE){
		// [1] return
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return UpdateObjectFromRepresentationRequest(gqlRequest, ");
		stream << GetDecapitalizedValue(requestClassName);
		stream << QStringLiteral(", object, errorMessage);");
		FeedStream(stream, 1, false);
	}
	// INSERT
	else if (operationType == imtsdl::CSdlDocumentType::OT_INSERT)
	{
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

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return CreateObjectFromRepresentation(");
		stream << GetDecapitalizedValue(requestClassName);
		stream << GetInputExtractionStringForTypeName(sdlRequestInfo.request, sdlRequestInfo.containerClassName);
		stream << QStringLiteral(", newObjectId, errorMessage);");
		FeedStream(stream, 1, false);
	}

	// [1] end of section
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 2, false);



}


QString CGqlCollectionControllerBaseClassGeneratorComp::GetInputExtractionStringForTypeName(const imtsdl::CSdlRequest& sdlRequest, const QString typeName) const
{
	QString retVal = QStringLiteral(".GetRequestedArguments().");

	imtsdl::SdlFieldList requestArguments = sdlRequest.GetInputArguments();

	for (const imtsdl::CSdlField& sdlField: requestArguments){
		QString callChain;
		if (FindCallChainForField(sdlField, typeName, callChain)){
			retVal.append(sdlField.GetId());
			retVal.append(callChain);

			return retVal;
		}
	}

	SendErrorMessage(0, QString("Unable to find reference name '%1' in input arguments for '%2'").arg(sdlRequest.GetName(), typeName));

	return QString();
}


bool CGqlCollectionControllerBaseClassGeneratorComp::FindCallChainForField(const imtsdl::CSdlField& aSdlField, const QString typeName, QString& callChain) const
{
	bool isCustom = false;
	ConvertType(aSdlField, &isCustom);
	if (!isCustom){
		return false;
	}

	imtsdl::CSdlType sdlType;
	bool isTypeExsists = GetSdlTypeForField(aSdlField, m_sdlTypeListCompPtr->GetSdlTypes(false), sdlType);
	Q_ASSERT(isTypeExsists);

	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
		if (sdlField.GetType() == typeName || FindCallChainForField(sdlField, typeName, callChain)){
			callChain.append(QStringLiteral(".Get"));
			callChain.append(GetCapitalizedValue(sdlField.GetId()));
			callChain.append(QStringLiteral("()"));

			return true;
		}
	}

	if (aSdlField.GetType() == typeName){
		return true;
	}

	return false;
}


} // namespace imtsdlgen

