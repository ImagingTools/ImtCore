#include <imtsdl/CSdlTools.h>


// Qt includes

// imtsdl includes
#include <imtsdl/CSdlField.h>
#include <imtsdl/CSdlType.h>

namespace imtsdl
{

// public static variables
const QString CSdlTools::s_variantMapModificatorArgumentName = QStringLiteral("VMap");
const QString CSdlTools::s_variantMapClassMemberName = QStringLiteral("_m_fieldsMap"); // add first underscore to avoid ambiguity


// public static methods

QString CSdlTools::ConvertType(const CSdlField& sdlField, bool* isCustomPtr, bool* isComplexPtr, bool* isArrayPtr)
{
	QString retVal;
	if (sdlField.IsArray()){
		retVal += QStringLiteral("QList<");
		if (isArrayPtr != nullptr){
			*isArrayPtr = true;
		}
	}
	else {
		if (isArrayPtr != nullptr){
			*isArrayPtr = false;
		}
	}

	retVal += ConvertType(sdlField.GetType(), isCustomPtr, isComplexPtr);

	if (sdlField.IsArray()){
		retVal += QStringLiteral(">");
	}

	return retVal;
}


QString CSdlTools::ConvertType(const QString& sdlType, bool* isCustomPtr, bool* isComplexPtr)
{
	// A signed 32‐bit integer
	if (sdlType == QStringLiteral("Int") || sdlType == QStringLiteral("Integer")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("int");
	}

	// A signed double-precision floating-point value
	if (sdlType == QStringLiteral("Float")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("float");
	}

	// A signed double-precision floating-point value
	if (sdlType == QStringLiteral("Double")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("double");
	}

	// A UTF‐8 character sequence
	if (sdlType == QStringLiteral("String")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = true;
		}

		return QStringLiteral("QString");
	}

	// true or false
	if (sdlType == QStringLiteral("Boolean") || sdlType == QStringLiteral("Bool")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = false;
		}

		return QStringLiteral("bool");
	}

	// (serialized as a String): A unique identifier that's often used to refetch an object or as the key for a cache.
	if (sdlType == QStringLiteral("ID")){
		if (isCustomPtr != nullptr){
			*isCustomPtr = false;
		}
		if (isComplexPtr != nullptr){
			*isComplexPtr = true;
		}

		return QStringLiteral("QByteArray");
	}

	// custom type (another class)
	if (isCustomPtr != nullptr){
		*isCustomPtr = true;
	}
	if (isComplexPtr != nullptr){
		*isComplexPtr = true;
	}

	return QStringLiteral("C") + sdlType;
}


void CSdlTools::FeedStream(QTextStream& stream, uint lines, bool flush)
{
	for (uint i = 0; i < lines; ++i){
		stream << '\n';
	}

	if (flush){
		stream.flush();
	}
}


void CSdlTools::FeedLineHorizontally(QString& line, uint indents, char indentDelimiter)
{
	for (uint i = 0; i < indents; ++i){
		line += indentDelimiter;
	}
}


void CSdlTools::FeedStreamHorizontally(QTextStream& stream, uint indents, char indentDelimiter)
{
	for (uint i = 0; i < indents; ++i){
		stream << indentDelimiter;
	}
}


QString CSdlTools::GetCapitalizedValue(const QString& inputValue)
{
	if (inputValue.isEmpty()){
		return inputValue;
	}

	QString retVal = inputValue;
	retVal[0] = retVal[0].toUpper();

	return retVal;
}


QString CSdlTools::GetDecapitalizedValue(const QString& inputValue)
{
	if (inputValue.isEmpty()){
		return inputValue;
	}

	QString retVal = inputValue;
	retVal[0] = retVal[0].toLower();

	return retVal;
}


bool CSdlTools::IsTypeHasFundamentalTypes(const CSdlType& sdlType, QSet<QString>* foundTypesPtr)
{
	bool retVal = false;

	for(const CSdlField& sdlField: sdlType.GetFields()){
		bool isComplex = true;
		bool isArray = false;
		const QString convertedType = ConvertType(sdlField, nullptr, &isComplex, &isArray);
		isComplex = isComplex || isArray;

		if (foundTypesPtr != nullptr && !isComplex){
			foundTypesPtr->insert(convertedType);
		}

		retVal = retVal || !isComplex;
	}

	return retVal;
}


bool CSdlTools::IsTypeHasNonFundamentalTypes(const CSdlType& sdlType, QSet<QString>* foundTypesPtr)
{
	bool retVal = false;

	for(const CSdlField& sdlField: sdlType.GetFields()){
		bool isComplex = false;
		bool isArray = false;
		ConvertType(sdlField, nullptr, &isComplex, &isArray);

		if (foundTypesPtr != nullptr && isComplex){
			foundTypesPtr->insert(ConvertType(sdlField.GetType()));
		}

		// add QList as non fundamental type (for includes)
		if (foundTypesPtr != nullptr && isArray){
			foundTypesPtr->insert(QStringLiteral("QList"));
		}

		retVal = retVal || isComplex;
	}

	return true;
}

QString CSdlTools::GetFromVariantConversionString(const CSdlField& sdlField)
{
	const QString sdlType = sdlField.GetType();

	if (sdlType == QStringLiteral("Int") || sdlType == QStringLiteral("Integer")){
		return QStringLiteral("toInt()");
	}
	if (sdlType == QStringLiteral("Float")){
		return QStringLiteral("toFloat()");
	}
	if (sdlType == QStringLiteral("Double")){
		return QStringLiteral("toDouble()");
	}
	if (sdlType == QStringLiteral("String")){
		return QStringLiteral("toString()");
	}
	if (sdlType == QStringLiteral("Boolean") || sdlType == QStringLiteral("Bool")){
		return QStringLiteral("toBool()");
	}
	if (sdlType == QStringLiteral("ID")){
		return QStringLiteral("toByteArray()");
	}

	/// \todo add convert type and add initialization of custom types
	return "";
}

QString CSdlTools::FromVariantMapAccessString(const CSdlField& sdlField)
{
	QString retVal = s_variantMapClassMemberName;
	retVal += '[' ;
	retVal += '"';
	retVal += sdlField.GetId();
	retVal += '"';
	retVal += ']';

	return retVal;
}


} // namespace imtsdl
