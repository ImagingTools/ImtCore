#include "CSdlClassCodeGeneratorComp.h"


// C includes
#include <iostream>

// Qt includes
#include <QtCore/QDir>

//Acf includes
#include <istd/CSystem.h>
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>
#include <ifile/CFileNameParam.h>

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

	if (!m_argumentParserCompPtr->IsCppEnabled()){
		return retVal;
	}

	if (!m_baseClassExtenderCompPtr.IsValid()){
		SendCriticalMessage(0, "BaseClassExtender is not set");
		I_CRITICAL();

		return iproc::IProcessor::TS_INVALID;
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
			std::cout << QString(outputDirectoryPath + "/C" + sdlType.GetName() + ".h").toStdString() << std::endl;
			std::cout << QString(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp").toStdString() << std::endl;
		}

		return iproc::IProcessor::TS_OK;
	}

	SdlTypeList sdlTypeList = m_sdlTypeListCompPtr->GetSdlTypes();

	// first create all files with basic mathods
	for (const CSdlType& sdlType: sdlTypeList){
		m_headerFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".h"));
		m_sourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp"));

		if (!BeginClassFiles(sdlType)){
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

	QMap<QString, QString> baseClassList = m_argumentParserCompPtr->GetBaseClassList();
	if (!baseClassList.isEmpty()){

		for (const CSdlType& sdlType: sdlTypeList){
			iprm::CParamsSet paramsSet;

			ifile::CFileNameParam headerFileNameParam;
			headerFileNameParam.SetPath(outputDirectoryPath + "/C" + sdlType.GetName() + ".h");
			paramsSet.SetEditableParameter(QByteArrayLiteral("HeaderFile"), &headerFileNameParam);

			iprm::COptionsManager baseClassDirectivesList;
			for (auto iter = baseClassList.cbegin(); iter != baseClassList.cend(); ++iter){
				baseClassDirectivesList.InsertOption(iter.value(), iter.key().toUtf8());
			}

			int extendResult = m_baseClassExtenderCompPtr->DoProcessing(&paramsSet, &baseClassDirectivesList, nullptr);
			if (extendResult != TS_OK){
				SendErrorMessage(0, "Unable to extend");
				I_CRITICAL();

				return extendResult;
			}
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
		m_headerFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".h"));
		m_sourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlType.GetName() + ".cpp"));
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
	bool isQtCommentAdded = false;
	bool hasComplexTypes = m_argumentParserCompPtr->IsModificatorEnabled(s_variantMapModificatorArgumentName);
	hasComplexTypes = IsTypeHasNonFundamentalTypes(sdlType, &complexTypeList) || hasComplexTypes;

	if (hasComplexTypes){
		// Add Qt types
		if (complexTypeList.contains(QStringLiteral("QByteArray"))){
			if (!isQtCommentAdded){
				ifStream << QStringLiteral("// Qt includes");
				FeedStream(ifStream, 1, false);
				isQtCommentAdded = true;
			}
			ifStream << QStringLiteral("#include <QtCore/QByteArray>");
			FeedStream(ifStream, 1, false);
		}
		if (complexTypeList.contains(QStringLiteral("QString"))){
			if (!isQtCommentAdded){
				ifStream << QStringLiteral("// Qt includes");
				FeedStream(ifStream, 1, false);
				isQtCommentAdded = true;
			}
			ifStream << QStringLiteral("#include <QtCore/QString>");
			FeedStream(ifStream, 1, false);
		}
		if (complexTypeList.contains(QStringLiteral("QList"))){
			if (!isQtCommentAdded){
				ifStream << QStringLiteral("// Qt includes");
				FeedStream(ifStream, 1, false);
				isQtCommentAdded = true;
			}
			ifStream << QStringLiteral("#include <QtCore/QList>");
			FeedStream(ifStream, 1, false);
		}
		// if variant map is enabled we need to add QVariant and QVariantMap
		if (m_argumentParserCompPtr->IsModificatorEnabled(s_variantMapModificatorArgumentName)){
			if (!isQtCommentAdded){
				ifStream << QStringLiteral("// Qt includes");
				FeedStream(ifStream, 1, false);
				isQtCommentAdded = true;
			}
			ifStream << QStringLiteral("#include <QtCore/QVariant>");
			FeedStream(ifStream, 1, false);
			ifStream << QStringLiteral("#include <QtCore/QVariantMap>");
			FeedStream(ifStream, 1, false);
		}

		// remove qt types from list
		complexTypeList.remove(QStringLiteral("QByteArray"));
		complexTypeList.remove(QStringLiteral("QString"));
		complexTypeList.remove(QStringLiteral("QList"));
		if (!complexTypeList.isEmpty()){
			FeedStream(ifStream, 1, false);
		}

		// Add user custom types
		bool isCustomGeneratedCommentAdded = false;
		for (QSet<QString>::const_iterator complexIter = complexTypeList.cbegin(); complexIter != complexTypeList.cend(); ++complexIter){
			if (!isCustomGeneratedCommentAdded){
				ifStream << QStringLiteral("// ") << m_argumentParserCompPtr->GetNamespace() << QStringLiteral(" includes");
				FeedStream(ifStream, 1, false);
				isCustomGeneratedCommentAdded = true;
			}

			const QString& complexTypeName = *complexIter;
			ifStream << QStringLiteral("#include \"") << complexTypeName << QStringLiteral(".h\"");
			FeedStream(ifStream, 1, false);
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
		ifStream << QStringLiteral("\tC") << sdlType.GetName() << QStringLiteral("();");
		FeedStream(ifStream, 2);
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

	// Add compare operator
	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("bool operator==(const ");
	ifStream << 'C' << sdlType.GetName();
	ifStream << QStringLiteral("& other) const;");
	FeedStream(ifStream, 1, false);

	// Add invrted compare operator
	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("bool operator!=(const ");
	ifStream << 'C' << sdlType.GetName();
	ifStream << QStringLiteral("& other) const { return !(operator==(other)); }");
	FeedStream(ifStream, 2, false);

	// defining class members
	ifStream << QStringLiteral("private:");
	FeedStream(ifStream, 1, false);

	for (const CSdlField& sdlField: sdlType.GetFields()){
		ifStream << '\t' << ConvertType(sdlField) << " m_" << GetDecapitalizedValue(sdlField.GetId()) << ";";
		FeedStream(ifStream, 1, false);
	}

	if (m_argumentParserCompPtr->IsModificatorEnabled(s_variantMapModificatorArgumentName)){
		ifStream << QStringLiteral("\tQVariantMap ") << s_variantMapClassMemberName << ';';
	}

	FeedStream(ifStream);

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
				ifStream << ',';
				FeedStream(ifStream, 1, false);
			}

			ifStream << '\t' << "m_" << GetDecapitalizedValue(sdlField.GetId()) << '(';

			if (convertedType == QStringLiteral("int") ||
				convertedType == QStringLiteral("long") ||
				convertedType == QStringLiteral("long long"))
			{
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
		GenerateAccessMethodsImpl(ifStream, sdlType.GetName(), sdlField);
	}

	return true;
}


bool CSdlClassCodeGeneratorComp::EndSourceClassFile(const CSdlType& sdlType)
{
	QTextStream ifStream(m_sourceFilePtr.GetPtr());

	// Add compare operator
	// a)declare
	ifStream << QStringLiteral("bool ");
	ifStream << 'C' << sdlType.GetName();
	ifStream << QStringLiteral("::operator==(const ");
	ifStream << 'C' << sdlType.GetName();
	ifStream << QStringLiteral("& other) const\n{");
	FeedStream(ifStream, 1, false);

	// b)implement
	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("bool retVal = true;");
	FeedStream(ifStream, 1, false);
	for (const CSdlField& sdlField: sdlType.GetFields()){
		FeedStreamHorizontally(ifStream);
		ifStream << QStringLiteral("retVal = retVal && (m_");
		ifStream << GetDecapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral(" == other.m_");
		ifStream << GetDecapitalizedValue(sdlField.GetId());
		ifStream << QStringLiteral(");");
		FeedStream(ifStream, 1, false);
	}

	// c)complete
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("return retVal;");
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 3);

	return true;
}


bool CSdlClassCodeGeneratorComp::EndClassFiles(const CSdlType& sdlType)
{
	QString namespaceString;

	// finish header
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

	// add Q_DECLARE_METATYPE macro to use the type as a custom type in QVariant
	if (m_argumentParserCompPtr->IsModificatorEnabled(s_variantMapModificatorArgumentName)){
		FeedStream(headerStream, 1, false);
		headerStream << QStringLiteral("Q_DECLARE_METATYPE(");
		if (sdlNamespace.length() > 0){
			headerStream << sdlNamespace;
			headerStream << ':' << ':';
		}
		headerStream << 'C' << sdlType.GetName();
		headerStream << QStringLiteral(");");
	}
	FeedStream(headerStream, 3, true);

	// finish source
	EndSourceClassFile(sdlType);
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


QString CSdlClassCodeGeneratorComp::GenerateAccessMethods(
			const CSdlField& sdlField,
			uint indents,
			bool generateGetter,
			bool generateSetter,
			bool generateExistenceChecks)
{
	QString retVal;

	if (generateGetter){
		FeedLineHorizontally(retVal, indents);

		retVal += QStringLiteral("[[nodiscard]] ");
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

	if (generateExistenceChecks){
		if (m_argumentParserCompPtr->IsModificatorEnabled(s_variantMapModificatorArgumentName)){
			FeedLineHorizontally(retVal, indents);

			retVal += QStringLiteral("[[nodiscard]] bool Has");
			retVal += GetCapitalizedValue(sdlField.GetId());
			retVal += QStringLiteral("() const;\n");
		}
	}

	return retVal;
}


void CSdlClassCodeGeneratorComp::GenerateAccessMethodsImpl(
			QTextStream& stream,
			const QString className,
			const CSdlField& sdlField,
			uint indents,
			bool generateGetter,
			bool generateSetter,
			bool generateExistenceChecks)
{
	if (generateGetter){
		FeedStreamHorizontally(stream, indents);

		stream << ConvertType(sdlField);
		stream << QStringLiteral(" C") + className + QStringLiteral("::");
		stream << QStringLiteral("Get") + GetCapitalizedValue(sdlField.GetId());
		stream << QStringLiteral("() const\n{");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, indents + 1);
		stream << QStringLiteral("return m_");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(";\n}");
		FeedStream(stream, 3, false);
	}

	if (generateSetter){
		FeedStreamHorizontally(stream, indents);

		// name of method
		stream << QStringLiteral("void");
		stream << QStringLiteral(" C") + className + QStringLiteral("::");
		stream << QStringLiteral("Set") + GetCapitalizedValue(sdlField.GetId());
		stream << '(';

		bool isCustom = false;
		const QString convertedType = ConvertType(sdlField, &isCustom);

		bool isComplex = sdlField.IsArray() ||
					sdlField.GetType() == QStringLiteral("String") ||
					sdlField.GetType() == QStringLiteral("ID") ||
					isCustom;

		if (isComplex){
			stream << QStringLiteral("const ");
		}

		stream << convertedType;

		if (isComplex){
			stream << '&';
		}

		stream << ' ';
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(")\n{");
		FeedStream(stream, 1, false);

		// impl of method
		FeedStreamHorizontally(stream, indents + 1);
		stream << QStringLiteral("if (");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(" != m_");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(" || ");
		stream << FromVariantMapAccessString(sdlField);
		stream << QStringLiteral(".isNull()");
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, indents + 2);
		stream << QStringLiteral("m_");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(" = ");
		stream << GetDecapitalizedValue(sdlField.GetId());
		stream << QStringLiteral(";");
		FeedStream(stream, 1, false);

		if (m_argumentParserCompPtr->IsModificatorEnabled(s_variantMapModificatorArgumentName)){
			FeedStreamHorizontally(stream, indents + 2);
			if (isCustom && sdlField.IsArray()){
				stream << QStringLiteral("QVariantList temp") << GetCapitalizedValue(sdlField.GetId()) << QStringLiteral("List;");

				FeedStream(stream, 1, false);
				FeedStreamHorizontally(stream, indents + 2);
				stream << QStringLiteral("for (const auto& tempValue: std::as_const(");
				stream << GetDecapitalizedValue(sdlField.GetId());
				stream << QStringLiteral(")){");

				FeedStream(stream, 1, false);
				FeedStreamHorizontally(stream, indents + 3);
				stream << QStringLiteral("temp") << GetCapitalizedValue(sdlField.GetId()) << QStringLiteral("List");
				stream << QStringLiteral(" << QVariant::fromValue(tempValue);");

				FeedStream(stream, 1, false);
				FeedStreamHorizontally(stream, indents + 2);
				stream << '}';

				FeedStream(stream, 1, false);
				FeedStreamHorizontally(stream, indents + 2);
				stream << FromVariantMapAccessString(sdlField) << QStringLiteral(" = ");
				stream << QStringLiteral("temp") << GetCapitalizedValue(sdlField.GetId()) << QStringLiteral("List;");
			}
			else if (isCustom){
				stream << FromVariantMapAccessString(sdlField) << QStringLiteral(".setValue(");
				stream << GetDecapitalizedValue(sdlField.GetId()) << ')' << ';';
			}
			else {
				stream << FromVariantMapAccessString(sdlField) << QStringLiteral(" = ");
				stream << GetDecapitalizedValue(sdlField.GetId()) << ';';
			}
			FeedStream(stream, 1, false);
		}

		FeedStreamHorizontally(stream, indents + 1);
		stream << '}';
		FeedStream(stream, 1, false);

		stream << '}';
		FeedStream(stream, 3);
	}

	if (generateExistenceChecks){
		if (m_argumentParserCompPtr->IsModificatorEnabled(s_variantMapModificatorArgumentName)){
			FeedStreamHorizontally(stream, indents);

			stream << QStringLiteral("bool");
			stream << QStringLiteral(" C") + className + QStringLiteral("::");
			stream << QStringLiteral("Has");
			stream << GetCapitalizedValue(sdlField.GetId());
			stream << QStringLiteral("() const");
			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream, indents);
			stream << '{';

			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream, indents + 1);
			stream << QStringLiteral("return bool(!");
			stream << FromVariantMapAccessString(sdlField);
			stream << QStringLiteral(".isNull());");

			FeedStream(stream, 1, false);
			FeedStreamHorizontally(stream, indents);
			stream << '}';

			FeedStream(stream, 3, false);
		}
	}
}


} // namespace imtsdl

