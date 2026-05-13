// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgenqml/CQmlCodeRequestGeneratorComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QXmlStreamWriter>
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


iproc::IProcessor::TaskState CQmlCodeRequestGeneratorComp::DoProcessing(
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

	// create paramsset to store object
	iprm::CParamsSet qmlObjectParams;
	QList<QmldirFileInfo> qmldirFileInfoList;

	imtsdl::SdlRequestList sdlRequestList = m_sdlRequestListCompPtr->GetRequests(true);
	for (const imtsdl::CSdlRequest& sdlRequest : sdlRequestList){
		QFile file;

		imtsdl::CSdlRequest::Type requestType = sdlRequest.GetType();
		QString functionName;
		QString parentQmlName;
		QString qmlClassName = sdlRequest.GetName();
		switch (requestType){
		case imtsdl::CSdlRequest::T_SUBSCRIPTION:
		if (qmlClassName.startsWith("On")) {
			qmlClassName.remove(0, 2);
		}
		if (qmlClassName.endsWith("ed")) {
			qmlClassName.chop(2);
		}
		case imtsdl::CSdlRequest::T_QUERY:
			qmlClassName += QStringLiteral("ModelProvider");
			functionName = QStringLiteral("requestDataModel");
			parentQmlName = QStringLiteral("GqlBasedDataModelProvider");
			break; 
		case imtsdl::CSdlRequest::T_MUTATION:
			qmlClassName += QStringLiteral("ModelController");
			functionName = QStringLiteral("saveDataModel");
			parentQmlName = QStringLiteral("GqlBasedDataModelController");
			break;
		default:
			break;
		}

		QString qmlClassFileName = qmlClassName + QStringLiteral(".qml");

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

		QTextStream stream(&file);
		stream << QStringLiteral("import QtQuick 2.0");
		FeedStream(stream, 1, false);
		stream << QStringLiteral("import imtguigql 1.0");
		FeedStream(stream, 1, false);

		QString qmlImportDeclaration = sdlRequest.GetQmlImportDeclaration();
		if (!qmlImportDeclaration.isEmpty()){
			stream << QStringLiteral("import ") << qmlImportDeclaration;
			FeedStream(stream, 1, false);
		}

		imtsdl::CSdlField sdlInputField;
		imtsdl::SdlFieldList inputFieldList;
		imtsdl::SdlFieldList inputArguments = sdlRequest.GetInputArguments();
		if (!inputArguments.isEmpty()){
			sdlInputField = inputArguments[0];
			imtsdl::CSdlType inputType;
			[[maybe_unused]]const bool isTypeFound = GetSdlTypeForField(
						sdlInputField,
						m_sdlTypeListCompPtr->GetSdlTypes(false),
						inputType);
			inputFieldList = inputType.GetFields();
		}

		QString inputQmlImportDeclaration = GetQmlImportDeclarationByField(sdlInputField);
		if (!inputQmlImportDeclaration.isEmpty()){
			stream << QStringLiteral("import ") << inputQmlImportDeclaration;
			FeedStream(stream, 1, false);
		}

		imtsdl::CSdlField outputSdlField = sdlRequest.GetOutputArgument();
		QString outputQmlImportDeclaration = GetQmlImportDeclarationByField(outputSdlField);
		if (!outputQmlImportDeclaration.isEmpty()){
			stream << QStringLiteral("import ") << outputQmlImportDeclaration;
			FeedStream(stream, 1, false);
		}

		FeedStream(stream, 1, false);
		stream << parentQmlName << QStringLiteral(" {");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("gqlCommandId: \"") << sdlRequest.GetName() << QStringLiteral("\"");
		FeedStream(stream, 1, false);

		if (!inputArguments.isEmpty()){
			FeedStreamHorizontally(stream, 1);
			stream << QStringLiteral("inputModel: ") << sdlInputField.GetType() << QStringLiteral("{}");
			FeedStream(stream, 1, false);
		}

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("responseModel: ");
		std::shared_ptr<imtsdl::CSdlEntryBase> sdlEntryBasePtr = FindEntryByName(outputSdlField.GetType(), m_sdlTypeListCompPtr->GetSdlTypes(false), imtsdl::SdlEnumList(), imtsdl::SdlUnionList());
		if (sdlEntryBasePtr){
			stream << outputSdlField.GetType() << QStringLiteral("{}");
		}
		else{
			stream << QStringLiteral("null");
		}
		FeedStream(stream, 2, false);

		FeedStreamHorizontally(stream, 1);
		stream << QStringLiteral("function ") << functionName << QStringLiteral("(") ;
		for (int i = 0; i < inputFieldList.size(); i++){
			if (i > 0){
				stream << QStringLiteral(", ");
			}

			stream << inputFieldList[i].GetId();
		}
		stream << QStringLiteral("){");

		FeedStream(stream, 1, false);

		for (int i = 0; i < inputFieldList.size(); i++){
			QString fileldId = inputFieldList[i].GetId();
			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("inputModel.m_") << fileldId << QStringLiteral(" = ") << fileldId;
			FeedStream(stream, 1, false);
		}

		FeedStreamHorizontally(stream, 2);
		stream << functionName << QStringLiteral("Base()");
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


bool CQmlCodeRequestGeneratorComp::UpdateQmldirParams(iprm::IParamsSet& qmldirDataParams, const QList<QmldirFileInfo>& infoList)
{
	iprm::IParamsManager* objectsParamsManagerPtr = dynamic_cast<iprm::IParamsManager*>(qmldirDataParams.GetEditableParameter(QmldirModelParamIds::Objects));
	if (objectsParamsManagerPtr == nullptr){
		qCritical() << "Unexpected params provided created";
		I_CRITICAL();

		return false;
	}

	for (const QmldirFileInfo& fileInfo: infoList){
		// create paramsset to store object
		iprm::CParamsSet qmlObjectParams;

		// set singleton
		iprm::CEnableableParam isSingletonParam(fileInfo.IsSingleton);
		qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectIsSingleton, &isSingletonParam);

		// get object's info
		// a) type
		iprm::CNameParam objectNameParam;
		objectNameParam.SetName(fileInfo.Type);
		qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectTypeName, &objectNameParam);
		// b) version
		iprm::CNameParam objectVerionNameParam;
		objectVerionNameParam.SetName(fileInfo.Version);
		qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectVersionName, &objectVerionNameParam);
		// c) file
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


QString CQmlCodeRequestGeneratorComp::GetQmlImportDeclarationByField(const imtsdl::CSdlField& sdlField) const
{
	imtsdl::CSdlType sdlType;
	[[maybe_unused]]const bool isTypeFound = GetSdlTypeForField(
				sdlField,
				m_sdlTypeListCompPtr->GetSdlTypes(false),
				sdlType);

	return sdlType.GetQmlImportDeclaration();
}


} // namespace imtsdlgenqml


