#include "CQmlCodeGeneratorComp.h"


// C includes
#include <iostream>

// Qt includes
#include <QtCore/QDir>
#include <QtCore/QXmlStreamWriter>

//Acf includes
#include <istd/CSystem.h>
#include <iprm/TParamsPtr.h>
#include <iprm/ITextParam.h>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgen
{


int CQmlCodeGeneratorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_sdlEnumListCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());

	int retVal = TS_OK;

	if (!m_argumentParserCompPtr->IsQmlEnabled()){
		return retVal;
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

	if (m_argumentParserCompPtr->IsDependenciesMode() || !m_argumentParserCompPtr->GetDepFilePath().isEmpty()){
		if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
			if (!m_customSchemaParamsCompPtr.IsValid()){
				SendErrorMessage(0, "Application is not configured with custom parameters. Auto join is not possible. Please specify paths to join explicitly(use -J option), or disable join.");

				return TS_INVALID;
			}

			QStringList qmlFilePaths;
			qmlFilePaths << GetAutoDefinedQmlQrcFilePath(*m_customSchemaParamsCompPtr, m_argumentParserCompPtr->GetOutputDirectoryPath());
			PrintFiles(std::cout, qmlFilePaths, m_argumentParserCompPtr->GetGeneratorType());
			PrintFiles(m_argumentParserCompPtr->GetDepFilePath(), qmlFilePaths, *m_dependentSchemaListCompPtr);
		}
		else{
			QStringList cumulatedFiles;
			// generated QRC file
			cumulatedFiles << QString(outputDirectoryPath + "/" + GetQmlModuleNameFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr) + ".qrc");

			PrintFiles(std::cout, cumulatedFiles, m_argumentParserCompPtr->GetGeneratorType());
		}

		if (m_argumentParserCompPtr->IsDependenciesMode()){
			return TS_OK;
		}
	}

	imtsdl::SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes(true);

	// first create all files with basic mathods
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		m_qmlFilePtr.SetPtr(new QFile(outputDirectoryPath + '/' + sdlType.GetName() + ".qml"));

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

	// Then let extenders to make changes
	const int extendersCount = m_codeGeneratorExtenderListCompPtr.GetCount();
	for (int i = 0; i < extendersCount; ++i){
		iproc::IProcessor* extenderPtr = m_codeGeneratorExtenderListCompPtr[i];
		Q_ASSERT(extenderPtr != nullptr);

		int extenderResult = extenderPtr->DoProcessing(paramsPtr, inputPtr, outputPtr, progressManagerPtr);
		if (extenderResult != TS_OK){
			return extenderResult;
		}
		retVal = qMax(retVal, extenderResult);
	}

	// Reopen files to complete processing
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		m_qmlFilePtr.SetPtr(new QFile(outputDirectoryPath + '/' + sdlType.GetName() + ".qml"));
		if (!ReOpenFiles()){
			SendErrorMessage(0, QString("Unable to reopen files"));
			I_CRITICAL();

			return TS_INVALID;
		}

		// And complete the processing
		EndQmlFile(sdlType);
	}

	// then create a qmldir file
	QFile qmldirFile(outputDirectoryPath + "/qmldir");
	if (!qmldirFile.open(QIODevice::WriteOnly)){
		SendCriticalMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(qmldirFile.fileName(), qmldirFile.errorString()));
		I_CRITICAL();

		return TS_INVALID;
	}

	const QString qmlModuleName = GetQmlModuleNameFromParamsOrArguments(m_customSchemaParamsCompPtr, m_argumentParserCompPtr);
	QTextStream qmldirStream(&qmldirFile);
	qmldirStream << QStringLiteral("module ");
	qmldirStream << qmlModuleName;
	FeedStream(qmldirStream, 2, false);

	// define a version
	QString moduleVersion = QStringLiteral("1.0");
	if (m_customSchemaParamsCompPtr.IsValid()){
		iprm::TParamsPtr<iprm::ITextParam> versionNameParamPtr(m_customSchemaParamsCompPtr.GetPtr(), imtsdl::SdlCustomSchemaKeys::VersionName.toUtf8(), false);
		if (versionNameParamPtr.IsValid()){
			moduleVersion = versionNameParamPtr->GetText();
		}
	}
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		// add QML file
		qmldirStream << sdlType.GetName();
		qmldirStream << ' ' << moduleVersion << ' ';
		qmldirStream << sdlType.GetName();
		qmldirStream << QStringLiteral(".qml");
		FeedStream(qmldirStream, 1, false);
	}

	// and finally create a QRC file
	QString qrcFilePath = outputDirectoryPath + "/" + qmlModuleName + ".qrc";
	if (m_argumentParserCompPtr->IsAutoJoinEnabled()){
		qrcFilePath = GetAutoDefinedQmlQrcFilePath(*m_customSchemaParamsCompPtr, m_argumentParserCompPtr->GetOutputDirectoryPath());
	}
	QFile qrcFile(qrcFilePath);
	if (!qrcFile.open(QIODevice::WriteOnly)){
		SendCriticalMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(qrcFile.fileName(), qrcFile.errorString()));
		I_CRITICAL();

		return TS_INVALID;
	}

	/// \todo remove it. Use persistence!
	QXmlStreamWriter xmlWriter(&qrcFile);
	xmlWriter.setAutoFormatting(1);

	xmlWriter.writeStartDocument();
	xmlWriter.writeStartElement("RCC");
	xmlWriter.writeStartElement("qresource");
	xmlWriter.writeAttribute("prefix", "/qml");

	// add QML files
	for (const imtsdl::CSdlType& sdlType: sdlTypeList){
		// add QML file
		xmlWriter.writeStartElement("file");
		xmlWriter.writeAttribute("alias", qmlModuleName + '/' + sdlType.GetName() + ".qml");
		xmlWriter.writeCharacters(sdlType.GetName() + ".qml");
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

	return retVal;
}


// private methods

bool CQmlCodeGeneratorComp::ReOpenFiles()
{
	if (!m_qmlFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)){
		SendCriticalMessage(0,
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
	m_qmlFilePtr->close();

	return retVal;
}


bool CQmlCodeGeneratorComp::BeginQmlFile(const imtsdl::CSdlType& sdlType)
{
	if (!m_qmlFilePtr->open(QIODevice::WriteOnly)){
		SendCriticalMessage(0,
						 QString("Unable to open file: '%1'. Error: %2")
							 .arg(m_qmlFilePtr->fileName(), m_qmlFilePtr->errorString()));
		I_CRITICAL();

		return false;
	}

	// add imports for external types
	QSet<QString> requiredImports;
	const imtsdl::SdlTypeList allTypes = m_sdlTypeListCompPtr->GetSdlTypes(false);
	const imtsdl::SdlEnumList enumList = m_sdlEnumListCompPtr->GetEnums(false);
	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		bool isCustom = false;
		ConvertType(field, &isCustom);
		if (!isCustom){
			// imports is required only for custom types
			continue;
		}


		const std::shared_ptr<imtsdl::CSdlEntryBase> foundEntry = GetSdlTypeOrEnumForField(field, allTypes, enumList);
		if (!foundEntry){
			SendCriticalMessage(0, QString("Unable to find type for %1:%2").arg(field.GetId(), field.GetType()));
			I_CRITICAL();

			return false;
		}

		if (foundEntry->IsExternal()){
			QString qmlImportDeclaration = foundEntry->GetQmlImportDeclaration();
			if (!qmlImportDeclaration.isEmpty())
				requiredImports << qmlImportDeclaration;
		}
	}

	QTextStream ifStream(m_qmlFilePtr.GetPtr());

	// import section
	ifStream << QStringLiteral("import QtQuick");
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("import imtcontrols 1.0");
	FeedStream(ifStream, 1, false);

	// add required Imports for external types
	for (const QString& importDeclaration: std::as_const(requiredImports)){
		ifStream << QStringLiteral("import ");
		ifStream << importDeclaration;
		FeedStream(ifStream, 1, false);
	}
	FeedStream(ifStream, 1, false);

	// Base QML
	ifStream << QStringLiteral("BaseClass {");

	// add id
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("id: ") << GetDecapitalizedValue(sdlType.GetName());


	// container's props
	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 1);

		ifStream << QStringLiteral("property ");

		const std::shared_ptr<imtsdl::CSdlEntryBase> foundEntryPtr = GetSdlTypeOrEnumForField(sdlField, allTypes, enumList);
		const bool isEnum = bool(dynamic_cast<const imtsdl::CSdlEnum*>(foundEntryPtr.get()) != nullptr);

		bool isCustom = false;
		QString convertedType = QmlConvertType(sdlField.GetType(), &isCustom);
		if (sdlField.IsArray() && (!isCustom || isEnum)){
			ifStream << QStringLiteral("var");
		}
		else if (sdlField.IsArray()){
			ifStream << QStringLiteral("BaseModel");
		}
		else if (isEnum){
			ifStream << QStringLiteral("string");
		}
		else {
			ifStream << QmlConvertType(sdlField.GetType());
		}
		// use 'm_' prefix to avoid ambiguity
		ifStream << QStringLiteral(" m_") << GetDecapitalizedValue(sdlField.GetId());
		ifStream << ':' << ' ';
		if (sdlField.IsArray() && (!isCustom || isEnum)){
			ifStream << QStringLiteral("[]");
		}
		else if (!isCustom || isEnum){
			if (convertedType == QStringLiteral("int") ||
				convertedType == QStringLiteral("real") ||
				convertedType == QStringLiteral("double"))
			{
				ifStream << QStringLiteral("0");
			}
			else if (convertedType == QStringLiteral("bool")){
				ifStream << QStringLiteral("false");
			}
			else if (convertedType == QStringLiteral("string") || isEnum)
			{
				ifStream << QStringLiteral("''");
			}
		}
		else {
			if (sdlField.IsArray()){
				ifStream << QStringLiteral("BaseModel");
			}
			else {
				ifStream << convertedType;
			}
			ifStream << ' ' << '{';
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
	ifStream << QStringLiteral("function getJSONKeyForProperty(propertyId){");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("switch (propertyId){");
	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
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
	ifStream << QStringLiteral("function createComponent(propertyId){");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("switch (propertyId){");

	for (const imtsdl::CSdlField& sdlField: sdlType.GetFields()){
		bool isCustom = false;
		const QString convertedType = QmlConvertType(sdlField.GetType(), &isCustom);

		const std::shared_ptr<imtsdl::CSdlEntryBase> foundEntryPtr = GetSdlTypeOrEnumForField(sdlField, allTypes, enumList);
		const bool isEnum = bool(dynamic_cast<const imtsdl::CSdlEnum*>(foundEntryPtr.get()) != nullptr);

		// skip simple scalars and list of scalars
		if (!isCustom || isEnum){
			continue;
		}

		FeedStream(ifStream, 1, false);

		imtsdl::CSdlType foundType;
		[[maybe_unused]]const bool isTypeFound = GetSdlTypeForField(
			sdlField,
			m_sdlTypeListCompPtr->GetSdlTypes(false),
			foundType);
		Q_ASSERT(isTypeFound);

		FeedStreamHorizontally(ifStream, 3);
		ifStream << QStringLiteral("case 'm_") << GetDecapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral("': return Qt.createComponent('qrc:/qml/");
		ifStream << BuildQmlImportDeclarationFromParams(foundType.GetSchemaParams(), QStringLiteral("Sdl"), false) << '/';
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

	FeedStream(ifStream, 1, false);

	return true;
}


bool CQmlCodeGeneratorComp::EndQmlFile(const imtsdl::CSdlType& /*sdlType*/)
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


} // namespace imtsdlgen

