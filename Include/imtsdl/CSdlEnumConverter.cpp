// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtsdl/CSdlEnumConverter.h>


// Qt includes
#include <QtCore/QDir>
#include <QtCore/QRegularExpression>

// ImtCore includes
#include <imtsdl/CSdlTools.h>
#include <imtsdl/CSdlEnum.h>


namespace imtsdl
{


// static variables
CSdlEnumConverter::ConversionType CSdlEnumConverter::s_defaultConversionType = CSdlEnumConverter::CT_AS_IS;


// public methods

QString CSdlEnumConverter::ConvertEnumValueToStringEquivalent(const QString& enumValue, ConversionType conversionType)
{
	if (conversionType == CT_AS_IS){
		return enumValue;
	}

	char separator = ' ';
	if (enumValue.contains('-')){
		separator = '-';
	}
	else if (enumValue.contains('_')){
		separator = '_';
	}

	// check if a single word in upper case
	bool isAllUpper = true;
	for (int index = 0; index < enumValue.length(); ++index){
		QChar symbol = enumValue[index];
		if (!symbol.isUpper()){
			isAllUpper = false;
			break;
		}
	}


	// split string into words
	QStringList parts;
	if (separator == ' ' && !isAllUpper){
		int position = 0;
		for (int index = 0; index < enumValue.length(); ++index){
			QChar symbol = enumValue[index];
			if(symbol.isUpper() && index != 0){
				parts << enumValue.mid(position, index - position);
				position = index;
			}
		}
		// add last word
		parts << enumValue.mid(position, -1);
	}
	else {
		parts = enumValue.split(separator);
	}

	// if a single string
	if (parts.isEmpty()){
		parts << enumValue;
	}

	// case correction
	QMutableListIterator partsIter(parts);
	while(partsIter.hasNext()){
		QString& currentString = partsIter.next();
		switch (conversionType){
		case CT_UPPER_SNAKE_CASE:
		case CT_UPPER_KEBAB_CASE:
			currentString = currentString.toUpper();
			break;
		case CT_LOWER_SNAKE_CASE:
		case CT_LOWER_KEBAB_CASE:
			currentString = currentString.toLower();
			break;
		case CT_UPPER_CAMEL_CASE:
		case CT_LOWER_CAMEL_CASE:
			currentString = currentString.toLower();
			currentString[0] = currentString[0].toUpper();
			break;
		default:
			Q_ASSERT(false);
			break;
		}
	}

	// and join into a single string
	QString retVal;
	switch (conversionType){
	case CT_UPPER_SNAKE_CASE:
	case CT_LOWER_SNAKE_CASE:
		retVal = parts.join('_');
		break;
	case CT_LOWER_KEBAB_CASE:
	case CT_UPPER_KEBAB_CASE:
		retVal = parts.join('-');
		break;
	default:
		retVal = parts.join("");
		break;
	}

	if (conversionType == CT_LOWER_CAMEL_CASE){
		retVal[0] = retVal[0].toLower();
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
	QString typeNamespace = CSdlTools::BuildNamespaceFromParams(sdlEnum.GetSchemaParams(), false, true);
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


void CSdlEnumConverter::WriteEnumConversionFromString(
			QTextStream& stream,
			const CSdlEnum& sdlEnum,
			const QString& sourceVariableName,
			const QString& targetVariableName,
			const QString& relatedNamespace,
			uint hIndents)
{
	QString typeNamespace = CSdlTools::BuildNamespaceFromParams(sdlEnum.GetSchemaParams(), false, true);
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
	// add default exit
	CSdlTools::FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("else {");
	CSdlTools::FeedStream(stream, 1, false);

	// report an error
	CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("I_IF_DEBUG(qWarning() << QString(\"%1:%2 Error: Unexpected value for enum '%3', actual: '%4'\")");
	stream << QStringLiteral(".arg(__FILE__, QString::number(__LINE__), \"");
	stream << sdlEnum.GetName() << QStringLiteral("\", ");
	stream << sourceVariableName << QStringLiteral(");)");
	CSdlTools::FeedStream(stream, 2, false);

	CSdlTools::FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	CSdlTools::FeedStream(stream, 1, false);

	CSdlTools::FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	CSdlTools::FeedStream(stream, 1, false);
}



} // namespace imtsdl
