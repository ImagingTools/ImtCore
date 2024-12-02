#include <imtsdl/CSdlEnumConverter.h>


// Qt includes
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QRegularExpression>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CSdlTools.h>


namespace imtsdl
{


QString CSdlEnumConverter::ConvertEnumValueToStringEquivalent(const QString& enumValue, ConversionType conversionType)
{
	QString retVal;

	switch (conversionType) {
	case CT_AS_IS:
		retVal = enumValue;
		break;
	case CT_UPPER_CAMEL_CASE:
		retVal = EncodeToUpperCamelCase(enumValue);
		break;
	case CT_LOWER_CAMEL_CASE:
		retVal = EncodeToLowerCamelCase(enumValue);
		break;
	case CT_UPPER_SNAKE_CASE:
		retVal = EncodeToUpperSnakeCase(enumValue);
		break;
	case CT_LOWER_SNAKE_CASE:
		retVal = EncodeToLowerSnakeCase(enumValue);
		break;
	case CT_UPPER_KEBAB_CASE:
		retVal = EncodeToUpperKebabCase(enumValue);
		break;
	case CT_LOWER_KEBAB_CASE:
		retVal = EncodeToLowerKebabCase(enumValue);
		break;
	default:
		Q_ASSERT_X(false, "Unexpected conversionType", __func__);
		break;
	}

	return retVal;
}


void CSdlEnumConverter::WriteConversionFromEnum(
			QTextStream& stream,
			const CSdlEnum& sdlEnum,
			const QString& sourceVariableName,
			const QString& targetVariableName,
			const QString& relatedNamespace,
			uint hIndents)
{
	QString typeNamespace = CSdlTools::BuildNamespaceFromParams(sdlEnum.GetSchemaParams());
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
	const QString completeTypeName = typeNamespace + sdlEnum.GetName();

	CSdlTools::FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("switch (");
	stream << sourceVariableName;
	stream << QStringLiteral("){");
	CSdlTools::FeedStream(stream, 1, false);

	for (const auto& enumValue: sdlEnum.GetValues()){
		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("case ");
		CSdlTools::FeedStream(stream, 1, false);

		CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
		stream << completeTypeName;
		stream << ':' << ':';
		stream << enumValue.first;
		stream << ':';
		CSdlTools::FeedStream(stream, 1, false);

		CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
		stream << targetVariableName;
		stream << ' ' << '=' << ' ';
		stream << '"' << enumValue.second << '"' << ';';
		CSdlTools::FeedStream(stream, 1, false);

		CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("break;");
		CSdlTools::FeedStream(stream, 1, false);
	}

	CSdlTools::FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("default:");
	CSdlTools::FeedStream(stream, 1, false);

	CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("Q_ASSERT(false);");
	CSdlTools::FeedStream(stream, 1, false);

	CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("break;");
	CSdlTools::FeedStream(stream, 1, false);

	CSdlTools::FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	CSdlTools::FeedStream(stream, 1, false);
}


void CSdlEnumConverter::WriteConversionFromString(
			QTextStream& stream,
			const CSdlEnum& sdlEnum,
			const QString& sourceVariableName,
			const QString& targetVariableName,
			const QString& relatedNamespace,
			uint hIndents)
{
	QString typeNamespace = CSdlTools::BuildNamespaceFromParams(sdlEnum.GetSchemaParams());
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
	const QString completeTypeName = typeNamespace + sdlEnum.GetName();

	bool isFirstIteration = true;
	for (const auto& enumValue: sdlEnum.GetValues()){

		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		if (!isFirstIteration){
			stream << QStringLiteral("else ");
		}
		stream << QStringLiteral("if(");
		stream << sourceVariableName;
		stream << QStringLiteral(" == \"");
		stream << enumValue.second;
		stream << QStringLiteral("\"){");
		CSdlTools::FeedStream(stream, 1, false);

		CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
		stream << targetVariableName;
		stream << ' ' << '=' << ' ';
		stream << completeTypeName;
		stream << ':' << ':';
		stream << enumValue.first;
		stream << ';';
		CSdlTools::FeedStream(stream, 1, false);

		CSdlTools::FeedStreamHorizontally(stream, hIndents);
		stream << '}';
		CSdlTools::FeedStream(stream, 1, false);

		isFirstIteration = false;
	}
}


QStringList CSdlEnumConverter::SplitSingleStringIntoWords(const QString& sourceString)
{
	Q_ASSERT_X(false, "NOT implemented", __func__);
	return QStringList();
}


QString CSdlEnumConverter::EncodeToUpperCamelCase(const QString& sourceString)
{
	Q_ASSERT_X(false, "NOT implemented", __func__);
	return QString();
}


QString CSdlEnumConverter::EncodeToLowerCamelCase(const QString& sourceString)
{
	Q_ASSERT_X(false, "NOT implemented", __func__);
	return QString();
}


QString CSdlEnumConverter::EncodeToUpperSnakeCase(const QString& sourceString)
{
	Q_ASSERT_X(false, "NOT implemented", __func__);
	return QString();
}


QString CSdlEnumConverter::EncodeToLowerSnakeCase(const QString& sourceString)
{
	Q_ASSERT_X(false, "NOT implemented", __func__);
	return QString();
}


QString CSdlEnumConverter::EncodeToUpperKebabCase(const QString& sourceString)
{
	Q_ASSERT_X(false, "NOT implemented", __func__);
	return QString();
}


QString CSdlEnumConverter::EncodeToLowerKebabCase(const QString& sourceString)
{
	Q_ASSERT_X(false, "NOT implemented", __func__);
	return QString();
}


} // namespace imtsdl
