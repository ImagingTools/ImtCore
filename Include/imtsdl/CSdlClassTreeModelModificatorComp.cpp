#include "CSdlClassTreeModelModificatorComp.h"


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdl
{


// protected methods

// reimplemented (CSdlClassModificatorBaseComp)

bool CSdlClassTreeModelModificatorComp::ProcessHeaderClassFile(const CSdlType& sdlType)
{
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
	ofStream << QStringLiteral("\t[[nodiscard]] bool WriteToModel(");
	ofStream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	ofStream << QStringLiteral("::CTreeItemModel& model, int modelIndex = 0) const;");
	FeedStream(ofStream, 1, false);
	ofStream << QStringLiteral("\t[[nodiscard]] static bool ReadFromModel(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const ");
	ofStream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	ofStream << ("::CTreeItemModel& model, int modelIndex = 0);");
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
	ofStream << QStringLiteral("::WriteToModel(");
	ofStream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	ofStream << QStringLiteral("::CTreeItemModel& model, int modelIndex) const\n{");
	FeedStream(ofStream, 1, false);

	// add write logic for each field
	for (const CSdlField& field: sdlType.GetFields()){
		AddFieldWriteToModelCode(ofStream, field, sdlType);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\n\treturn true;\n}");
	FeedStream(ofStream, 3, false);

	// read method implementation
	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::ReadFromModel(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const ");
	ofStream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	ofStream << QStringLiteral("::CTreeItemModel& model, int modelIndex)\n{");
	FeedStream(ofStream, 1, false);

	for (const CSdlField& field: sdlType.GetFields()){
		AddFieldReadFromModelCode(ofStream, field, sdlType);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\treturn true;\n}");

	FeedStream(ofStream, 3);

	return true;
}


// private methods

void CSdlClassTreeModelModificatorComp::AddFieldWriteToModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType)
{
	bool isArray = false;
	bool isCustom = false;
	ConvertType(field, &isCustom, nullptr, &isArray);
	if (isCustom && isArray){
		AddCustomArrayFieldWriteToModelCode(stream, field, sdlType);

		return;
	}

	else if (isArray){
		AddPrimitiveArrayFieldWriteToModelCode(stream, field, sdlType);

		return;
	}

	else if (isCustom){
		AddCustomFieldWriteToModelCode(stream, field, sdlType);

		return;
	}

	// Process scalar value
	FeedStreamHorizontally(stream);
	if (field.IsRequired()){
		stream << QStringLiteral("if (!");
		stream << FromInternalMapCheckString(field) << QStringLiteral("){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;\n\t}");
		FeedStream(stream, 1, false);

		QString tempListVarName;
		GenerateListTempValueCode(stream, field, tempListVarName);

		FeedStreamHorizontally(stream);
		stream << QStringLiteral("model.SetData(\"") << field.GetId() << QStringLiteral("\", ");
		if (field.IsArray()){
			stream << tempListVarName;
		}
		else {
			stream << QStringLiteral("m_") << GetDecapitalizedValue(field.GetId());
		}
		stream << QStringLiteral(", modelIndex);");
	}
	else {
		stream << QStringLiteral("if (");
		stream << FromInternalMapCheckString(field);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);

		QString tempListVarName;
		GenerateListTempValueCode(stream, field, tempListVarName, 2);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("model.SetData(\"") << field.GetId() << QStringLiteral("\", ");
		if (field.IsArray()){
			stream << tempListVarName;
		}
		else {
			stream << QStringLiteral("m_") << GetDecapitalizedValue(field.GetId());
		}
		stream << QStringLiteral(", modelIndex);\n\t}");
	}

	FeedStream(stream, 1, false);
}


void CSdlClassTreeModelModificatorComp::AddFieldReadFromModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType)
{
	bool isArray = false;
	bool isCustom = false;
	ConvertType(field, &isCustom, nullptr, &isArray);
	if (isCustom && isArray){
		AddCustomArrayFieldReadFromModelCode(stream, field, sdlType);

		return;
	}

	else if (isArray){
		AddPrimitiveArrayFieldReadFromModelCode(stream, field, sdlType);

		return;
	}

	else if (isCustom){
		AddCustomFieldReadFromModelCode(stream, field, sdlType);

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


void CSdlClassTreeModelModificatorComp::AddCustomFieldWriteToModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType)
{
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	if (field.IsRequired()){
		stream << QStringLiteral("if (!");
		stream << FromInternalMapCheckString(field) << QStringLiteral("){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;\n\t}");
		FeedStream(stream, 1, false);
		AddCustomFieldWriteToModelImplCode(stream, field, sdlType);
	}
	else {
		stream << QStringLiteral("if (");
		stream << FromInternalMapCheckString(field);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		AddCustomFieldWriteToModelImplCode(stream, field, sdlType, 2);
		stream << QStringLiteral("\n\t}");
	}
}


void CSdlClassTreeModelModificatorComp::AddCustomFieldWriteToModelImplCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType, quint16 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);

	// Create a new model
	stream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	stream << QStringLiteral("::CTreeItemModel* ");
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
	stream << QStringLiteral(".WriteToModel(*");
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


void CSdlClassTreeModelModificatorComp::AddCustomFieldReadFromModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType)
{
	FeedStreamHorizontally(stream);

	stream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	stream << QStringLiteral("::CTreeItemModel* ");
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

		AddCustomFieldReadFromModelImplCode(stream, field, sdlType);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("DataModelPtr != nullptr){");
		FeedStream(stream, 1, false);

		AddCustomFieldReadFromModelImplCode(stream, field, sdlType, 2);
		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassTreeModelModificatorComp::AddCustomFieldReadFromModelImplCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType, quint16 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);

	// define readed container
	stream << ConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr);
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);

	// read from model
	stream << QStringLiteral("const bool is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Readed = ");
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, false);
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


void CSdlClassTreeModelModificatorComp::AddPrimitiveArrayFieldWriteToModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType)
{
	if (field.IsRequired()){
		if (field.IsArray() && field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, true);
		}
		else if (!field.IsArray() || field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, false);
		}
		AddPrimitiveArrayFieldWriteToModelImplCode(stream, field, sdlType);
		FeedStream(stream, 1, false);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (");
		stream << FromInternalMapCheckString(field);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		AddPrimitiveArrayFieldWriteToModelImplCode(stream, field, sdlType, 2);
		stream << QStringLiteral("\n\t}");
		FeedStream(stream, 1, false);
	}
}


void CSdlClassTreeModelModificatorComp::AddPrimitiveArrayFieldWriteToModelImplCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType, quint16 hIndents)
{
	// add a new model,to store list
	const QString newTreeModelVarName = QStringLiteral("new") + GetCapitalizedValue(field.GetId()) + QStringLiteral("ModelPtr");
	FeedStreamHorizontally(stream, hIndents);
	stream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	stream << QStringLiteral("::CTreeItemModel* ") << newTreeModelVarName;
	stream << QStringLiteral(" = model.AddTreeModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << newTreeModelVarName << QStringLiteral("->setIsArray(true);");
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

	// inLoop: insert ien item to model
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << newTreeModelVarName << QStringLiteral("->InsertNewItem();");
	FeedStream(stream, 1, false);

	// inLoop: add item and check
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << newTreeModelVarName << QStringLiteral("->SetData(QByteArray(), m_") << GetDecapitalizedValue(field.GetId());
	stream << '[' << treeModelIndexVarName << ']';
	stream << QStringLiteral(", ") << treeModelIndexVarName << ')' << ';';
	FeedStream(stream, 1, false);

	// end of loop
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
}


void CSdlClassTreeModelModificatorComp::AddPrimitiveArrayFieldReadFromModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType)
{
	FeedStreamHorizontally(stream, 1);
	stream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	stream << QStringLiteral("::CTreeItemModel* ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model = model.GetTreeItemModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);

	// value checks
	if (field.IsRequired()){
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model == nullptr){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);

		AddPrimitiveArrayFieldReadFromModelImplCode(stream, field);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model != nullptr){");
		FeedStream(stream, 1, false);

		AddPrimitiveArrayFieldReadFromModelImplCode(stream, field, 2);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassTreeModelModificatorComp::AddPrimitiveArrayFieldReadFromModelImplCode(QTextStream& stream, const CSdlField& field, quint16 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);

	const QString countVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Count");
	// declare count value
	stream << QStringLiteral("int ") << countVariableName << QStringLiteral(" = ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model->GetItemsCount();");
	FeedStream(stream, 1, false);

	// value checks
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
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, false);
	stream << ' ' << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral(" = ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model->GetData(QByteArray(), ");
	stream << indexVariableName << QStringLiteral(").");
	stream << GetFromVariantConversionStringExt(field, true);
	stream << ';';
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
}


void CSdlClassTreeModelModificatorComp::AddCustomArrayFieldWriteToModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType)
{
	if (field.IsRequired()){
		if (field.IsArray() && field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, true);
		}
		else if (!field.IsArray() || field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, false);
		}
		AddCustomArrayFieldWriteToModelImplCode(stream, field, sdlType);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (");
		stream << FromInternalMapCheckString(field);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		AddCustomArrayFieldWriteToModelImplCode(stream, field, sdlType, 2);
		stream << QStringLiteral("\n\t}");
	}
}


void CSdlClassTreeModelModificatorComp:: AddCustomArrayFieldWriteToModelImplCode(
			QTextStream& stream,
			const CSdlField& field,
			const CSdlType& sdlType,
			quint16 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);

	// add a new model,to store list
	const QString newTreeModelVarName = QStringLiteral("new") + GetCapitalizedValue(field.GetId()) + QStringLiteral("ModelPtr");

	stream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	stream << QStringLiteral("::CTreeItemModel* ") << newTreeModelVarName;
	stream << QStringLiteral(" = model.AddTreeModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << newTreeModelVarName << QStringLiteral("->setIsArray(true);");
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

	// inLoop: insert ien item to model
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << newTreeModelVarName << QStringLiteral("->InsertNewItem();");
	FeedStream(stream, 1, false);

	// inLoop: add item and check
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!m_") << GetDecapitalizedValue(field.GetId());
	stream << '[' << treeModelIndexVarName << ']';
	stream << QStringLiteral(".WriteToModel(*") << newTreeModelVarName;
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


void CSdlClassTreeModelModificatorComp::AddCustomArrayFieldReadFromModelCode(QTextStream& stream, const CSdlField& field, const CSdlType& sdlType)
{
	FeedStreamHorizontally(stream, 1);
	stream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	stream << QStringLiteral("::CTreeItemModel* ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model = model.GetTreeItemModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);

	// value checks
	if (field.IsRequired()){
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model == nullptr){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);

		AddCustomArrayFieldReadFromModelImplCode(stream, field);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model != nullptr){");
		FeedStream(stream, 1, false);

		AddCustomArrayFieldReadFromModelImplCode(stream, field, 2);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}

void CSdlClassTreeModelModificatorComp:: AddCustomArrayFieldReadFromModelImplCode(
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
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, false);
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);

	// inLoop: read and checks
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!");
	stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, false);
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

