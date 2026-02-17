// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CSdlClassGqlModificatorComp.h"


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlType.h>


namespace imtsdlgencpp
{


// static variables

const static QStringList s_integerNumberMetaTypes = {
		"QMetaType::Int",
		"QMetaType::UInt",
		"QMetaType::LongLong",
		"QMetaType::ULongLong",
		"QMetaType::Long",
		"QMetaType::Short",
		"QMetaType::ULong",
		"QMetaType::UShort",
		"QMetaType::UChar"
};

const static QStringList s_numberMetaTypes = QStringList({
		"QMetaType::Float",
		"QMetaType::Double"}) + s_integerNumberMetaTypes;

// static helpers


void WriteTypeMultiConditionCheck(
			QTextStream& stream,
			const imtsdl::CSdlField& /*field*/,
			const QString& variableName,
			const QStringList& metaTypeList,
			bool expected = false)
{
	if (metaTypeList.size() < 2){
		I_CRITICAL();

		return;
	}
	// first
	stream << metaTypeList.constFirst();
	if (expected){
		stream << QStringLiteral(" || ");
	}
	else {
		stream << QStringLiteral(" && ");
	}

	// [1 : size()-1]
	for (qsizetype i = 1; i < metaTypeList.size() - 1; ++i){
		const QString& type = metaTypeList[i];
		stream << variableName;
		if(expected){
			stream << '=';
		}
		else{
			stream << '!';
		}
		stream << '=' << ' ';
		stream << type;
		if (expected){
			stream << QStringLiteral(" || ");
		}
		else {
			stream << QStringLiteral(" && ");
		}
	}

	// last
	stream << variableName;
	if(expected){
		stream << '=';
	}
	else{
		stream << '!';
	}
	stream << '=' << ' ';
	stream << metaTypeList.constLast();
}


// public methods

// reimplemented (IIncludeDirectivesProvider)

QList<imtsdl::IncludeDirective> CSdlClassGqlModificatorComp::GetIncludeDirectives() const
{
	static QList<imtsdl::IncludeDirective> retVal = {CreateImtDirective(QStringLiteral("<imtgql/CGqlParamObject.h>"))};

	return retVal;
}


// reimplemented (ICxxModifier)

QString CSdlClassGqlModificatorComp::GetReadMethodName() const
{
	return QStringLiteral("ReadFromGraphQlObject");
}


QString CSdlClassGqlModificatorComp::GetWriteMethodName() const
{
	return QStringLiteral("WriteToGraphQlObject");
}


ICxxModifier::ArgumentList CSdlClassGqlModificatorComp::GetArguments() const
{
	ArgumentList retVal;

	Argument arg;
	arg.Name = QStringLiteral("gqlObject");
	arg.Type = QStringLiteral("::imtgql::CGqlParamObject");
	retVal << arg;

	return retVal;
}


// protected methods

// reimplemented (CObjectModificatorCompBase)

QString CSdlClassGqlModificatorComp::GetContainerObjectClassName() const
{
	return QStringLiteral("::imtgql::CGqlParamObject");
}


QString CSdlClassGqlModificatorComp::GetContainerObjectVariableName() const
{
	return QStringLiteral("gqlObject");
}


QString CSdlClassGqlModificatorComp::GetScalarArrayContainerObjectClassName() const
{
	return QStringLiteral("QVariantList");
}


QString CSdlClassGqlModificatorComp::GetCustomArrayContainerObjectClassName() const
{
	return QStringLiteral("QList<::imtgql::CGqlParamObject>");
}


QString CSdlClassGqlModificatorComp::GetArrayContainerObjectVariableName() const
{
	return QStringLiteral("gqlArray");
}


bool CSdlClassGqlModificatorComp::AddFieldValueWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, uint /*horizontalIndents*/) const
{
	stream << GetContainerObjectVariableName();
	stream << QStringLiteral(".InsertParam(");
	stream << '"' << field.GetId() << '"' << QStringLiteral(", ");
	stream << QStringLiteral("QVariant(") << variableName << ')';
	stream << QStringLiteral(");");

	return true;
}


bool CSdlClassGqlModificatorComp::AddObjectValueWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, uint /*horizontalIndents*/) const
{
	stream << GetContainerObjectVariableName();
	stream << QStringLiteral(".InsertParam(");
	stream << '"' << field.GetId() << '"';
	stream << ',' << ' ';
	stream << variableName;
	stream << ')' << ';';

	return true;
}


QString CSdlClassGqlModificatorComp::AddCreationOfTemporaryArray(QTextStream& stream, const imtsdl::CSdlField& field) const
{
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;
	ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, nullptr, &isEnum, &isUnion);

	if (isCustom && !isEnum){
		stream << GetCustomArrayContainerObjectClassName();
	}
	else {
		stream << GetScalarArrayContainerObjectClassName();
	}
	stream << ' ';
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("DataObjectList;");

	return GetDecapitalizedValue(field.GetId()) + QStringLiteral("DataObjectList");
}


bool CSdlClassGqlModificatorComp::AddFieldValueAppendToObjectArray(QTextStream& stream, const imtsdl::CSdlField& field, const QString& arrayContainerVariableName, const QString& variableName, uint /*horizontalIndents*/) const
{
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;
	ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, nullptr, &isEnum, &isUnion);

	stream << arrayContainerVariableName;
	if (isCustom && !isEnum && !isUnion){
		stream << QStringLiteral(" << ");
		stream << variableName;
		stream << ';';
	}
	else {
		stream << QStringLiteral(" << ") << variableName << ';';
	}

	return true;
}


bool CSdlClassGqlModificatorComp::AddArrayWriteToObject(QTextStream& stream, const imtsdl::CSdlField& field, const QString& variableName, const QString& targetObjectVariableName, uint /*horizontalIndents*/) const
{
	if (targetObjectVariableName.isEmpty()){
		stream << GetContainerObjectVariableName();
	}
	else {
		stream << targetObjectVariableName;
	}
	stream << QStringLiteral(".InsertParam(");
	stream << '"' << field.GetId() << '"';
	stream << ',' << ' ';
	stream << variableName;
	stream << ')' << ';';

	return true;
}


bool CSdlClassGqlModificatorComp::AddContainerValueCheckConditionBegin(QTextStream& stream, const imtsdl::CSdlField& field, bool expected, quint16 horizontalIndents) const
{
	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;
	const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);
	bool isUserType = (isCustom && !isEnum && !isUnion);

	uint hIndents = horizontalIndents;
	FeedStreamHorizontally(stream, hIndents);

	if (expected && isArray && !field.IsNonEmpty()){ ///< if array could be empty emplace property
		stream << QStringLiteral("if (");
		stream << GetContainerObjectVariableName();
		stream << QStringLiteral(".ContainsParam(\"");
		stream << field.GetId();
		stream << QStringLiteral("\")){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << field.GetId();
		stream << QStringLiteral(".emplace();");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
		stream << '}';
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
	}
	stream << QStringLiteral("if (");
	if (!expected){
		stream << '!';
	}

	stream << GetContainerObjectVariableName();
	stream << QStringLiteral(".ContainsParam(\"");
	stream << field.GetId();
	stream << QStringLiteral("\")");

	if (!isArray && !isUnion){
		if (expected){
			stream << QStringLiteral(" && ");
		}
		else {
			stream << QStringLiteral(" || ");
		}
		stream << '(';
	}

	if (isArray && !isUserType && !isUnion){ // array of scalars
		if (expected){
			stream << QStringLiteral(" && !");
		}
		else {
			stream << QStringLiteral(" || ");
		}
		stream << '(';
		stream << GetContainerObjectVariableName();
		stream << QStringLiteral("[\"");
		stream << field.GetId();
		stream << QStringLiteral("\"].isNull())");
	}

	else if (isArray && !isUnion){
		if (expected){
			stream << QStringLiteral(" && (");
			stream << GetContainerObjectVariableName();
			stream << QStringLiteral(".GetObjectsCount(\"");
			stream << field.GetId();
			stream << QStringLiteral("\") ");
			stream << '>';
			stream << QStringLiteral(" 0)");
		}
	}

	else if (isUserType){
		stream << GetContainerObjectVariableName();
		stream << QStringLiteral(".GetParamArgumentObjectPtr(\"");
		stream << field.GetId();
		stream << QStringLiteral("\") ");
		if (expected){
			stream << '!';
		}
		else {
			stream << '=';
		}
		stream << QStringLiteral("= nullptr");
	}

	else if (!isUnion){
		stream << GetContainerObjectVariableName();
		stream << QStringLiteral("[\"");
		stream << field.GetId();
		stream << QStringLiteral("\"].userType() ");
		if(!expected){
			stream << '!';
		}
		else{
			stream << '=';
		}
		stream << '=' << ' ';

		// known-limitation text protocols do not support the "byte" type. for byte array also use string
		if (	isEnum ||
				convertedType == QStringLiteral("QString") ||
				convertedType == QStringLiteral("QByteArray"))
		{
			stream << QStringLiteral("QMetaType::QString");
			if (expected){
				stream << QStringLiteral(" || ");
			}
			else {
				stream << QStringLiteral(" && ");
			}
			stream << GetContainerObjectVariableName();
			stream << QStringLiteral("[\"");
			stream << field.GetId();
			stream << QStringLiteral("\"].userType() ");
			if(expected){
				stream << '=';
			}
			else{
				stream << '!';
			}
			stream << QStringLiteral("= QMetaType::QByteArray");
		}
		else if (	convertedType == QStringLiteral("int")||
					convertedType == QStringLiteral("long") ||
					convertedType == QStringLiteral("qlonglong"))
		{
			WriteTypeMultiConditionCheck(
				stream,
				field, 
				GetContainerObjectVariableName() + QStringLiteral("[\"") + field.GetId() + QStringLiteral("\"].userType() "),
				s_integerNumberMetaTypes,
				expected);
		}
		// known-limitation for text protocols, there is no difference between float and double numbers.
		else if (convertedType == QStringLiteral("float") || convertedType == QStringLiteral("double")){
			WriteTypeMultiConditionCheck(
				stream,
				field, 
				GetContainerObjectVariableName() + QStringLiteral("[\"") + field.GetId() + QStringLiteral("\"].userType() "),
				s_numberMetaTypes,
				expected);
		}
		else if (convertedType == QStringLiteral("bool")){
			stream << QStringLiteral("QMetaType::Bool");
		}
		else {
			SendCriticalMessage(0, QString("Unexpected type for field '%1'").arg(field.GetId()));
			I_CRITICAL();

			return false;
		}
	}

	if (!isArray && !isUnion){
		stream << ')';
	}

	stream << QStringLiteral("){");

	return true;
}


bool CSdlClassGqlModificatorComp::AddContainerValueReadFromObject(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const QString& variableName,
			quint16 /*horizontalIndents*/) const
{
	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;
	const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);
	bool isUserType = (isCustom && !isEnum && !isUnion);

	if (isUserType && !field.IsArray()){
		stream << '*';
	}

	if (variableName.isEmpty()){
		stream << GetContainerObjectVariableName();
	}
	else {
		stream << variableName;
	}

	if (isUserType){
		stream << QStringLiteral(".GetParamArgumentObjectPtr(\"");
		stream << field.GetId();
		stream << QStringLiteral("\")");
	}
	else {
		stream << '[' << '"';
		stream << field.GetId();
		stream << QStringLiteral("\"]");
		if (!isUnion){
			stream << QStringLiteral(".to");
			stream << GetConvertEndForFieldString(field, false);
		}
	}

	return true;
}


bool CSdlClassGqlModificatorComp::AddContainerListAccessCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const QString& variableName,
			quint16 horizontalIndents,
			ListAccessResult& result) const
{
	result.listCountVariableName			= GetDecapitalizedValue(field.GetId()) + QStringLiteral("ElementsCount");
	result.listCountVariableType			= QStringLiteral("qsizetype");

	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;
	const QString convertedType = ConvertTypeOrEnumOrUnion(field, m_sdlEnumListCompPtr->GetEnums(false), m_sdlUnionListCompPtr->GetUnions(false), &isCustom, nullptr, &isArray, &isEnum, &isUnion);
	bool isUserType = (isCustom && !isEnum && !isUnion);

	if (isUserType){
		stream << QStringLiteral("const qsizetype ");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("ElementsCount = ");
		stream << GetContainerObjectVariableName();
		stream << QStringLiteral(".GetObjectsCount(\"");
		stream << field.GetId();
		stream << QStringLiteral("\");");

		result.listVariableName = '*' + GetDecapitalizedValue(field.GetId()) + QStringLiteral("DataObjectPtr");
		result.customAccessedElementName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("DataObjectPtr");
		result.isCustomAccessedElementPointer	= true;
		result.customListAccessCode.clear();
		QTextStream accessCodeStream(&result.customListAccessCode);
		accessCodeStream << QStringLiteral("const ");
		accessCodeStream << GetContainerObjectClassName();
		accessCodeStream << QStringLiteral("* ") << result.customAccessedElementName;
		accessCodeStream << QStringLiteral(" = ");
		accessCodeStream << GetContainerObjectVariableName();
		accessCodeStream << QStringLiteral(".GetParamArgumentObjectPtr(\"");
		accessCodeStream << field.GetId();
		accessCodeStream << QStringLiteral("\", $(index));");
		FeedStream(accessCodeStream, 1, false);

		// check NULL
		FeedStreamHorizontally(accessCodeStream, horizontalIndents + 1);
		accessCodeStream << QStringLiteral("if (");
		accessCodeStream << result.customAccessedElementName;
		accessCodeStream << QStringLiteral(" == nullptr){");
		FeedStream(accessCodeStream, 1, false);

		FeedStreamHorizontally(accessCodeStream, horizontalIndents + 2);
		accessCodeStream << QStringLiteral("qDebug() << \"invalid type\" << ") << result.customAccessedElementName;
		accessCodeStream << QStringLiteral(";");
		FeedStream(accessCodeStream, 1, false);

		FeedStreamHorizontally(accessCodeStream, horizontalIndents + 2);
		accessCodeStream << QStringLiteral("return false;");
		FeedStream(accessCodeStream, 1, false);

		FeedStreamHorizontally(accessCodeStream, horizontalIndents + 1);
		accessCodeStream << '}';
		FeedStream(accessCodeStream, 1, false);
	}
	else if (isUnion){
		stream << QStringLiteral("const QList<const ");
		stream << GetContainerObjectClassName();
		stream << QStringLiteral("*> ");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("DataList = ");
		stream << GetContainerObjectVariableName();
		stream << QStringLiteral(".GetParamArgumentObjectPtrList(\"");
		stream << field.GetId();
		stream << QStringLiteral("\");");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, horizontalIndents);
		stream << QStringLiteral("const qsizetype ");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("ElementsCount = ");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("DataList.size();");

		result.listVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("DataList");

		result.customAccessedElementName = "a";
		QTextStream accessStream(&result.customListAccessCode);
		accessStream << ' ' << variableName;
		accessStream << QStringLiteral(" = ");
		accessStream << GetDecapitalizedValue(field.GetId());
		accessStream << QStringLiteral("DataList[$(index)];");
		FeedStream(accessStream);

		FeedStreamHorizontally(accessStream, horizontalIndents + 1);
		accessStream << QStringLiteral("if (");
		accessStream << variableName;
		accessStream << QStringLiteral(" == nullptr){");
		FeedStream(accessStream);

		FeedStreamHorizontally(accessStream, horizontalIndents + 2);
		accessStream << QStringLiteral("return false;");
		FeedStream(accessStream);

		FeedStreamHorizontally(accessStream, horizontalIndents + 1);
		accessStream << '}';
		FeedStream(accessStream);

		FeedStreamHorizontally(accessStream, horizontalIndents + 1);
		accessStream << QStringLiteral("QString ");
		/// \bug \todo fix it
		accessStream << GetDecapitalizedValue(variableName.mid(4));
		accessStream << ("DataValueTypename = ");
		accessStream << variableName;
		accessStream << QStringLiteral("->GetParamArgumentValue(\"__typename\").toString();");
	}
	else{
		stream << QStringLiteral("const QVariant ");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("Data = ");
		stream << GetContainerObjectVariableName();
		stream << QStringLiteral("[\"");
		stream << field.GetId();
		stream << QStringLiteral("\"];");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, horizontalIndents);
		stream << QStringLiteral("const QVariantList ");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("DataList = ");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("Data.toList();");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, horizontalIndents);
		stream << QStringLiteral("const qsizetype ");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("ElementsCount = ");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("DataList.size();");
		if (!isUnion){
			result.toObjectTransformMethod		= QStringLiteral(".to") + GetConvertEndForFieldString(field, true);
		}
		result.listVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("DataList");
	}

	return true;
}


// private methods

QString CSdlClassGqlModificatorComp::GetConvertEndForFieldString(const imtsdl::CSdlField& field, bool forType) const
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
	else if (convertedType == QStringLiteral("QByteArray")){
		retVal += QStringLiteral("ByteArray");
	}
	else if (convertedType == QStringLiteral("float")){
		retVal += QStringLiteral("Float");
	}
	else if (convertedType == QStringLiteral("double")){
		retVal += QStringLiteral("Double");
	}
	else if (convertedType == QStringLiteral("int")){
		retVal += QStringLiteral("Int");
	}
	else if (convertedType == QStringLiteral("long")){
		retVal += QStringLiteral("Long");
	}
	else if (convertedType == QStringLiteral("qlonglong")){
		retVal += QStringLiteral("LongLong");
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


CSdlUnionConverter::ConversionType CSdlClassGqlModificatorComp::GetUnionScalarConversionType() const
{
	return CT_GQL_SCALAR;
}


CSdlUnionConverter::ConversionType CSdlClassGqlModificatorComp::GetUnionArrayConversionType() const
{
	return CT_GQL_ARRAY;
}


void CSdlClassGqlModificatorComp::AddUnionFieldValueReadFromObject(QTextStream& stream, const imtsdl::CSdlField& field, bool /*optional*/, quint16 hIndents) const
{
	bool hasComplexTypes = false;
	bool hasScalarTypes = false;

	std::shared_ptr<imtsdl::CSdlEntryBase> foundEntryPtr = FindEntryByName(field.GetType());
	imtsdl::CSdlUnion* unionPtr = dynamic_cast<imtsdl::CSdlUnion*>(foundEntryPtr.get());
	Q_ASSERT(unionPtr != nullptr);
	const QStringList unionTypeList = unionPtr->GetTypes();
	for (const QString& unionType: unionTypeList){
		std::shared_ptr<imtsdl::CSdlEntryBase> foundUnionEntryPtr = FindEntryByName(unionType);
		if (foundUnionEntryPtr != nullptr){
			hasComplexTypes = true;
		}
		else {
			hasScalarTypes = true;
		}
	}

	Q_ASSERT(hasComplexTypes || hasScalarTypes);
	const QString tempDataPtrVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("DataObjectPtr");
	const QString unionSourceVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("VariantValue");
	if (hasComplexTypes){
		stream << QStringLiteral("const ");
		stream << GetContainerObjectClassName();
		stream << QStringLiteral("* ");
		stream << tempDataPtrVarName;
		stream << QStringLiteral(" = ");
		stream << GetContainerObjectVariableName();
		stream << QStringLiteral(".GetParamArgumentObjectPtr(\"");
		stream << field.GetId() << QStringLiteral("\");");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("if (!");
		stream << tempDataPtrVarName;
		stream << QStringLiteral("->ContainsParam(\"__typename\")){");
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
		stream << tempDataPtrVarName;
		stream << QStringLiteral("->GetParamArgumentValue(\"__typename\").toString();");
		FeedStream(stream, 1, false);
	}

	if (hasScalarTypes){
		if (hasComplexTypes){
			FeedStreamHorizontally(stream, hIndents);
		}
		stream << QStringLiteral("QVariant ");
		stream << unionSourceVarName;
		stream << QStringLiteral(" = ");
		stream << GetContainerObjectVariableName();
		stream << QStringLiteral("[\"");
		stream << field.GetId();
		stream << QStringLiteral("\"];");
		FeedStream(stream, 1, false);
	}

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
								 tempDataPtrVarName);
}


void CSdlClassGqlModificatorComp::AddUnionFieldValueWriteToObject(QTextStream& /*stream*/, const imtsdl::CSdlField& /*field*/, bool /*optional*/, quint16 /*hIndents*/) const
{

}

QString CSdlClassGqlModificatorComp::GetUnionListElementType(bool forScalar) const
{
	if (forScalar){
		return QStringLiteral("QVariant");
	}

	return QStringLiteral("::imtgql::CGqlParamObject*");
}


void CSdlClassGqlModificatorComp::WriteTypenameToObjectCode(QTextStream& stream, const imtsdl::CSdlType& sdlType) const 
{
	stream << GetContainerObjectVariableName();
	stream << QStringLiteral(".InsertParam(");
	stream << QStringLiteral("\"__typename\", ");
	stream << QStringLiteral("QVariant(\"") << sdlType.GetName() << "\")";
	stream << QStringLiteral(");");
}



} // namespace imtsdlgencpp

