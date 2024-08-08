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
	Q_ASSERT(m_qmlKeysFilePrefixAttrPtr.IsValid());
	Q_ASSERT(m_qmlKeysFileSuffixAttrPtr.IsValid());

	int retVal = iproc::IProcessor::TS_OK;

	if (!m_argumentParserCompPtr->IsQmlEnabled()) {
		return retVal;
	}

	const QString outputDirectoryPath = QDir::cleanPath(m_argumentParserCompPtr->GetOutputDirectoryPath());
	if (outputDirectoryPath.isEmpty()) {
		SendCriticalMessage(0, "Output path is not provided");
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	if (!istd::CSystem::EnsurePathExists(outputDirectoryPath)) {
		SendErrorMessage(0, QString("Unable to create path '%1'").arg(outputDirectoryPath));
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
	}

	if (m_argumentParserCompPtr->IsDependenciesMode()) {
		SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes();
		for (const CSdlType& sdlType: sdlTypeList) {
			std::cout << QString(outputDirectoryPath + '/' + sdlType.GetName() + ".qml").toStdString() << std::endl;
			std::cout << QString(outputDirectoryPath + '/' + GetQmlKeysWrappedName(sdlType.GetName()) + ".qml").toStdString() << std::endl;
		}
		// qmldir
		std::cout << QString(outputDirectoryPath + '/' + "qmldir").toStdString() << std::endl;
		// generated QRC file
		std::cout << QString(outputDirectoryPath + "/" + m_argumentParserCompPtr->GetNamespace() + ".qrc").toStdString() << std::endl;

		return iproc::IProcessor::TS_OK;
	}

	SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes();

	// first create all files with basic mathods
	for (const CSdlType& sdlType: sdlTypeList) {
		m_qmlFilePtr.SetPtr(new QFile(outputDirectoryPath + '/' + sdlType.GetName() + ".qml"));
		m_qmlKeysFilePtr.SetPtr(new QFile(outputDirectoryPath + '/' + GetQmlKeysWrappedName(sdlType.GetName()) + ".qml"));

		if (!BeginQmlFile(sdlType)) {
			SendErrorMessage(0, QString("Unable to begin files"));
			I_CRITICAL();

			return iproc::IProcessor::TS_INVALID;
		}

		// Close files so that extenders can make their own changes
		if (!CloseFiles()) {
			SendErrorMessage(0, QString("Unable to close files"));
			I_CRITICAL();

			return iproc::IProcessor::TS_INVALID;
		}
	}

	// Then let extenders to make changes
	const int extendersCount = m_codeGeneratorExtenderListCompPtr.GetCount();
	for (int i = 0; i < extendersCount; ++i) {
		iproc::IProcessor* extenderPtr = m_codeGeneratorExtenderListCompPtr[i];
		Q_ASSERT(extenderPtr != nullptr);

		const int extenderResult = extenderPtr->DoProcessing(paramsPtr, inputPtr, outputPtr, progressManagerPtr);
		if (extenderResult != iproc::IProcessor::TS_OK) {
			return extenderResult;
		}
		retVal = qMax(retVal, extenderResult);
	}

	// Reopen files to complete processing
	for (const CSdlType& sdlType: sdlTypeList) {
		m_qmlFilePtr.SetPtr(new QFile(outputDirectoryPath + '/' + sdlType.GetName() + ".qml"));
		m_qmlKeysFilePtr.SetPtr(new QFile(outputDirectoryPath + '/' + GetQmlKeysWrappedName(sdlType.GetName()) + ".qml"));
		if (!ReOpenFiles()) {
			SendErrorMessage(0, QString("Unable to reopen files"));
			I_CRITICAL();

			return iproc::IProcessor::TS_INVALID;
		}

		// And complete the processing
		EndQmlFile(sdlType);
	}

	// then create a qmldir file
	QFile qmldirFile(outputDirectoryPath + "/qmldir");
	if (!qmldirFile.open(QIODevice::WriteOnly)) {
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
	for (const CSdlType& sdlType: sdlTypeList) {
		// add QML file
		qmldirStream << sdlType.GetName();
		qmldirStream << QStringLiteral(" 1.0 ");
		qmldirStream << sdlType.GetName();
		qmldirStream << QStringLiteral(".qml");
		FeedStream(qmldirStream, 1, false);

		// add QML Keys file as singleton
		qmldirStream << QStringLiteral("singleton ");
		qmldirStream << GetQmlKeysWrappedName(sdlType.GetName());
		qmldirStream << QStringLiteral(" 1.0 ");
		qmldirStream << GetQmlKeysWrappedName(sdlType.GetName());
		qmldirStream << QStringLiteral(".qml");
		FeedStream(qmldirStream, 1, false);
	}

	// and finally create a QRC file
	QFile qrcFile(outputDirectoryPath + "/" + currentNamespace + ".qrc");
	if (!qrcFile.open(QIODevice::WriteOnly)) {
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

	// add QML files
	for (const CSdlType& sdlType: sdlTypeList) {
		// add QML file
		xmlWriter.writeStartElement("file");
		xmlWriter.writeAttribute("alias", currentNamespace + '/' + sdlType.GetName() + ".qml");
		xmlWriter.writeCharacters(sdlType.GetName() + ".qml");
		xmlWriter.writeEndElement();

		// add QML Keys file
		xmlWriter.writeStartElement("file");
		xmlWriter.writeAttribute("alias", currentNamespace + '/' + GetQmlKeysWrappedName(sdlType.GetName()) + ".qml");
		xmlWriter.writeCharacters(GetQmlKeysWrappedName(sdlType.GetName()) + ".qml");
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
	if (!m_qmlFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)) {
		SendErrorMessage(0,
					QString("Unable to open file: '%1'. Error: %2")
							 .arg(m_qmlFilePtr->fileName(), m_qmlFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	if (!m_qmlKeysFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)) {
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(m_qmlKeysFilePtr->fileName(), m_qmlKeysFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	return true;
}


bool CQmlCodeGeneratorComp::CloseFiles()
{
	bool retVal = true;

	retVal = m_qmlFilePtr->flush();
	m_qmlFilePtr->close();

	retVal = retVal && m_qmlKeysFilePtr->flush();
	Q_ASSERT(retVal);

	m_qmlKeysFilePtr->close();

	return retVal;
}


bool CQmlCodeGeneratorComp::BeginQmlFile(const CSdlType& sdlType)
{
	if (!m_qmlFilePtr->open(QIODevice::WriteOnly)) {
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(m_qmlFilePtr->fileName(), m_qmlFilePtr->errorString()));

		return false;
	}

	if (!m_qmlKeysFilePtr->open(QIODevice::WriteOnly)) {
		SendErrorMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(m_qmlKeysFilePtr->fileName(), m_qmlKeysFilePtr->errorString()));

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

	// add id
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("id :") << GetDecapitalizedValue(sdlType.GetName());


	// container's props
	for (const CSdlField& sdlField: sdlType.GetFields()) {
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 1);

		ifStream << QStringLiteral("property ");
		if (sdlField.IsArray()) {
			ifStream << QStringLiteral("BaseModel");
		}
		else {
			ifStream << QmlConvertType(sdlField.GetType());
		}
		// use 'm_' prefix to avoid ambiguity
		ifStream << QStringLiteral(" m_") << GetDecapitalizedValue(sdlField.GetId());
		bool isCustom = false;
		QString convertedType = QmlConvertType(sdlField.GetType(), &isCustom);
		if (!isCustom) {
			ifStream << ':' << ' ';
			if (convertedType == QStringLiteral("int") ||
				convertedType == QStringLiteral("real") ||
				convertedType == QStringLiteral("double"))
			{
				ifStream << QStringLiteral("0");
			}
			else if (convertedType == QStringLiteral("bool")) {
				ifStream << QStringLiteral("false");
			}
			else if (convertedType == QStringLiteral("string"))
			{
				ifStream << QStringLiteral("''");
			}
		}
		else if (!sdlField.IsArray()) {
			ifStream << ':' << ' ' << convertedType << ' ' << '{';
			ifStream << QStringLiteral("owner: ");
			ifStream << GetDecapitalizedValue(sdlType.GetName());
			ifStream << '}';
		}
	}
	FeedStream(ifStream, 1, false);

	// base class reimplementation methods
	// getJSONKeyForProperty
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("function getJSONKeyForProperty(propertyId) {");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("switch (propertyId) {");
	for (const CSdlField& sdlField: sdlType.GetFields()) {
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 3);
		ifStream << QStringLiteral("case 'm_") << GetDecapitalizedValue(sdlField.GetId());
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
	ifStream << QStringLiteral("function createComponent(propertyId) {");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("switch (propertyId) {");
	for (const CSdlField& sdlField: sdlType.GetFields()) {
		bool isCustom = false;
		const QString convertedType = QmlConvertType(sdlField.GetType(), &isCustom);
		if (!isCustom && !sdlField.IsArray()) {
			// skip simple scalars
			continue;
		}

		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 3);
		ifStream << QStringLiteral("case 'm_") << GetDecapitalizedValue(sdlField.GetId());
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
	ifStream << QStringLiteral("function createMe() {");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("return Qt.createComponent('");
	ifStream << sdlType.GetName();
	ifStream << QStringLiteral(".qml').createObject()");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << '}'; // end of function

	FeedStream(ifStream, 1, false);

	// -----fill QML keys file
	QTextStream qmlKeyFileStream(m_qmlKeysFilePtr.GetPtr());

	// Begin QtObject
	qmlKeyFileStream << QStringLiteral("pragma Singleton");
	FeedStream(qmlKeyFileStream, 2, false);
	qmlKeyFileStream << QStringLiteral("import QtQuick 2.0");
	FeedStream(qmlKeyFileStream, 2, false);

	qmlKeyFileStream << QStringLiteral("QtObject {");
	for (const CSdlField& sdlField: sdlType.GetFields()) {
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


bool CQmlCodeGeneratorComp::EndQmlFile(const CSdlType& /*sdlType*/)
{
	QTextStream ifStream(m_qmlFilePtr.GetPtr());
	ifStream << '}';
	FeedStream(ifStream, 2);

	QTextStream qmlKeyFileStream(m_qmlKeysFilePtr.GetPtr());
	qmlKeyFileStream << '}';
	FeedStream(qmlKeyFileStream, 2);

	return true;
}


void CQmlCodeGeneratorComp::AbortCurrentProcessing()
{
	m_qmlFilePtr->close();
	m_qmlKeysFilePtr->close();

	I_CRITICAL();

	m_qmlFilePtr->remove();
	m_qmlKeysFilePtr->remove();
}

QString CQmlCodeGeneratorComp::GetQmlKeysWrappedName(const QString& originalName) const
{
	return *m_qmlKeysFilePrefixAttrPtr + originalName + *m_qmlKeysFileSuffixAttrPtr;
}


} // namespace imtsdl

