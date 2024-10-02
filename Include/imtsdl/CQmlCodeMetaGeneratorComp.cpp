#include "CQmlCodeMetaGeneratorComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QXmlStreamWriter>
#include <QtCore/QTextStream>

//Acf includes
#include <istd/CSystem.h>
#include <iprm/IParamsManager.h>
#include <iprm/CParamsSet.h>
#include <iprm/CEnableableParam.h>
#include <iprm/CNameParam.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>
#include <imtsdl/CQmldirFilePersistenceComp.h>


namespace imtsdl
{


int CQmlCodeMetaGeneratorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_qmlKeysFileSuffixAttrPtr.IsValid());
	Q_ASSERT(m_qmldirFilePersistanceCompPtr.IsValid());

	if (!m_argumentParserCompPtr->IsQmlEnabled()){
		return TS_OK;
	}

	if (m_argumentParserCompPtr->IsDependenciesMode()){
		// nothing todo

		return TS_OK;
	}

	QString outputDirectoryPath = QDir::cleanPath(m_argumentParserCompPtr->GetOutputDirectoryPath());
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

	SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes();

	// first create all files with basic mathods
	for (const CSdlType& sdlType: sdlTypeList){
		m_qmlKeysFilePtr.SetPtr(new QFile(outputDirectoryPath + '/' + GetQmlKeysWrappedName(sdlType.GetName()) + ".qml"));

		if (!BeginQmlFile(sdlType)){
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

	// Reopen files to complete processing
	for (const CSdlType& sdlType: sdlTypeList){
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

	for (const CSdlType& sdlType: sdlTypeList){
		// create paramsset to store object
		iprm::CParamsSet qmlObjectParams;

		// set singleton
		iprm::CEnableableParam isSingletonParam(true);
		qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectIsSingleton, &isSingletonParam);

		// get object's info
		// a) type
		iprm::CNameParam objectNameParam;
		objectNameParam.SetName(GetQmlKeysWrappedName(sdlType.GetName()));
		qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectTypeName, &objectNameParam);
		// b) version
		iprm::CNameParam objectVerionNameParam;
		objectVerionNameParam.SetName(QStringLiteral(" 1.0 "));
		qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectVersionName, &objectVerionNameParam);
		// c) file
		iprm::CNameParam objectFileNameParam;
		objectFileNameParam.SetName(GetQmlKeysWrappedName(sdlType.GetName()) + QStringLiteral(".qml"));
		qmlObjectParams.SetEditableParameter(QmldirModelParamIds::ObjectFileName, &objectFileNameParam);

		int indexOfInsertedSet = objectsParamsManagerPtr->InsertParamsSet();
		iprm::IParamsSet* objectEntryParamsSetPtr = objectsParamsManagerPtr->GetParamsSet(indexOfInsertedSet);
		if (objectEntryParamsSetPtr == nullptr){
			SendCriticalMessage(0, "Unable to create params set for entry", __func__);
			I_CRITICAL();

			return TS_INVALID;
		}
		const bool isCopied = objectEntryParamsSetPtr->CopyFrom(qmlObjectParams);
		if (!isCopied){
			SendCriticalMessage(0, "Unable to set params set for entry", __func__);
			I_CRITICAL();

			return TS_INVALID;
		}
	}

	int saveStatus = m_qmldirFilePersistanceCompPtr->SaveToFile(qmldirDataParams, outputDirectoryPath + "/qmldir");
	if (saveStatus != ifile::IFilePersistence::OS_OK){
		SendErrorMessage(0, QString("Unable to save qmldir file data from '%1'").arg(outputDirectoryPath + "/qmldir"));

		return TS_INVALID;
	}

	// and finally create a QRC file
	const QString qmlModuleName = GetQmlModuleNameFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr);
	QFile qrcFile(outputDirectoryPath + "/" + qmlModuleName + ".qrc");
	if (!qrcFile.open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(qrcFile.fileName(), qrcFile.errorString()));
		I_CRITICAL();

		return TS_INVALID;
	}

	QXmlStreamWriter xmlWriter(&qrcFile);
	xmlWriter.setAutoFormatting(1);

	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement("RCC");
	xmlWriter.writeStartElement("qresource");
	xmlWriter.writeAttribute("prefix", "/qml");

	// add QML files
	for (const CSdlType& sdlType: sdlTypeList){
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


// private methods

bool CQmlCodeMetaGeneratorComp::ReOpenFiles()
{
	if (!m_qmlKeysFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)){
		SendErrorMessage(0,
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


bool CQmlCodeMetaGeneratorComp::BeginQmlFile(const CSdlType& sdlType)
{
	if (!m_qmlKeysFilePtr->open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(m_qmlKeysFilePtr->fileName(), m_qmlKeysFilePtr->errorString()));

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
	for (const CSdlField& sdlField: sdlType.GetFields()){
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


bool CQmlCodeMetaGeneratorComp::EndQmlFile(const CSdlType& /*sdlType*/)
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


QString CQmlCodeMetaGeneratorComp::GetQmlKeysWrappedName(const QString& originalName) const
{
	return originalName + *m_qmlKeysFileSuffixAttrPtr;
}



} // namespace imtsdl

