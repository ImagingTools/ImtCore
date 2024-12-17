#include "CSdlClassGqlModificatorComp.h"


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgen
{


// protected methods

// reimplemented (CSdlClassModificatorBaseComp)

bool CSdlClassGqlModificatorComp::ProcessHeaderClassFile(const imtsdl::CSdlType& sdlType)
{
	if (m_headerFilePtr == nullptr){
		SendCriticalMessage(0, "Unable to process header file. Pointer is not set!");
		I_CRITICAL();

		return false;
	}

	m_logTag = QStringLiteral("GqlModificator");

	QTextStream ofStream(m_headerFilePtr);

	// add write method definition
	ofStream << QStringLiteral("\t[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlObject& request) const;");
	FeedStream(ofStream, 1, false);

	// add read method definition
	ofStream << QStringLiteral("\t[[nodiscard]] static bool ReadFromGraphQlObject(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const ::imtgql::CGqlObject& request);");
	FeedStream(ofStream, 1, false);

	// add read method definition
	ofStream << QStringLiteral("\t[[nodiscard]] static bool OptReadFromGraphQlObject(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const ::imtgql::CGqlObject& request);");
	FeedStream(ofStream, 2);

	return true;
}


bool CSdlClassGqlModificatorComp::ProcessSourceClassFile(const imtsdl::CSdlType& sdlType)
{
	if (m_sourceFilePtr == nullptr){
		SendCriticalMessage(0, "Unable to process source file. Pointer is not set!");
		I_CRITICAL();

		return false;
	}

	QTextStream ofStream(m_sourceFilePtr);

	// write method implementation
	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::WriteToGraphQlObject(::imtgql::CGqlObject& request) const");
	FeedStream(ofStream, 1, false);
	ofStream << '{';
	FeedStream(ofStream, 1, false);

	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldWriteToRequestCode(ofStream, field, false);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\treturn true;\n}");
	FeedStream(ofStream, 2);

	// read method implementation
	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::ReadFromGraphQlObject(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const ::imtgql::CGqlObject& request)\n{");
	FeedStream(ofStream, 1, false);

	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldReadFromRequestCode(ofStream, field, false);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\treturn true;\n}");
	FeedStream(ofStream, 3);

	// Opt read method implementation
	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::OptReadFromGraphQlObject(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const ::imtgql::CGqlObject& request)\n{");
	FeedStream(ofStream, 1, false);

	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldReadFromRequestCode(ofStream, field, true);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\treturn true;\n}");
	FeedStream(ofStream, 3);

	return true;
}


// private methods

// read methods

void CSdlClassGqlModificatorComp::AddFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	bool isCustom = false;
	bool isArray = false;
	bool isEnum = false;
	ConvertTypeOrEnum(field, m_sdlEnumListCompPtr->GetEnums(false), &isCustom, nullptr, &isArray, &isEnum);

	if ((isCustom && !isEnum) && isArray){
		AddCustomListFieldReadFromRequestCode(stream, field, optional);
	}
	else if ((isCustom && !isEnum) && !isArray){
		AddCustomFieldReadFromRequestCode(stream, field, optional);
	}
	else if ((!isCustom || isEnum) && isArray){
		AddScalarListFieldReadFromRequestCode(stream, field, optional);
	}
	else {
		AddScalarFieldReadFromRequestCode(stream, field, optional);
	}
}


void CSdlClassGqlModificatorComp::AddScalarFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	AddExtractValueFromRequestCode(stream, field);
	FeedStreamHorizontally(stream);

	if (!optional && field.IsRequired()){
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


void CSdlClassGqlModificatorComp::AddScalarListFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("QVariant ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data = request.GetFieldArgumentValue(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);

	if (!optional && field.IsRequired()){
		AddCheckScalarListRequiredValueCode(stream, field);

		AddSetScalarListValueToObjectCode(stream, field);
		FeedStream(stream, 1, false);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (!") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.isNull()){");
		FeedStream(stream, 1, false);

		AddSetScalarListValueToObjectCode(stream, field, 2);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassGqlModificatorComp::AddCustomFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	AddExtractCustomValueFromRequestCode(stream, field);
	FeedStreamHorizontally(stream);

	if (!optional && field.IsRequired()){
		AddCheckCustomRequiredValueCode(stream, field);
		FeedStreamHorizontally(stream);

		AddSetCustomValueToObjectCode(stream, field, optional);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr != nullptr){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		AddSetCustomValueToObjectCode(stream, field, optional, 2);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassGqlModificatorComp::AddCustomListFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("int ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Count = request.GetObjectsCount(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);

	if (!optional && field.IsRequired()){
		if (field.IsNonEmpty()){
			AddCheckCustomListRequiredValueCode(stream, field);
		}

		AddSetCustomListValueToObjectCode(stream, field, optional);
		FeedStream(stream, 1, false);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Count > 0){");
		FeedStream(stream, 1, false);

		AddSetCustomListValueToObjectCode(stream, field, optional, 2);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


// write methods

void CSdlClassGqlModificatorComp::AddFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	bool isCustom = false;
	bool isArray = false;
	bool isEnum = false;
	ConvertTypeOrEnum(field, m_sdlEnumListCompPtr->GetEnums(false), &isCustom, nullptr, &isArray, &isEnum);

	const bool isFieldRequired = field.IsRequired();
	if (isFieldRequired){
		if (field.IsArray() && field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, true);
		}
		else if (!field.IsArray() || field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, false);
		}
	}
	else {
		AddBeginSelfCheckNonRequiredValueCode(stream, field);
	}

	const int hIndents = (isFieldRequired ? 1 : 2);
	if ((isCustom && !isEnum) && isArray){
		AddCustomListFieldWriteToRequestCode(stream, field, hIndents, optional);
	}
	else if ((isCustom && !isEnum) && !isArray){
		AddCustomFieldWriteToRequestCode(stream, field, hIndents, optional);
	}
	else if ((!isCustom || isEnum) && isArray){
		AddScalarListFieldWriteToRequestCode(stream, field, hIndents, optional);
	}
	else {
		AddScalarFieldWriteToRequestCode(stream, field, hIndents, optional);
	}

	if (!isFieldRequired){
		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassGqlModificatorComp::AddScalarFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, uint hIndents)
{
	QString tempListVarName;
	GenerateListTempValueCode(stream, field, tempListVarName, hIndents);

	bool isEnum = false;
	ConvertTypeOrEnum(
		field,
		m_sdlEnumListCompPtr->GetEnums(false),
		nullptr,
		nullptr,
		nullptr,
		&isEnum);

	if (isEnum){
		FeedStreamHorizontally(stream, hIndents);
		const QString enumSourceVarName = QStringLiteral("m_") +  GetDecapitalizedValue(field.GetId());
		const QString enumConvertedVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("StringValue");

		// declare target value, to store value
		stream << QStringLiteral("QString ");
		stream << enumConvertedVarName << ';';
		FeedStream(stream, 1, false);

		imtsdl::CSdlEnum foundEnum;
		[[maybe_unused]] bool found = GetSdlEnumForField(field, m_sdlEnumListCompPtr->GetEnums(false), foundEnum);
		Q_ASSERT(found);

		WriteConversionFromEnum(stream, foundEnum, enumSourceVarName, enumConvertedVarName, m_originalSchemaNamespaceCompPtr->GetText(), hIndents);

		FeedStreamHorizontally(stream, hIndents);

		stream << QStringLiteral("request.InsertField(");
		stream << '"' << field.GetId() << '"' << QStringLiteral(", ");
		if (field.IsArray()){
			stream << tempListVarName;
		}
		else {
			stream << QStringLiteral("QVariant(") << enumConvertedVarName << ')';
		}
		stream << QStringLiteral(");");
		FeedStream(stream, 1, false);
	}
	else {
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("request.InsertField(");
		stream << '"' << field.GetId() << '"' << QStringLiteral(", ");
		if (field.IsArray()){
			stream << tempListVarName;
		}
		else {
			stream << QStringLiteral("QVariant(m_") << GetDecapitalizedValue(field.GetId()) << ')';
		}
		stream << QStringLiteral(");");
		FeedStream(stream, 1, false);
	}
}


void CSdlClassGqlModificatorComp::AddScalarListFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, uint hIndents)
{
	// Perform the same actions as for a single value.
	AddScalarFieldWriteToRequestCode(stream, field, optional, hIndents);
}


void CSdlClassGqlModificatorComp::AddCustomFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	// declare temp GQL object
	const QString dataObjectVariableName = field.GetId() + QStringLiteral("DataObject");
	stream << QStringLiteral("::imtgql::CGqlObject ") << dataObjectVariableName << ';';
	FeedStream(stream, 1, false);

	// add me to temp object and checks
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!m_");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral(".WriteToGraphQlObject(");
	stream << dataObjectVariableName;
	stream << QStringLiteral(")){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// insert temp GQL object
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("request.InsertField(");
	stream << '"' << field.GetId() << '"';
	stream << ',' << ' ';
	stream << dataObjectVariableName;
	stream << ')' << ';';
	FeedStream(stream, 1, false);
}


void CSdlClassGqlModificatorComp::AddCustomListFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	// declare temp GQL object list
	const QString dataObjectVariableName = field.GetId() + QStringLiteral("DataObject");
	const QString dataObjectListVariableName = dataObjectVariableName + QStringLiteral("List");
	stream << QStringLiteral("QList<::imtgql::CGqlObject> ") << dataObjectListVariableName << ';';
	FeedStream(stream, 1, false);

	// loop declaration
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("for (const auto& iterableValue : std::as_const(m_");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral(")){");
	FeedStream(stream, 1, false);

	// inLoop: declare temp object (inserted to list)
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("::imtgql::CGqlObject ") << dataObjectVariableName << ';';
	FeedStream(stream, 1, false);

	// inLoop: add me to temp object and checks
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!iterableValue.WriteToGraphQlObject(");
	stream << dataObjectVariableName << QStringLiteral(")){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	// inLoop: add temp object to list
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << dataObjectListVariableName;
	stream << QStringLiteral(" << ");
	stream << dataObjectVariableName << ';';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// and finally add list to GQL
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("request.InsertField(");
	stream << '"' << field.GetId() << '"';
	stream << ',' << ' ' << dataObjectListVariableName;
	stream << ')' << ';';
	FeedStream(stream, 1, false);
}


// help methods


// general help methods for scalar

void CSdlClassGqlModificatorComp::AddExtractValueFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, quint32 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("QVariant ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data = request.GetFieldArgumentValue(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CSdlClassGqlModificatorComp::AddDataCheckRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, quint32 hIndents)
{
	stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.isNull()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CSdlClassGqlModificatorComp::AddSetValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();

	bool isEnum = false;
	const QString convertedType = ConvertTypeWithNamespace(
		field,
		sdlNamespace,
		*m_sdlTypeListCompPtr,
		*m_sdlEnumListCompPtr,
		nullptr,
		nullptr,
		nullptr,
		&isEnum);

	if (isEnum){
		const QString dataVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Data");
		const QString enumSourceVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("StringValue");
		const QString enumConvertedVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("EnumValue");

		// declare target value, to store value
		stream << convertedType << ' ' << enumConvertedVarName << ';';
		FeedStream(stream, 1, false);

		// declare temp value, to store string equivalent
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("QString ");
		stream << enumSourceVarName;
		stream << QStringLiteral(" = ");
		stream << dataVarName;
		stream << QStringLiteral(".toString();");
		FeedStream(stream, 1, false);

		imtsdl::CSdlEnum foundEnum;
		[[maybe_unused]] bool found = GetSdlEnumForField(field, m_sdlEnumListCompPtr->GetEnums(false), foundEnum);
		Q_ASSERT(found);

		WriteConversionFromString(stream, foundEnum, enumSourceVarName, enumConvertedVarName, m_originalSchemaNamespaceCompPtr->GetText());

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("object.Set") << GetCapitalizedValue(field.GetId()) << '(';
		stream << enumConvertedVarName << QStringLiteral(");");
	}
	else {
		stream << QStringLiteral("object.Set") << GetCapitalizedValue(field.GetId()) << '(';
		stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.");
		stream << GetFromVariantConversionStringExt(field) << QStringLiteral(");");
	}
}


// general help methods for custom

void CSdlClassGqlModificatorComp::AddExtractCustomValueFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const ::imtgql::CGqlObject* ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr = request.GetFieldArgumentObjectPtr(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CSdlClassGqlModificatorComp::AddCheckCustomRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr == nullptr){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CSdlClassGqlModificatorComp::AddSetCustomValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, uint hIndents)
{
	// declare object to read
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, false);
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);

	// declare bool variable and read data method
	stream << QStringLiteral("const bool is") << GetCapitalizedValue(field.GetId()) << QStringLiteral("Read = ");
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, false);
	stream << QStringLiteral("::");
	if (optional){
		stream << QStringLiteral("Opt");
	}
	stream << QStringLiteral("ReadFromGraphQlObject(");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data, *");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);

	// check the result of reading
	stream << QStringLiteral("if (is") << GetCapitalizedValue(field.GetId()) << QStringLiteral("Read){");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);

	// set object data
	stream << QStringLiteral("object.Set") << GetCapitalizedValue(field.GetId()) << '(';
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
}


// general help methods for custom list

void CSdlClassGqlModificatorComp::AddCheckCustomListRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Count <= 0){");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CSdlClassGqlModificatorComp::AddSetCustomListValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, uint hIndents)
{
	// declare list
	FeedStreamHorizontally(stream, hIndents);
	stream << ConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr);
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << QStringLiteral("List;");
	FeedStream(stream, 1, false);

	// declare 'for(;;)' loop
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("for (int ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Index = 0; ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Index != ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Count ; ++");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Index){");
	FeedStream(stream, 1, false);

	// declare iteration var
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("const ::imtgql::CGqlObject* ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr = request.GetFieldArgumentObjectPtr(");
	stream << '"' << field.GetId() << '"' << ',' << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Index);");
	FeedStream(stream, 1, false);

	// checks for itaration var
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr == nullptr){");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';

	// declare read variable
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, false);
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);

	// read
	stream << QStringLiteral("if (!");
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, false);
	stream << QStringLiteral("::");
	if (optional){
		stream << QStringLiteral("Opt");
	}
	stream << QStringLiteral("ReadFromGraphQlObject(");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral(", *");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr)){");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	// add to temp list
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("List");
	stream << QStringLiteral(" << ");
	stream << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// set value
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("object.Set");
	stream << GetCapitalizedValue(field.GetId()) << '(';
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("List");
	stream << ')' << ';';
}


// general help methods for scalar list

void CSdlClassGqlModificatorComp::AddCheckScalarListRequiredValueCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.isNull()){");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CSdlClassGqlModificatorComp::AddSetScalarListValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	// declare list to sotre extracted values
	FeedStreamHorizontally(stream, hIndents);
	stream << ConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr);
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << QStringLiteral("List;");
	FeedStream(stream, 1, false);

	// extract values from GQL object
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("QVariantList ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataList = ");
	stream << GetDecapitalizedValue(field.GetId()) <<  QStringLiteral("Data.toList();");
	FeedStream(stream, 1, false);

	// Get count of list
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("qsizetype ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Count = ");
	stream << GetDecapitalizedValue(field.GetId()) <<  QStringLiteral("DataList.size();");
	FeedStream(stream, 1, false);

	// declare 'for(;;)' loop
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("for (qsizetype ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Index = 0; ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Index != ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Count ; ++");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Index){");
	FeedStream(stream, 1, false);

	// declare read variable
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, false);
	stream << ' ' << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral(" = ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataList[");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Index].");
	stream << GetFromVariantConversionStringExt(field, true) << ';';
	FeedStream(stream, 1, false);

	// add to temp list
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("List");
	stream << QStringLiteral(" << ");
	stream << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// set value
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("object.Set");
	stream << GetCapitalizedValue(field.GetId()) << '(';
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("List");
	stream << ')' << ';';
}

QList<imtsdl::IncludeDirective> CSdlClassGqlModificatorComp::GetIncludeDirectives() const
{
	static QList<imtsdl::IncludeDirective> retVal = {CreateImtDirective(QStringLiteral("<imtgql/CGqlObject.h>"))};

	return retVal;
}


} // namespace imtsdlgen


