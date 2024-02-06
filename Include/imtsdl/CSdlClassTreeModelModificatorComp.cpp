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
		if (isCustom || isArray){
			SendErrorMessage(0, "SDL TreeItemModel does not supports arrays and custom values");

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
	ofStream << QStringLiteral("\t[[nodiscard]] bool AddMeToModel(imtbase::CTreeItemModel& model, int modelIndex, const QList<QString>& requiredFields = QList<QString>()) const;");
	FeedStream(ofStream, 1, false);
	ofStream << QStringLiteral("\t[[nodiscard]] static bool ReadFromModel(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const imtbase::CTreeItemModel& model, int modelIndex);");
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

	// them add write logic for each field
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



} // namespace imtsdl

