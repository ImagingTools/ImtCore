#include "CSdlClassCodeGeneratorComp.h"


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

	SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes();
	for (const CSdlType& sdlType: sdlTypeList){
		m_headerFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".h"));
		m_sourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp"));
		BeginClassFiles(sdlType);

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

		EndClassFiles();
	}

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

	/// \todo optimize it (exclude extra) and add custom value's header
	ifStream << QStringLiteral("#include <QtCore/QByteArray>") << '\n';
	ifStream << QStringLiteral("#include <QtCore/QString>") << '\n';
	ifStream << QStringLiteral("#include <QtCore/QList>") << '\n';
	FeedStream(ifStream, 2, false);

	ifStream << QStringLiteral("class C") << sdlType.GetName() << '\n';
	ifStream << QStringLiteral("{");
	FeedStream(ifStream);

	// defining class members
	for (const CSdlField& sdlField: sdlType.GetFields()){
		ifStream << '\t' << ConvertType(sdlField) << " m_" << GetDecapitalizedValue(sdlField.GetId()) << "; \n";
	}
	FeedStream(ifStream);

	ifStream << QStringLiteral("public:\n");

	// default constructor for defining primitive types
	ifStream << QStringLiteral("\tC") << sdlType.GetName() << QStringLiteral("();\n");
	FeedStream(ifStream);

	// defining member's access methods
	for (const CSdlField& sdlField: sdlType.GetFields()){
		ifStream << GenerateAccessMethods(sdlField);
		FeedStream(ifStream);
	}

	// remove last line after access methods definition
	ifStream.seek(ifStream.pos() - 1);

	return true;
}


bool CSdlClassCodeGeneratorComp::BeginSourceClassFile(const CSdlType& sdlType)
{
	QTextStream ifStream(m_sourceFilePtr.GetPtr());

	// include section
	ifStream << QStringLiteral("#include \"C");
	ifStream << sdlType.GetName() << QStringLiteral(".h\"");
	FeedStream(ifStream, 3);

	// default constructor implementation

	// method implementation
	for (const CSdlField& sdlField: sdlType.GetFields()){
		ifStream << GenerateAccessMethodsImpl(sdlType.GetName(), sdlField);
	}

	return true;
}


bool CSdlClassCodeGeneratorComp::EndClassFiles()
{
	QTextStream headerStream(m_headerFilePtr.GetPtr());
	headerStream << QStringLiteral("};\n");
	headerStream.flush();

	QTextStream sourceStream(m_sourceFilePtr.GetPtr());
	sourceStream.flush();

	m_headerFilePtr->close();
	m_sourceFilePtr->close();

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
		retVal += QStringLiteral(" ){\n");
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


QString CSdlClassCodeGeneratorComp::ConvertType(const CSdlField& sdlField, bool* isCustomPtr) const
{
	QString retVal;
	if (sdlField.IsArray()){
		retVal += QStringLiteral("QList<");
	}

	retVal += ConvertType(sdlField.GetType(), isCustomPtr);

	if (sdlField.IsArray()){
		retVal += QStringLiteral(">");
	}

	return retVal;
}


QString CSdlClassCodeGeneratorComp::ConvertType(const QString& sdlType, bool* isCustomPtr) const
{
	// A signed 32‐bit integer
	if (sdlType == QStringLiteral("Int") || sdlType == QStringLiteral("Integer")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("int");
	}

	// A signed double-precision floating-point value
	if (sdlType == QStringLiteral("Float")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("float");
	}

	// A signed double-precision floating-point value
	if (sdlType == QStringLiteral("Double")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("double");
	}

	// A UTF‐8 character sequence
	if (sdlType == QStringLiteral("String")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("QString");
	}

	// true or false
	if (sdlType == QStringLiteral("Boolean") || sdlType == QStringLiteral("Bool")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("bool");
	}

	// (serialized as a String): A unique identifier that's often used to refetch an object or as the key for a cache.
	if (sdlType == QStringLiteral("ID")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}

		return QStringLiteral("QByteArray");
	}

	// custom type (another class)
	if (isCustomPtr != nullptr){
		*isCustomPtr = true;
	}
	
	return sdlType;
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


} // namespace imtsdl

