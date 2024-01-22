#include "CSdlClassCodeGeneratorComp.h"


// C includes
#include <iostream>

// Qt includes
#include <QtCore/QDir>

//Acf includes
#include <istd/CSystem.h>

// imtsdl includes
#include <imtsdl/CSdlType.h>


namespace imtsdl
{


int CSdlClassCodeGeneratorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());

	int retVal = iproc::IProcessor::TS_OK;

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
			std::cout << QString(outputDirectoryPath + "/C" + sdlType.GetName() + ".h").toStdString();
			std::cout << QString(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp").toStdString();
		}

		return iproc::IProcessor::TS_OK;
	}

	SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes();
	for (const CSdlType& sdlType: sdlTypeList){
		m_headerFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".h"));
		m_sourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp"));
		BeginClassFiles(sdlType);

		// Close files so that extenders can make their own changes
		if (!CloseFiles()){
			SendErrorMessage(0, QString("Unable to close files"));
			I_CRITICAL();

			return iproc::IProcessor::TS_INVALID;
		}

		// Then let extenders to make changes
		const int extendersCount = m_codeGeneratorExtenderListCompPtr.GetCount();
		for (int i = 0; i < extendersCount; ++i){
			iproc::IProcessor* extenderPtr = m_codeGeneratorExtenderListCompPtr[i];
			Q_ASSERT(extenderPtr == nullptr);

			const int extenderResult = extenderPtr->DoProcessing(paramsPtr, inputPtr, outputPtr, progressManagerPtr);
			if (extenderResult != iproc::IProcessor::TS_OK){
				return extenderResult;
			}
			retVal = qMax(retVal, extenderResult);
		}

		// Reopen files to complete processing
		if (!ReOpenFiles()){
			SendErrorMessage(0, QString("Unable to reopen files"));
			I_CRITICAL();

			return iproc::IProcessor::TS_INVALID;
		}

		// And complete the processing
		EndClassFiles(sdlType);
	}

	return retVal;
}


// private methods

bool CSdlClassCodeGeneratorComp::ReOpenFiles()
{
	if (!m_headerFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)){
		SendErrorMessage(0,
					QString("Unable to open file: '%1'. Error: %2")
						.arg(m_headerFilePtr->fileName(), m_headerFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	if (!m_sourceFilePtr->open(QIODevice::WriteOnly | QIODevice::Append)){
		SendErrorMessage(0,
						QString("Unable to open file: '%1'. Error: %2")
							.arg(m_sourceFilePtr->fileName(), m_sourceFilePtr->errorString()));

		AbortCurrentProcessing();

		return false;
	}

	return true;
}


bool CSdlClassCodeGeneratorComp::CloseFiles()
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


bool CSdlClassCodeGeneratorComp::BeginClassFiles(const CSdlType& sdlType)
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
	retVal = retVal && BeginHeaderClassFile(sdlType);
	retVal = retVal && BeginSourceClassFile(sdlType);

	return retVal;
}


bool CSdlClassCodeGeneratorComp::BeginHeaderClassFile(const CSdlType& sdlType)
{
	QTextStream ifStream(m_headerFilePtr.GetPtr());

	ifStream << QStringLiteral("#pragma once");
	FeedStream(ifStream, 3, false);

	QSet<QString> complexTypeList;
	if (IsTypeHasNonFundamentalTypes(sdlType, &complexTypeList)){

		// Add Qt types
		bool isQtCommentAdded = false;
		if (complexTypeList.contains(QStringLiteral("QByteArray"))){
			if (!isQtCommentAdded){
				ifStream << QStringLiteral("// Qt includes\n");
				isQtCommentAdded = true;
			}
			ifStream << QStringLiteral("#include <QtCore/QByteArray>\n");
		}
		if (complexTypeList.contains(QStringLiteral("QString"))){
			if (!isQtCommentAdded){
				ifStream << QStringLiteral("// Qt includes\n");
				isQtCommentAdded = true;
			}
			ifStream << QStringLiteral("#include <QtCore/QString>\n");
		}
		if (complexTypeList.contains(QStringLiteral("QList"))){
			if (!isQtCommentAdded){
				ifStream << QStringLiteral("// Qt includes\n");
				isQtCommentAdded = true;
			}
			ifStream << QStringLiteral("#include <QtCore/QList>\n");
		}

		// remove qt types from list
		complexTypeList.remove(QStringLiteral("QByteArray"));
		complexTypeList.remove(QStringLiteral("QString"));
		complexTypeList.remove(QStringLiteral("QList"));
		if (!complexTypeList.isEmpty()){
			FeedStream(ifStream, 2, false);
		}

		// Add user custom types
		bool isCustomGeneratedCommentAdded = false;
		for (QSet<QString>::const_iterator complexIter = complexTypeList.cbegin(); complexIter != complexTypeList.cend(); ++complexIter){
			if (!isCustomGeneratedCommentAdded){
				ifStream << QStringLiteral("// ") << m_argumentParserCompPtr->GetNamespace() << QStringLiteral(" includes\n");
				isCustomGeneratedCommentAdded = true;
			}

			const QString& complexTypeName = *complexIter;
			ifStream << QStringLiteral("#include \"") << complexTypeName << QStringLiteral(".h\"\n");
		}
	}
	FeedStream(ifStream, 2, false);


	// namespace begin
	QString namespaceString;
	const QString sdlNamespace = m_argumentParserCompPtr->GetNamespace();
	if (!sdlNamespace.isEmpty()){
		namespaceString = QStringLiteral("namespace ");
		namespaceString+= sdlNamespace;
		namespaceString += QStringLiteral("\n{");
	}

	if (!namespaceString.isEmpty()){
		ifStream << namespaceString;
		FeedStream(ifStream, 3, false);
	}

	// class begin
	ifStream << QStringLiteral("class C") << sdlType.GetName() << '\n';
	ifStream << QStringLiteral("{");
	FeedStream(ifStream, 1, false);

	ifStream << QStringLiteral("public:");
	FeedStream(ifStream, 1, false);

	// default constructor for defining primitive types
	if (IsTypeHasFundamentalTypes(sdlType)){
		ifStream << QStringLiteral("\tC") << sdlType.GetName() << QStringLiteral("();\n");
		FeedStream(ifStream);
	}

	// defining member's access methods
	for (const CSdlField& sdlField: sdlType.GetFields()){
		ifStream << GenerateAccessMethods(sdlField);
		FeedStream(ifStream);
	}
	ifStream.flush();

	return true;
}


bool CSdlClassCodeGeneratorComp::EndHeaderClassFile(const CSdlType& sdlType)
{
	QTextStream ifStream(m_headerFilePtr.GetPtr());

	// defining class members
	ifStream << QStringLiteral("private:\n");
	for (const CSdlField& sdlField: sdlType.GetFields()){
		ifStream << '\t' << ConvertType(sdlField) << " m_" << GetDecapitalizedValue(sdlField.GetId()) << ";\n";
	}
	ifStream.flush();

	return true;
}


bool CSdlClassCodeGeneratorComp::BeginSourceClassFile(const CSdlType& sdlType)
{
	QTextStream ifStream(m_sourceFilePtr.GetPtr());

	// include section
	ifStream << QStringLiteral("#include \"C");
	ifStream << sdlType.GetName() << QStringLiteral(".h\"");
	FeedStream(ifStream, 3);

	// namespace begin
	QString namespaceString;
	const QString sdlNamespace = m_argumentParserCompPtr->GetNamespace();
	if (!sdlNamespace.isEmpty()){
		namespaceString = QStringLiteral("namespace ");
		namespaceString+= sdlNamespace;
		namespaceString += QStringLiteral("\n{");
	}

	if (!namespaceString.isEmpty()){
		ifStream << namespaceString;
		FeedStream(ifStream, 3);
	}

	// default constructor implementation
	if (IsTypeHasFundamentalTypes(sdlType)){
		ifStream << ConvertType(sdlType.GetName()) << QStringLiteral("::") << ConvertType(sdlType.GetName());
		ifStream << QStringLiteral("()\n:");
		bool isAdded = false;

		SdlFieldList sdlFieldList = sdlType.GetFields();

		for (int i = 0; i < sdlFieldList.count(); ++i){
			CSdlField& sdlField = sdlFieldList[i];
			bool isComplex = true;
			bool isArray = true;
			const QString convertedType = ConvertType(sdlField, nullptr, &isComplex, &isArray);
			if (isComplex || isArray){
				// we need to initialize only fundamental types
				continue;
			}

			if (isAdded){
				ifStream << QStringLiteral(",\n");
			}

			ifStream << '\t' << "m_" << GetDecapitalizedValue(sdlField.GetId()) << '\(';

			if (convertedType == QStringLiteral("int")){
				ifStream << QStringLiteral("0");
			}
			else if (convertedType == QStringLiteral("float")){
				ifStream << QStringLiteral("0.00f");
			}
			else if (convertedType == QStringLiteral("double")){
				ifStream << QStringLiteral("0.00");
			}
			else if (convertedType == QStringLiteral("bool")){
				ifStream << QStringLiteral("false");
			}
			ifStream << (')');
			isAdded = true;
		}

		ifStream << QStringLiteral("\n{\n}");
		FeedStream(ifStream, 2);
	}

	// access methods implementation
	for (const CSdlField& sdlField: sdlType.GetFields()){
		ifStream << GenerateAccessMethodsImpl(sdlType.GetName(), sdlField);
	}

	return true;
}


bool CSdlClassCodeGeneratorComp::EndClassFiles(const CSdlType& sdlType)
{
	QString namespaceString;
	// end of namespace
	const QString sdlNamespace = m_argumentParserCompPtr->GetNamespace();
	if (!sdlNamespace.isEmpty()){
		namespaceString += QStringLiteral("} // namespace ");
		namespaceString += sdlNamespace;
	}

	EndHeaderClassFile(sdlType);
	QTextStream headerStream(m_headerFilePtr.GetPtr());
	headerStream << QStringLiteral("};");
	FeedStream(headerStream, 3, false);
	if (!namespaceString.isEmpty()){
		headerStream << namespaceString;
		FeedStream(headerStream, 2, false);
	}
	headerStream.flush();

	QTextStream sourceStream(m_sourceFilePtr.GetPtr());
	if (!namespaceString.isEmpty()){
		sourceStream << namespaceString;
		FeedStream(sourceStream, 2);
	}
	sourceStream.flush();

	CloseFiles();

	return true;
}


void CSdlClassCodeGeneratorComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	I_CRITICAL();

	m_headerFilePtr->remove();
	m_sourceFilePtr->remove();
}


QString CSdlClassCodeGeneratorComp::GenerateAccessMethods(const CSdlField& sdlField, uint indents, bool generateGetter, bool generateSetter)
{
	QString retVal;

	if (generateGetter){
		FeedLineHorizontally(retVal, indents);

		retVal += ConvertType(sdlField);
		retVal += QStringLiteral(" Get") + GetCapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral("() const;\n");
	}

	if (generateSetter){
		FeedLineHorizontally(retVal, indents);

		retVal += QStringLiteral("void Set") + GetCapitalizedValue(sdlField.GetId());
		retVal += '(';

		bool isCustom = false;
		const QString convertedType = ConvertType(sdlField, &isCustom);

		bool isComplex = sdlField.IsArray() ||
					sdlField.GetType() == QStringLiteral("String") ||
					sdlField.GetType() == QStringLiteral("ID") ||
					isCustom;

		if (isComplex){
			retVal += QStringLiteral("const ");
		}

		retVal += convertedType;

		if (isComplex){
			retVal += '&';
		}

		retVal += ' ';
		retVal += GetDecapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral(");\n");
	}

	return retVal;
}


QString CSdlClassCodeGeneratorComp::GenerateAccessMethodsImpl(const QString className, const CSdlField& sdlField, uint indents, bool generateGetter, bool generateSetter)
{
	QString retVal;

	if (generateGetter){
		FeedLineHorizontally(retVal, indents);

		retVal += ConvertType(sdlField);
		retVal += QStringLiteral(" C") + className + QStringLiteral("::");
		retVal += QStringLiteral("Get") + GetCapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral("() const\n{\n");
		FeedLineHorizontally(retVal, indents + 1);
		retVal += QStringLiteral("return m_");
		retVal += GetDecapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral(";\n}\n\n\n");
	}

	if (generateSetter){
		FeedLineHorizontally(retVal, indents);

		// name of method
		retVal += QStringLiteral("void");
		retVal += QStringLiteral(" C") + className + QStringLiteral("::");
		retVal += QStringLiteral("Set") + GetCapitalizedValue(sdlField.GetId());
		retVal += '(';

		bool isCustom = false;
		const QString convertedType = ConvertType(sdlField, &isCustom);

		bool isComplex = sdlField.IsArray() ||
					sdlField.GetType() == QStringLiteral("String") ||
					sdlField.GetType() == QStringLiteral("ID") ||
					isCustom;

		if (isComplex){
			retVal += QStringLiteral("const ");
		}

		retVal += convertedType;

		if (isComplex){
			retVal += '&';
		}

		retVal += ' ';
		retVal += GetDecapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral(")\n{\n");

		// impl of method
		FeedLineHorizontally(retVal, indents + 1);
		retVal += QStringLiteral("if (");
		retVal += GetDecapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral(" != m_");
		retVal += GetDecapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral("){\n");
		FeedLineHorizontally(retVal, indents + 2);
		retVal += QStringLiteral("m_");
		retVal += GetDecapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral(" = ");
		retVal += GetDecapitalizedValue(sdlField.GetId());
		retVal += QStringLiteral(";\n");
		FeedLineHorizontally(retVal, indents + 1);
		retVal += QStringLiteral("}\n");

		retVal += QStringLiteral("}\n\n\n");
	}

	return retVal;
}


QString CSdlClassCodeGeneratorComp::ConvertType(const CSdlField& sdlField, bool* isCustomPtr, bool* isComplexPtr, bool* isArrayPtr) const
{
	QString retVal;
	if (sdlField.IsArray()){
		retVal += QStringLiteral("QList<");
		if (isArrayPtr != nullptr){
			*isArrayPtr = true;
		}
	}
	else {
		if (isArrayPtr != nullptr){
			*isArrayPtr = false;
		}
	}

	retVal += ConvertType(sdlField.GetType(), isCustomPtr, isComplexPtr);

	if (sdlField.IsArray()){
		retVal += QStringLiteral(">");
	}

	return retVal;
}


QString CSdlClassCodeGeneratorComp::ConvertType(const QString& sdlType, bool* isCustomPtr, bool* isComplexPtr) const
{
	// A signed 32‐bit integer
	if (sdlType == QStringLiteral("Int") || sdlType == QStringLiteral("Integer")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("int");
	}

	// A signed double-precision floating-point value
	if (sdlType == QStringLiteral("Float")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("float");
	}

	// A signed double-precision floating-point value
	if (sdlType == QStringLiteral("Double")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("double");
	}

	// A UTF‐8 character sequence
	if (sdlType == QStringLiteral("String")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = true;
		}

		return QStringLiteral("QString");
	}

	// true or false
	if (sdlType == QStringLiteral("Boolean") || sdlType == QStringLiteral("Bool")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("bool");
	}

	// (serialized as a String): A unique identifier that's often used to refetch an object or as the key for a cache.
	if (sdlType == QStringLiteral("ID")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = true;
		}

		return QStringLiteral("QByteArray");
	}

	// custom type (another class)
	if (isCustomPtr != nullptr){
		*isCustomPtr = true;
	}
	if (isComplexPtr != nullptr){
		*isComplexPtr = true;
	}
	
	return QStringLiteral("C") + sdlType;
}


void CSdlClassCodeGeneratorComp::FeedStream(QTextStream& stream, uint lines, bool flush) const
{
	for (uint i = 0; i < lines; ++i){
		stream << '\n';
	}

	if (flush){
		stream.flush();
	}
}


void CSdlClassCodeGeneratorComp::FeedLineHorizontally(QString& line, uint indents, char indentDelimiter) const
{
	for (uint i = 0; i < indents; ++i){
		line += indentDelimiter;
	}
}


QString CSdlClassCodeGeneratorComp::GetCapitalizedValue(const QString& inputValue) const
{
	if (inputValue.isEmpty()){
		return inputValue;
	}

	QString retVal = inputValue;
	retVal[0] = retVal[0].toUpper();

	return retVal;
}


QString CSdlClassCodeGeneratorComp::GetDecapitalizedValue(const QString& inputValue) const
{
	if (inputValue.isEmpty()){
		return inputValue;
	}

	QString retVal = inputValue;
	retVal[0] = retVal[0].toLower();

	return retVal;
}


bool CSdlClassCodeGeneratorComp::IsTypeHasFundamentalTypes(const CSdlType& sdlType, QSet<QString>* foundTypesPtr) const
{
	bool retVal = false;

	for(const CSdlField& sdlField: sdlType.GetFields()){
		bool isComplex = true;
		bool isArray = false;
		const QString convertedType = ConvertType(sdlField, nullptr, &isComplex, &isArray);
		isComplex = isComplex || isArray;

		if (foundTypesPtr != nullptr && !isComplex){
			foundTypesPtr->insert(convertedType);
		}

		retVal = retVal || !isComplex;
	}

	return retVal;
}


bool CSdlClassCodeGeneratorComp::IsTypeHasNonFundamentalTypes(const CSdlType& sdlType, QSet<QString>* foundTypesPtr) const
{
	bool retVal = false;

	for(const CSdlField& sdlField: sdlType.GetFields()){
		bool isComplex = false;
		bool isArray = false;
		ConvertType(sdlField, nullptr, &isComplex, &isArray);

		if (foundTypesPtr != nullptr && isComplex){
			foundTypesPtr->insert(ConvertType(sdlField.GetType()));
		}

		// add QList as non fundamental type (for includes)
		if (foundTypesPtr != nullptr && isArray){
			foundTypesPtr->insert(QStringLiteral("QList"));
		}

		retVal = retVal || isComplex;
	}

	return true;
}


} // namespace imtsdl

