// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgenqml/CQmlCodeOperationGeneratorComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QTextStream>
#include <QtCore/QStringLiteral>

// ACF includes
#include <istd/CSystem.h>
#include <iprm/IParamsManager.h>
#include <iprm/CParamsSet.h>
#include <iprm/CEnableableParam.h>
#include <iprm/CNameParam.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlRequest.h>
#include <imtsdlgenqml/CQmldirFilePersistenceComp.h>
#include <imtsdlgenqml/imtsdlgenqml.h>


namespace imtsdlgenqml
{


iproc::IProcessor::TaskState CQmlCodeOperationGeneratorComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlRequestListCompPtr.IsValid());

	if (!m_argumentParserCompPtr->IsQmlEnabled()){
		return TS_OK;
	}

	if (m_argumentParserCompPtr->IsDependenciesMode()){
		return TS_OK;
	}

	QString outputDirectoryPath = imtsdl::CSdlTools::GetCompleteOutputPath(m_customSchemaParamsCompPtr, *m_argumentParserCompPtr, true, false);
	if (outputDirectoryPath.isEmpty()){
		SendCriticalMessage(0, "Output path is not provided");
		I_CRITICAL();

		return TS_INVALID;
	}

	// get auto defined path if namespace is not provided
	if (m_argumentParserCompPtr->GetNamespace().isEmpty()){
		if (m_customSchemaParamsCompPtr.IsValid()){
			outputDirectoryPath = GetAutoDefinedQmlBasePath(*m_customSchemaParamsCompPtr, outputDirectoryPath);
		}
	}

	if (!istd::CSystem::EnsurePathExists(outputDirectoryPath)){
		SendErrorMessage(0, QString("Unable to create path '%1'").arg(outputDirectoryPath));
		I_CRITICAL();

		return TS_INVALID;
	}

	iprm::CParamsSet qmldirDataParams;
	int loadStatus = m_qmldirFilePersistanceCompPtr->LoadFromFile(qmldirDataParams, outputDirectoryPath + "/qmldir");
	if (loadStatus != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, QString("Unable to load qmldir file data from '%1'").arg(outputDirectoryPath + "/qmldir"));

		return TS_INVALID;
	}

	iprm::IParamsManager* objectsParamsManagerPtr = dynamic_cast<iprm::IParamsManager*>(qmldirDataParams.GetEditableParameter(QmldirModelParamIds::Objects));
	if (objectsParamsManagerPtr == nullptr){
		SendCriticalMessage(0, "Invalid params created");
		I_CRITICAL();

		return TS_INVALID;
	}

	QList<QmldirFileInfo> qmldirFileInfoList;

	imtsdl::SdlRequestList sdlRequestList = m_sdlRequestListCompPtr->GetRequests(true);
	for (const imtsdl::CSdlRequest& sdlRequest : sdlRequestList){
		imtsdl::CSdlRequest::Type requestType = sdlRequest.GetType();

		// Subscriptions are not supported in this MVP
		if (requestType == imtsdl::CSdlRequest::T_SUBSCRIPTION){
			continue;
		}

		const QString requestName = sdlRequest.GetName();
		const QString qmlClassName = requestName + QStringLiteral("Operation");
		const QString qmlClassFileName = qmlClassName + QStringLiteral(".qml");
		const int requestTypeInt = (requestType == imtsdl::CSdlRequest::T_MUTATION) ? 1 : 0;

		QFile file;
		file.setFileName(outputDirectoryPath + '/' + qmlClassFileName);

		if (!file.open(QIODevice::WriteOnly)){
			SendCriticalMessage(0, QString("Unable to open file: '%1'. Error: %2").arg(file.fileName(), file.errorString()));
			I_CRITICAL();
			return TS_INVALID;
		}

		QmldirFileInfo qmldirFileInfo;
		qmldirFileInfo.Version = GetTypeVersion(sdlRequest);
		qmldirFileInfo.Type = qmlClassName;
		qmldirFileInfo.FileName = qmlClassFileName;
		qmldirFileInfoList << qmldirFileInfo;

		// Resolve input type
		imtsdl::CSdlField sdlInputField;
		imtsdl::SdlFieldList inputFieldList;
		imtsdl::SdlFieldList inputArguments = sdlRequest.GetInputArguments();
		if (!inputArguments.isEmpty()){
			sdlInputField = inputArguments[0];
			imtsdl::CSdlType inputType;
			[[maybe_unused]] const bool isTypeFound = GetSdlTypeForField(
						sdlInputField,
						m_sdlTypeListCompPtr->GetSdlTypes(false),
						inputType);
			inputFieldList = inputType.GetFields();
		}

		// Resolve output type
		imtsdl::CSdlField outputSdlField = sdlRequest.GetOutputArgument();
		const bool hasOutputType = bool(FindEntryByName(
					outputSdlField.GetType(),
					m_sdlTypeListCompPtr->GetSdlTypes(false)));

		// Collect required imports
		QSet<QString> requiredImports;

		// Import for the request schema (same module usually provides both types)
		const QString requestQmlImport = sdlRequest.GetQmlImportDeclaration();
		if (!requestQmlImport.isEmpty()){
			requiredImports << requestQmlImport;
		}
		const QString inputQmlImport = GetQmlImportDeclarationByField(sdlInputField);
		if (!inputQmlImport.isEmpty()){
			requiredImports << inputQmlImport;
		}
		const QString outputQmlImport = GetQmlImportDeclarationByField(outputSdlField);
		if (!outputQmlImport.isEmpty()){
			requiredImports << outputQmlImport;
		}

		// Write QML file
		QTextStream stream(&file);

		stream << QStringLiteral("import QtQuick 2.0");
		FeedStream(stream, 1, false);
		stream << QStringLiteral("import imtguigql 1.0");
		FeedStream(stream, 1, false);

		for (const QString& importDecl : std::as_const(requiredImports)){
			stream << QStringLiteral("import ") << importDecl;
			FeedStream(stream, 1, false);
		}

		FeedStream(stream, 1, false);
		stream << QStringLiteral("SdlOperation {");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("id: root");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("gqlCommandId: \"") << requestName << QStringLiteral("\"");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("requestType: ") << QString::number(requestTypeInt);
		FeedStream(stream, 1, false);

		// Input properties
		if (!inputFieldList.isEmpty()){
			FeedStream(stream, 1, false);
			for (const imtsdl::CSdlField& field : inputFieldList){
				const InputPropertyInfo propInfo = ResolveInputPropertyInfo(field);
				FeedStreamHorizontally(stream, 1);
				stream << QStringLiteral("property ") << propInfo.qmlType
					   << QStringLiteral(" m_") << GetDecapitalizedValue(field.GetId())
					   << QStringLiteral(": ") << propInfo.defaultVal;
				FeedStream(stream, 1, false);
			}
		}

		// Typed result instance
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("result: ");
		if (hasOutputType){
			stream << outputSdlField.GetType() << QStringLiteral(" {}");
		}
		else{
			stream << QStringLiteral("null");
		}
		FeedStream(stream, 1, false);

		// Input object instance (if there are input fields)
		if (!inputArguments.isEmpty() && !inputFieldList.isEmpty()){
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream, 1);
			stream << sdlInputField.GetType() << QStringLiteral(" {");
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("id: _inputObj");
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream, 1);
			stream << QStringLiteral("}");
			FeedStream(stream, 1, false);
		}

		// execute() override
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("function execute() {");
		FeedStream(stream, 1, false);

		if (!inputArguments.isEmpty() && !inputFieldList.isEmpty()){
			for (const imtsdl::CSdlField& field : inputFieldList){
				FeedStreamHorizontally(stream, 2);
				stream << QStringLiteral("_inputObj.m_") << GetDecapitalizedValue(field.GetId())
					   << QStringLiteral(" = m_") << GetDecapitalizedValue(field.GetId());
				FeedStream(stream, 1, false);
			}
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("_sendRequest(_inputObj)");
		}
		else{
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("_sendRequest(null)");
		}

		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);

		stream << QStringLiteral("}");
		FeedStream(stream, 1, false);

		file.close();
	}

	if (!UpdateQmldirParams(qmldirDataParams, qmldirFileInfoList)){
		SendCriticalMessage(0, "Unable to update qmldir file params");

		return TS_INVALID;
	}

	int saveStatus = m_qmldirFilePersistanceCompPtr->SaveToFile(qmldirDataParams, outputDirectoryPath + "/qmldir");
	if (saveStatus != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, QString("Unable to save qmldir file data from '%1'").arg(outputDirectoryPath + "/qmldir"));

		return TS_INVALID;
	}

	return TS_OK;
}


bool CQmlCodeOperationGeneratorComp::UpdateQmldirParams(iprm::IParamsSet& qmldirDataParams, const QList<QmldirFileInfo>& infoList)
{
	iprm::IParamsManager* objectsParamsManagerPtr = dynamic_cast<iprm::IParamsManager*>(qmldirDataParams.GetEditableParameter(QmldirModelParamIds::Objects));
	if (objectsParamsManagerPtr == nullptr){
		qCritical() << "Unexpected params provided";
		I_CRITICAL();

		return false;
	}

	for (const QmldirFileInfo& fileInfo: infoList){
		iprm::CParamsSet qmlObjectParams;

		iprm::CEnableableParam isSingletonParam(fileInfo.IsSingleton);
		qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectIsSingleton, &isSingletonParam);

		iprm::CNameParam objectNameParam;
		objectNameParam.SetName(fileInfo.Type);
		qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectTypeName, &objectNameParam);

		iprm::CNameParam objectVersionNameParam;
		objectVersionNameParam.SetName(fileInfo.Version);
		qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectVersionName, &objectVersionNameParam);

		iprm::CNameParam objectFileNameParam;
		objectFileNameParam.SetName(fileInfo.FileName);
		qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectFileName, &objectFileNameParam);

		int indexOfInsertedSet = objectsParamsManagerPtr->InsertParamsSet();
		iprm::IParamsSet* objectEntryParamsSetPtr = objectsParamsManagerPtr->GetParamsSet(indexOfInsertedSet);
		if (objectEntryParamsSetPtr == nullptr){
			qCritical() << "Unable to create params set for entry";
			I_CRITICAL();

			return false;
		}

		const bool isCopied = objectEntryParamsSetPtr->CopyFrom(qmlObjectParams);
		if (!isCopied){
			qCritical() << "Unable to set params set for entry";
			I_CRITICAL();

			return false;
		}
	}

	return true;
}


QString CQmlCodeOperationGeneratorComp::GetQmlImportDeclarationByField(const imtsdl::CSdlField& sdlField) const
{
	imtsdl::CSdlType sdlType;
	[[maybe_unused]] const bool isTypeFound = GetSdlTypeForField(
				sdlField,
				m_sdlTypeListCompPtr->GetSdlTypes(false),
				sdlType);

	return sdlType.GetQmlImportDeclaration();
}


CQmlCodeOperationGeneratorComp::InputPropertyInfo
CQmlCodeOperationGeneratorComp::ResolveInputPropertyInfo(const imtsdl::CSdlField& sdlField)
{
	InputPropertyInfo info;

	if (sdlField.IsArray()){
		// All array input properties are represented as var
		info.qmlType = QStringLiteral("var");
		info.defaultVal = QStringLiteral("[]");
		return info;
	}

	bool isCustom = false;
	const QString qmlType = QmlConvertType(sdlField.GetType(), &isCustom);

	if (isCustom){
		// Complex object — expose as var, caller provides the object instance
		info.qmlType = QStringLiteral("var");
		info.defaultVal = QStringLiteral("null");
		return info;
	}

	// Scalar types: use typed QML property; required fields get their
	// natural default, optional fields fall back to null via var.
	if (sdlField.IsRequired()){
		info.qmlType = qmlType;
		if (qmlType == QStringLiteral("int") ||
			qmlType == QStringLiteral("real") ||
			qmlType == QStringLiteral("double"))
		{
			info.defaultVal = QStringLiteral("0");
		}
		else if (qmlType == QStringLiteral("bool")){
			info.defaultVal = QStringLiteral("false");
		}
		else{
			// string, and any other scalar
			info.defaultVal = QStringLiteral("\"\"");
		}
	}
	else{
		// Optional scalar — use var so null can be expressed
		info.qmlType = QStringLiteral("var");
		info.defaultVal = QStringLiteral("null");
	}

	return info;
}


} // namespace imtsdlgenqml
