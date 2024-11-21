#include "CSdlClassGqlModificatorComp.h"


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgenv2
{


// protected methods

// reimplemented (CSdlClassModificatorBaseComp)

bool CSdlClassGqlModificatorComp::ProcessHeaderClassFile(const imtsdl::CSdlType& sdlType)
{
	QTextStream ofStream(m_headerFilePtr.GetPtr());
	QTextStream ifStream(m_originalHeaderFilePtr.GetPtr());
	while (!ifStream.atEnd()){
		const QString readLine = ifStream.readLine();

		// check if we reached end of include declaration (namespace begin)
		static QRegularExpression namespaceRegExp(QStringLiteral("\\s*namespace"));
		if (namespaceRegExp.match(readLine).hasMatch()){
			ofStream.seek(ofStream.pos() - 1); // remove extra new line
			ofStream << QStringLiteral("// imtgql includes");
			FeedStream(ofStream, 1, false);
			ofStream << QStringLiteral("#include <imtgql/CGqlObject.h>");
			FeedStream(ofStream, 3, false);
		}
		ofStream << readLine;
		FeedStream(ofStream);
	}

	// add method definitions
	ofStream << QStringLiteral("\t[[nodiscard]] static bool WriteToGraphQlObject(const ");
	ofStream << GetTypeVerstion(sdlType);
	ofStream << ("& object, ::imtgql::CGqlObject& request);");
	FeedStream(ofStream, 1, false);
	ofStream << QStringLiteral("\t[[nodiscard]] static bool ReadFromGraphQlObject(");
	ofStream << GetTypeVerstion(sdlType);
	ofStream << QStringLiteral("& object, const ::imtgql::CGqlObject& request);");
	FeedStream(ofStream, 2);


	return true;
}


bool CSdlClassGqlModificatorComp::ProcessSourceClassFile(const imtsdl::CSdlType& sdlType)
{
	QTextStream ofStream(m_sourceFilePtr.GetPtr());
	QTextStream ifStream(m_originalSourceFilePtr.GetPtr());
	while (!ifStream.atEnd()){
		ofStream << ifStream.readLine();
		FeedStream(ofStream);
	}


	// add method implementation
	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::WriteToGraphQlObject(const ");
	ofStream << GetTypeVerstion(sdlType);
	ofStream << ("& object, ::imtgql::CGqlObject& request)\n{");
	FeedStream(ofStream, 1, false);

	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldWriteToRequestCode(ofStream, field);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\treturn true;\n}");
	FeedStream(ofStream, 3);




	// read method implementation
	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::ReadFromGraphQlObject(");
	ofStream << GetTypeVerstion(sdlType);
	ofStream << QStringLiteral("& object, const ::imtgql::CGqlObject& request)\n{");
	FeedStream(ofStream, 1, false);

	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldReadFromRequestCode(ofStream, field);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\treturn true;\n}");
	FeedStream(ofStream, 3);



	return true;
}


// private methods

// read methods

void CSdlClassGqlModificatorComp::AddFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	bool isCustom = false;
	bool isArray = false;
	ConvertType(field, &isCustom, nullptr, &isArray);

	if (isCustom && isArray){
		AddCustomListFieldReadFromRequestCode(stream, field);
	}
	else if (isCustom && !isArray){
		AddCustomFieldReadFromRequestCode(stream, field);
	}
	else if (!isCustom && isArray){
		AddScalarListFieldReadFromRequestCode(stream, field);
	}
	else {
		AddScalarFieldReadFromRequestCode(stream, field);
	}
}


void CSdlClassGqlModificatorComp::AddScalarFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field)
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


void CSdlClassGqlModificatorComp::AddScalarListFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("QVariant ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data = request.GetFieldArgumentValue(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);

	if (field.IsRequired()){
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


void CSdlClassGqlModificatorComp::AddCustomFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field)
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


void CSdlClassGqlModificatorComp::AddCustomListFieldReadFromRequestCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("int ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Count = request.GetObjectsCount(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);

	if (field.IsRequired()){
		if (field.IsNonEmpty()){
			AddCheckCustomListRequiredValueCode(stream, field);
		}

		AddSetCustomListValueToObjectCode(stream, field);
		FeedStream(stream, 1, false);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Count > 0){");
		FeedStream(stream, 1, false);

		AddSetCustomListValueToObjectCode(stream, field, 2);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


// write methods

void CSdlClassGqlModificatorComp::AddFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	bool isCustom = false;
	bool isArray = false;
	ConvertType(field, &isCustom, nullptr, &isArray);

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
	if (isCustom && isArray){
		AddCustomListFieldWriteToRequestCode(stream, field, hIndents);
	}
	else if (isCustom && !isArray){
		AddCustomFieldWriteToRequestCode(stream, field, hIndents);
	}
	else if (!isCustom && isArray){
		AddScalarListFieldWriteToRequestCode(stream, field, hIndents);
	}
	else {
		AddScalarFieldWriteToRequestCode(stream, field, hIndents);
	}

	if (!isFieldRequired){
		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassGqlModificatorComp::AddScalarFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	QString tempListVarName;
	GenerateListTempValueCode(stream, field, tempListVarName, hIndents);

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


void CSdlClassGqlModificatorComp::AddScalarListFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	// Perform the same actions as for a single value.
	AddScalarFieldWriteToRequestCode(stream, field, hIndents);
}


void CSdlClassGqlModificatorComp::AddCustomFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
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


void CSdlClassGqlModificatorComp::AddCustomListFieldWriteToRequestCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
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
	stream << QStringLiteral("object.Set") << GetCapitalizedValue(field.GetId()) << '(';
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.");
	stream << GetFromVariantConversionString(field) << QStringLiteral(");");
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


void CSdlClassGqlModificatorComp::AddSetCustomValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	// declare object to read
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, false);
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);

	// declare bool variable and read data method
	stream << QStringLiteral("const bool is") << GetCapitalizedValue(field.GetId()) << QStringLiteral("Read = ");
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, false);
	stream << QStringLiteral("::ReadFromGraphQlObject(");
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


void CSdlClassGqlModificatorComp::AddSetCustomListValueToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, uint hIndents)
{
	// declare list
	FeedStreamHorizontally(stream, hIndents);
	stream << ConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr);
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << QStringLiteral("List;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);

	// declare 'for(;;)' loop
	stream << QStringLiteral("for (int ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Index = 0; ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Index != ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Count ; ++");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Index){");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);

	// declare iteration var
	stream << QStringLiteral("const ::imtgql::CGqlObject* ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr = request.GetFieldArgumentObjectPtr(");
	stream << '"' << field.GetId() << '"' << ',' << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Index);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);

	// checks for itaration var
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
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, false);
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);

	// read
	stream << QStringLiteral("if (!");
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, false);
	stream << QStringLiteral("::ReadFromGraphQlObject(");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral(", *");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr)){");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';

	// add to temp list
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("List");
	stream << QStringLiteral(" << ");
	stream << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';

	// set value
	FeedStream(stream, 1, false);
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
	stream << ConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr);
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
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, false);
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


} // namespace imtsdlgenv2


