#include "CSdlClassTreeModelModificatorComp.h"


// Qt includes
#include <QtCore/QRegularExpression>

// imtsdl includes
#include <imtsdl/CSdlType.h>


namespace imtsdl
{


// protected methods

// reimplemented (CSdlClassModificatorBaseComp)

bool CSdlClassTreeModelModificatorComp::ProcessHeaderClassFile(const CSdlType& sdlType)
{
	for (const CSdlField& field: sdlType.GetFields()){
		bool isArray = false;
		bool isCustom = false;
		ConvertType(field, &isCustom, nullptr, &isArray);
		if (!isCustom && isArray){
			SendErrorMessage(0, "SDL TreeItemModel does not supports arrays of scalar values. Use Array of custom values");

			return false;
		}
	}

	QTextStream ofStream(m_headerFilePtr.GetPtr());
	QTextStream ifStream(m_originalHeaderFilePtr.GetPtr());
	while (!ifStream.atEnd()){
		const QString readLine = ifStream.readLine();

		// check if we reached end of include declaration (namespace begin)
		static QRegularExpression namespaceRegExp(QStringLiteral("\\s*namespace"));
		if (namespaceRegExp.match(readLine).hasMatch()){
			ofStream.seek(ofStream.pos() - 1); // remove extra new line
			ofStream << QStringLiteral("// imtbase includes");
			FeedStream(ofStream, 1, false);
			ofStream << QStringLiteral("#include <imtbase/CTreeItemModel.h>");
			FeedStream(ofStream, 3, false);
		}
		ofStream << readLine;
		FeedStream(ofStream);
	}

	// add method definitions
	ofStream << QStringLiteral("\t[[nodiscard]] bool AddMeToModel(imtbase::CTreeItemModel& model, int modelIndex = 0, const QList<QString>& requiredFields = QList<QString>()) const;");
	FeedStream(ofStream, 1, false);
	ofStream << QStringLiteral("\t[[nodiscard]] static bool ReadFromModel(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const imtbase::CTreeItemModel& model, int modelIndex = 0);");
	FeedStream(ofStream, 2);

	return true;
}


bool CSdlClassTreeModelModificatorComp::ProcessSourceClassFile(const CSdlType& sdlType)
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
	ofStream << QStringLiteral("::AddMeToModel(imtbase::CTreeItemModel& model, int modelIndex, const QList<QString>& requiredFields) const\n{");

	// first add fields check
	// a)add static variable with all suppotred fields
	FeedStream(ofStream, 1, false);
	FeedStreamHorizontally(ofStream);
	ofStream << QStringLiteral("static QList<QString> supportedFieldNameList = {");
	SdlFieldList fieldList = sdlType.GetFields();
	const int fieldsCount = fieldList.count();
	for (int fieldIndex = 0; fieldIndex < fieldsCount - 1; ++fieldIndex){
		FeedStream(ofStream, 1, false);
		FeedStreamHorizontally(ofStream, 4);
		ofStream << '"' << fieldList[fieldIndex].GetId() << '"' << ',';
	}
	FeedStream(ofStream, 1, false);
	FeedStreamHorizontally(ofStream, 4);
	ofStream << '"' << fieldList[fieldsCount - 1].GetId() << '"' << "};";
	FeedStream(ofStream, 1, false);

	// b)ensure all requested fields is known
	FeedStream(ofStream, 1, false);
	FeedStreamHorizontally(ofStream);
	ofStream << QStringLiteral("for (const QString& field: requiredFields){");
	FeedStream(ofStream, 1, false);
	FeedStreamHorizontally(ofStream, 2);
	ofStream << QStringLiteral("if (!supportedFieldNameList.contains(field)){");
	FeedStream(ofStream, 1, false);
	FeedStreamHorizontally(ofStream, 3);
	ofStream << QStringLiteral("return false;");
	ofStream << QStringLiteral("\n\t\t}\n\t}");
	FeedStream(ofStream, 1, false);

	// then add write logic for each field
	for (const CSdlField& field: sdlType.GetFields()){
		AddFieldWriteToModelCode(ofStream, field);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\n\treturn true;\n}");

	FeedStream(ofStream, 3, false);

	// read method implementation
	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::ReadFromModel(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const imtbase::CTreeItemModel& model, int modelIndex)\n{");
	FeedStream(ofStream, 1, false);

	for (const CSdlField& field: sdlType.GetFields()){
		AddFieldReadFromModelCode(ofStream, field);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\treturn true;\n}");

	FeedStream(ofStream, 3);

	return true;
}


// private methods

void CSdlClassTreeModelModificatorComp::AddFieldWriteToModelCode(QTextStream& stream, const CSdlField& field)
{
	bool isArray = false;
	bool isCustom = false;
	ConvertType(field, &isCustom, nullptr, &isArray);
	if (isCustom && isArray){
		AddCustomArrayFieldWriteToModelCode(stream, field);

		return;
	}

	else if (isCustom){
		AddCustomFieldWriteToModelCode(stream, field);

		return;
	}

	// Process scalar value
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	if (field.IsRequired()){
		stream << QStringLiteral("if (");
		stream << FromVariantMapAccessString(field) << QStringLiteral(".isNull()){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;\n\t}");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("model.SetData(\"") << field.GetId() << QStringLiteral("\", ");
		stream << FromVariantMapAccessString(field) << QStringLiteral(", modelIndex);");
	}
	else {
		stream << QStringLiteral("if (!");
		stream << FromVariantMapAccessString(field);
		stream << QStringLiteral(".isNull() && (requiredFields.isEmpty() || requiredFields.contains(\"");
		stream << field.GetId() << QStringLiteral("\"))){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("model.SetData(\"") << field.GetId() << QStringLiteral("\", ");
		stream << FromVariantMapAccessString(field);
		stream << QStringLiteral(", modelIndex);\n\t}");
	}
}


void CSdlClassTreeModelModificatorComp::AddFieldReadFromModelCode(QTextStream& stream, const CSdlField& field)
{
	bool isArray = false;
	bool isCustom = false;
	ConvertType(field, &isCustom, nullptr, &isArray);
	if (isCustom && isArray){
		AddCustomArrayFieldReadFromModelCode(stream, field);
		return;
	}

	else if (isCustom){
		AddCustomFieldReadFromModelCode(stream, field);
		return;
	}

	// Process scalar value
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("QVariant ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data = model.GetData(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
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
		stream << QStringLiteral("object.Set") << GetCapitalizedValue(field.GetId()) << '(';
		stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.");
		stream << GetFromVariantConversionString(field) << QStringLiteral(");");
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (!") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.isNull()){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("object.Set") << GetCapitalizedValue(field.GetId()) << '(';
		stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.");
		stream << GetFromVariantConversionString(field) << QStringLiteral(");");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassTreeModelModificatorComp::AddCustomFieldWriteToModelCode(QTextStream& stream, const CSdlField& field)
{
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	if (field.IsRequired()){
		stream << QStringLiteral("if (");
		stream << FromVariantMapAccessString(field) << QStringLiteral(".isNull()){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;\n\t}");
		FeedStream(stream, 1, false);
		AddCustomFieldWriteToModelImplCode(stream, field);
	}
	else {
		stream << QStringLiteral("if (!");
		stream << FromVariantMapAccessString(field);
		stream << QStringLiteral(".isNull() && (requiredFields.isEmpty() || requiredFields.contains(\"");
		stream << field.GetId() << QStringLiteral("\"))){");
		FeedStream(stream, 1, false);
		AddCustomFieldWriteToModelImplCode(stream, field, 2);
		stream << QStringLiteral("\n\t}");
	}
}


void CSdlClassTreeModelModificatorComp::AddCustomFieldWriteToModelImplCode(QTextStream& stream, const CSdlField& field, quint16 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);

	// Create a new model
	stream << QStringLiteral("imtbase::CTreeItemModel* ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("NewModelPtr = model.AddTreeModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);

	// Define check variable and add to model
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const bool is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Added = m_");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral(".AddMeToModel(*");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("NewModelPtr, 0);");
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
}


void CSdlClassTreeModelModificatorComp::AddCustomFieldReadFromModelCode(QTextStream& stream, const CSdlField& field)
{
	FeedStreamHorizontally(stream);

	stream << QStringLiteral("imtbase::CTreeItemModel* ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("DataModelPtr = model.GetTreeItemModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);

	if (field.IsRequired()){
		stream << QStringLiteral("if (");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("DataModelPtr == nullptr){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);

		AddCustomFieldReadToModelImplCode(stream, field);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("DataModelPtr != nullptr){");
		FeedStream(stream, 1, false);

		AddCustomFieldReadToModelImplCode(stream, field, 2);
		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassTreeModelModificatorComp::AddCustomFieldReadToModelImplCode(QTextStream& stream, const CSdlField& field, quint16 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);

	// define readed container
	stream << 'C' << GetCapitalizedValue(field.GetType());
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);

	// read from model
	stream << QStringLiteral("const bool is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Readed = ");
	stream << 'C' << GetCapitalizedValue(field.GetType());
	stream << QStringLiteral("::ReadFromModel(");
	stream << GetDecapitalizedValue(field.GetId());
	stream << ',' << ' ' << QStringLiteral("*");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("DataModelPtr, modelIndex);");	
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
	FeedStreamHorizontally(stream, hIndents);

	// setting a container value to object
	stream << QStringLiteral("object.Set");
	stream << GetCapitalizedValue(field.GetId());
	stream << '(' << GetDecapitalizedValue(field.GetId()) << ')' << ';';
	FeedStream(stream, 1, false);
}


void CSdlClassTreeModelModificatorComp::AddCustomArrayFieldWriteToModelCode(QTextStream& stream, const CSdlField& field)
{
	FeedStreamHorizontally(stream);
	if (field.IsRequired()){
		stream << QStringLiteral("if (");
		stream << FromVariantMapAccessString(field) << QStringLiteral(".isNull()){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;\n\t}");
		FeedStream(stream, 1, false);
		AddCustomArrayFieldWriteToModelImplCode(stream, field);
	}
	else {
		stream << QStringLiteral("if (!");
		stream << FromVariantMapAccessString(field);
		stream << QStringLiteral(".isNull() && (requiredFields.isEmpty() || requiredFields.contains(\"");
		stream << field.GetId() << QStringLiteral("\"))){");
		FeedStream(stream, 1, false);
		AddCustomArrayFieldWriteToModelImplCode(stream, field, 2);
		stream << QStringLiteral("\n\t}");
	}
}


void CSdlClassTreeModelModificatorComp:: AddCustomArrayFieldWriteToModelImplCode(
			QTextStream& stream,
			const CSdlField& field,
			quint16 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);

	// add a new model,to store list
	const QString newTreeModelVarName = QStringLiteral("new") + GetCapitalizedValue(field.GetId()) + QStringLiteral("ModelPtr");
	stream << QStringLiteral("imtbase::CTreeItemModel* ") << newTreeModelVarName;
	stream << QStringLiteral(" = model.AddTreeModel(") << '"' << field.GetId() << '"' << QStringLiteral(");");
	FeedStream(stream, 1, false);

	// declare loop
	FeedStreamHorizontally(stream, hIndents);
	const QString treeModelIndexVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	stream << QStringLiteral("for (qsizetype ") << treeModelIndexVarName;
	stream << QStringLiteral(" = 0; ") << treeModelIndexVarName;
	stream << QStringLiteral(" < m_") << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral(".size(); ++") << treeModelIndexVarName;
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// inLoop: add item and check
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!m_") << GetDecapitalizedValue(field.GetId());
	stream << '[' << treeModelIndexVarName << ']';
	stream << QStringLiteral(".AddMeToModel(*") << newTreeModelVarName;
	stream << QStringLiteral(", ") << treeModelIndexVarName;
	stream << QStringLiteral(")){");
	FeedStream(stream, 1, false);

	// inLoop: error report
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';

	// end of loop
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
}


void CSdlClassTreeModelModificatorComp::AddCustomArrayFieldReadFromModelCode(QTextStream& stream, const CSdlField& field)
{
	FeedStreamHorizontally(stream, 1);
	stream << QStringLiteral("imtbase::CTreeItemModel* ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model = model.GetTreeItemModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);

	if (field.IsRequired() || field.IsNonEmpty()){

		// value checks
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model == nullptr){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);

		AddCustomArrayFieldReadToModelImplCode(stream, field);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model != nullptr){");
		FeedStream(stream, 1, false);

		AddCustomArrayFieldReadToModelImplCode(stream, field, 2);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}

void CSdlClassTreeModelModificatorComp:: AddCustomArrayFieldReadToModelImplCode(
			QTextStream& stream,
			const CSdlField& field,
			quint16 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);

	const QString countVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Count");
	// declare count value
	stream << QStringLiteral("int ") << countVariableName << QStringLiteral(" = ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model->GetItemsCount();");
	FeedStream(stream, 1, false);

	// value checks
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (") << countVariableName << QStringLiteral(" <= 0){");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// declare temp list var
	const QString listVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("List");
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("QList<C") << GetCapitalizedValue(field.GetType()) << '>';
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
	stream << 'C' << GetCapitalizedValue(field.GetType()) << ' ';
	stream << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);

	// inLoop: read and checks
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!");
	stream << 'C' << GetCapitalizedValue(field.GetType());
	stream << QStringLiteral("::ReadFromModel(");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral(", *");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model, ");
	stream << indexVariableName << QStringLiteral(")){");
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
	stream << QStringLiteral("object.Set");
	stream << GetCapitalizedValue(field.GetId());
	stream << '(' << listVariableName << ')' << ';';
	FeedStream(stream, 1, false);
}



} // namespace imtsdl

