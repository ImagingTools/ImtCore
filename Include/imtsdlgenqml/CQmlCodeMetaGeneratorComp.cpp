// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdlgenqml/CQmlCodeMetaGeneratorComp.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QTextStream>

//Acf includes
#include <QtCore/qstringliteral.h>
#include <istd/CSystem.h>
#include <iprm/IParamsManager.h>
#include <iprm/CParamsSet.h>
#include <iprm/CEnableableParam.h>
#include <iprm/CNameParam.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlDocumentType.h>
#include <imtsdlgenqml/CQmldirFilePersistenceComp.h>
#include <imtsdlgenqml/imtsdlgenqml.h>


namespace imtsdlgenqml
{


const static inline QString s_typePlaceholder = QStringLiteral("$(Type)");
const static inline QString s_subtypePlaceholder = QStringLiteral("$(Subtype)");


iproc::IProcessor::TaskState CQmlCodeMetaGeneratorComp::DoProcessing(
			const iprm::IParamsSet* /*paramsPtr*/,
			const istd::IPolymorphic* /*inputPtr*/,
			istd::IChangeable* /*outputPtr*/,
			ibase::IProgressManager* /*progressManagerPtr*/)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_qmlKeysFileSuffixAttrPtr.IsValid());
	Q_ASSERT(m_qmlEnumValuesFileSuffixAttrPtr.IsValid());
	Q_ASSERT(m_qmldirFilePersistanceCompPtr.IsValid());
	Q_ASSERT(m_sdlDocumentListCompPtr.IsValid());

	if (!m_argumentParserCompPtr->IsQmlEnabled()){
		return TS_OK;
	}

	if (m_argumentParserCompPtr->IsDependenciesMode()){
		// nothing todo

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

	imtsdl::SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes(true);

	/// \todo refactor this \c CloseFiles - remove; \c BeginQmlFile - rename -> CreateCommandIdsFile
	// first create all files with basic mathods
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		m_qmlKeysFilePtr.SetPtr(new QFile(outputDirectoryPath + '/' + GetQmlKeysWrappedName(sdlType.GetName()) + ".qml"));

		if (!BeginQmlFile(sdlType)){
			SendErrorMessage(0, QString("Unable to process files"));

			return TS_INVALID;
		}

		// Close files so that extenders can make their own changes
		if (!CloseFiles()){
			SendErrorMessage(0, QString("Unable to close files"));
			I_CRITICAL();

			return TS_INVALID;
		}
	}

	// Reopen files to complete processing
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		m_qmlKeysFilePtr.SetPtr(new QFile(outputDirectoryPath + '/' + GetQmlKeysWrappedName(sdlType.GetName()) + ".qml"));
		if (!ReOpenFiles()){
			SendErrorMessage(0, QString("Unable to reopen files"));
			I_CRITICAL();

			return TS_INVALID;
		}

		// And complete the processing
		EndQmlFile(sdlType);
	}



	// then update a qmldir file
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
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		QmldirFileInfo qmldirFileInfo;
		qmldirFileInfo.IsSingleton = true;
		qmldirFileInfo.Version = GetTypeVersion(sdlType);
		qmldirFileInfo.Type = GetQmlKeysWrappedName(sdlType.GetName());
		qmldirFileInfo.FileName = GetQmlKeysWrappedName(sdlType.GetName()) + QStringLiteral(".qml");
		qmldirFileInfoList << qmldirFileInfo;
	}

	const imtsdl::SdlDocumentTypeList documentTypeList =  m_sdlDocumentListCompPtr->GetDocumentTypes(true);
	for (const imtsdl::CSdlDocumentType& sdlDocumentType: documentTypeList){
		QmldirFileInfo qmldirFileInfo;
		qmldirFileInfo.IsSingleton = true;
		qmldirFileInfo.Version = GetTypeVersion(sdlDocumentType);
		QString fileName = GetQmlCollectionInfoWrappedName(sdlDocumentType.GetName(), QString());
		QString type = fileName;
		type.remove(".qml");
		qmldirFileInfo.Type = type;
		qmldirFileInfo.FileName = fileName;
		qmldirFileInfoList << qmldirFileInfo;

		for (const imtsdl::CSdlDocumentType& sdlDocumentSubtype: sdlDocumentType.GetSubtypes()){
			QmldirFileInfo subtypeQmldirFileInfo;
			subtypeQmldirFileInfo.IsSingleton = true;
			subtypeQmldirFileInfo.Version = GetTypeVersion(sdlDocumentType);
			QString subtypeFileName = GetQmlCollectionInfoWrappedName(sdlDocumentType.GetName(), sdlDocumentSubtype.GetName());
			subtypeQmldirFileInfo.FileName = subtypeFileName;
			QString subType = subtypeFileName;
			subType.remove(".qml");
			subtypeQmldirFileInfo.Type = subType;

			qmldirFileInfoList << subtypeQmldirFileInfo;
		}
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

	// create a schema info
	bool isGenerated = GenerateCollectionSchemaInfo();
	if (!isGenerated){
		SendInfoMessage(0, "Unable to generate collection QML info");

		return TS_INVALID;
	}

	// and finally create a QRC file
	const QString qmlModuleName = GetQmlModuleNameFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr);
	QFile qrcFile(outputDirectoryPath + "/" + qmlModuleName + ".qrc");
	if (!qrcFile.open(QIODevice::WriteOnly)){
		SendCriticalMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(qrcFile.fileName(), qrcFile.errorString()));
		I_CRITICAL();

		return TS_INVALID;
	}

	/// \todo remove this! use persistance!!!!!!!
	QXmlStreamWriter xmlWriter(&qrcFile);
	xmlWriter.setAutoFormatting(true);

	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement("RCC");
	xmlWriter.writeStartElement("qresource");
	xmlWriter.writeAttribute("prefix", "/qml");

	// add QML files for types
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		// add QML file
		xmlWriter.writeStartElement("file");
		xmlWriter.writeAttribute("alias", qmlModuleName + '/' + sdlType.GetName() + ".qml");
		xmlWriter.writeCharacters(sdlType.GetName() + ".qml");
		xmlWriter.writeEndElement();

		// add QML Keys file
		xmlWriter.writeStartElement("file");
		xmlWriter.writeAttribute("alias", qmlModuleName + '/' + GetQmlKeysWrappedName(sdlType.GetName()) + ".qml");
		xmlWriter.writeCharacters(GetQmlKeysWrappedName(sdlType.GetName()) + ".qml");
		xmlWriter.writeEndElement();
	}

	// add QML files for document types
	for (const imtsdl::CSdlDocumentType& sdlType: documentTypeList){
		xmlWriter.writeStartElement("file");
		xmlWriter.writeAttribute("alias", qmlModuleName + '/' + GetQmlCollectionInfoWrappedName(sdlType.GetName(), QString()));
		xmlWriter.writeCharacters(GetQmlCollectionInfoWrappedName(sdlType.GetName(), QString()));
		xmlWriter.writeEndElement();

		// and add files for document's subtypes
		for (const imtsdl::CSdlDocumentType& sdlSubtype: sdlType.GetSubtypes()){
			xmlWriter.writeStartElement("file");
			xmlWriter.writeAttribute("alias", qmlModuleName + '/' + GetQmlCollectionInfoWrappedName(sdlType.GetName(), sdlSubtype.GetName()));
			xmlWriter.writeCharacters(GetQmlCollectionInfoWrappedName(sdlType.GetName(), sdlSubtype.GetName()));
			xmlWriter.writeEndElement();
		}
	}

	// also add qmldir file
	xmlWriter.writeStartElement("file");
	xmlWriter.writeAttribute("alias", qmlModuleName + "/qmldir");
	xmlWriter.writeCharacters("qmldir");
	xmlWriter.writeEndElement();

	xmlWriter.writeEndElement(); // end qresource
	xmlWriter.writeEndElement(); // end RCC
	xmlWriter.writeEndDocument();

	qrcFile.flush();
	qrcFile.close();

	return TS_OK;
}


/// special private method \todo generalize it!
bool CQmlCodeMetaGeneratorComp::UpdateQmldirParams(iprm::IParamsSet& qmldirDataParams, const QList<QmldirFileInfo>& infoList)
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


// private methods

bool CQmlCodeMetaGeneratorComp::ReOpenFiles()
{
	if (!m_qmlKeysFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)){
		SendCriticalMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(m_qmlKeysFilePtr->fileName(), m_qmlKeysFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	return true;
}


bool CQmlCodeMetaGeneratorComp::CloseFiles()
{
	bool retVal = true;

	retVal = retVal && m_qmlKeysFilePtr->flush();

	m_qmlKeysFilePtr->close();

	return retVal;
}


bool CQmlCodeMetaGeneratorComp::BeginQmlFile(const imtsdl::CSdlType& sdlType)
{
	if (!m_qmlKeysFilePtr->open(QIODevice::WriteOnly)){
		SendCriticalMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(m_qmlKeysFilePtr->fileName(), m_qmlKeysFilePtr->errorString()));
		I_CRITICAL();

		return false;
	}

	// -----fill QML keys file
	QTextStream qmlKeyFileStream(m_qmlKeysFilePtr.GetPtr());

	// Begin QtObject
	qmlKeyFileStream << QStringLiteral("pragma Singleton");
	FeedStream(qmlKeyFileStream, 2, false);
	qmlKeyFileStream << QStringLiteral("import QtQuick 2.0");
	FeedStream(qmlKeyFileStream, 2, false);

	qmlKeyFileStream << QStringLiteral("QtObject {");
	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
		FeedStream(qmlKeyFileStream, 1, false);
		FeedStreamHorizontally(qmlKeyFileStream, 1);

		// use 's_' prefix to avoid ambiguity
		qmlKeyFileStream << QStringLiteral("readonly property string s_");
		qmlKeyFileStream << GetDecapitalizedValue(sdlField.GetId());
		qmlKeyFileStream << QStringLiteral(": '");
		qmlKeyFileStream << sdlField.GetId() << '\'';
	}
	FeedStream(qmlKeyFileStream, 1, false);

	return true;
}


bool CQmlCodeMetaGeneratorComp::EndQmlFile(const imtsdl::CSdlType& /*sdlType*/)
{
	QTextStream qmlKeyFileStream(m_qmlKeysFilePtr.GetPtr());
	qmlKeyFileStream << '}';
	FeedStream(qmlKeyFileStream, 2);

	return true;
}


void CQmlCodeMetaGeneratorComp::AbortCurrentProcessing()
{
	m_qmlKeysFilePtr->close();

	I_CRITICAL();

	m_qmlKeysFilePtr->remove();
}


bool CQmlCodeMetaGeneratorComp::GenerateCollectionSchemaInfo()
{
	const imtsdl::SdlDocumentTypeList documentTypeList =  m_sdlDocumentListCompPtr->GetDocumentTypes(true);
	for (const imtsdl::CSdlDocumentType& documentType: documentTypeList){
		// create a file for document type
		if (!GenerateCollectionSchemaInfoForType(documentType, documentType.GetName(), QString())){
			return false;
		}
		// ...and for it's subtypes
		const imtsdl::SdlDocumentTypeList documentSubtypeList = documentType.GetSubtypes();
		for (const imtsdl::CSdlDocumentType& documentSubtype: documentSubtypeList){
			if (!GenerateCollectionSchemaInfoForType(documentSubtype, documentType.GetName(), documentSubtype.GetName())){
				return false;
			}
		}
	}
	
	return true;
}


bool CQmlCodeMetaGeneratorComp::GenerateCollectionSchemaInfoForType(const imtsdl::CSdlDocumentType& documentType, const QString& typeName, const QString& subtypeName)
{
	QString outputDirectoryPath = imtsdl::CSdlTools::GetCompleteOutputPath(m_customSchemaParamsCompPtr, *m_argumentParserCompPtr, true, false);
	// get auto defined path if namespace is not provided
	if (m_argumentParserCompPtr->GetNamespace().isEmpty()){
		if (m_customSchemaParamsCompPtr.IsValid()){
			outputDirectoryPath = GetAutoDefinedQmlBasePath(*m_customSchemaParamsCompPtr, outputDirectoryPath);
		}
	}

	QFile collectionSchemaInfoFile(outputDirectoryPath + '/' + GetQmlCollectionInfoWrappedName(typeName, subtypeName));
	if (!collectionSchemaInfoFile.open(QIODevice::WriteOnly)){
		SendErrorMessage(0, QString("Unable to open file '%1'. Error: %2").arg(collectionSchemaInfoFile.fileName(), collectionSchemaInfoFile.errorString()));
		return false;
	}
	QTextStream stream(&collectionSchemaInfoFile);
	stream << QStringLiteral("pragma Singleton");
	FeedStream(stream, 2, false);
	
	stream << QStringLiteral("import QtQuick 2.0");
	FeedStream(stream, 2, false);

	stream << QStringLiteral("QtObject {");
	FeedStream(stream, 1, false);

	const QMultiMap<imtsdl::CSdlDocumentType::OperationType, imtsdl::CSdlRequest> operationList = documentType.GetOperationsList();
	for (auto operationIter = operationList.cbegin(); operationIter != operationList.cend(); ++ operationIter){
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("readonly property string ");

		const QString operationTypeName = imtsdl::CSdlDocumentType::ToString(operationIter.key());
		Q_ASSERT_X(!operationTypeName.isEmpty(), __func__, "Unexpected operation type name!");
		stream << operationTypeName;

		stream << QStringLiteral("CommandId: '");
		stream << operationIter.value().GetName();
		stream << '\'';
		FeedStream(stream, 1, false);
	}
	stream << '}';

	return true;
}


QString CQmlCodeMetaGeneratorComp::GetQmlKeysWrappedName(const QString& originalName) const
{
	return originalName + *m_qmlKeysFileSuffixAttrPtr;
}


QString CQmlCodeMetaGeneratorComp::GetQmlCollectionInfoWrappedName(const QString& typeName, const QString& subtypeName) const
{
	QString retVal = *m_collectionInfoFileMaskAttrPtr;
	retVal.replace(s_typePlaceholder, typeName);
	retVal.replace(s_subtypePlaceholder, subtypeName);
	retVal += QStringLiteral(".qml");
	
	return retVal;
}



} // namespace imtsdlgenqml

