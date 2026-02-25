// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include "CObjectModificatorCompBase.h"


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/ISdlProcessArgumentsParser.h>
#include <imtsdl/CSdlType.h>
#include <imtsdl/CSdlEnum.h>


namespace imtsdlgencpp
{

// protected methods

// comfort methods

void CObjectModificatorCompBase::WriteMethodCall(QTextStream& stream, MetdodType type, const QString& objectName, bool isPointer) const
{
	if (!objectName.isEmpty()){
		stream << objectName;

		if (isPointer){
			stream << '-' << '>';
		}
		else {
			stream << '.';
		}
	}

	switch (type){
	case MT_OPT_READ:
		stream << QStringLiteral("Opt");
	case MT_READ:
		stream << GetCapitalizedValue(GetReadMethodName());
		break;
	case MT_OPT_WRITE:
		stream << QStringLiteral("Opt");
	case MT_WRITE:
		stream << GetCapitalizedValue(GetWriteMethodName());
		break;
	default:
		I_CRITICAL();
		break;
	}
}


std::shared_ptr<imtsdl::CSdlEntryBase> CObjectModificatorCompBase::FindEntryByName(const QString& entryName, bool onlyLocal) const
{
	imtsdl::SdlTypeList typeList = m_sdlTypeListCompPtr.IsValid() ? m_sdlTypeListCompPtr->GetSdlTypes(onlyLocal): imtsdl::SdlTypeList();
	imtsdl::SdlEnumList enumList = m_sdlEnumListCompPtr.IsValid() ? m_sdlEnumListCompPtr->GetEnums(onlyLocal) : imtsdl::SdlEnumList();
	imtsdl::SdlUnionList unionList = m_sdlUnionListCompPtr.IsValid() ? m_sdlUnionListCompPtr->GetUnions(onlyLocal) : imtsdl::SdlUnionList();

	return imtsdl::CSdlTools::FindEntryByName(entryName, typeList, enumList, unionList);
}


void CObjectModificatorCompBase::WriteSetValueToStruct(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const QString& objectName) const
{
	stream << '*';
	stream << objectName;
	stream << '.';
	stream << field.GetId();
	stream << ' ' << '=' << ' ';
}


// reimplemented (CSdlClassModificatorBaseComp)

bool CObjectModificatorCompBase::ProcessHeaderClassFile(const imtsdl::CSdlType& /*sdlType*/, QIODevice* headerDevicePtr, const iprm::IParamsSet* /*paramsPtr*/) const
{
	QTextStream ofStream(headerDevicePtr);

	// add method definitions
	// write
	FeedStreamHorizontally(ofStream, 2);
	ofStream << QStringLiteral("[[nodiscard]] bool ");
	WriteMethodCall(ofStream, MT_WRITE);
	ofStream << '(';
	ofStream << GetContainerObjectClassName();
	ofStream << '&' << ' ';
	ofStream << GetDecapitalizedValue(GetContainerObjectVariableName());
	ofStream << QStringLiteral(") const;");
	FeedStream(ofStream, 1, false);

	// read
	FeedStreamHorizontally(ofStream, 2);
	ofStream << QStringLiteral("[[nodiscard]] bool ");
	WriteMethodCall(ofStream, MT_READ);
	ofStream << QStringLiteral("(const ");
	ofStream << GetContainerObjectClassName();
	ofStream << '&' << ' ';
	ofStream << GetDecapitalizedValue(GetContainerObjectVariableName());
	ofStream << QStringLiteral(");");
	FeedStream(ofStream, 1);

	// opt read
	FeedStreamHorizontally(ofStream, 2);
	ofStream << QStringLiteral("[[nodiscard]] bool ");
	WriteMethodCall(ofStream, MT_OPT_READ);
	ofStream << QStringLiteral("(const ");
	ofStream << GetContainerObjectClassName();
	ofStream << '&' << ' ';
	ofStream << GetDecapitalizedValue(GetContainerObjectVariableName());
	ofStream << QStringLiteral(");");
	FeedStream(ofStream, 1);

	return true;
}


bool CObjectModificatorCompBase::ProcessSourceClassFile(const imtsdl::CSdlType& sdlType, QIODevice* sourceDevicePtr, const iprm::IParamsSet* /* paramsPtr */) const
{
	Q_ASSERT(m_argumentParserCompPtr.IsValid());
	Q_ASSERT(m_sdlTypeListCompPtr.IsValid());
	Q_ASSERT(m_sdlEnumListCompPtr.IsValid());
	Q_ASSERT(m_sdlUnionListCompPtr.IsValid());
	Q_ASSERT(m_originalSchemaNamespaceCompPtr.IsValid());

	QTextStream ofStream(sourceDevicePtr);

	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(sdlType, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);
	structNameConverter.addVersion = true;

	// add method implementation

	// a)write
	ofStream << QStringLiteral("bool ");
	ofStream << structNameConverter.GetString();
	ofStream << ':' << ':';
	WriteMethodCall(ofStream, MT_WRITE);
	ofStream << '(';
	ofStream << GetContainerObjectClassName();
	ofStream << '&' << ' ';
	ofStream << GetDecapitalizedValue(GetContainerObjectVariableName());
	ofStream << QStringLiteral(") const");
	FeedStream(ofStream, 1, false);

	ofStream << '{';
	FeedStream(ofStream, 1, false);

	// add write logic for each field
	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldWriteToObjectCode(ofStream, field, false);
		FeedStream(ofStream, 1, false);
	}

	// add __typename field if required
	bool addTypeName = m_argumentParserCompPtr->GetTypenameWriteMode() == imtsdl::ISdlProcessArgumentsParser::TWM_ALWAYS;
	if (addTypeName){
		FeedStreamHorizontally(ofStream);
		WriteTypenameToObjectCode(ofStream, sdlType);
		FeedStream(ofStream, 2, false);
	}
	

	// finish write implementation
	FeedStreamHorizontally(ofStream);
	ofStream << QStringLiteral("return true;");

	FeedStream(ofStream, 1, false);
	ofStream << '}';
	FeedStream(ofStream, 3, false);

	// b)read
	ofStream << QStringLiteral("bool ");
	ofStream << structNameConverter.GetString();
	ofStream << ':' << ':';
	WriteMethodCall(ofStream, MT_READ);
	ofStream << QStringLiteral("(const ");
	ofStream << GetContainerObjectClassName();
	ofStream << '&' << ' ';
	ofStream << GetDecapitalizedValue(GetContainerObjectVariableName());
	ofStream << QStringLiteral(")");
	FeedStream(ofStream, 1);

	ofStream << '{';
	FeedStream(ofStream, 1, false);

	// add read logic for each field
	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldReadFromObjectCode(ofStream, field, false);
		FeedStream(ofStream, 1, false);
	}

	// finish read implementation
	FeedStreamHorizontally(ofStream);
	ofStream << QStringLiteral("return true;");
	FeedStream(ofStream, 1, false);

	ofStream << '}';
	FeedStream(ofStream, 3, false);

	// c)opt read
	ofStream << QStringLiteral("bool ");
	ofStream << structNameConverter.GetString();
	ofStream << ':' << ':';
	WriteMethodCall(ofStream, MT_OPT_READ);
	ofStream << QStringLiteral("(const ");
	ofStream << GetContainerObjectClassName();
	ofStream << '&' << ' ';
	ofStream << GetDecapitalizedValue(GetContainerObjectVariableName());
	ofStream << QStringLiteral(")");
	FeedStream(ofStream, 1, false);

	ofStream << '{';
	FeedStream(ofStream, 1, false);

	// add OPT read logic for each field
	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldReadFromObjectCode(ofStream, field, true);
		FeedStream(ofStream, 1, false);
	}

	// finish opt read implementation
	FeedStreamHorizontally(ofStream);
	ofStream << QStringLiteral("return true;");
	FeedStream(ofStream, 1, false);

	ofStream << '}';
	FeedStream(ofStream, 3, false);

	return true;
}


// private methods

// basic helpers methods

void CObjectModificatorCompBase::AddFieldWriteToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const
{
	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;
	ConvertTypeOrEnumOrUnion(
		field,
		m_sdlEnumListCompPtr->GetEnums(false),
		m_sdlUnionListCompPtr->GetUnions(false),
		&isCustom,
		nullptr,
		&isArray,
		&isEnum,
		&isUnion);

	if ((isCustom && !isEnum && !isUnion) && isArray){
		AddCustomArrayFieldWriteToObjectCode(stream, field, optional);
	}
	else if (isCustom && !isEnum && !isUnion){
		AddCustomFieldWriteToObjectCode(stream, field, optional);
	}
	else if (isArray){
		AddArrayFieldWriteToObjectCode(stream, field, optional);
	}
	else {
		AddScalarFieldWriteToObjectCode(stream, field, optional, isEnum, isUnion);
	}
}


void CObjectModificatorCompBase::AddFieldReadFromObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const
{
	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;
	ConvertTypeOrEnumOrUnion(
		field,
		m_sdlEnumListCompPtr->GetEnums(false),
		m_sdlUnionListCompPtr->GetUnions(false),
		&isCustom,
		nullptr,
		&isArray,
		&isEnum,
		&isUnion);

	if ((isCustom && !isEnum && !isUnion) && isArray){
		AddCustomArrayFieldReadFromObjectCode(stream, field, optional);
	}
	else if ((isCustom && !isEnum && !isUnion)){
		AddCustomFieldReadFromObjectCode(stream, field, optional);
	}
	else if (isArray){
		AddArrayFieldReadFromObjectCode(stream, field, optional, isEnum, isUnion);
	}
	else {
		AddFieldValueReadFromObject(stream, field, optional, isEnum, isUnion);
	}
}


// write helpers

void CObjectModificatorCompBase::AddScalarFieldWriteToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, bool isEnum, bool isUnion) const
{
	const bool isStrict = bool(!optional && field.IsRequired());

	FeedStreamHorizontally(stream);

	stream << QStringLiteral("if (");
	stream << GetNullCheckString(field, isStrict) << QStringLiteral("){");
	FeedStream(stream, 1, false);

	if (isStrict){

		AddErrorReport(stream, QStringLiteral("Field: '%3' doesn't exist, but required"), 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;\n\t}");
		FeedStream(stream, 1, false);
	}

	const quint16 hIndents = 1 + quint16(!isStrict);
	if (isEnum){
		FeedStreamHorizontally(stream, hIndents);
		const QString enumSourceVarName = '*' + field.GetId();
		const QString enumConvertedVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("StringValue");

		// declare target value, to store value
		stream << QStringLiteral("QString ");
		stream << enumConvertedVarName << ';';
		FeedStream(stream, 1, false);

		imtsdl::CSdlEnum foundEnum;
		[[maybe_unused]] bool found = GetSdlEnumForField(field, m_sdlEnumListCompPtr->GetEnums(false), foundEnum);
		Q_ASSERT(found);

		WriteConversionFromEnum(stream, foundEnum, enumSourceVarName, enumConvertedVarName, m_originalSchemaNamespaceCompPtr->GetText(), hIndents);

		FeedStreamHorizontally(stream, hIndents);
		AddFieldValueWriteToObject(stream, field, enumConvertedVarName, hIndents);
	}
	else if (isUnion){
		const QString unionSourceVarName = field.GetId() + ".GetPtr()";
		imtsdl::CSdlUnion foundUnion;
		[[maybe_unused]] bool found = GetSdlUnionForField(field, m_sdlUnionListCompPtr->GetUnions(false), foundUnion);
		Q_ASSERT(found);

		WriteConversionFromUnion(
			stream,
			foundUnion,
			unionSourceVarName,
			field.GetId(),
			m_originalSchemaNamespaceCompPtr->GetText(),
			field.GetId(),
			QString(),
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr,
			hIndents,
			GetUnionScalarConversionType(),
			QString(),
			QString(),
			QStringLiteral("false"));
	}
	else{
		FeedStreamHorizontally(stream, hIndents);
		AddFieldValueWriteToObject(stream, field, QStringLiteral("*") + field.GetId(), hIndents);
	}

	if (!isStrict){
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream);
		stream << '}';
	}
	FeedStream(stream, 1, false);
}


/// \todo optimize it! \c AddScalarFieldWriteToObjectCode has simular code!
void CObjectModificatorCompBase::AddCustomFieldWriteToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const
{
	FeedStreamHorizontally(stream);
	if (field.IsRequired()){
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field) << QStringLiteral("){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;\n\t}");
		FeedStream(stream, 1, false);

		AddCustomFieldWriteToObjectImplCode(stream, field, optional);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);

		AddCustomFieldWriteToObjectImplCode(stream, field, optional, 2);
		FeedStream(stream, 1, false);
		stream << QStringLiteral("\t}");
		FeedStream(stream, 1, false);
	}
}


void CObjectModificatorCompBase::AddCustomFieldWriteToObjectImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool optional,
			quint16 hIndents) const
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);

	// Create an object container
	const QString objectContainerVariableName = GetDecapitalizedValue(field.GetId()) + GetCapitalizedValue(GetContainerObjectVariableName());
	FeedStreamHorizontally(stream, hIndents);
	stream << GetContainerObjectClassName();
	stream << ' ' << objectContainerVariableName;
	stream << ';';
	FeedStream(stream, 1, false);

	// Define check variable and add to model
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const bool is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Added = ");
	WriteMethodCall(stream, (optional ? MT_OPT_WRITE : MT_WRITE), field.GetId(), true);
	stream << '(';
	stream << objectContainerVariableName;
	stream << ')' << ';';
	FeedStream(stream, 1, false);

	// add checks
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Added){");
	FeedStream(stream, 1, false);

	AddErrorReport(stream, QStringLiteral("Unable to write field: '%3'"), hIndents + 1, QStringList({QString("\"%1\"").arg(field.GetId())}));

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// insert temp object to requested container
	FeedStreamHorizontally(stream, hIndents);
	AddObjectValueWriteToObject(stream, field, objectContainerVariableName, hIndents);
}


void CObjectModificatorCompBase::AddArrayFieldWriteToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const
{
	const bool isStrict = bool(!optional && field.IsRequired());
	if (isStrict){
		if (field.IsArray() && field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, true);
		}
		else if (!field.IsArray() || field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, false);
		}
		AddArrayFieldWriteToObjectImplCode(stream, field, optional);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		
		AddArrayFieldWriteToObjectImplCode(stream, field, optional, 2);
		stream << QStringLiteral("\t}");
		FeedStream(stream, 1, false);
	}
}


void CObjectModificatorCompBase::AddArrayFieldWriteToObjectImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool /*optional*/,
			quint16 hIndents) const
{
	FeedStreamHorizontally(stream, hIndents);

	// create a new array to contain custom type
	const QString newObjectArrayVarName = AddCreationOfTemporaryArray(stream, field);
	FeedStream(stream, 1, false);

	// declare loop
	FeedStreamHorizontally(stream, hIndents);
	const QString dataIndexVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	stream << QStringLiteral("for (qsizetype ") << dataIndexVarName;
	stream << QStringLiteral(" = 0; ") << dataIndexVarName;
	stream << QStringLiteral(" < ") << field.GetId();
	stream << QStringLiteral("->size(); ++") << dataIndexVarName;
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);


	bool isEnum = false;
	bool isUnion = false;
	ConvertTypeOrEnumOrUnion(
		field,
		m_sdlEnumListCompPtr->GetEnums(false),
		m_sdlUnionListCompPtr->GetUnions(false),
		nullptr,
		nullptr,
		nullptr,
		&isEnum,
		&isUnion);

	QString variableName = (field.GetId() + QStringLiteral("->at(") + dataIndexVarName + ')');

	if (isEnum){
		imtsdl::CSdlEnum sdlEnum;
		[[maybe_unused]] bool found = GetSdlEnumForField(field, m_sdlEnumListCompPtr->GetEnums(false), sdlEnum);

		const QString enumSourceVarName =  '*' + variableName;
		const QString enumConvertedVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("StringValue");
		variableName = enumConvertedVarName;

		// declare target value, to store value
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("QString ");
		stream << enumConvertedVarName << ';';
		FeedStream(stream, 1, false);

		WriteConversionFromEnum(stream, sdlEnum, enumSourceVarName, enumConvertedVarName, m_originalSchemaNamespaceCompPtr->GetText(), hIndents + 1);
	}
	else if (isUnion){
		imtsdl::CSdlUnion sdlUnion;
		[[maybe_unused]] bool found = GetSdlUnionForField(field, m_sdlUnionListCompPtr->GetUnions(false), sdlUnion);

		const QString unionSourceVarName = variableName + QStringLiteral(".GetPtr()");

		WriteConversionFromUnion(stream,
			sdlUnion,
			unionSourceVarName,
			newObjectArrayVarName,
			m_originalSchemaNamespaceCompPtr->GetText(),
			field.GetId(),
			dataIndexVarName,
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr,
			hIndents + 1,
			GetUnionArrayConversionType(),
			QString(),
			QString(),
			QStringLiteral("false"));
	}

	// inLoop: add item
	if (!isUnion){
		if (!isEnum){
			variableName.prepend('*');
		}
		FeedStreamHorizontally(stream, hIndents + 1);
		AddFieldValueAppendToObjectArray(
			stream,
			field,
			newObjectArrayVarName,
			variableName,
			hIndents);
		FeedStream(stream, 1, false);
	}

	// end of loop
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// add temp object to array container
	FeedStreamHorizontally(stream, hIndents);
	AddArrayWriteToObject(stream, field, newObjectArrayVarName, QString(), hIndents);
	FeedStream(stream, 1, false);
}


void CObjectModificatorCompBase::AddCustomArrayFieldWriteToObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const
{
	if (!optional && field.IsRequired()){
		if (field.IsArray()){
			AddArrayInternalChecksFail(stream, field, field.IsNonEmpty());
		}
		AddCustomArrayFieldWriteToObjectImplCode(stream, field, optional);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);

		AddCustomArrayFieldWriteToObjectImplCode(stream, field, optional, 2);
		stream << QStringLiteral("\n\t}");
	}
	FeedStream(stream, 1, false);
}


/// \todo optimize it! this method has same code as \c AddArrayFieldWriteToObjectImplCode (mb join them)
void CObjectModificatorCompBase:: AddCustomArrayFieldWriteToObjectImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool optional,
			quint16 hIndents) const
{
	// create a new object array to contain custom type
	FeedStreamHorizontally(stream, hIndents);
	const QString newObjectArrayVarName = AddCreationOfTemporaryArray(stream, field);
	FeedStream(stream, 1, false);

	// declare loop
	FeedStreamHorizontally(stream, hIndents);
	const QString dataIndexVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	stream << QStringLiteral("for (qsizetype ") << dataIndexVarName;
	stream << QStringLiteral(" = 0; ") << dataIndexVarName;
	stream << QStringLiteral(" < ") << field.GetId();
	stream << QStringLiteral("->size(); ++") << dataIndexVarName;
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// inLoop: create temp object to contin value
	FeedStreamHorizontally(stream, hIndents + 1);
	const QString newObjectVarName = QStringLiteral("new") + GetCapitalizedValue(field.GetId()) + GetCapitalizedValue(GetContainerObjectVariableName());
	stream << GetContainerObjectClassName();
	stream << ' ' << newObjectVarName << ';';
	FeedStream(stream, 1, false);

	// inLoop: add item and check
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!");
	WriteMethodCall(
				stream,
				(optional ? MT_OPT_WRITE : MT_WRITE),
				field.GetId() + QStringLiteral("->at(") + dataIndexVarName + ')',
				true);
	stream << '(';
	stream << newObjectVarName;
	stream << QStringLiteral(")){");
	FeedStream(stream, 1, false);

	AddErrorReport(stream, QStringLiteral("Unable to write field: '%3'"), hIndents + 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

	// inLoop: error report
	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	// write serialized object object to array
	FeedStreamHorizontally(stream, hIndents + 1);
	AddFieldValueAppendToObjectArray(stream, field, newObjectArrayVarName, newObjectVarName, hIndents);
	FeedStream(stream, 1, false);

	// end of loop
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// add temp object to array container
	FeedStreamHorizontally(stream, hIndents);
	AddArrayWriteToObject(stream, field, newObjectArrayVarName, QString(), hIndents);
}


// read helpers


void CObjectModificatorCompBase::AddFieldValueReadFromObject(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, bool isEnum, bool isUnion, quint16 hIndents) const
{
	const bool isStrict = bool(!optional && field.IsRequired());
	bool isCustom = false;
	QString convertedType = CSdlGenTools::OptListConvertTypeWithNamespaceStruct(
		field,
		m_originalSchemaNamespaceCompPtr->GetText(),
		*m_sdlTypeListCompPtr,
		*m_sdlEnumListCompPtr,
		*m_sdlUnionListCompPtr,
		true,
		&isCustom,
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		false);

	AddContainerValueCheckConditionBegin(stream, field, !isStrict, hIndents);
	FeedStream(stream, 1, false);

	if (isStrict){
		AddErrorReport(stream, QStringLiteral("Field: '%3' doesn't exist, but required"), 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	const quint16 hhIndents = hIndents + quint16(!isStrict);
	const QString enumSourceVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("StringValue");
	const QString unionSourceVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("VariantValue");
	CSdlUnionConverter::ConversionType conversionType = GetUnionScalarConversionType();

	FeedStreamHorizontally(stream, hhIndents);

	if (isUnion){
		AddUnionFieldValueReadFromObject(stream, field, optional, hhIndents);
		if(!isStrict){
			FeedStreamHorizontally(stream, 1);
			stream << '}';
			FeedStream(stream, 1, false);
		}

		return;

		if (isCustom){
			/// \todo move it to correct derived clases
			if (conversionType == CSdlUnionConverter::CT_GQL_SCALAR || conversionType == CSdlUnionConverter::CT_GQL_ARRAY){}
			else{
				stream << QStringLiteral("QJsonObject itemObject = jsonObject.value(\"");
				stream << field.GetId() <<  QStringLiteral("\").toObject();");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 1);
				stream << QStringLiteral("if (!itemObject.contains(\"__typename\")){");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 2);
				stream << QStringLiteral("qDebug() << \"invalid typename for: ") << field.GetId() <<  QStringLiteral("\";");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 2);
				stream << QStringLiteral("return false;");
				FeedStream(stream, 1, false);

				FeedStreamHorizontally(stream, 1);
				stream << '}';
			}
			FeedStream(stream, 1, false);
		}
		else{
		// declare temp value, to store variant equivalent
		stream << QStringLiteral("const QVariant ");
		stream << unionSourceVarName;
		}
	}
	else if (isEnum){
		// declare temp value, to store string equivalent
		stream << QStringLiteral("const QString ");
		stream << enumSourceVarName;
	}
	else{
		stream << field.GetId();
	}

	stream << ' ' << '=' << ' ';
	if (isUnion && isCustom){
		if (conversionType == CSdlUnionConverter::CT_GQL_SCALAR || conversionType == CSdlUnionConverter::CT_GQL_ARRAY){
		}
		else{
			stream << QStringLiteral("itemObject.value(\"__typename\").toString()");
		}
	}
	else{
		AddContainerValueReadFromObject(stream, field, QString(), hhIndents);
	}
	stream << ';';
	FeedStream(stream, 1, false);

	// finalize enum write to object
	if (isEnum){
		imtsdl::CSdlEnum foundEnum;
		[[maybe_unused]] bool found = GetSdlEnumForField(field, m_sdlEnumListCompPtr->GetEnums(false), foundEnum);
		Q_ASSERT(found);

		WriteEnumConversionFromString(stream, foundEnum, enumSourceVarName, field.GetId(), m_originalSchemaNamespaceCompPtr->GetText(), hhIndents);
	}
	else if (isUnion){
		imtsdl::CSdlUnion foundUnion;
		[[maybe_unused]] bool found = GetSdlUnionForField(field, m_sdlUnionListCompPtr->GetUnions(false), foundUnion);
		Q_ASSERT(found);

		WriteUnionConversionFromData(stream,
			foundUnion,
			unionSourceVarName,
			field.GetId(),
			m_originalSchemaNamespaceCompPtr->GetText(),
			"modelIndex",
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr,
			hhIndents,
			GetUnionScalarConversionType(),
			field.GetId(),
			QString());
	}

	if(!isStrict){
		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


/// \todo optimize it! \c AddFieldValueReadFromObject has simular code! (or not...)
void CObjectModificatorCompBase::AddCustomFieldReadFromObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const
{
	const bool isStrict = bool(!optional && field.IsRequired());

	AddContainerValueCheckConditionBegin(stream, field, !isStrict, 1);
	FeedStream(stream, 1, false);

	if (isStrict){
		AddErrorReport(stream, QStringLiteral("Field: '%3' doesn't exist, but required"), 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	AddCustomFieldReadFromObjectImplCode(stream, field, optional, 1 + quint16(!isStrict));

	if (!isStrict){
		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CObjectModificatorCompBase::AddCustomFieldReadFromObjectImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool optional,
			quint16 hIndents) const
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);

	// reset pointer for object
	const QString variableName = field.GetId();
	const QString variableCheckName = QStringLiteral("is%1Read").arg(GetCapitalizedValue(field.GetId()));

	FeedStreamHorizontally(stream, hIndents);
	stream << variableName;
	stream << QStringLiteral(" = ");
	structNameConverter.addVersion = true;
	stream << structNameConverter.GetString();
	stream << QStringLiteral("();");
	FeedStream(stream, 1, false);

	// read from object
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const bool ");
	stream << variableCheckName;
	stream << QStringLiteral(" = ");

	WriteMethodCall(stream, (optional ? MT_OPT_READ : MT_READ), variableName, true);
	stream << '(' ;
	AddContainerValueReadFromObject(stream, field, QString(), hIndents);
	stream << ')' << ';';

	FeedStream(stream, 1, false);

	// reading checks
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!");
	stream << variableCheckName;
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);

	AddErrorReport(stream, QStringLiteral("Unable to read field: '%3'"), hIndents + 1, QStringList({QString("\"%1\"").arg(field.GetId())}));

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);
}



void CObjectModificatorCompBase::AddArrayFieldReadFromObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional, bool isEnum, bool isUnion) const
{
	const bool isStrict = bool(!optional && field.IsRequired());

	AddContainerValueCheckConditionBegin(stream, field, !isStrict, 1);
	FeedStream(stream, 1, false);

	if (isStrict){
		AddErrorReport(stream, QStringLiteral("Field: '%3' doesn't exist, but required"), 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);

	}

	AddArrayFieldReadFromObjectImplCode(stream, field, optional, isEnum, isUnion, 1 + quint16(!isStrict));

	if (!isStrict){
		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CObjectModificatorCompBase::AddArrayFieldReadFromObjectImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool /*optional*/,
			bool isEnum,
			bool isUnion,
			quint16 hIndents) const
{
	ListAccessResult result;

	FeedStreamHorizontally(stream, hIndents);
	AddContainerListAccessCode(stream, field, QStringLiteral("temp") + GetCapitalizedValue(field.GetId()), hIndents, result);
	FeedStream(stream, 1, false);

	// value non empty checks
	if (field.IsNonEmpty()){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("if (") << result.listCountVariableName << QStringLiteral(" <= 0){");
		FeedStream(stream, 1, false);

		AddErrorReport(stream, QStringLiteral("Field: '%3' is empty"), hIndents, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	// reset a list value
	FeedStreamHorizontally(stream, hIndents);
	stream << field.GetId();
	stream << ' ' << '=' << ' ';
	stream << ConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr);
	stream << '(' << ')' << ';';
	FeedStream(stream, 1, false);

	//declare for loop
	const QString indexVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("for (") << result.listCountVariableType << ' ' << indexVariableName << QStringLiteral(" = 0; ");
	stream << indexVariableName << QStringLiteral(" < ") << result.listCountVariableName;
	stream << QStringLiteral("; ++") << indexVariableName << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// inLoop: declare temp var
	FeedStreamHorizontally(stream, hIndents + 1);
	if (isEnum){
		stream << QStringLiteral("const QString");
	}
	else if (isUnion){
		/// \todo refactor it

		std::shared_ptr<imtsdl::CSdlEntryBase> entryPtr = FindEntryByName(field.GetType());
		Q_ASSERT(entryPtr != nullptr);
		imtsdl::CSdlUnion* unionPtr = dynamic_cast<imtsdl::CSdlUnion*>(entryPtr.get());
		Q_ASSERT(unionPtr != nullptr);
		std::shared_ptr<imtsdl::CSdlEntryBase> unionFirstEntryPtr = FindEntryByName(unionPtr->GetTypes().constFirst());
		bool forScalar = bool(unionFirstEntryPtr == nullptr);
		stream << QStringLiteral("const ") << GetUnionListElementType(forScalar);
	}
	else{
		stream << OptListConvertTypeWithNamespace(field, QString(), *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr);
	}
	if(result.customListAccessCode.isEmpty()){
		stream << QStringLiteral(" temp") << GetCapitalizedValue(field.GetId());
		stream << ' ' << '=' << ' ';
		stream << result.listVariableName;
		stream << '[';
		stream << indexVariableName;
		stream << ']';
		if (!result.toObjectTransformMethod.isEmpty()){
			stream << result.toObjectTransformMethod;
		}
		stream << ';';
	}
	else {
		Q_ASSERT(result.customListAccessCode.contains("$(index)"));
		Q_ASSERT(!result.customAccessedElementName.isEmpty());
		stream << result.customListAccessCode.replace("$(index)", indexVariableName);
	}
	FeedStream(stream, 1, false);

	// inLoop: add temp variable to object's List
	if (isEnum){
		const QString enumSourceVarName = QStringLiteral("temp") + GetCapitalizedValue(field.GetId());
		const QString dataVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("DataValue");

		imtsdl::CSdlEnum foundEnum;
		[[maybe_unused]] bool found = GetSdlEnumForField(field, m_sdlEnumListCompPtr->GetEnums(false), foundEnum);
		Q_ASSERT(found);

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << OptListConvertTypeWithNamespace(field, m_originalSchemaNamespaceCompPtr->GetText(), *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr);
		stream << ' ' << dataVarName << ';';
		FeedStream(stream, 1, false);

		WriteEnumConversionFromString(stream, foundEnum, enumSourceVarName, dataVarName, m_originalSchemaNamespaceCompPtr->GetText(), hIndents + 1);

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << field.GetId();
		stream << QStringLiteral("->append(");
		stream << dataVarName;
		stream << ')' << ';';
		FeedStream(stream, 1, false);
	}
	else if (isUnion){
		const QString unionSourceVarName = QStringLiteral("temp") + GetCapitalizedValue(field.GetId());
		const QString dataVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("DataValue");

		imtsdl::CSdlUnion foundUnion;
		[[maybe_unused]] bool found = GetSdlUnionForField(field, m_sdlUnionListCompPtr->GetUnions(false), foundUnion);
		Q_ASSERT(found);

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << OptListConvertTypeWithNamespace(field,
			m_originalSchemaNamespaceCompPtr->GetText(),
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr);
		stream << ' ' << dataVarName << QStringLiteral(";");
		FeedStream(stream, 1, false);

		WriteUnionConversionFromData(
			stream,
			foundUnion,
			unionSourceVarName,
			dataVarName,
			m_originalSchemaNamespaceCompPtr->GetText(),
			indexVariableName,
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr,
			hIndents + 1,
			GetUnionArrayConversionType(),
			GetDecapitalizedValue(field.GetId()),
			unionSourceVarName);

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << field.GetId();
		stream << QStringLiteral("->append(");
		stream << dataVarName;
		stream << ')' << ';';
		FeedStream(stream, 1, false);
	}
	else{
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << field.GetId();
		stream << QStringLiteral("->append(");
		if (!result.customAccessedElementName.isEmpty()){
			stream << result.customAccessedElementName;
		}
		else {
			stream << QStringLiteral("temp") << GetCapitalizedValue(field.GetId());
		}
		stream << ')' << ';';
		FeedStream(stream, 1, false);
	}

	// inLoop: end
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CObjectModificatorCompBase::AddCustomArrayFieldReadFromObjectCode(QTextStream& stream, const imtsdl::CSdlField& field, bool optional) const
{
	const bool isStrict = bool(!optional && field.IsRequired());
	AddContainerValueCheckConditionBegin(stream, field, !isStrict, 1);
	FeedStream(stream, 1, false);

	if (isStrict){
		AddErrorReport(stream, QStringLiteral("Field: '%3' doesn't exist, but required"), 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	AddCustomArrayFieldReadToObjectImplCode(stream, field, optional, 1 + quint16(!isStrict));

	if (!isStrict){
		FeedStreamHorizontally(stream, 1);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CObjectModificatorCompBase:: AddCustomArrayFieldReadToObjectImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool optional,
			quint16 hIndents) const
{
	ListAccessResult result;

	FeedStreamHorizontally(stream, hIndents);
	AddContainerListAccessCode(stream, field, QString(), hIndents, result);
	FeedStream(stream, 1, false);

	// value non empty checks
	if (field.IsNonEmpty()){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("if (") << result.listCountVariableName << QStringLiteral(" <= 0){");
		FeedStream(stream, 1, false);

		AddErrorReport(stream, QStringLiteral("Field: '%3' is empty"), hIndents, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);
	structNameConverter.addVersion = true;
	structNameConverter.listWrap = true;

	// reset a list value
	FeedStreamHorizontally(stream, hIndents);
	stream << field.GetId();
	stream << ' ' << '=' << ' ';
	stream << structNameConverter.GetString();
	stream << '(' << ')' << ';';
	FeedStream(stream, 1, false);

	//declare for loop
	const QString indexVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("for (") << result.listCountVariableType << ' ' << indexVariableName << QStringLiteral(" = 0; ");
	stream << indexVariableName << QStringLiteral(" < ") << result.listCountVariableName;
	stream << QStringLiteral("; ++") << indexVariableName << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// if custom access
	if (!result.customListAccessCode.isEmpty()){
		FeedStreamHorizontally(stream, hIndents + 1);
		Q_ASSERT(result.customListAccessCode.contains("$(index)"));
		Q_ASSERT(!result.customAccessedElementName.isEmpty());
		stream << result.customListAccessCode.replace("$(index)", indexVariableName);
	}

	// inLoop: declare temp var
	FeedStreamHorizontally(stream, hIndents + 1);
	structNameConverter.listWrap = false;
	stream << structNameConverter.GetString();
	stream << QStringLiteral(" temp") << GetCapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);

	// inLoop: read and checks
	FeedStreamHorizontally(stream, hIndents + 1);

	stream << QStringLiteral("if (!");
	WriteMethodCall(stream, (!optional ? MT_READ : MT_OPT_READ), QStringLiteral("temp") + GetCapitalizedValue(field.GetId()));
	stream << '(' << result.listVariableName;
	if (result.customListAccessCode.isEmpty()){
		stream << '[';
		stream << indexVariableName;
		stream << ']';
		if (!result.toObjectTransformMethod.isEmpty()){
			stream << result.toObjectTransformMethod;
		}
	}
	stream << QStringLiteral(")){");
	FeedStream(stream, 1, false);

	AddErrorReport(stream, QStringLiteral("Unable to read field '%3'"), hIndents + 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	// inLoop: add temp variable to object's List
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << field.GetId();
	stream << QStringLiteral("->append(");
	stream <<  QStringLiteral("temp") << GetCapitalizedValue(field.GetId());
	stream << ')' << ';';
	FeedStream(stream, 1, false);

	// inLoop: end
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);
}


QString CObjectModificatorCompBase::GetUnionListElementType(bool /*forScalar*/) const
{
	return QStringLiteral("QVariant");
}


} // namespace imtsdlgencpp

