#include "CGqlWrapClassCodeGeneratorComp.h"


// C includes
#include <iostream>

// Qt includes
#include <QtCore/QDir>

//Acf includes
#include <istd/CSystem.h>

// imtsdl includes
#include <imtsdl/CSdlRequest.h>


namespace imtsdl
{


int CGqlWrapClassCodeGeneratorComp::DoProcessing(
			const iprm::IParamsSet* paramsPtr,
			const istd::IPolymorphic* inputPtr,
			istd::IChangeable* outputPtr,
			ibase::IProgressManager* progressManagerPtr)
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlRequestListCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());

	int retVal = iproc::IProcessor::TS_OK;

	if (!m_argumentParserCompPtr->IsGqlEnabled()){
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

	SdlRequestList sdlRequestList = m_sdlRequestListCompPtr->GetRequests();
	if (m_argumentParserCompPtr->IsDependenciesMode()){
		for (const CSdlRequest& sdlRequest: sdlRequestList){
			std::cout << QString(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.h").toStdString() << std::endl;
			std::cout << QString(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.cpp").toStdString() << std::endl;
		}

		return iproc::IProcessor::TS_OK;
	}

	for (const CSdlRequest& sdlRequest: sdlRequestList){
		m_headerFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.h"));
		m_sourceFilePtr.SetPtr(new QFile(outputDirectoryPath + "/C" + sdlRequest.GetName() + "GqlRequest.cpp"));

		if (!ProcessFiles(sdlRequest)){
			SendErrorMessage(0, QString("Unable to begin files"));
			I_CRITICAL();

			return iproc::IProcessor::TS_INVALID;
		}

		if (!CloseFiles()){
			SendErrorMessage(0, QString("Unable to finalize files"));
			I_CRITICAL();

			return iproc::IProcessor::TS_INVALID;
		}
	}

	return retVal;
}


// private methods

bool CGqlWrapClassCodeGeneratorComp::CloseFiles()
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


bool CGqlWrapClassCodeGeneratorComp::ProcessFiles(const CSdlRequest& sdlRequest)
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
	retVal = retVal && ProcessHeaderClassFile(sdlRequest);
	retVal = retVal && ProcessSourceClassFile(sdlRequest);

	return retVal;
}


bool CGqlWrapClassCodeGeneratorComp::ProcessHeaderClassFile(const CSdlRequest& sdlRequest)
{
	QTextStream ifStream(m_headerFilePtr.GetPtr());

	// preprocessor's section
	ifStream << QStringLiteral("#pragma once");
	FeedStream(ifStream, 3, false);

	QSet<QString> complexTypeList;
	bool hasComplexTypes = m_argumentParserCompPtr->IsModificatorEnabled(s_variantMapModificatorArgumentName);
	hasComplexTypes = IsTypeHasNonFundamentalTypes(sdlRequest, &complexTypeList) || hasComplexTypes;

	if (hasComplexTypes){
		bool isQtCommentAdded = false;
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

		// Add user's custom types
		// first add include comment
		if (!complexTypeList.isEmpty()){
			ifStream << QStringLiteral("// ") << m_argumentParserCompPtr->GetNamespace() << QStringLiteral(" includes");
			FeedStream(ifStream, 1, false);
		}
		// then add inclides

		/// \todo resolve path in the future
		for (QSet<QString>::const_iterator complexIter = complexTypeList.cbegin(); complexIter != complexTypeList.cend(); ++complexIter){
			const QString& complexTypeName = *complexIter;
			ifStream << QStringLiteral("#include \"") << complexTypeName << QStringLiteral(".h\"");
			FeedStream(ifStream, 1, false);
		}
	}

	// add imtgql includes
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("//imtgql includes");
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("#include <imtgql/CGqlRequest.h>");
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("#include <imtgql/CGqlObject.h>");
	FeedStream(ifStream, 3, false);

	// namespace begin
	const QString sdlNamespace = m_argumentParserCompPtr->GetNamespace();
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("namespace ");
		ifStream <<  sdlNamespace;
		FeedStream(ifStream, 1, false);
		ifStream <<  QStringLiteral("{");
		FeedStream(ifStream, 1, false);
	}

	// RequestInfo struct Begin
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("struct ");
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestInfo");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);

	// RequestInfo struct props
	GenerateFieldRequestInfo(ifStream, sdlRequest.GetOutputArgument());

	// RequestInfo struct End
	ifStream << '}' << ';';
	FeedStream(ifStream, 1, false);

	// RequestArguments struct Begin
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("struct ");
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestArguments");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);

	// RequestArguments struct props
	SdlFieldList requestArguments = sdlRequest.GetInputArguments();
	for (const CSdlField& sdlField: requestArguments){
		FeedStreamHorizontally(ifStream, 1);
		ifStream << ConvertType(sdlField) << ' ' << sdlField.GetId() << ';';
		FeedStream(ifStream, 1, false);
	}

	// RequestArguments struct End
	ifStream << '}' << ';';
	FeedStream(ifStream, 1, false);


	// main GQL-wrap class begin
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("class C") << sdlRequest.GetName() << QStringLiteral("GqlRequest");
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("{");
	FeedStream(ifStream, 1, false);

	// public section
	ifStream << QStringLiteral("public:");
	FeedStream(ifStream, 1, false);

	// default constructor with GraphQL request
	FeedStreamHorizontally(ifStream);
	ifStream << 'C' << sdlRequest.GetName() << QStringLiteral("GqlRequest (const imtgql::CGqlRequest& gqlRequest);");
	FeedStream(ifStream, 1, false);

	// validation method
	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("bool IsValid() const;");
	FeedStream(ifStream, 1, false);

	// GetRequestedArguments method
	FeedStreamHorizontally(ifStream);
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestArguments GetRequestedArguments() const;");
	FeedStream(ifStream, 1, false);

	// GetRequestInfo method
	FeedStreamHorizontally(ifStream);
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestInfo GetRequestInfo() const;");

	FeedStream(ifStream, 1, false);

	// private section
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("private:");
	FeedStream(ifStream, 1, false);

	// validation property
	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("bool m_isValid;");
	FeedStream(ifStream, 1, false);

	// Arguments property
	FeedStreamHorizontally(ifStream);
	ifStream << GetCapitalizedValue(sdlRequest.GetName()) << QStringLiteral("RequestArguments m_requestedArguments;");
	FeedStream(ifStream, 1, false);

	// Info property
	FeedStreamHorizontally(ifStream);
	ifStream << GetCapitalizedValue(sdlRequest.GetName()) << QStringLiteral("RequestInfo m_requestInfo;");
	FeedStream(ifStream, 1, false);

	// main GQL-wrap end of class
	ifStream << QStringLiteral("};");
	FeedStream(ifStream, 1, false);

	// end of namespace
	FeedStream(ifStream, 2, false);
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("} // namespace ");
		ifStream << sdlNamespace;
	}
	FeedStream(ifStream, 1, true);

	return true;
}


bool CGqlWrapClassCodeGeneratorComp::ProcessSourceClassFile(const CSdlRequest& sdlRequest)
{
	QTextStream ifStream(m_sourceFilePtr.GetPtr());

	// include section
	ifStream << QStringLiteral("#include \"C");
	ifStream << sdlRequest.GetName() << QStringLiteral("GqlRequest.h\"");
	FeedStream(ifStream, 3);

	// namespace begin
	const QString sdlNamespace = m_argumentParserCompPtr->GetNamespace();
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("namespace ");
		ifStream << sdlNamespace;
		FeedStream(ifStream, 1, false);
		ifStream << '{';
		FeedStream(ifStream, 1, false);
	}

	const QString className = 'C' + sdlRequest.GetName() + QStringLiteral("GqlRequest");
	// implementation of methods

	// MAIN CONSTRUCTOR + PARSING
	FeedStream(ifStream, 2, false);
	ifStream << className << ':' << ':' << className;
	ifStream << QStringLiteral("(const imtgql::CGqlRequest& gqlRequest)");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);
	GenerateRequestParsing(ifStream, sdlRequest, 1);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// IsValid
	FeedStream(ifStream, 2, false);
	ifStream << className << ':' << ':' ;
	ifStream << QStringLiteral("IsValid() const");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream);
	ifStream << "return m_isValid;";
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// GetRequestedArguments
	FeedStream(ifStream, 2, false);
	ifStream << className << ':' << ':' ;
	ifStream << QStringLiteral("GetRequestedArguments() const");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream);
	ifStream << "return m_requestedArguments;";
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// GetRequestInfo
	FeedStream(ifStream, 2, false);
	ifStream << className << ':' << ':' ;
	ifStream << QStringLiteral("GetRequestInfo() const");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream);
	ifStream << "return m_requestInfo;";
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// end of namespace
	FeedStream(ifStream, 2, false);
	if (!sdlNamespace.isEmpty()){
		ifStream << QStringLiteral("} // namespace ");
		ifStream << sdlNamespace;
	}
	FeedStream(ifStream, 1, true);

	return true;
}


void CGqlWrapClassCodeGeneratorComp::GenerateFieldRequestInfo(
			QTextStream& stream,
			const CSdlField& sdlField,
			uint hIndents,
			bool createStructDefinition)
{
	CSdlType sdlType;
	[[maybe_unused]]bool isTypeFound = GetSdlTypeForField(sdlField, m_sdlTypeListCompPtr->GetSdlTypes(), sdlType);
	Q_ASSERT(isTypeFound);

	if (createStructDefinition ){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("struct ");
		stream << sdlField.GetType();
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, hIndents);
		stream << '{';
		FeedStream(stream, 1, false);
	}
	// first add general prop
	FeedStreamHorizontally(stream, hIndents + createStructDefinition);
	stream << QStringLiteral("bool isRequested = true;");
	FeedStream(stream, 1, false);

	// then add general props
	SdlFieldList customTypes;
	for (const CSdlField& fieldFromType: sdlType.GetFields()){
		FeedStreamHorizontally(stream, hIndents + createStructDefinition);
		stream << QStringLiteral("bool is") << GetCapitalizedValue(fieldFromType.GetId()) << QStringLiteral("Requested = ");
/// \todo fix it in \c GenerateRequestParsing
#if SDL__PARSING_OF_REQUESTED_FIELDS_FIXED
		if (fieldFromType.IsRequired()){
			stream << QStringLiteral("true");
		}
		else {
			stream << QStringLiteral("false");
		}
#else
		stream << QStringLiteral("true");
#endif
		stream << ';';
		FeedStream(stream, 1, false);

		// add custom types for nested structs creation
		bool isCustom = false;
		ConvertType(fieldFromType, &isCustom);
		if (isCustom) {
			customTypes << fieldFromType;
		}
	}

	// and finally create all custom types;
	for (const CSdlField& customType: customTypes){
		GenerateFieldRequestInfo(stream, customType, hIndents + 1, true);
	}

	if (createStructDefinition ){
		FeedStreamHorizontally(stream, hIndents);
		stream << '}' << ' ';
		stream << sdlField.GetId() << ';';
		FeedStream(stream, 1, false);
	}
}


void CGqlWrapClassCodeGeneratorComp::GenerateRequestParsing(
			QTextStream& ifStream,
			const CSdlRequest& sdlRequest,
			uint hIndents)
{
	FeedStreamHorizontally(ifStream, hIndents);
	ifStream << "/// \\todo GenerateRequestParsing";

	FeedStream(ifStream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AbortCurrentProcessing()
{
	m_headerFilePtr->close();
	m_sourceFilePtr->close();

	I_CRITICAL();

	m_headerFilePtr->remove();
	m_sourceFilePtr->remove();
}


} // namespace imtsdl

