// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
#include <imtsdl/CSdlField.h>
#include <imtsdl/CSdlEnum.h>
#include <imtsdlgenqml/CSdlQmlGenerationResult.h>
#include <imtsdlgenqml/CQmlGenTools.h>


namespace imtsdlgenqml
{


iproc::IProcessor::TaskState CQmlCodeGeneratorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_sdlEnumListCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());

	TaskState retVal = TS_OK;

	if (!m_argumentParserCompPtr->IsQmlEnabled()){
		return retVal;
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

	if (m_argumentParserCompPtr->IsDependenciesMode() || !m_argumentParserCompPtr->GetDepFilePath().isEmpty()){
		if (!m_customSchemaParamsCompPtr.IsValid()){
			SendErrorMessage(0, "Application is not configured with custom parameters. Auto join is not possible. Please specify paths to join explicitly(use -J option), or disable join.");

			return TS_INVALID;
		}

		QStringList qmlFilePaths;
		qmlFilePaths << GetAutoDefinedQmlQrcFilePath(*m_customSchemaParamsCompPtr, imtsdl::CSdlTools::GetCompleteOutputPath(m_customSchemaParamsCompPtr, *m_argumentParserCompPtr, true, false));
		PrintFiles(std::cout, qmlFilePaths, m_argumentParserCompPtr->GetGeneratorType());
		PrintFiles(m_argumentParserCompPtr->GetDepFilePath(), qmlFilePaths, *m_dependentSchemaListCompPtr);

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

	// Then let extenders make changes
	const int extendersCount = m_codeGeneratorExtenderListCompPtr.GetCount();
	for (int i = 0; i < extendersCount; ++i){
		IProcessor* extenderPtr = m_codeGeneratorExtenderListCompPtr[i];
		Q_ASSERT(extenderPtr != nullptr);

		TaskState extenderResult = extenderPtr->DoProcessing(paramsPtr, inputPtr, outputPtr, progressManagerPtr);
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
	QString qrcFilePath = GetAutoDefinedQmlQrcFilePath(*m_customSchemaParamsCompPtr, GetCompleteOutputPath(m_customSchemaParamsCompPtr, *m_argumentParserCompPtr, true, false));
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
	xmlWriter.setAutoFormatting(true);

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

	// Create generation result file if generation was successful
	if (retVal == TS_OK && !m_argumentParserCompPtr->GetDepFilePath().isEmpty()){
		CSdlQmlGenerationResult generationResult;
		generationResult.SetCreatedAt(QDateTime::currentDateTimeUtc());
		generationResult.SetGeneratorVersion(QStringLiteral("1.0"));
		
		// Track the created folders
		QSet<QString> createdFolders;
		createdFolders.insert(outputDirectoryPath);
		generationResult.SetCreatedFolders(createdFolders);
		
		// Place generation info file next to depfile
		const QFileInfo depFileInfo(m_argumentParserCompPtr->GetDepFilePath());
		const QString generationInfoPath = depFileInfo.absolutePath() + QStringLiteral("/generation_info.json");
		
		if (!CQmlGenTools::UpdateGenerationResult(generationInfoPath, generationResult)){
			SendWarningMessage(0, QString("Unable to create generation info file: '%1'").arg(generationInfoPath));
		}
	}

	return retVal;
}


// private methods

bool CQmlCodeGeneratorComp::ReOpenFiles()
{
	Q_ASSERT(m_qmlFilePtr != nullptr);
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

	Q_ASSERT(m_qmlFilePtr != nullptr);
	retVal = m_qmlFilePtr->flush();
	m_qmlFilePtr->close();

	return retVal;
}


bool CQmlCodeGeneratorComp::BeginQmlFile(const imtsdl::CSdlType& sdlType)
{
	Q_ASSERT(m_qmlFilePtr != nullptr);
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
	const imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr->GetUnions(false);
	const imtsdl::SdlFieldList typeFieldList = sdlType.GetFields();

	for (const imtsdl::CSdlField& field: typeFieldList){
		bool isCustom = false;
		ConvertType(field, &isCustom);
		if (!isCustom){
			// imports is required only for custom types
			continue;
		}


		const std::shared_ptr<imtsdl::CSdlEntryBase> foundEntry = GetSdlTypeOrEnumOrUnionForField(field, allTypes, enumList, unionList);
		if (!foundEntry){
			SendCriticalMessage(0, QString("Unable to find type for %1:%2").arg(field.GetId(), field.GetType()));

			return false;
		}

		if (foundEntry->IsExternal()){
			QString qmlImportDeclaration = foundEntry->GetQmlImportDeclaration();
			if (!qmlImportDeclaration.isEmpty()){
				requiredImports << qmlImportDeclaration;
			}
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

	// add typename
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("readonly property string __typename: '") << sdlType.GetName() << '\'';

	// container's props
	for (const imtsdl::CSdlField& sdlField: typeFieldList){
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 1);

		ifStream << QStringLiteral("property ");

		const std::shared_ptr<imtsdl::CSdlEntryBase> foundEntryPtr = GetSdlTypeOrEnumOrUnionForField(sdlField, allTypes, enumList, unionList);
		const bool isEnum = bool(dynamic_cast<const imtsdl::CSdlEnum*>(foundEntryPtr.get()) != nullptr);
		const auto* unionPtr = dynamic_cast<const imtsdl::CSdlUnion*>(foundEntryPtr.get());

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
		else if (unionPtr != nullptr){
			ifStream << QStringLiteral("var");
		}
		else {
			ifStream << convertedType;
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
			ifStream << QStringLiteral("null");
		}
	}

	FeedStream(ifStream, 2, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("Component.onCompleted: {");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("this._internal.removed = [");
	for (int index = 0; index < typeFieldList.count(); index++){
		if (index > 0){
			ifStream << QStringLiteral(",");
		}
		const imtsdl::CSdlField& sdlField = typeFieldList[index];
		ifStream << QStringLiteral("\"m_") << GetDecapitalizedValue(sdlField.GetId()) << QStringLiteral("\"");
	}
	ifStream << QStringLiteral("]");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("}");
	FeedStream(ifStream, 1, false);

	// public class comfort methods
	// has<PROPERTY>()
	for (const imtsdl::CSdlField& sdlField: typeFieldList){
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("function has");
		ifStream << GetCapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral("(){");
		FeedStream(ifStream, 1, false);

		FeedStreamHorizontally(ifStream, 2);
		ifStream << QStringLiteral("return (m_");
		ifStream << GetDecapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral(" !== undefined && m_");
		ifStream << GetDecapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral(" !== null)");

		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 1);
		ifStream << '}';
		FeedStream(ifStream, 1, false);
	}

	// create<PROPERTY>()
	for (const imtsdl::CSdlField& sdlField: typeFieldList){
		const std::shared_ptr<imtsdl::CSdlEntryBase> foundEntryPtr = GetSdlTypeOrEnumOrUnionForField(sdlField, allTypes, enumList, unionList);

		bool isEnum = bool (dynamic_cast<imtsdl::CSdlEnum*>(foundEntryPtr.get()) != nullptr);
		bool isCustom = false;
		QmlConvertType(sdlField.GetType(), &isCustom);
		const bool isUserType = isCustom && !isEnum;

		if ((!isCustom && !sdlField.IsArray()) || isEnum){
			continue;
		}

		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("function emplace");
		ifStream << GetCapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral("(typename){");
		FeedStream(ifStream, 1, false);

		FeedStreamHorizontally(ifStream, 2);
		ifStream << QStringLiteral("m_");
		ifStream << GetDecapitalizedValue(sdlField.GetId());
		if (isUserType || (sdlField.IsArray() && isUserType)){
			ifStream << QStringLiteral(" = createComponent('");
			ifStream << QStringLiteral("m_");
			ifStream << GetDecapitalizedValue(sdlField.GetId());
			ifStream << QStringLiteral("', typename).createObject(this)");
			FeedStream(ifStream, 1, false);
			FeedStreamHorizontally(ifStream, 2);
			ifStream << QStringLiteral("m_");
			ifStream << GetDecapitalizedValue(sdlField.GetId());
			ifStream << QStringLiteral(".owner = this");
		}
		else if (sdlField.IsArray()){
			ifStream << QStringLiteral(" = []");
		}
		else{
			Q_ASSERT_X(0, "CQmlCodeGeneratorComp::BeginQmlFile", "Invalid property");
		}
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 2);
		ifStream << QStringLiteral("this._internal.removeAt('");
		ifStream << QStringLiteral("m_");
		ifStream << GetDecapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral("')");

		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 1);
		ifStream << '}';
		FeedStream(ifStream, 1, false);
	}

	// remove<PROPERTY>()
	for (const imtsdl::CSdlField& sdlField: typeFieldList){
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("function remove");
		ifStream << GetCapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral("(){");
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 2);
		ifStream << QStringLiteral("removeKey('");
		ifStream << QStringLiteral("m_");
		ifStream << GetDecapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral("')");
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 1);
		ifStream << '}';
		FeedStream(ifStream, 1, false);
	}

	// base class reimplementation methods
	// getJSONKeyForProperty
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("function getJSONKeyForProperty(propertyId){");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("switch (propertyId){");
	for (const imtsdl::CSdlField& sdlField: typeFieldList){
		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 3);
		ifStream << QStringLiteral("case 'm_") << GetDecapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral("': return '") << sdlField.GetId() << '\'';
	}
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 3);
	ifStream << QStringLiteral("case '__typename': return '__typename'");

	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << '}'; // end of switch
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << '}'; // end of function

	// createElement (arrays of custom only)
	FeedStream(ifStream, 2, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("function createElement(propertyId, typename){");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("switch (propertyId){");
	FeedStream(ifStream, 1, false);
	for (const imtsdl::CSdlField& sdlField: typeFieldList){
		bool isCustom = false;
		const std::shared_ptr<imtsdl::CSdlEntryBase> foundEntryPtr = GetSdlTypeOrEnumOrUnionForField(sdlField, allTypes, enumList, unionList);
		const QString convertedType = QmlConvertType(sdlField.GetType(), &isCustom);
		const bool isEnum = bool(dynamic_cast<const imtsdl::CSdlEnum*>(foundEntryPtr.get()) != nullptr);
		const bool isUserType = isCustom && !isEnum;
		const auto* unionPtr = dynamic_cast<const imtsdl::CSdlUnion*>(foundEntryPtr.get());

		// skip simple scalars and list of scalars
		if (!sdlField.IsArray() || !isUserType || (sdlField.IsArray() && !isUserType)){
			continue;
		}

		FeedStreamHorizontally(ifStream, 3);
		ifStream << QStringLiteral("case 'm_") << GetDecapitalizedValue(sdlField.GetId());

		if (unionPtr != nullptr){
			const QStringList typeList = unionPtr->GetTypes();

			ifStream << QStringLiteral("': {");
			FeedStream(ifStream, 1, false);
			FeedStreamHorizontally(ifStream, 4);
			ifStream << QStringLiteral("switch (typename){");
			FeedStream(ifStream, 1, false);

			for (const QString& typeName: typeList){
				std::shared_ptr<imtsdl::CSdlEntryBase> foundTypePtr = FindEntryByName(typeName, m_sdlTypeListCompPtr->GetSdlTypes(false));

				if (!foundTypePtr){
					continue;
				}

				const QString convertedSubtype = QmlConvertType(typeName);
				FeedStreamHorizontally(ifStream, 5);
				ifStream << QStringLiteral("case '") << typeName;
				ifStream << QStringLiteral("': return Qt.createComponent('qrc:/qml/");
				ifStream << BuildQmlImportDeclarationFromParams(foundTypePtr->GetSchemaParams(), QStringLiteral("Sdl"), false) << '/';
				ifStream << convertedSubtype << QStringLiteral(".qml')");
				FeedStream(ifStream, 1, false);
			}

			FeedStreamHorizontally(ifStream, 4);
			ifStream << '}';
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 3);
			ifStream << '}';
			FeedStream(ifStream, 1, false);
		}
		else {
			ifStream << QStringLiteral("': return Qt.createComponent('qrc:/qml/");
			ifStream << BuildQmlImportDeclarationFromParams(foundEntryPtr->GetSchemaParams(), QStringLiteral("Sdl"), false) << '/';
			ifStream << convertedType << QStringLiteral(".qml')");
			FeedStream(ifStream, 1, false);
		}
	}

	FeedStreamHorizontally(ifStream, 2);
	ifStream << '}'; // end of switch
	FeedStream(ifStream, 1, false);
	
	FeedStreamHorizontally(ifStream, 1);
	ifStream << '}'; // end of function

	// comfort createElement function
	for (const imtsdl::CSdlField& sdlField: typeFieldList){
		bool isCustom = false;
		const std::shared_ptr<imtsdl::CSdlEntryBase> foundEntryPtr = GetSdlTypeOrEnumOrUnionForField(sdlField, allTypes, enumList, unionList);
		QmlConvertType(sdlField.GetType(), &isCustom);
		const bool isEnum = bool(dynamic_cast<const imtsdl::CSdlEnum*>(foundEntryPtr.get()) != nullptr);
		const bool isUserType = isCustom && !isEnum;

		if (!sdlField.IsArray() || !isUserType || (sdlField.IsArray() && !isUserType)){
			continue;
		}

		FeedStream(ifStream, 2, false);

		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("function create");
		ifStream << GetCapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral("ArrayElement(typename){");
		FeedStream(ifStream, 1, false);

		FeedStreamHorizontally(ifStream, 2);
		ifStream << QStringLiteral("return createElement('m_");
		ifStream << GetDecapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral("', typename).createObject()");
		FeedStream(ifStream, 1, false);

		FeedStreamHorizontally(ifStream, 1);
		ifStream << '}';
	}

	// createComponent (custom and arrays only)
	FeedStream(ifStream, 2, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("function createComponent(propertyId, typename){");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("let retVal;");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("switch (propertyId){");
	FeedStream(ifStream, 1, false);

	for (const imtsdl::CSdlField& sdlField: typeFieldList){
		bool isCustom = false;
		const QString convertedType = QmlConvertType(sdlField.GetType(), &isCustom);

		const std::shared_ptr<imtsdl::CSdlEntryBase> foundEntryPtr = GetSdlTypeOrEnumOrUnionForField(sdlField, allTypes, enumList, unionList);
		const bool isEnum = bool(dynamic_cast<const imtsdl::CSdlEnum*>(foundEntryPtr.get()) != nullptr);
		const bool isUserType = isCustom && !isEnum;

		// skip simple scalars and list of scalars
		if (!isUserType || (sdlField.IsArray() && !isUserType)){
			continue;
		}

		const auto* unionPtr = dynamic_cast<const imtsdl::CSdlUnion*>(foundEntryPtr.get());
		if (sdlField.IsArray()){
			FeedStreamHorizontally(ifStream, 3);
			ifStream << QStringLiteral("case 'm_") << GetDecapitalizedValue(sdlField.GetId());
			ifStream << QStringLiteral("':{");
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 4);
			ifStream << QStringLiteral("retVal = Qt.createComponent('qrc:/qml/imtcontrols/Base/BaseModel.qml')");
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 4);
			ifStream << QStringLiteral("retVal.owner = ");
			ifStream << GetDecapitalizedValue(sdlType.GetName());
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 4);
			ifStream << QStringLiteral("return retVal");
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 3);
			ifStream << '}';
		}
		else if (unionPtr != nullptr){
			const QStringList typeList = unionPtr->GetTypes();

			FeedStreamHorizontally(ifStream, 3);
			ifStream << QStringLiteral("case 'm_") << GetDecapitalizedValue(sdlField.GetId()) << QStringLiteral("': {");
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 4);
			ifStream << QStringLiteral("switch (typename){");
			FeedStream(ifStream, 1, false);

			for (const QString& typeName: typeList){
				std::shared_ptr<imtsdl::CSdlEntryBase> foundTypePtr =
					FindEntryByName(typeName, m_sdlTypeListCompPtr->GetSdlTypes(false));

				if (!foundTypePtr){
					continue;
				}

				const QString convertedSubtype = QmlConvertType(typeName);
				FeedStreamHorizontally(ifStream, 5);
				ifStream << QStringLiteral("case '") << typeName;
				ifStream << QStringLiteral("': return Qt.createComponent('qrc:/qml/");
				ifStream << BuildQmlImportDeclarationFromParams(foundTypePtr->GetSchemaParams(), QStringLiteral("Sdl"), false) << '/';
				ifStream << convertedSubtype << QStringLiteral(".qml')");
				FeedStream(ifStream, 1, false);
			}

			FeedStreamHorizontally(ifStream, 4);
			ifStream << '}';
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 3);
			ifStream << '}';
		}
		else {
			imtsdl::CSdlType foundType;
			[[maybe_unused]]const bool isTypeFound = GetSdlTypeForField(
				sdlField,
				m_sdlTypeListCompPtr->GetSdlTypes(false),
				foundType);
			Q_ASSERT(isTypeFound);

			FeedStreamHorizontally(ifStream, 3);
			ifStream << QStringLiteral("case 'm_") << GetDecapitalizedValue(sdlField.GetId());
			ifStream << QStringLiteral("': return Qt.createComponent('qrc:/qml/");
			ifStream << BuildQmlImportDeclarationFromParams(foundEntryPtr->GetSchemaParams(), QStringLiteral("Sdl"), false) << '/';
			ifStream << convertedType << QStringLiteral(".qml')");
		}
		FeedStream(ifStream, 1, false);
	}

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

	// getType
	FeedStream(ifStream, 2, false);
	FeedStreamHorizontally(ifStream, 1);
	ifStream << QStringLiteral("function getPropertyType(propertyId){");
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << QStringLiteral("switch (propertyId){");

	for (const imtsdl::CSdlField& sdlField: typeFieldList){
		bool isCustom = false;
		const QString convertedType = QmlConvertType(sdlField.GetType(), &isCustom);

		const std::shared_ptr<imtsdl::CSdlEntryBase> foundEntryPtr = GetSdlTypeOrEnumOrUnionForField(sdlField, allTypes, enumList, unionList);
		const bool isEnum = bool(dynamic_cast<const imtsdl::CSdlEnum*>(foundEntryPtr.get()) != nullptr);
		const bool isUnion = bool(dynamic_cast<const imtsdl::CSdlUnion*>(foundEntryPtr.get()) != nullptr);

		const QString propertyName = QStringLiteral("m_") + GetDecapitalizedValue(sdlField.GetId());

		FeedStream(ifStream, 1, false);
		FeedStreamHorizontally(ifStream, 3);
		ifStream << QStringLiteral("case '") << propertyName;
		ifStream << QStringLiteral("': ");
		if (sdlField.IsArray()){
			if (isCustom){
				ifStream << QStringLiteral("return 'BaseModel'");
			}
			else{
				ifStream << QStringLiteral("return 'array'");
			}
		}
		else if (isEnum){
			ifStream << QStringLiteral("return 'string'");
		}
		else if (isUnion){
			ifStream << '{';
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 4);
			ifStream << QStringLiteral("if (!") << propertyName;
			ifStream << QStringLiteral(" || !") << propertyName;
			ifStream << QStringLiteral(".__typename){");
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 5);
			ifStream << QStringLiteral("return '") << convertedType << '\'';
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 4);
			ifStream << '}';
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 4);
			ifStream << QStringLiteral("else {");
			FeedStream(ifStream, 1, false);


			FeedStreamHorizontally(ifStream, 5);
			ifStream << QStringLiteral("return ") << propertyName << QStringLiteral(".__typename");
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 4);
			ifStream << '}';
			FeedStream(ifStream, 1, false);

			FeedStreamHorizontally(ifStream, 3);
			ifStream << '}';
		}
		else {
			ifStream << QStringLiteral("return '") << convertedType << '\'';
		}
	}

	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream, 2);
	ifStream << '}'; // end of switch
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
	Q_ASSERT(m_qmlFilePtr != nullptr);
	m_qmlFilePtr->close();

	I_CRITICAL();

	m_qmlFilePtr->remove();
}


} // namespace imtsdlgenqml

