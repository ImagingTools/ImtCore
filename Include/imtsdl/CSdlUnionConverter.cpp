#include <imtsdl/CSdlUnionConverter.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{


// static variables
CSdlUnionConverter::ConversionType CSdlUnionConverter::s_defaultConversionType = CSdlUnionConverter::CT_AS_IS;


// public methods

//QString CSdlUnionConverter::ConvertUnionToStringEquivalent(const QString& unionValue, ConversionType conversionType)
//{
//	if (conversionType == CT_AS_IS){
//		return unionValue;
//	}
//
//	char separator = ' ';
//	if (unionValue.contains('-')){
//		separator = '-';
//	}
//	else if (unionValue.contains('_')){
//		separator = '_';
//	}
//
//	// check if a single word in upper case
//	bool isAllUpper = true;
//	for (int index = 0; index < unionValue.length(); ++index){
//		QChar symbol = unionValue[index];
//		if (!symbol.isUpper()){
//			isAllUpper = false;
//			break;
//		}
//	}
//
//
//	// split string into words
//	QStringList parts;
//	if (separator == ' ' && !isAllUpper){
//		int position = 0;
//		for (int index = 0; index < unionValue.length(); ++index){
//			QChar symbol = unionValue[index];
//			if(symbol.isUpper() && index != 0){
//				parts << unionValue.mid(position, index - position);
//				position = index;
//			}
//		}
//		// add last word
//		parts << unionValue.mid(position, -1);
//	}
//	else {
//		parts = unionValue.split(separator);
//	}
//
//	// if a single string
//	if (parts.isEmpty()){
//		parts << unionValue;
//	}
//
//	// case correction
//	QMutableListIterator partsIter(parts);
//	while(partsIter.hasNext()){
//		QString& currentString = partsIter.next();
//		switch (conversionType) {
//		case CT_UPPER_SNAKE_CASE:
//		case CT_UPPER_KEBAB_CASE:
//			currentString = currentString.toUpper();
//			break;
//		case CT_LOWER_SNAKE_CASE:
//		case CT_LOWER_KEBAB_CASE:
//			currentString = currentString.toLower();
//			break;
//		case CT_UPPER_CAMEL_CASE:
//		case CT_LOWER_CAMEL_CASE:
//			currentString = currentString.toLower();
//			currentString[0] = currentString[0].toUpper();
//			break;
//		default:
//			Q_ASSERT(false);
//			break;
//		}
//	}
//
//	// and join into a single string
//	QString retVal;
//	switch (conversionType) {
//	case CT_UPPER_SNAKE_CASE:
//	case CT_LOWER_SNAKE_CASE:
//		retVal = parts.join('_');
//		break;
//	case CT_LOWER_KEBAB_CASE:
//	case CT_UPPER_KEBAB_CASE:
//		retVal = parts.join('-');
//		break;
//	default:
//		retVal = parts.join("");
//		break;
//	}
//
//	if (conversionType == CT_LOWER_CAMEL_CASE){
//		retVal[0] = retVal[0].toLower();
//	}
//
//	return retVal;
//}


void CSdlUnionConverter::WriteConversionFromUnion(
			QTextStream& stream,
			const CSdlUnion& sdlUnion,
			const QString& sourceVariableName,
			const QString& targetVariableName,
			const QString& relatedNamespace,
			uint hIndents)
{
	QString typeNamespace = CSdlTools::BuildNamespaceFromParams(sdlUnion.GetSchemaParams(), false, true);
	if (typeNamespace != relatedNamespace){
		while (!typeNamespace.endsWith(QStringLiteral("::"))){
			typeNamespace.append(':');
		}
		// use global namespace
		if (!typeNamespace.startsWith(QStringLiteral("::"))){
			typeNamespace.prepend(QStringLiteral("::"));
		}
	}
	else {
		typeNamespace.clear();
	}
	const QString completeTypeName = typeNamespace + sdlUnion.GetName();

	bool isFirstIteration = true;
	for (const auto& sdlType : sdlUnion.GetTypes()){
		// ToDo Handle complex datatypes?
		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		if (!isFirstIteration){
			stream << QStringLiteral("else ");
		}
		stream << QStringLiteral("if (const ") << sdlType.first << QStringLiteral("* val = std::get_if<") << sdlType.first << QStringLiteral(">(&(");
		stream << sourceVariableName;
		stream << QStringLiteral("))){");
		CSdlTools::FeedStream(stream, 1, false);

		CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
		stream << targetVariableName;
		stream << QStringLiteral(" = *val;");
		CSdlTools::FeedStream(stream, 1, false);

		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("}");
		CSdlTools::FeedStream(stream, 1, false);

		isFirstIteration = false;
	}
}


void CSdlUnionConverter::WriteUnionConversionFromString(
			QTextStream& stream,
			const CSdlUnion& sdlUnion,
			const QString& sourceVariableName,
			const QString& targetVariableName,
			const QString& relatedNamespace,
			uint hIndents)
{
	bool isFirstIteration = true;
	for (const auto& sdlType : sdlUnion.GetTypes()){
		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		if (!isFirstIteration){
			stream << QStringLiteral("else ");
		}
		stream << QStringLiteral("if (") << sourceVariableName << QStringLiteral(".canConvert<") << sdlType.first << QStringLiteral(">()){");
		CSdlTools::FeedStream(stream, 1, false);

		CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
		stream << targetVariableName;
		stream << QStringLiteral(" = ") << sourceVariableName << QStringLiteral(".value<") << sdlType.first << QStringLiteral(">();");
		CSdlTools::FeedStream(stream, 1, false);


		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("}");
		CSdlTools::FeedStream(stream, 1, false);

		isFirstIteration = false;
	}
}



} // namespace imtsdl
