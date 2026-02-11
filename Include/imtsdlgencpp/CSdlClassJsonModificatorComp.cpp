// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSdlClassJsonModificatorComp.h"


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgencpp
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


// reimplemented (ICxxModifier)

QString CSdlClassJsonModificatorComp::GetReadMethodName() const
{
	return QStringLiteral("ReadFromJsonObject");
}


QString CSdlClassJsonModificatorComp::GetWriteMethodName() const
{
	return QStringLiteral("WriteToJsonObject");
}


ICxxModifier::ArgumentList CSdlClassJsonModificatorComp::GetArguments() const
{
	ArgumentList retVal;

	Argument arg;
	arg.Name = QStringLiteral("jsonObject");
	arg.Type = QStringLiteral("QJsonObject");
	retVal << arg;

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


bool CSdlClassJsonModificatorComp::AddObjectValueWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, uint /*horizontalIndents*/) const
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


bool CSdlClassJsonModificatorComp::AddFieldValueAppendToObjectArray(QTextStream& stream, const imtsdl::CSdlField& field, const QString& arrayContainerVariableName, const QString& variableName, uint /*horizontalIndents*/) const
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


bool CSdlClassJsonModificatorComp::AddArrayWriteToObject(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const QString& variableName,
			const QString& targetObjectVariableName,
			uint /*horizontalIndents*/) const
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


bool CSdlClassJsonModificatorComp::AddContainerValueCheckConditionBegin(QTextStream& stream, const imtsdl::CSdlField& field, bool expected, quint16 horizontalIndents) const
{
	FeedStreamHorizontally(stream, horizontalIndents);
	stream << QStringLiteral("if (");
	if (!expected){
		stream << '!';
	}

	stream << GetContainerObjectVariableName();
	stream << QStringLiteral(".contains(\"");
	stream << field.GetId();
	stream << QStringLiteral("\")");
	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;
	const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);

	if (!isUnion){
		stream << ' ';
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

		if(isArray){
			stream << QStringLiteral("Array");
		}
		else if (	isEnum ||
				 convertedType == QStringLiteral("QString") ||
				 convertedType == QStringLiteral("QByteArray"))
		{
			stream << QStringLiteral("String");
		}
		else if (isCustom){
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
		stream << QStringLiteral("()");
	}

	stream << QStringLiteral("){");

	return true;
}


bool CSdlClassJsonModificatorComp::AddContainerValueReadFromObject(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const QString& variableName,
			quint16 /*horizontalIndents*/) const
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


bool CSdlClassJsonModificatorComp::AddContainerListAccessCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const QString& variableName,
			quint16 horizontalIndents,
			ListAccessResult& result) const
{
	std::shared_ptr<imtsdl::CSdlEntryBase> foundEntry = FindEntryByName(field.GetType());
	bool idUnion = (foundEntry && (dynamic_cast<imtsdl::CSdlUnion*>(foundEntry.get()) != nullptr));

	if (idUnion){
		result.customAccessedElementName = "a";
		QTextStream accessStream(&result.customListAccessCode);
		accessStream << ' ' << variableName;
		accessStream << QStringLiteral(" = ");
		accessStream << GetDecapitalizedValue(field.GetId());
		accessStream << QStringLiteral("JsonArray[$(index)].toVariant();");
		FeedStream(accessStream);

		FeedStreamHorizontally(accessStream, horizontalIndents + 1);
		accessStream << QStringLiteral("QString ");
		/// \bug \todo fix it
		accessStream << GetDecapitalizedValue(variableName.mid(4));
		accessStream << ("DataValueTypename = ");
		accessStream << GetDecapitalizedValue(field.GetId());
		accessStream << QStringLiteral("JsonArray[$(index)].toObject().value(\"__typename\").toString();");
	}

	stream << QStringLiteral("const QJsonArray ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("JsonArray = ");
	stream << GetContainerObjectVariableName();
	stream << '[' << '"';
	stream << field.GetId();
	stream << QStringLiteral("\"].toArray();");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, horizontalIndents);
	stream << QStringLiteral("const qsizetype ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("ArrayCount = ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("JsonArray.size();");

	result.listVariableName					= GetDecapitalizedValue(field.GetId()) + QStringLiteral("JsonArray");
	result.listCountVariableName			= GetDecapitalizedValue(field.GetId()) + QStringLiteral("ArrayCount");
	result.listCountVariableType			= QStringLiteral("qsizetype");
	result.toObjectTransformMethod			= QStringLiteral(".to") + GetConvertEndForFieldString(field, true);


	return true;
}


CSdlUnionConverter::ConversionType CSdlClassJsonModificatorComp::GetUnionScalarConversionType() const
{
	return CSdlUnionConverter::ConversionType::CT_JSON_SCALAR;
}


CSdlUnionConverter::ConversionType CSdlClassJsonModificatorComp::GetUnionArrayConversionType() const
{
	return CSdlUnionConverter::ConversionType::CT_JSON_ARRAY;
}


void CSdlClassJsonModificatorComp::AddUnionFieldValueReadFromObject(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool /*optional*/,
			quint16 hIndents) const
{
	bool hasComplexTypes = false;
	bool hasScalarTypes = false;

	std::shared_ptr<imtsdl::CSdlEntryBase> foundEntryPtr = FindEntryByName(field.GetType());
	imtsdl::CSdlUnion* unionPtr = dynamic_cast<imtsdl::CSdlUnion*>(foundEntryPtr.get());
	Q_ASSERT(unionPtr != nullptr);
	const QStringList unionTypeList = unionPtr->GetTypes();
	for (const QString& unionType: unionTypeList){
		std::shared_ptr<imtsdl::CSdlEntryBase> foundEntryPtr = FindEntryByName(unionType);
		if (foundEntryPtr != nullptr){
			hasComplexTypes = true;
		}
		else {
			hasScalarTypes = true;
		}
	}

	Q_ASSERT(hasComplexTypes || hasScalarTypes);
	if (hasComplexTypes){
		const QString tempDataPtrVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Object");
		stream << QStringLiteral("const ");
		stream << GetContainerObjectClassName();
		stream << ' ' << tempDataPtrVarName;
		stream << QStringLiteral(" = ");
		stream << GetContainerObjectVariableName();
		stream << QStringLiteral(".value(\"");
		stream << field.GetId() << QStringLiteral("\").toObject();");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("if (!");
		stream << GetContainerObjectVariableName();
		stream << QStringLiteral(".value(\"");
		stream << field.GetId();
		stream << QStringLiteral("\").isObject()");
		stream << QStringLiteral(" || !");
		stream << tempDataPtrVarName;
		stream << QStringLiteral(".contains(\"__typename\")");

		stream << ')' << '{';
		FeedStream(stream, 1, false);

		AddErrorReport(stream, QStringLiteral("__typename for field '%3' is missing, but expected"), hIndents + 1, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
		stream << '}';
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("QString ");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("Typename = ");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("Object.value(\"__typename\").toString();");
		FeedStream(stream, 1, false);
	}

	if (hasScalarTypes){
		if (hasComplexTypes){
			FeedStreamHorizontally(stream, hIndents);
		}
		stream << QStringLiteral("QVariant ");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("VariantValue = ");
		stream << GetContainerObjectVariableName();
		stream << QStringLiteral("[\"");
		stream << field.GetId();
		stream << QStringLiteral("\"].toVariant();");
		FeedStream(stream, 1, false);
	}

	const QString unionSourceVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("VariantValue");
	WriteUnionConversionFromData(stream,
								 *unionPtr,
								 unionSourceVarName,
								 field.GetId(),
								 m_originalSchemaNamespaceCompPtr->GetText(),
								 "modelIndex",
								 *m_sdlTypeListCompPtr,
								 *m_sdlEnumListCompPtr,
								 *m_sdlUnionListCompPtr,
								 hIndents,
								 GetUnionScalarConversionType(),
								 field.GetId(),
								 QString());
}


void CSdlClassJsonModificatorComp::AddUnionFieldValueWriteToObject(QTextStream& /*stream*/, const imtsdl::CSdlField& /*field*/, bool /*optional*/, quint16 /*hIndents*/) const
{

}


void CSdlClassJsonModificatorComp::WriteTypenameToObjectCode(QTextStream& stream, const imtsdl::CSdlType& sdlType) const 
{
	stream << GetContainerObjectVariableName();
	stream << QStringLiteral("[\"__typename\"] = \"");
	stream << sdlType.GetName();
	stream << "\";";
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




} // namespace imtsdlgencpp

