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
	QTextStream ofStream(m_headerFilePtr.GetPtr());
	QTextStream ifStream(m_originalHeaderFilePtr.GetPtr());
	bool isIncludesAdded = false;
	while (!ifStream.atEnd()){
		bool addLine = true;
		const QString readLine = ifStream.readLine();

		if (!isIncludesAdded){
			// first lookup for previous include remark
			if (readLine == QStringLiteral("// Qt includes")){
				addLine = false;
				FeedStream(ofStream, 1, false);
				ofStream << readLine;
				FeedStream(ofStream, 1, true);

				AddIncludeDirective(ofStream, false);
				isIncludesAdded = true;
			}
			// if we first, check if we reached end of include declaration (namespace begin)
			static QRegularExpression namespaceRegExp(QStringLiteral("\\s*namespace"));
			if (namespaceRegExp.match(readLine).hasMatch()){
				AddIncludeDirective(ofStream, true);
				isIncludesAdded = true;
				FeedStream(ofStream, 3, false);
			}
		}

		if (addLine){
			ofStream << readLine;
		}
		FeedStream(ofStream);
	}

	// add method definitions
	ofStream << QStringLiteral("\t[[nodiscard]] static bool WriteToJsonObject(const ");
	ofStream << GetSdlEntryVersion(sdlType);
	ofStream << ("& object, QJsonObject& jsonObject);");
	FeedStream(ofStream, 1, false);
	ofStream << QStringLiteral("\t[[nodiscard]] static bool ReadFromJsonObject(");
	ofStream << GetSdlEntryVersion(sdlType);
	ofStream << QStringLiteral("& object, const QJsonObject& jsonObject);");
	FeedStream(ofStream, 2);

	return true;
}


bool CSdlClassJsonModificatorComp::ProcessSourceClassFile(const imtsdl::CSdlType& sdlType)
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
	ofStream << QStringLiteral("::WriteToJsonObject(const ");
	ofStream << GetSdlEntryVersion(sdlType);
	ofStream << ("& object, QJsonObject& jsonObject)\n{");

	// add write logic for each field
	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		FeedStream(ofStream, 1, false);
		AddFieldWriteToJsonCode(ofStream, field);
	}

	// finish add implementation
	ofStream << QStringLiteral("\n\treturn true;\n}");
	FeedStream(ofStream, 3, false);

	// read method implementation
	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::ReadFromJsonObject(");
	ofStream << GetSdlEntryVersion(sdlType);
	ofStream << QStringLiteral("& object, const QJsonObject& jsonObject)\n{");

	// add write logic for each field
	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		FeedStream(ofStream, 1, false);
		AddFieldReadFromJsonCode(ofStream, field);
	}
	FeedStream(ofStream, 1, false);

	// finish read implementation
	ofStream << QStringLiteral("\treturn true;\n}");
	FeedStream(ofStream, 3);

	return true;
}


// private methods

void CSdlClassJsonModificatorComp::AddIncludeDirective(QTextStream& stream, bool addRemark)
{
	if (addRemark){
		stream.seek(stream.pos() - 1); // remove extra new line
		stream << QStringLiteral("// Qt includes");
		FeedStream(stream, 1, false);
	}

	stream << QStringLiteral("#include <QtCore/QJsonObject>");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("#include <QtCore/QJsonArray>");
	FeedStream(stream, 1, false);
	stream << QStringLiteral("#include <QtCore/QJsonValue>");
}


void CSdlClassJsonModificatorComp::AddFieldWriteToJsonCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	bool isArray = false;
	bool isCustom = false;
	ConvertType(field, &isCustom, nullptr, &isArray);

	if (isCustom && isArray){
		AddCustomArrayFieldWriteToJsonCode(stream, field);

		return;
	}

	else if (isCustom){
		AddCustomFieldWriteToJsonCode(stream, field);

		return;
	}

	else if (isArray){
		AddArrayFieldWriteToJsonCode(stream, field);

		return;
	}

	// Process scalar value
	FeedStreamHorizontally(stream);
	if (field.IsRequired()){

		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false) << QStringLiteral("){");
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


void CSdlClassJsonModificatorComp::AddFieldReadFromJsonCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	bool isArray = false;
	bool isCustom = false;
	ConvertType(field, &isCustom, nullptr, &isArray);
	if (isCustom && isArray){
		AddCustomArrayFieldReadFromJsonCode(stream, field);
		return;
	}

	else if (isCustom){
		AddCustomFieldReadFromJsonCode(stream, field);
		return;
	}

	else if (isArray){
		AddArrayFieldReadFromJsonCode(stream, field);

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
	if (field.IsRequired()){
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


void CSdlClassJsonModificatorComp::AddCustomFieldWriteToJsonCode(QTextStream& stream, const imtsdl::CSdlField& field)
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
		AddCustomFieldWriteToJsonImplCode(stream, field);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		AddCustomFieldWriteToJsonImplCode(stream, field, 2);
		stream << QStringLiteral("\n\t}");
	}
}


void CSdlClassJsonModificatorComp::AddCustomFieldWriteToJsonImplCode(QTextStream& stream, const imtsdl::CSdlField& field, quint16 hIndents)
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


void CSdlClassJsonModificatorComp::AddArrayFieldWriteToJsonCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	if (field.IsRequired()){
		if (field.IsArray() && field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, true);
		}
		else if (!field.IsArray() || field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, false);
		}
		AddArrayFieldWriteToJsonImplCode(stream, field);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);

		AddArrayFieldWriteToJsonImplCode(stream, field, 2);
		stream << QStringLiteral("\n\t}");
		FeedStream(stream, 1, false);
	}
}


void CSdlClassJsonModificatorComp::AddArrayFieldWriteToJsonImplCode(QTextStream& stream, const imtsdl::CSdlField& field, quint16 hIndents)
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


void CSdlClassJsonModificatorComp::AddCustomFieldReadFromJsonCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	if (field.IsRequired()){
		AddJsonValueCheckAndConditionBegin(stream, field, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
		AddCustomFieldReadFromJsonImplCode(stream, field);
	}
	else {
		AddJsonValueCheckAndConditionBegin(stream, field, true);
		AddCustomFieldReadFromJsonImplCode(stream, field, 2);
		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassJsonModificatorComp::AddCustomFieldReadFromJsonImplCode(QTextStream& stream, const imtsdl::CSdlField& field, quint16 hIndents)
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



void CSdlClassJsonModificatorComp::AddArrayFieldReadFromJsonCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	if (field.IsRequired()){
		AddJsonValueCheckAndConditionBegin(stream, field, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
		AddArrayFieldReadFromJsonImplCode(stream, field);
	}
	else {
		AddJsonValueCheckAndConditionBegin(stream, field, true);
		AddArrayFieldReadFromJsonImplCode(stream, field, 2);
		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassJsonModificatorComp::AddArrayFieldReadFromJsonImplCode(QTextStream& stream, const imtsdl::CSdlField& field, quint16 hIndents)
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
	stream << ConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr);
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


void CSdlClassJsonModificatorComp::AddCustomArrayFieldWriteToJsonCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	if (field.IsRequired()){
		if (field.IsArray() && field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, true);
		}
		else if (!field.IsArray() || field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, false);
		}
		AddCustomArrayFieldWriteToJsonImplCode(stream, field);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);

		AddCustomArrayFieldWriteToJsonImplCode(stream, field, 2);
		stream << QStringLiteral("\n\t}");
	}
}


void CSdlClassJsonModificatorComp:: AddCustomArrayFieldWriteToJsonImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
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


void CSdlClassJsonModificatorComp::AddCustomArrayFieldReadFromJsonCode(QTextStream& stream, const imtsdl::CSdlField& field)
{
	if (field.IsRequired()){
		AddJsonValueCheckAndConditionBegin(stream, field, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);

		AddCustomArrayFieldReadToJsonImplCode(stream, field);
	}
	else {
		AddJsonValueCheckAndConditionBegin(stream, field, true);
		AddCustomArrayFieldReadToJsonImplCode(stream, field, 2);

		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassJsonModificatorComp:: AddCustomArrayFieldReadToJsonImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
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
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, false);
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


} // namespace imtsdlgenv2

