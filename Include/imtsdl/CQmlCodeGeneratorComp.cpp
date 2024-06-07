#include "CQmlCodeGeneratorComp.h"


// C includes
#include <iostream>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QXmlStreamWriter>

//Acf includes
#include <istd/CSystem.h>

// imtsdl includes
#include <imtsdl/CSdlType.h>


namespace imtsdl
{


int CQmlCodeGeneratorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());

	int retVal = iproc::IProcessor::TS_OK;

	if (!m_argumentParserCompPtr->IsQmlEnabled()){
		return retVal;
	}

	const QString outputDirectoryPath = QDir::cleanPath(m_argumentParserCompPtr->GetOutputDirectoryPath());
	if (outputDirectoryPath.isEmpty()){
		SendCriticalMessage(0, "Output path is not provided");
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	if (!istd::CSystem::EnsurePathExists(outputDirectoryPath)){
		SendErrorMessage(0, QString("Unable to create path '%1'").arg(outputDirectoryPath));
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	if (m_argumentParserCompPtr->IsDependenciesMode()){
		SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes();
		for (const CSdlType& sdlType: sdlTypeList){
			std::cout << QString(outputDirectoryPath + '/' + sdlType.GetName() + ".qml").toStdString() << std::endl;
		}
		// qmldir
		std::cout << QString(outputDirectoryPath + "qmldir").toStdString() << std::endl;
		// generated QRC file
		std::cout << QString(outputDirectoryPath + "/" + m_argumentParserCompPtr->GetNamespace() + ".qrc").toStdString() << std::endl;

		return iproc::IProcessor::TS_OK;
	}

	SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes();

	// first create all files with basic mathods
	for (const CSdlType& sdlType: sdlTypeList){
		m_qmlFilePtr.SetPtr(new QFile(outputDirectoryPath + '/' + sdlType.GetName() + ".qml"));

		if (!BeginQmlFile(sdlType)){
			SendErrorMessage(0, QString("Unable to begin files"));
			I_CRITICAL();

			return iproc::IProcessor::TS_INVALID;
		}

		// Close files so that extenders can make their own changes
		if (!CloseFiles()){
			SendErrorMessage(0, QString("Unable to close files"));
			I_CRITICAL();

			return iproc::IProcessor::TS_INVALID;
		}
	}

	// Then let extenders to make changes
	const int extendersCount = m_codeGeneratorExtenderListCompPtr.GetCount();
	for (int i = 0; i < extendersCount; ++i){
		iproc::IProcessor* extenderPtr = m_codeGeneratorExtenderListCompPtr[i];
		Q_ASSERT(extenderPtr != nullptr);

		const int extenderResult = extenderPtr->DoProcessing(paramsPtr, inputPtr, outputPtr, progressManagerPtr);
		if (extenderResult != iproc::IProcessor::TS_OK){
			return extenderResult;
		}
		retVal = qMax(retVal, extenderResult);
	}

	// Reopen files to complete processing
	for (const CSdlType& sdlType: sdlTypeList){
		m_qmlFilePtr.SetPtr(new QFile(outputDirectoryPath + '/' + sdlType.GetName() + ".qml"));
		if (!ReOpenFiles()){
			SendErrorMessage(0, QString("Unable to reopen files"));
			I_CRITICAL();

			return iproc::IProcessor::TS_INVALID;
		}

		// And complete the processing
		EndQmlFile(sdlType);
	}

	// then create a qmldir file
	QFile qmldirFile(outputDirectoryPath + "/qmldir");
	if (!qmldirFile.open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(qmldirFile.fileName(), qmldirFile.errorString()));
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	const QString currentNamespace = m_argumentParserCompPtr->GetNamespace();
	QTextStream qmldirStream(&qmldirFile);
	qmldirStream << QStringLiteral("module ");
	qmldirStream << currentNamespace;
	FeedStream(qmldirStream, 2, false);
	for (const CSdlType& sdlType: sdlTypeList){
		qmldirStream << sdlType.GetName();
		qmldirStream << QStringLiteral(" 1.0 ");
		qmldirStream << sdlType.GetName() << ".qml";
		FeedStream(qmldirStream, 1, false);
	}

	// and finally create a QRC file
	QFile qrcFile(outputDirectoryPath + "/" + currentNamespace + ".qrc");
	if (!qrcFile.open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(qrcFile.fileName(), qrcFile.errorString()));
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	QXmlStreamWriter xmlWriter(&qrcFile);
	xmlWriter.setAutoFormatting(1);

	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement("RCC");
	xmlWriter.writeStartElement("qresource");
	xmlWriter.writeAttribute("prefix", "/qml");

	for (const CSdlType& sdlType: sdlTypeList){
		xmlWriter.writeStartElement("file");
		xmlWriter.writeAttribute("alias", currentNamespace + '/' + sdlType.GetName() + ".qml");
		xmlWriter.writeCharacters(sdlType.GetName() + ".qml");
		xmlWriter.writeEndElement();
	}
	// also add qmldir file
	xmlWriter.writeStartElement("file");
	xmlWriter.writeAttribute("alias", currentNamespace + "/qmldir");
	xmlWriter.writeCharacters("qmldir");
	xmlWriter.writeEndElement();

	xmlWriter.writeEndElement(); // end qresource
	xmlWriter.writeEndElement(); // end RCC
	xmlWriter.writeEndDocument();

	qrcFile.flush();
	qrcFile.close();

	return retVal;
}


// private methods

bool CQmlCodeGeneratorComp::ReOpenFiles()
{
	if (!m_qmlFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)){
		SendErrorMessage(0,
					QString("Unable to open file: '%1'. Error: %2")
							 .arg(m_qmlFilePtr->fileName(), m_qmlFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	return true;
}


bool CQmlCodeGeneratorComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_qmlFilePtr->flush();
	Q_ASSERT(retVal);

	m_qmlFilePtr->close();

	return retVal;
}


bool CQmlCodeGeneratorComp::BeginQmlFile(const CSdlType& sdlType)
{
	if (!m_qmlFilePtr->open(QIODevice::WriteOnly)){
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(m_qmlFilePtr->fileName(), m_qmlFilePtr->errorString()));

		return false;
	}

	QTextStream ifStream(m_qmlFilePtr.GetPtr());

	// import section
	ifStream << QStringLiteral("import QtQuick");
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("import imtcontrols 1.0");
	FeedStream(ifStream, 2, false);

	// Base QML
	ifStream << QStringLiteral("BaseClass {");

	// container's props
	for (const CSdlField& sdlField: sdlType.GetFields()){
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 1);

		ifStream << QStringLiteral("property ");
		if (sdlField.IsArray()){
			ifStream << QStringLiteral("BaseModel");
		}
		else {
			ifStream << QmlConvertType(sdlField.GetType());
		}
		// use 'm_' prefix to avoid ambiguity
		ifStream << QStringLiteral(" m_") << sdlField.GetId();
	}
	FeedStream(ifStream, 1, false);

	// base class reimplementation methods
	// getJSONKeyForProperty
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("function getJSONKeyForProperty(propertyId){");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("switch (propertyId){");
	for (const CSdlField& sdlField: sdlType.GetFields()){
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 3);
		ifStream << QStringLiteral("case 'm_") << sdlField.GetId();
		ifStream << QStringLiteral("': return '") << sdlField.GetId() << '\'';
	}
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << '}'; // end of switch
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << '}'; // end of function

	// createComponent (custom and arrays only)
	FeedStream(ifStream, 2, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("function createComponent(propertyId){");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("switch (propertyId){");
	for (const CSdlField& sdlField: sdlType.GetFields()){
		bool isCustom = false;
		const QString convertedType = QmlConvertType(sdlField.GetType(), &isCustom);
		if (!isCustom && !sdlField.IsArray()){
			// skip simple scalars
			continue;
		}

		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 3);
		ifStream << QStringLiteral("case 'm_") << sdlField.GetId();
		ifStream << QStringLiteral("': return Qt.createComponent('");
		ifStream << convertedType << QStringLiteral(".qml')");
	}
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << '}'; // end of switch
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << '}'; // end of function

	// createMe
	FeedStream(ifStream, 2, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("function createMe(){");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("return Qt.createComponent('");
	ifStream << sdlType.GetName();
	ifStream << QStringLiteral(".qml').createObject()");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << '}'; // end of function

	// static props as internal QtObject
	FeedStream(ifStream, 2, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("property QtObject _keys: QtObject{");
	// and static props container alias; use '_' prefix to avoid ambiguity (private properies by the JS convention)
	for (const CSdlField& sdlField: sdlType.GetFields()){
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 2);

		// use 's_' prefix to avoid ambiguity
		ifStream << QStringLiteral("readonly property string s_");
		ifStream << sdlField.GetId();
		ifStream << QStringLiteral(": '");
		ifStream << sdlField.GetId() << '\'';
	}
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << '}'; // end of QtObject

	FeedStream(ifStream, 1, false);
	return true;
}


bool CQmlCodeGeneratorComp::EndQmlFile(const CSdlType& sdlType)
{
	QTextStream ifStream(m_qmlFilePtr.GetPtr());
	ifStream << '}';
	FeedStream(ifStream, 2);

	return true;
}


void CQmlCodeGeneratorComp::AbortCurrentProcessing()
{
	m_qmlFilePtr->close();

	I_CRITICAL();

	m_qmlFilePtr->remove();
}


} // namespace imtsdl

