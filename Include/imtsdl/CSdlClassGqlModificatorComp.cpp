#include "CSdlClassGqlModificatorComp.h"


// Qt includes
#include <QtCore/QRegularExpression>

// imtsdl includes
#include <imtsdl/CSdlType.h>


namespace imtsdl
{


// protected methods

// reimplemented (CSdlClassModificatorBaseComp)

bool CSdlClassGqlModificatorComp::ProcessHeaderClassFile(const CSdlType& sdlType)
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

	// add method definition
	ofStream << QStringLiteral("\t[[nodiscard]] static bool ReadFromGraphQlRequest(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const imtgql::CGqlObject& request);");
	FeedStream(ofStream, 2);

	return true;
}


bool CSdlClassGqlModificatorComp::ProcessSourceClassFile(const CSdlType& sdlType)
{
	QTextStream ofStream(m_sourceFilePtr.GetPtr());
	QTextStream ifStream(m_originalSourceFilePtr.GetPtr());
	while (!ifStream.atEnd()){
		ofStream << ifStream.readLine();
		FeedStream(ofStream);
	}

	// read method implementation
	ofStream << QStringLiteral("bool C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("::ReadFromGraphQlRequest(C");
	ofStream << sdlType.GetName();
	ofStream << QStringLiteral("& object, const imtgql::CGqlObject& request)\n{");
	FeedStream(ofStream, 1, false);

	for (const CSdlField& field: sdlType.GetFields()){
		AddFieldReadFromRequestCode(ofStream, field);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\treturn true;\n}");

	FeedStream(ofStream, 3);

	return true;
}


// private methods

void CSdlClassGqlModificatorComp::AddFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field)
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


void CSdlClassGqlModificatorComp::AddScalarFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field)
{
	AddExtractValueFromRequestCode(stream, field);
	FeedStreamHorizontally(stream);

	if (field.IsRequired()){
		AddCheckRequiredValueCode(stream, field);
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


void CSdlClassGqlModificatorComp::AddScalarListFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field)
{
	/// \todo implement it
	qFatal("Not supported(");
}


void CSdlClassGqlModificatorComp::AddCustomFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field)
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


void CSdlClassGqlModificatorComp::AddCustomListFieldReadFromRequestCode(QTextStream& stream, const CSdlField& field)
{
	/// \todo implement it
	qFatal("Not supported(");
}


// general help methods for scalar

void CSdlClassGqlModificatorComp::AddExtractValueFromRequestCode(QTextStream& stream, const CSdlField& field, quint32 hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("QVariant ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data = request.GetFieldArgumentValue(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CSdlClassGqlModificatorComp::AddCheckRequiredValueCode(QTextStream& stream, const CSdlField& field, quint32 hIndents)
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


void CSdlClassGqlModificatorComp::AddSetValueToObjectCode(QTextStream& stream, const CSdlField& field)
{
	stream << QStringLiteral("object.Set") << GetCapitalizedValue(field.GetId()) << '(';
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.");
	stream << GetFromVariantConversionString(field) << QStringLiteral(");");
}


// general help methods for custom

void CSdlClassGqlModificatorComp::AddExtractCustomValueFromRequestCode(QTextStream& stream, const CSdlField& field, uint hIndents)
{
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const imtgql::CGqlObject* ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("DataObjectPtr = request.GetFieldArgumentObjectPtr(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
	FeedStream(stream, 1, false);
}


void CSdlClassGqlModificatorComp::AddCheckCustomRequiredValueCode(QTextStream& stream, const CSdlField& field, uint hIndents)
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


void CSdlClassGqlModificatorComp::AddSetCustomValueToObjectCode(QTextStream& stream, const CSdlField& field, uint hIndents)
{
	// declare object to read
	stream << 'C' << GetCapitalizedValue(field.GetType());
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data;");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);

	// declare bool variable and read data method
	stream << QStringLiteral("const bool is") << GetCapitalizedValue(field.GetId()) << QStringLiteral("Read = ");
	stream << 'C' << GetCapitalizedValue(field.GetType()) << QStringLiteral("::ReadFromGraphQlRequest(");
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


} // namespace imtsdl

