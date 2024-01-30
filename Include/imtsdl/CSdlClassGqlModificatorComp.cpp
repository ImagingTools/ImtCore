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
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("QVariant ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data = request.GetFieldArgumentValue(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(");");
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

