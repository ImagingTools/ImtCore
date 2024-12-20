#include "CSdlClassJsonModificatorComp.h"


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgenv2
{


// protected methods

// reimplemented (CSdlClassModificatorBaseComp)

bool CSdlClassJsonModificatorComp::ProcessHeaderClassFile(const imtsdl::CSdlType& sdlType)
{
	if (m_headerFilePtr == nullptr){
		SendCriticalMessage(0, "Unable to process header file. Pointer is not set!");
		I_CRITICAL();

		return false;
	}

	QTextStream ofStream(m_headerFilePtr);

	// add method definitions
	ofStream << QStringLiteral("\t[[nodiscard]] static bool WriteToJsonObject(const ");
	ofStream << GetSdlEntryVersion(sdlType);
	ofStream << ("& object, QJsonObject& jsonObject);");
	FeedStream(ofStream, 1, false);

	ofStream << QStringLiteral("\t[[nodiscard]] static bool ReadFromJsonObject(");
	ofStream << GetSdlEntryVersion(sdlType);
	ofStream << QStringLiteral("& object, const QJsonObject& jsonObject);");
	FeedStream(ofStream, 1);

	ofStream << QStringLiteral("\t[[nodiscard]] static bool OptReadFromJsonObject(");
	ofStream << GetSdlEntryVersion(sdlType);
	ofStream << QStringLiteral("& object, const QJsonObject& jsonObject);");
	FeedStream(ofStream, 2);

	return true;
}


bool CSdlClassJsonModificatorComp::ProcessSourceClassFile(const imtsdl::CSdlType& sdlType)
{
	if (m_sourceFilePtr == nullptr){
		SendCriticalMessage(0, "Unable to process source file. Pointer is not set!");
		I_CRITICAL();

		return false;
	}

	QTextStream ofStream(m_sourceFilePtr);

	// add method implementation

	// read method implementation
	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::ReadFromJsonObject(");
	ofStream << GetSdlEntryVersion(sdlType);
	ofStream << QStringLiteral("& object, const QJsonObject& jsonObject)\n{");

	// add write logic for each field
	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		FeedStream(ofStream, 1, false);
		AddFieldReadFromJsonCode(ofStream, field, false);
	}
	FeedStream(ofStream, 1, false);

	// finish read implementation
	ofStream << QStringLiteral("\treturn true;\n}");
	FeedStream(ofStream, 3);

	// opt read method implementation
	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::OptReadFromJsonObject(");
	ofStream << GetSdlEntryVersion(sdlType);
	ofStream << QStringLiteral("& object, const QJsonObject& jsonObject)\n{");
	FeedStream(ofStream, 1, false);

	// add write logic for each field
	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldReadFromJsonCode(ofStream, field, true);
		FeedStream(ofStream, 1, false);
	}

	// finish read implementation
	ofStream << QStringLiteral("\treturn true;\n}");
	FeedStream(ofStream, 3);

	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::WriteToJsonObject(const ");
	ofStream << GetSdlEntryVersion(sdlType);
	ofStream << ("& object, QJsonObject& jsonObject)\n{");

	// add write logic for each field
	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldWriteToJsonCode(ofStream, field, false);
		FeedStream(ofStream, 1, false);
	}

	// finish add implementation
	ofStream << QStringLiteral("\n\treturn true;\n}");
	FeedStream(ofStream, 3, false);

	return true;
}


// private methods

void CSdlClassJsonModificatorComp::AddFieldWriteToJsonCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	[[maybe_unused]]const QString convertedType = ConvertTypeWithNamespace(
		field,
		m_originalSchemaNamespaceCompPtr->GetText(),
		*m_sdlTypeListCompPtr,
		*m_sdlEnumListCompPtr,
		&isCustom,
		nullptr,
		&isArray,
		&isEnum);
	FeedStream(stream, 1, false);
	Q_ASSERT(!convertedType.isEmpty());

	if ((isCustom && !isEnum) && isArray){
		AddCustomArrayFieldWriteToJsonCode(stream, field, optional);

		return;
	}

	else if (isCustom && !isEnum){
		AddCustomFieldWriteToJsonCode(stream, field, optional);

		return;
	}

	else if (isArray){
		AddArrayFieldWriteToJsonCode(stream, field, optional);

		return;
	}

	// Process scalar value
	FeedStreamHorizontally(stream);
	if (!optional && field.IsRequired()){

		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, true) << QStringLiteral("){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;\n\t}");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("jsonObject[\"") << field.GetId() << QStringLiteral("\"] = ");
		stream << QStringLiteral("QJsonValue::fromVariant(*object.");
		stream << field.GetId() << QStringLiteral(");");
	}
	else {
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false) << QStringLiteral("){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("jsonObject[\"") << field.GetId() << QStringLiteral("\"] = ");
		stream << QStringLiteral("QJsonValue::fromVariant(*object.");
		stream << field.GetId() << QStringLiteral(");");
		stream << QStringLiteral("\n\t}");
	}
	FeedStream(stream, 1, false);
}


void CSdlClassJsonModificatorComp::AddFieldReadFromJsonCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	const QString convertedType = ConvertTypeWithNamespace(
		field,
		m_originalSchemaNamespaceCompPtr->GetText(),
		*m_sdlTypeListCompPtr,
		*m_sdlEnumListCompPtr,
		&isCustom,
		nullptr,
		&isArray,
		&isEnum);
	if ((isCustom && !isEnum) && isArray){
		AddCustomArrayFieldReadFromJsonCode(stream, field, optional);
		return;
	}

	else if ((isCustom && !isEnum)){
		AddCustomFieldReadFromJsonCode(stream, field, optional);
		return;
	}

	else if (isArray){
		AddArrayFieldReadFromJsonCode(stream, field, optional);

		return;
	}

	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	// Process scalar value
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("QVariant ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data = jsonObject.value(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(").toVariant();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream);
	if (!optional && field.IsRequired()){
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.isNull()){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << GetSettingValueString(
					field,
					sdlNamespace,
					*m_sdlTypeListCompPtr,
			GetDecapitalizedValue(field.GetId()) + QStringLiteral("Data.") + GetFromVariantConversionStringExt(field));

	}
	else {
		stream << QStringLiteral("if (!") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.isNull()){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		stream << GetSettingValueString(
				field,
				sdlNamespace,
				*m_sdlTypeListCompPtr,
				GetDecapitalizedValue(field.GetId()) + QStringLiteral("Data.") + GetFromVariantConversionStringExt(field));
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
	}
	FeedStream(stream, 1, false);
}


void CSdlClassJsonModificatorComp::AddCustomFieldWriteToJsonCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	if (field.IsRequired()){
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field) << QStringLiteral("){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;\n\t}");
		FeedStream(stream, 1, false);
		AddCustomFieldWriteToJsonImplCode(stream, field, optional);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		AddCustomFieldWriteToJsonImplCode(stream, field, optional, 2);
		stream << QStringLiteral("\n\t}");
	}
}


void CSdlClassJsonModificatorComp::AddCustomFieldWriteToJsonImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool optional,
			quint16 hIndents)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, false);

	// Create an object container
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("QJsonObject ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("Json;");
	FeedStream(stream, 1, false);

	// Define check variable and add to model
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const bool is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Added = ");
	stream << structNameConverter.GetString();
	stream << QStringLiteral("::WriteToJsonObject(*object.");
	stream << field.GetId();
	stream << QStringLiteral(", ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("Json);");
	FeedStream(stream, 1, false);

	// add checks
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Added){");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// insert temp object to requested container
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("jsonObject[\"") << GetCapitalizedValue(field.GetId()) << QStringLiteral("\"] = ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("Json;");
}


void CSdlClassJsonModificatorComp::AddArrayFieldWriteToJsonCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	if (!optional && field.IsRequired()){
		if (field.IsArray() && field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, true);
		}
		else if (!field.IsArray() || field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, false);
		}
		AddArrayFieldWriteToJsonImplCode(stream, field, optional);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		
		AddArrayFieldWriteToJsonImplCode(stream, field, optional, 2);
		stream << QStringLiteral("\n\t}");
		FeedStream(stream, 1, false);
	}
}


void CSdlClassJsonModificatorComp::AddArrayFieldWriteToJsonImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool optional,
			quint16 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);

	// create a new json array to contain custom type
	const QString newJsonArrayVarName = QStringLiteral("new") + GetCapitalizedValue(field.GetId()) + QStringLiteral("JsonArray");
	stream << QStringLiteral("QJsonArray ") << newJsonArrayVarName << QStringLiteral(";");
	FeedStream(stream, 1, false);

	// declare loop
	FeedStreamHorizontally(stream, hIndents);
	const QString dataIndexVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	stream << QStringLiteral("for (qsizetype ") << dataIndexVarName;
	stream << QStringLiteral(" = 0; ") << dataIndexVarName;
	stream << QStringLiteral(" < object.") << field.GetId();
	stream << QStringLiteral("->size(); ++") << dataIndexVarName;
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// inLoop: add item
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << newJsonArrayVarName <<  QStringLiteral(" << QJsonValue::fromVariant(object.") << field.GetId();
	stream << QStringLiteral("->at(") << dataIndexVarName << QStringLiteral("));");
	FeedStream(stream, 1, false);

	// end of loop
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// add temp json to array container
	FeedStreamHorizontally(stream, hIndents);
	stream << "jsonObject[\"" << GetCapitalizedValue(field.GetId()) << QStringLiteral("\"] = ") << newJsonArrayVarName << ';';
}


void CSdlClassJsonModificatorComp::AddCustomFieldReadFromJsonCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	if (!optional && field.IsRequired()){
		AddJsonValueCheckAndConditionBegin(stream, field, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
		AddCustomFieldReadFromJsonImplCode(stream, field, optional);
	}
	else {
		AddJsonValueCheckAndConditionBegin(stream, field, true);
		AddCustomFieldReadFromJsonImplCode(stream, field, optional, 2);
		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassJsonModificatorComp::AddCustomFieldReadFromJsonImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool optional,
			quint16 hIndents)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, false);

	// reset pointer for object
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("object.");
	stream << field.GetId();
	stream << QStringLiteral(".reset(new ");
	structNameConverter.addVersion = true;
	stream << structNameConverter.GetString();
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);

	// read from Json
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const bool is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Readed = ");
	structNameConverter.addVersion = false;
	stream << structNameConverter.GetString();
	stream << QStringLiteral("::ReadFromJsonObject(*object.");
	stream << field.GetId();
	stream << QStringLiteral(", jsonObject[\"");
	stream << field.GetId();
	stream << QStringLiteral("\"].toObject());");
	FeedStream(stream, 1, false);

	// reading checks
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Readed){");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);
}



void CSdlClassJsonModificatorComp::AddArrayFieldReadFromJsonCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	if (!optional && field.IsRequired()){
		AddJsonValueCheckAndConditionBegin(stream, field, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
		AddArrayFieldReadFromJsonImplCode(stream, field, optional);
	}
	else {
		AddJsonValueCheckAndConditionBegin(stream, field, true);
		AddArrayFieldReadFromJsonImplCode(stream, field, optional, 2);
		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassJsonModificatorComp::AddArrayFieldReadFromJsonImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool optional,
			quint16 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);

	// declare container array
	const QString arrayVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Array");
	stream << QStringLiteral("const QJsonArray ") << arrayVariableName << QStringLiteral(" = jsonObject[\"");
	stream << field.GetId() << QStringLiteral("\"].toArray();");
	FeedStream(stream, 1, false);

	// declare count value
	FeedStreamHorizontally(stream, hIndents);
	const QString countVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Count");
	stream << QStringLiteral("qsizetype ") << countVariableName << QStringLiteral(" = ");
	stream << arrayVariableName << QStringLiteral(".size();");
	FeedStream(stream, 1, false);

	// value non empty checks
	if (field.IsNonEmpty()){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("if (") << countVariableName << QStringLiteral(" <= 0){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, hIndents);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	// declare temp list var
	const QString listVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("List");
	FeedStreamHorizontally(stream, hIndents);
	stream << ConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr);
	stream <<  ' ' << listVariableName << ';';
	FeedStream(stream, 1, false);

	//declare for loop
	const QString indexVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("for (qsizetype ") << indexVariableName << QStringLiteral(" = 0; ");
	stream << indexVariableName << QStringLiteral(" < ") << countVariableName;
	stream << QStringLiteral("; ++") << indexVariableName << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// inLoop: add variable to tempList
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << listVariableName;
	stream << QStringLiteral(" << ");
	stream << arrayVariableName << '['  << indexVariableName << ']';
	/// \todo fix it use \c FromVariantConvertString
	stream << QStringLiteral(".to");

	// define convert method
	QString sdlType = field.GetType();
	if (sdlType == QStringLiteral("Int") || sdlType == QStringLiteral("Integer")){
		stream << QStringLiteral("Int");
	}
	else if (sdlType == QStringLiteral("Long") || sdlType == QStringLiteral("Integer") || sdlType == QStringLiteral("LongLong")){
		stream << QStringLiteral("Integer");
	}
	else if (sdlType == QStringLiteral("Float") || sdlType == QStringLiteral("Double")){
		stream << QStringLiteral("Double");
	}
	else if (sdlType == QStringLiteral("String") || sdlType == QStringLiteral("ID")){
		stream << QStringLiteral("String");
	}
	else if (sdlType == QStringLiteral("Boolean") || sdlType == QStringLiteral("Bool")){
		stream << QStringLiteral("Bool");
	}

	stream << QStringLiteral("();");
	FeedStream(stream, 1, false);

	// inLoop: end
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// set value list to object
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	FeedStreamHorizontally(stream, hIndents);
	stream << GetSettingValueString(field, sdlNamespace, *m_sdlTypeListCompPtr, listVariableName);
	FeedStream(stream, 1, false);
}


void CSdlClassJsonModificatorComp::AddCustomArrayFieldWriteToJsonCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	if (!optional && field.IsRequired()){
		if (field.IsArray() && field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, true);
		}
		else if (!field.IsArray() || field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, false);
		}
		AddCustomArrayFieldWriteToJsonImplCode(stream, field, optional);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);

		AddCustomArrayFieldWriteToJsonImplCode(stream, field, optional, 2);
		stream << QStringLiteral("\n\t}");
	}
}


void CSdlClassJsonModificatorComp:: AddCustomArrayFieldWriteToJsonImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool optional,
			quint16 hIndents)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, false);

	// create a new json array to contain custom type
	FeedStreamHorizontally(stream, hIndents);
	const QString newJsonArrayVarName = QStringLiteral("new") + GetCapitalizedValue(field.GetId()) + QStringLiteral("JsonArray");
	stream << QStringLiteral("QJsonArray ") << newJsonArrayVarName << QStringLiteral(";");
	FeedStream(stream, 1, false);

	// declare loop
	FeedStreamHorizontally(stream, hIndents);
	const QString dataIndexVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	stream << QStringLiteral("for (qsizetype ") << dataIndexVarName;
	stream << QStringLiteral(" = 0; ") << dataIndexVarName;
	stream << QStringLiteral(" < object.") << field.GetId();
	stream << QStringLiteral("->size(); ++") << dataIndexVarName;
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// inLoop: create temp object to contin value
	FeedStreamHorizontally(stream, hIndents + 1);
	const QString newJsonObjectVarName = QStringLiteral("new") + GetCapitalizedValue(field.GetId()) + QStringLiteral("JsonObject");
	stream << QStringLiteral("QJsonObject ") << newJsonObjectVarName << QStringLiteral(";");
	FeedStream(stream, 1, false);

	// inLoop: add item and check
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!");
	structNameConverter.addVersion = false;
	stream << structNameConverter.GetString();
	stream << QStringLiteral("::WriteToJsonObject(object.");
	stream << field.GetId();
	stream << QStringLiteral("->at(");
	stream << dataIndexVarName;
	stream << QStringLiteral("), ");
	stream << newJsonObjectVarName;
	stream << QStringLiteral(")){");
	FeedStream(stream, 1, false);

	// inLoop: error report
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	// write serialized json object to array
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << newJsonArrayVarName << QStringLiteral(" << ") << newJsonObjectVarName << ';';
	FeedStream(stream, 1, false);

	// end of loop
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// add temp json to array container
	FeedStreamHorizontally(stream, hIndents);
	stream << "jsonObject[\"" << GetCapitalizedValue(field.GetId()) << QStringLiteral("\"] = ") << newJsonArrayVarName << ';';
}


void CSdlClassJsonModificatorComp::AddCustomArrayFieldReadFromJsonCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional)
{
	if (!optional && field.IsRequired()){
		AddJsonValueCheckAndConditionBegin(stream, field, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
		
		AddCustomArrayFieldReadToJsonImplCode(stream, field, optional);
	}
	else {
		AddJsonValueCheckAndConditionBegin(stream, field, true);
		AddCustomArrayFieldReadToJsonImplCode(stream, field, optional, 2);

		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassJsonModificatorComp:: AddCustomArrayFieldReadToJsonImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool optional,
			quint16 hIndents)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, false);
	structNameConverter.addVersion = true;
	structNameConverter.listWrap = true;

	// declare container array
	FeedStreamHorizontally(stream, hIndents);
	const QString arrayVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Array");
	stream << QStringLiteral("const QJsonArray ") << arrayVariableName << QStringLiteral(" = jsonObject[\"");
	stream << field.GetId() << QStringLiteral("\"].toArray();");
	FeedStream(stream, 1, false);

	// declare count value
	FeedStreamHorizontally(stream, hIndents);
	const QString countVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Count");
	stream << QStringLiteral("qsizetype ") << countVariableName << QStringLiteral(" = ");
	stream << arrayVariableName << QStringLiteral(".size();");
	FeedStream(stream, 1, false);

	// value non empty checks
	if (field.IsNonEmpty()){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("if (") << countVariableName << QStringLiteral(" <= 0){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, hIndents);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	// declare temp list var
	const QString listVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("List");
	FeedStreamHorizontally(stream, hIndents);
	stream << structNameConverter.GetString();
	stream << ' ' << listVariableName << ';';
	FeedStream(stream, 1, false);

	//declare for loop
	const QString indexVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("for (int ") << indexVariableName << QStringLiteral(" = 0; ");
	stream << indexVariableName << QStringLiteral(" < ") << countVariableName;
	stream << QStringLiteral("; ++") << indexVariableName << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// inLoop: declare temp var
	FeedStreamHorizontally(stream, hIndents + 1);
	structNameConverter.listWrap = false;
	stream << structNameConverter.GetString();
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);

	// inLoop: read and checks
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!");
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, false);
	stream << QStringLiteral("::ReadFromJsonObject(");
	stream << GetDecapitalizedValue(field.GetId()) << ',' << ' ';
	stream << arrayVariableName << '[';
	stream << indexVariableName << QStringLiteral("].toObject())){");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	// inLoop: add variable to tempList
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << listVariableName;
	stream << QStringLiteral(" << ");
	stream << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);

	// inLoop: end
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// set value list to object
	FeedStreamHorizontally(stream, hIndents);
	stream << GetSettingValueString(field, sdlNamespace, *m_sdlTypeListCompPtr, listVariableName);
	FeedStream(stream, 1, false);
}

void CSdlClassJsonModificatorComp::AddJsonValueCheckAndConditionBegin(QTextStream& stream, const imtsdl::CSdlField& field, bool expected, quint16 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (");
	if (!expected){
		stream << '!';
	}
	stream << QStringLiteral("jsonObject.contains(\"");
	stream << field.GetId();
	stream << QStringLiteral("\")){");
	FeedStream(stream, 1, false);
}

QList<imtsdl::IncludeDirective> CSdlClassJsonModificatorComp::GetIncludeDirectives() const
{
	static QList<imtsdl::IncludeDirective> retVal = {
		CreateQtDirective(QStringLiteral("<QtCore/QJsonObject>")),
		CreateQtDirective(QStringLiteral("<QtCore/QJsonArray>")),
		CreateQtDirective(QStringLiteral("<QtCore/QJsonValue>"))
	};

	return retVal;
}


} // namespace imtsdlgenv2

