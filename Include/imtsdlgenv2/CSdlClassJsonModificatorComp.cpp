#include "CSdlClassJsonModificatorComp.h"


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgenv2
{


// public methods

// reimplemented (IIncludeDirectivesProvider)

QList<imtsdl::IncludeDirective> CSdlClassJsonModificatorComp::GetIncludeDirectives() const
{
	static QList<imtsdl::IncludeDirective> retVal = {
		CreateQtDirective(QStringLiteral("<QtCore/QJsonObject>")),
		CreateQtDirective(QStringLiteral("<QtCore/QJsonArray>")),
		CreateQtDirective(QStringLiteral("<QtCore/QJsonValue>"))
	};

	return retVal;
}


// protected methods

// reimplemented (CObjectModificatorCompBase)

QString CSdlClassJsonModificatorComp::GetContainerObjectClassName() const
{
	return QStringLiteral("QJsonObject");
}


QString CSdlClassJsonModificatorComp::GetContainerObjectVariableName() const
{
	return QStringLiteral("jsonObject");
}


QString CSdlClassJsonModificatorComp::GetScalarArrayContainerObjectClassName() const
{
	return QStringLiteral("QJsonArray");
}


QString CSdlClassJsonModificatorComp::GetCustomArrayContainerObjectClassName() const
{
	return QStringLiteral("QJsonArray");
}


QString CSdlClassJsonModificatorComp::GetArrayContainerObjectVariableName() const
{
	return QStringLiteral("jsonArray");
}


bool CSdlClassJsonModificatorComp::AddFieldValueWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, uint /*horizontalIndents*/) const
{
	stream << GetContainerObjectVariableName();
	stream << '[' << '"';
	stream << field.GetId();
	stream << '"' << ']' << ' ' << '=' << ' ';
	stream << "QJsonValue::fromVariant(";
	stream << variableName;
	stream << ")";
	stream << ';';

	return true;
}


bool CSdlClassJsonModificatorComp::AddObjectValueWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, uint horizontalIndents) const
{
	stream << GetContainerObjectVariableName();
	stream << '[' << '"';
	stream << field.GetId();
	stream << '"' << ']' << ' ' << '=' << ' ';
	stream << variableName;
	stream << ';';

	return true;
}


QString CSdlClassJsonModificatorComp::AddCreationOfTemporaryArray(QTextStream& stream, const imtsdl::CSdlField& field) const
{
	stream << GetScalarArrayContainerObjectClassName();
	stream << ' ' << QStringLiteral("new");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Array;");

	return QStringLiteral("new") + GetCapitalizedValue(field.GetId()) + QStringLiteral("Array");
}


bool CSdlClassJsonModificatorComp::AddFieldValueAppendToObjectArray(QTextStream& stream, const imtsdl::CSdlField& field, const QString& arrayContainerVariableName, const QString& variableName, uint horizontalIndents) const
{
	stream << arrayContainerVariableName;
	stream << ' '<< '<' << '<' << ' ';

	// special type for bytearray. Json Array Can't store bytes, only string
	// Special for Union
	bool isUnion = false;
	const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), nullptr, nullptr, nullptr, nullptr, &isUnion);
	if (convertedType == QStringLiteral("QByteArray")){
		stream << QStringLiteral("QString(");
		stream << variableName;
		stream << ')';
	}
	else if (isUnion){
		stream << QStringLiteral("QJsonValue::fromVariant(");
		stream << variableName;
		stream << ')';
	}
	else{
		stream << variableName;
	}

	stream << ';';

	return true;
}


bool CSdlClassJsonModificatorComp::AddArrayWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, const QString& targetObjectVariableName, uint horizontalIndents) const
{
	if (targetObjectVariableName.isEmpty()){
		stream << GetContainerObjectVariableName();
	}
	else {
		stream << targetObjectVariableName;
	}
	stream << '[' << '"';
	stream << field.GetId();
	stream << '"' << ']';
	stream << ' ' << '=' << ' ';
	stream << variableName;
	stream << ';';

	return true;
}


bool CSdlClassJsonModificatorComp::AddContainerValueCheckConditionBegin(QTextStream& stream, const imtsdl::CSdlField& field, bool expected, quint16 horizontalIndents)
{
	FeedStreamHorizontally(stream, horizontalIndents);
	stream << QStringLiteral("if (");
	if (!expected){
		stream << '!';
	}

	stream << GetContainerObjectVariableName();
	stream << QStringLiteral(".contains(\"");
	stream << field.GetId();
	stream << QStringLiteral("\") ");
	if(!expected){
		stream << '|' << '|' << ' ' << '!';
	}
	else {
		stream << '&' << '&';
	}

	stream << ' ' << GetContainerObjectVariableName();
	stream << QStringLiteral("[\"");
	stream << field.GetId();
	stream << QStringLiteral("\"].is");

	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;
	const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);

	if(isArray){
		stream << QStringLiteral("Array");
	}
	else if (	isEnum ||
			 convertedType == QStringLiteral("QString") ||
			 convertedType == QStringLiteral("QByteArray"))
	{
		stream << QStringLiteral("String");
	}
	else if (isCustom || isUnion){
		stream << QStringLiteral("Object");
	}
	else if (	convertedType == QStringLiteral("int") ||
				convertedType == QStringLiteral("long") ||
				convertedType == QStringLiteral("qlonglong") ||
				convertedType == QStringLiteral("float") ||
				convertedType == QStringLiteral("double"))
	{
		stream << QStringLiteral("Double");
	}
	else if (convertedType == QStringLiteral("bool")){
		stream << QStringLiteral("Bool");
	}
	else {
		SendCriticalMessage(0, QString("Unexpected type for field %1").arg(field.GetId()));
		I_CRITICAL();

		return false;
	}

	stream << QStringLiteral("()){");

	return true;
}


bool CSdlClassJsonModificatorComp::AddContainerValueReadFromObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, quint16 horizontalIndents)
{
	if (variableName.isEmpty()){
		stream << GetContainerObjectVariableName();
	}
	else {
		stream << variableName;
	}
	stream << '[' << '"';
	stream << field.GetId();
	stream << QStringLiteral("\"].to");

	stream << GetConvertEndForFieldString(field, false);

	return true;
}


bool CSdlClassJsonModificatorComp::AddContainerListAccessCode(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, quint16 horizontalIndents, ListAccessResult& result)
{
	stream << QStringLiteral("const QJsonArray ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("jsonArray = ");
	if (variableName.isEmpty()){
		stream << GetContainerObjectVariableName();
	}
	else {
		stream << variableName;
	}
	stream << '[' << '"';
	stream << field.GetId();
	stream << QStringLiteral("\"].toArray();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, horizontalIndents);
	stream << QStringLiteral("const qsizetype ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("ArrayCount = ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("jsonArray.size();");

	result.listVariableName					= GetDecapitalizedValue(field.GetId()) + QStringLiteral("jsonArray");
	result.listCountVariableName			= GetDecapitalizedValue(field.GetId()) + QStringLiteral("ArrayCount");
	result.listCountVariableType			= QStringLiteral("qsizetype");
	result.toObjectTransformMethod			= QStringLiteral(".to") + GetConvertEndForFieldString(field, true);

	return true;
}


// private methods

QString CSdlClassJsonModificatorComp::GetConvertEndForFieldString(const imtsdl::CSdlField& field, bool forType) const
{
	QString retVal;

	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;
	const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);

	if(!forType && isArray){
		retVal += QStringLiteral("Array");
	}
	else if (	isEnum ||
			 convertedType == QStringLiteral("QString"))
	{
		retVal += QStringLiteral("String");
	}
	else if (isUnion){
		retVal += QStringLiteral("Variant");
	}
	else if (isCustom){
		retVal += QStringLiteral("Object");
	}
	else if (convertedType == QStringLiteral("QByteArray")){
		retVal += QStringLiteral("String().toUtf8");
	}
	else if (	convertedType == QStringLiteral("float") ||
			 convertedType == QStringLiteral("double"))
	{
		retVal += QStringLiteral("Double");
	}
	else if (convertedType == QStringLiteral("int")){
		retVal += QStringLiteral("Int");
	}
	else if (	convertedType == QStringLiteral("long") ||
			 convertedType == QStringLiteral("qlonglong"))
	{
		retVal += QStringLiteral("Integer");
	}
	else if (convertedType == QStringLiteral("bool")){
		retVal += QStringLiteral("Bool");
	}
	else {
		SendCriticalMessage(0, QString("Unexpected type for field %1").arg(field.GetId()));
		I_CRITICAL();

		return QString();
	}
	retVal += QStringLiteral("()");

	return retVal;
}


QString CSdlClassJsonModificatorComp::GetReadMethodName() const
{
	return QStringLiteral("ReadFromJsonObject");
}


QString CSdlClassJsonModificatorComp::GetWriteMethodName() const
{
	return QStringLiteral("WriteToJsonObject");
}


imtsdlgen::ICxxModifier::ArgumentList CSdlClassJsonModificatorComp::GetArguments() const
{
	ArgumentList retVal;

	Argument arg;
	arg.Name = QStringLiteral("jsonObject");
	arg.Type = QStringLiteral("QJsonObject");
	retVal << arg;

	return retVal;
}




} // namespace imtsdlgenv2

