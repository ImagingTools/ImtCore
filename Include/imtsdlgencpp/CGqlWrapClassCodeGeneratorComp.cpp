// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CGqlWrapClassCodeGeneratorComp.h"


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QTextStream>

// Acf includes
#include <iprm/CParamsSet.h>
#include <iprm/COptionsManager.h>

// ImtCore includes
#include <imtsdl/CSimpleFileJoinerComp.h>
#include <imtsdl/CSdlRequest.h>


namespace imtsdlgencpp
{


// public methods

bool CGqlWrapClassCodeGeneratorComp::ProcessEntry (
	const imtsdl::CSdlEntryBase& sdlEntry,
	QIODevice* headerDevicePtr,
	QIODevice* sourceDevicePtr,
	const iprm::IParamsSet* paramsPtr) const
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlRequestListCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_dependentSchemaListCompPtr.IsValid());

	const auto* sdlRequestPtr = dynamic_cast<const imtsdl::CSdlRequest*>(&sdlEntry);
	if (sdlRequestPtr == nullptr || (headerDevicePtr == nullptr && sourceDevicePtr == nullptr)){
		I_CRITICAL();

		return false;
	}

	bool retVal = true;
	if (headerDevicePtr != nullptr){
		retVal = retVal && ProcessHeaderClassFile(*sdlRequestPtr, headerDevicePtr, paramsPtr);
	}
	if (sourceDevicePtr != nullptr){
		retVal = retVal && ProcessSourceClassFile(*sdlRequestPtr, sourceDevicePtr, paramsPtr);
	}

	return retVal;
}


// reimplemented (ICxxFileProcessor)

QList<imtsdl::IncludeDirective> CGqlWrapClassCodeGeneratorComp::GetIncludeDirectives() const
{
	static QList<imtsdl::IncludeDirective> retVal = {
		CreateImtDirective("<imtservergql/CPermissibleGqlRequestHandlerComp.h>")
	};

	return retVal;
}


// private methods

bool CGqlWrapClassCodeGeneratorComp::ProcessHeaderClassFile(const imtsdl::CSdlRequest& sdlRequest, QIODevice* headerDevicePtr, const iprm::IParamsSet* /*paramsPtr*/) const
{
	QTextStream ifStream(headerDevicePtr);

	const QString sdlNamespace = GetNamespaceFromParamsOrArguments(
		m_customSchemaParamsCompPtr,
		m_argumentParserCompPtr,
		false);

	// RequestInfo struct Begin
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("struct ");
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestInfo");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);

	imtsdl::CSdlField outputArgument = sdlRequest.GetOutputArgument();
	// RequestInfo struct props
	const bool isRequestInfoCreated = GenerateFieldRequestInfo(ifStream, outputArgument);
	if (!isRequestInfoCreated){
		SendErrorMessage(0, QString("Unable to create request info for request %1").arg(sdlRequest.GetName()));

		return false;
	}

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
	imtsdl::SdlFieldList requestArguments = sdlRequest.GetInputArguments();
	for (const imtsdl::CSdlField& sdlField: requestArguments){
		CStructNamespaceConverter structNameConverter(sdlField, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);

		FeedStreamHorizontally(ifStream, 1);
		ifStream << structNameConverter.GetString();
		ifStream << ' ' << sdlField.GetId() << ';';
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

	AddMethodDeclarations(ifStream, sdlRequest);

	// private section
	FeedStream(ifStream, 1, false);
	ifStream << QStringLiteral("private:");
	FeedStream(ifStream, 1, false);

	AddClassProperties(ifStream, sdlRequest);

	// main GQL-wrap end of class
	ifStream << QStringLiteral("};");
	FeedStream(ifStream, 3, false);

	return true;
}


bool CGqlWrapClassCodeGeneratorComp::ProcessSourceClassFile(const imtsdl::CSdlRequest& sdlRequest, QIODevice* sourceDevicePtr, const iprm::IParamsSet* /*paramsPtr*/) const
{
	QTextStream ifStream(sourceDevicePtr);

	const QString className = 'C' + sdlRequest.GetName() + QStringLiteral("GqlRequest");
	// implementation of methods

	// CommandId method
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("QByteArray ");
	ifStream << className << ':' << ':';
	ifStream << QStringLiteral("GetCommandId()");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral("return QByteArrayLiteral(\"");
	ifStream << sdlRequest.GetName();
	ifStream << '"' << ')' << ';';
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// setup GQL method
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("bool ");
	ifStream << className << ':' << ':';
	ifStream << QStringLiteral("SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const ");
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestArguments& requestArguments, const ");
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestInfo& /*requestInfo*/)");
	FeedStream(ifStream, 1, false);

	ifStream << '{';
	FeedStream(ifStream, 1, false);

	GenerateRequestSetup(ifStream, sdlRequest);
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// MAIN CONSTRUCTOR + PARSING
	FeedStream(ifStream, 2, false);
	ifStream << className << ':' << ':' << className;
	ifStream << QStringLiteral("(const ::imtgql::CGqlRequest& gqlRequest, bool optRead)");
	FeedStream(ifStream, 1, false);

	FeedStreamHorizontally(ifStream);
	ifStream << QStringLiteral(": m_isValid(true)");
	FeedStream(ifStream, 1, false);

	ifStream << '{';
	FeedStream(ifStream, 1, false);

	if (sdlRequest.GetInputArguments().isEmpty()){
		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("Q_UNUSED(optRead)");
		FeedStream(ifStream, 1, false);

		FeedStreamHorizontally(ifStream, 1);
		ifStream << QStringLiteral("m_isValid = true;");
		FeedStream(ifStream, 1, false);
	}

	GenerateRequestParsing(ifStream, sdlRequest, 1);

	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// IsValid
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("bool ");
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

	// GQL context
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("const ::imtgql::IGqlContext* ");
	ifStream << className << ':' << ':' ;
	ifStream << QStringLiteral("GetRequestContext() const");
	FeedStream(ifStream, 1, false);
	ifStream << '{';
	FeedStream(ifStream, 1, false);
	FeedStreamHorizontally(ifStream);
	ifStream << "return m_gqlContextPtr;";
	FeedStream(ifStream, 1, false);
	ifStream << '}';
	FeedStream(ifStream, 1, false);

	// GetRequestedArguments
	FeedStream(ifStream, 2, false);
	ifStream << QStringLiteral("const ");
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestArguments& ");
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
	ifStream << GetCapitalizedValue(sdlRequest.GetName());
	ifStream << QStringLiteral("RequestInfo ");
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

	return true;
}


bool CGqlWrapClassCodeGeneratorComp::GenerateFieldRequestInfo(
	QTextStream& stream,
	const imtsdl::CSdlField& sdlField,
	uint hIndents,
	bool createStructDefinition) const
{
	bool isUnion = false;
	imtsdl::CSdlType sdlType;
	bool isTypeFound = GetSdlTypeForField(sdlField, m_sdlTypeListCompPtr->GetSdlTypes(false), sdlType);
	if (!isTypeFound){
		imtsdl::CSdlUnion sdlUnion;
		isTypeFound = GetSdlUnionForField(sdlField, m_sdlUnionListCompPtr->GetUnions(false), sdlUnion);
		isUnion = isTypeFound;
	}

	if (!isTypeFound){
		SendErrorMessage(0, QString("Field %1 is not custom. Only cutom field allowed").arg(sdlField.GetType()));

		return false;
	}

	if (createStructDefinition ){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("struct ");
		stream << sdlField.GetId();
		stream << QStringLiteral("RequestInfo");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, hIndents);
		stream << '{';
		FeedStream(stream, 1, false);
	}

	// then add general props
	imtsdl::SdlFieldList customTypes;
	if (isUnion){
		/// \todo implement it
	}
	else{
		for (const imtsdl::CSdlField& fieldFromType: sdlType.GetFields()){
			FeedStreamHorizontally(stream, hIndents + createStructDefinition);
			stream << QStringLiteral("bool is") << GetCapitalizedValue(fieldFromType.GetId()) << QStringLiteral("Requested = ");
			stream << QStringLiteral("true");
			stream << ';';
			FeedStream(stream, 1, false);

			// add custom types for nested structs creation
			bool isCustom = false;
			bool isEnum = false;
			ConvertTypeOrEnumOrUnion(fieldFromType,
									 m_sdlEnumListCompPtr->GetEnums(false),
									 m_sdlUnionListCompPtr->GetUnions(false),
									 &isCustom,
									 nullptr,
									 nullptr,
									 &isEnum,
									 &isUnion);
			if (isCustom && !isEnum && !isUnion){
				customTypes << fieldFromType;
			}
		}
	}

	// and finally create all custom types;
	for (const imtsdl::CSdlField& customType: customTypes){
		if (customType.GetType() != sdlField.GetType()){
			const bool isRequestInfoCreated = GenerateFieldRequestInfo(stream, customType, hIndents + 1, true);
			if (!isRequestInfoCreated){
				SendErrorMessage(0, QString("Unable to create request info for type %1").arg(customType.GetType()));

				return false;
			}
		}
	}

	if (createStructDefinition ){
		FeedStreamHorizontally(stream, hIndents);
		stream << '}' << ' ';
		stream << sdlField.GetId() << ';';
		FeedStream(stream, 1, false);
	}

	return true;
}


void CGqlWrapClassCodeGeneratorComp::GenerateRequestParsing(
	QTextStream& stream,
	const imtsdl::CSdlRequest& sdlRequest,
	uint hIndents) const
{
	// Get context
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("m_gqlContextPtr = gqlRequest.GetRequestContext();");
	FeedStream(stream, 2, false);

	// declare and read gql protocol version
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const QByteArray protocolVersion = gqlRequest.GetProtocolVersion();");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// reading input arguments");
	FeedStream(stream, 1, false);

	for (const imtsdl::CSdlField& field: sdlRequest.GetInputArguments()){
		AddFieldReadFromRequestCode(stream, field);
	}
	FeedStream(stream, 1, false);

	// fill requested fields
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// reading requested fields");
	FeedStream(stream, 1, false);

	// Get requested fields from request

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const imtgql::CGqlFieldObject* requestedFieldsObjectPtr = nullptr;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!gqlRequest.GetFields().GetFieldIds().isEmpty()){");
	FeedStream(stream, 1, false);

	// get top-level request. Usually it always one => we need only first
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("requestedFieldsObjectPtr = gqlRequest.GetFields().GetFieldArgumentObjectPtr(gqlRequest.GetFields().GetFieldIds().constFirst());");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (requestedFieldsObjectPtr != nullptr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("const QByteArrayList requestedIds = requestedFieldsObjectPtr->GetFieldIds();");
	FeedStream(stream, 1, false);

	GenerateRequestedFieldsParsing(
		stream,
		sdlRequest.GetOutputArgument(),
		QStringLiteral("requestedIds"),
		QStringLiteral("requestedFieldsObjectPtr"),
		QString(),
		hIndents + 2);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

}


void CGqlWrapClassCodeGeneratorComp::GenerateRequestedFieldsParsing(
	QTextStream& stream,
	const imtsdl::CSdlField& sdlField,
	const QString& idListContainerParamName,
	const QString& gqlObjectVarName,
	const QString& complexFieldName,
	uint hIndents) const
{
	bool isCustom = false;
	bool isEnum = false;
	ConvertTypeOrEnum(sdlField, m_sdlEnumListCompPtr->GetEnums(false), &isCustom, nullptr, nullptr, &isEnum);
	if (!isCustom || isEnum){
		return;
	}

	std::shared_ptr<imtsdl::CSdlEntryBase> foundEntry = GetSdlTypeOrEnumOrUnionForField(sdlField, 
				m_sdlTypeListCompPtr->GetSdlTypes(false),
				m_sdlEnumListCompPtr->GetEnums(false),
				m_sdlUnionListCompPtr->GetUnions(false));
	const imtsdl::CSdlType* sdlTypePtr = dynamic_cast<imtsdl::CSdlType*>(foundEntry.get());
	const imtsdl::CSdlUnion* sdlUnionPtr = dynamic_cast<imtsdl::CSdlUnion*>(foundEntry.get());
	if(sdlTypePtr == nullptr && sdlUnionPtr == nullptr){
		I_CRITICAL();

		return;
	}

	// GenerateRequestedFieldsParsing(stream, foundEntry.get(), idListContainerParamName, gqlObjectVarName, complexFieldName, hIndents);
	if (sdlTypePtr != nullptr){
		GenerateRequestedFieldsParsing(stream, *sdlTypePtr, idListContainerParamName, gqlObjectVarName, complexFieldName, hIndents);
	}
}


void CGqlWrapClassCodeGeneratorComp::GenerateRequestedFieldsParsing(
	QTextStream& stream,
	const imtsdl::CSdlType& sdlType,
	const QString& idListContainerParamName,
	const QString& gqlObjectVarName,
	const QString& complexFieldName,
	uint hIndents) const
{
	const imtsdl::SdlFieldList typeFieldList = sdlType.GetFields();
	if (typeFieldList.isEmpty()){
		I_CRITICAL();

		return;
	}

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!");
	stream << idListContainerParamName;
	stream << QStringLiteral(".isEmpty()){");
	FeedStream(stream, 1, false);

	// Initialize variables with values from the specified list, if they are present.
	for (const imtsdl::CSdlField& typeField: typeFieldList){
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("m_requestInfo.");
		if (!complexFieldName.isEmpty()){
			stream << complexFieldName;
			if (!complexFieldName.endsWith('.')){
				stream << '.';
			}
		}
		stream << QStringLiteral("is");
		stream << GetCapitalizedValue(typeField.GetId());
		stream << QStringLiteral("Requested = ");
		if (typeField.IsRequired()){
			stream << QStringLiteral("true;");
		}
		else {
			stream << idListContainerParamName;
			stream << QStringLiteral(".contains(\"");
			stream << typeField.GetId();
			stream << '"' << ')' << ';';
		}
		FeedStream(stream, 1, false);
	}

	// Initialize variables for complex fields of this
	for (const imtsdl::CSdlField& typeField: typeFieldList){
		bool isCustom = false;
		bool isEnum = false;
		bool isUnion = false;
		ConvertTypeOrEnumOrUnion(typeField,
					m_sdlEnumListCompPtr->GetEnums(false),
					m_sdlUnionListCompPtr->GetUnions(false),
					&isCustom,
					nullptr,
					nullptr,
					&isEnum,
					&isUnion);
		if (!isCustom || isEnum || isUnion){
			continue;
		}

		std::shared_ptr<imtsdl::CSdlEntryBase> foundEntry = GetSdlTypeOrEnumOrUnionForField(typeField, m_sdlTypeListCompPtr->GetSdlTypes(false), m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false));
		const imtsdl::CSdlType* sdlTypePtr = dynamic_cast<imtsdl::CSdlType*>(foundEntry.get());
		if(sdlTypePtr == nullptr){
			I_CRITICAL();

			return;
		}

		// check if a type contains itself
		QStringList processedIdList = complexFieldName.split('.');
		if (processedIdList.contains(typeField.GetId())){

			FeedStreamHorizontally(stream, hIndents);
			stream << '}';
			FeedStream(stream, 1, false);

			return;
		}

		QString newComplexFieldName = complexFieldName;
		if (!newComplexFieldName.isEmpty()){
			newComplexFieldName += '.';
		}
		newComplexFieldName += typeField.GetId();

		const QString newGqlContainerVarName = GetDecapitalizedValue(typeField.GetId()) + QStringLiteral("RequestedFieldsPtr");
		const QString newIdListContainerVarName = GetDecapitalizedValue(typeField.GetId()) + QStringLiteral("RequestedIds");

		// first create a GQL-info object
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("const imtgql::CGqlFieldObject* ");
		stream << newGqlContainerVarName;
		stream << ' ' << '=' << ' ';
		stream << gqlObjectVarName;
		stream << QStringLiteral("->GetFieldArgumentObjectPtr(\"");
		stream << typeField.GetId();
		stream << '"' << ')' << ';';
		FeedStream(stream, 1, false);

		// ensure is not null
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("if (");
		stream << newGqlContainerVarName;
		stream << QStringLiteral(" != nullptr){");
		FeedStream(stream, 1, false);

		// get requested id list
		FeedStreamHorizontally(stream, hIndents + 2);
		stream << QStringLiteral("const QByteArrayList ");
		stream << newIdListContainerVarName;
		stream << ' ' << '=' << ' ';
		stream << newGqlContainerVarName;
		stream << QStringLiteral("->GetFieldIds();");
		FeedStream(stream, 1, true);

		// then generate field info (self nested is not allowed)
		if (sdlType != *sdlTypePtr){
			GenerateRequestedFieldsParsing(
				stream,
				*sdlTypePtr,
				newIdListContainerVarName,
				newGqlContainerVarName,
				newComplexFieldName,
				hIndents + 2);
		}

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::GenerateRequestSetup(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest, uint hIndents) const
{
	// set commandID
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("gqlRequest.SetCommandId(GetCommandId());");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("// writting input arguments");
	FeedStream(stream, 1, false);
	for (const imtsdl::CSdlField& field: sdlRequest.GetInputArguments()){
		AddFieldWriteToRequestCode(stream, field);
	}
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("return true;");
}


void CGqlWrapClassCodeGeneratorComp::AddMethodDeclarations(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest) const
{
	// CommandId method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("static QByteArray GetCommandId();");
	FeedStream(stream, 2, false);

	// client-side method (setup GQL request)
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("[[nodiscard]] static bool SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const ");
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("RequestArguments& requestArguments, const ");
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("RequestInfo& requestInfo = ");
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("RequestInfo()");
	stream << ')' << ';';
	FeedStream(stream, 2, false);

	// default constructor with GraphQL request
	FeedStreamHorizontally(stream);
	stream << 'C' << sdlRequest.GetName() << QStringLiteral("GqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead);");
	FeedStream(stream, 1, false);

	// validation method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("bool IsValid() const;");
	FeedStream(stream, 1, false);

	// GetRequestedArguments method
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("const ");
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("RequestArguments& GetRequestedArguments() const;");
	FeedStream(stream, 1, false);

	// GetRequestInfo method
	FeedStreamHorizontally(stream);
	stream << GetCapitalizedValue(sdlRequest.GetName());
	stream << QStringLiteral("RequestInfo GetRequestInfo() const;");
	FeedStream(stream, 1, false);

	// Get GQL context
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("const ::imtgql::IGqlContext* GetRequestContext() const;");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddClassProperties(QTextStream& stream, const imtsdl::CSdlRequest& sdlRequest) const
{
	// validation property
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("bool m_isValid;");
	FeedStream(stream, 1, false);

	// Arguments property
	FeedStreamHorizontally(stream);
	stream << GetCapitalizedValue(sdlRequest.GetName()) << QStringLiteral("RequestArguments m_requestedArguments;");
	FeedStream(stream, 1, false);

	// Info property
	FeedStreamHorizontally(stream);
	stream << GetCapitalizedValue(sdlRequest.GetName()) << QStringLiteral("RequestInfo m_requestInfo;");
	FeedStream(stream, 1, false);

	// Gql context
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("const ::imtgql::IGqlContext* m_gqlContextPtr;");
	FeedStream(stream, 1, false);

}


// read methods

bool CGqlWrapClassCodeGeneratorComp::AddFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field) const
{
	bool isCustom = false;
	bool isArray = false;
	ConvertType(field, &isCustom, nullptr, &isArray);

	if (isArray){
		QString errorString = QString("Arrays is not allowed in request arguments! FieldID = '%1', FieldType = '%2'").arg(field.GetId(), field.GetType());
		SendCriticalMessage(0, errorString);
		Q_ASSERT_X(false, __func__, errorString.toLocal8Bit());

		return false;
	}
	if (isCustom){
		AddCustomFieldReadFromRequestCode(stream, field);
	}
	else {
		AddScalarFieldReadFromRequestCode(stream, field);
	}

	return true;
}


void CGqlWrapClassCodeGeneratorComp::AddScalarFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field) const
{
	AddExtractValueFromRequestCode(stream, field);
	FeedStreamHorizontally(stream);

	if (field.IsRequired()){
		AddDataCheckRequiredValueCode(stream, field);
		FeedStreamHorizontally(stream);

		AddSetValueToObjectCode(stream, field);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (!") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.isNull()){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		AddSetValueToObjectCode(stream, field);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CGqlWrapClassCodeGeneratorComp::AddCustomFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field) const
{
	AddExtractCustomValueFromRequestCode(stream, field);
	FeedStreamHorizontally(stream);

	if (field.IsRequired()){
		AddCheckCustomRequiredValueCode(stream, field);
		FeedStreamHorizontally(stream);

		AddSetCustomValueToObjectCode(stream, field);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr != nullptr){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		AddSetCustomValueToObjectCode(stream, field, 2);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


// write methods

bool CGqlWrapClassCodeGeneratorComp::AddFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field) const
{
	bool isCustom = false;
	bool isArray = false;
	ConvertType(field, &isCustom, nullptr, &isArray);

	if (isArray){
		QString errorString = QString("Arrays is not allowed in request arguments! FieldID = '%1', FieldType = '%2'").arg(field.GetId(), field.GetType());
		SendCriticalMessage(0, errorString);
		Q_ASSERT_X(false, __func__, errorString.toLocal8Bit());

		return false;
	}

	if (isCustom){
		AddCustomFieldWriteToRequestCode(stream, field);
	}
	else {
		AddScalarFieldWriteToRequestCode(stream, field);
	}

	return true;
}


void CGqlWrapClassCodeGeneratorComp::AddScalarFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents) const
{
	QString tempListVarName;
	GenerateListTempValueCode(stream, field, tempListVarName, hIndents);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("request.InsertField(");
	if (field.IsArray()){
		stream << tempListVarName;
	}
	else {
		stream << QStringLiteral(", m_") << GetDecapitalizedValue(field.GetId());
	}
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddCustomFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents) const
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);

	// declare temp GQL object
	FeedStreamHorizontally(stream, hIndents);
	const QString dataObjectVariableName = field.GetId() + QStringLiteral("DataObject");
	stream << QStringLiteral("::imtgql::CGqlParamObject ") << dataObjectVariableName << ';';
	FeedStream(stream, 1, false);

	// add me to temp object and checks
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!");
	stream << QStringLiteral("requestArguments.");
	stream << field.GetId();
	stream << (".WriteToGraphQlObject(");
	stream << dataObjectVariableName;
	stream << QStringLiteral(")){");
	FeedStream(stream, 1, false);

	AddErrorReport(stream, QStringLiteral("Unable to create GQL Object"), hIndents + 1);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// insert temp GQL object
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("gqlRequest.AddParam(");
	stream << '"' << field.GetId() << '"';
	stream << ',' << ' ';
	stream << dataObjectVariableName;
	stream << ')' << ';';
	FeedStream(stream, 1, false);
}


// help methods


// general help methods for scalar

void CGqlWrapClassCodeGeneratorComp::AddExtractValueFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, quint32 hIndents) const
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("QVariant ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data = gqlRequest.GetParamObject(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddDataCheckRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, quint32 hIndents) const
{
	stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.isNull()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("m_isValid = false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("m_isValid = true;");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddSetValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field) const
{
	stream << QStringLiteral("m_requestedArguments.") << field.GetId() << QStringLiteral(" = ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.");
	stream << GetFromVariantConversionString(field) << ';';
}


// general help methods for custom

void CGqlWrapClassCodeGeneratorComp::AddExtractCustomValueFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents) const
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const ::imtgql::CGqlParamObject* ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr = gqlRequest.GetParamObject(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddCheckCustomRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents) const
{
	stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr == nullptr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("m_isValid = false;");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("m_isValid = true;");
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddSetCustomValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents) const
{
	// check if a protocol is acceptable
	stream << QStringLiteral("if (!protocolVersion.isEmpty()){");
	FeedStream(stream, 1, false);

	imtsdl::CSdlType foundType;
	[[maybe_unused]]const bool isTypeFound = GetSdlTypeForField(
		field,
		m_sdlTypeListCompPtr->GetSdlTypes(false),
		foundType);
	Q_ASSERT(isTypeFound);

	/// \todo check all versions
	const QString fieldVersion = GetTypeVersion(foundType);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (protocolVersion == \"");
	stream << fieldVersion;
	stream << QStringLiteral("\"){");
	FeedStream(stream, 1, false);

	/// \todo do this for all versions
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(
		foundType,
		sdlNamespace,
		*m_sdlTypeListCompPtr,
		*m_sdlEnumListCompPtr,
		*m_sdlUnionListCompPtr,
		false);

	QString typeVersion = structNameConverter.GetString();
	typeVersion += QStringLiteral("::PV_");
	typeVersion += GetSdlEntryVersion(foundType, false);
	AddSetCustomValueToObjectCodeImpl(stream, field, typeVersion, hIndents + 1);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("else {");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("qWarning() << QString(\"Bad request. Version %1 is not supported\").arg(qPrintable(protocolVersion));");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("m_isValid = false;");
	FeedStream(stream, 2, false);

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("else {");
	FeedStream(stream, 1, false);

	AddSetCustomValueToObjectCodeImpl(stream, field, QString(), hIndents);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
}

void CGqlWrapClassCodeGeneratorComp::AddSetCustomValueToObjectCodeImpl(
	QTextStream& stream,
	const imtsdl::CSdlField& field,
	const QString& typeVersion,
	uint hIndents) const
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);

	// declare bool variable and read data in private property
	const QString readVariableName = QStringLiteral("is") + GetCapitalizedValue(field.GetId()) + QStringLiteral("Read");

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("bool ");
	stream << readVariableName << ';';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (optRead){");
	FeedStream(stream, 1, false);
	
	AddReadFromRequestCode(stream, field, typeVersion, readVariableName, true, hIndents + 2);
	
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("else {");
	FeedStream(stream, 1, false);

	AddReadFromRequestCode(stream, field, typeVersion, readVariableName, false, hIndents + 2);
	
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	// update validation status
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("m_isValid = ") << readVariableName << ';';
	FeedStream(stream, 1, false);

	// check the result of reading...
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!") << readVariableName << QStringLiteral("){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CGqlWrapClassCodeGeneratorComp::AddReadFromRequestCode(
	QTextStream& stream,
	const imtsdl::CSdlField& field,
	const QString& typeVersion,
	const QString& readVariableName,
	bool optRead,
	uint hIndents) const
{
	FeedStreamHorizontally(stream, hIndents);
	stream << readVariableName;
	stream << QStringLiteral(" = ");
	stream << QStringLiteral("m_requestedArguments.") << field.GetId();
	stream << '.';
	if (optRead){
		stream << QStringLiteral("Opt");
	}
	stream <<  QStringLiteral("ReadFromGraphQlObject(*");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr");
	if (!typeVersion.isEmpty()){
		stream << ',' << ' '<< typeVersion;
	}
	stream << ')' << ';';
	FeedStream(stream, 1, false);
}


} // namespace imtsdlgencpp

