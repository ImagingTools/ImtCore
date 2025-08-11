#include "CSdlClassTreeModelModificatorComp.h"


// Qt includes
#include <QtCore/QRegularExpression>
#include <QtCore/QTextStream>

// ImtCore includes
#include <imtsdl/CSdlType.h>
#include <imtsdlgencpp/CSdlUnionConverter.h>


namespace imtsdlgencpp
{


// protected methods

// reimplemented (CSdlClassModificatorBaseComp)

bool CSdlClassTreeModelModificatorComp::ProcessHeaderClassFile(const imtsdl::CSdlType& /*sdlType*/)
{
	if (m_headerFilePtr == nullptr){
		SendCriticalMessage(0, "Unable to process header file. Pointer is not set!");
		I_CRITICAL();

		return false;
	}

	QTextStream ofStream(m_headerFilePtr);

	// add method definitions
	ofStream << QStringLiteral("\t\t[[nodiscard]] bool WriteToModel(");
	ofStream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	ofStream << QStringLiteral("::CTreeItemModel& model, int modelIndex = 0) const;");
	FeedStream(ofStream, 1, false);

	ofStream << QStringLiteral("\t\t[[nodiscard]] bool ReadFromModel(const ");
	ofStream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	ofStream << ("::CTreeItemModel& model, int modelIndex = 0);");
	FeedStream(ofStream, 1);

	ofStream << QStringLiteral("\t\t[[nodiscard]] bool OptReadFromModel(const ");
	ofStream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	ofStream << ("::CTreeItemModel& model, int modelIndex = 0);");
	FeedStream(ofStream, 1);

	return true;
}


bool CSdlClassTreeModelModificatorComp::ProcessSourceClassFile(const imtsdl::CSdlType& sdlType)
{
	if (m_sourceFilePtr == nullptr){
		SendCriticalMessage(0, "Unable to process source file. Pointer is not set!");
		I_CRITICAL();

		return false;
	}

	QTextStream ofStream(m_sourceFilePtr);

	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(sdlType, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);
	structNameConverter.addVersion = true;

	// add method implementation
	ofStream << QStringLiteral("bool ");
	ofStream << structNameConverter.GetString();
	ofStream << QStringLiteral("::WriteToModel(");
	ofStream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	ofStream << QStringLiteral("::CTreeItemModel& model, int modelIndex) const\n{");
	FeedStream(ofStream, 1, false);

	// add write logic for each field
	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldWriteToModelCode(ofStream, field, sdlType, false);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\n\treturn true;\n}");
	FeedStream(ofStream, 3, false);

	// read method implementation
	ofStream << QStringLiteral("bool ");
	ofStream << structNameConverter.GetString();
	ofStream << QStringLiteral("::ReadFromModel(");
	ofStream << QStringLiteral("const ");
	ofStream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	ofStream << QStringLiteral("::CTreeItemModel& model, int modelIndex)\n{");
	FeedStream(ofStream, 1, false);

	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldReadFromModelCode(ofStream, field, sdlType, false);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\treturn true;\n}");
	FeedStream(ofStream, 3);

	// opt read method implementation
	ofStream << QStringLiteral("bool ");
	ofStream << structNameConverter.GetString();
	ofStream << QStringLiteral("::OptReadFromModel(");
	ofStream << QStringLiteral("const ");
	ofStream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	ofStream << QStringLiteral("::CTreeItemModel& model, int modelIndex)\n{");
	FeedStream(ofStream, 1, false);

	for (const imtsdl::CSdlField& field: sdlType.GetFields()){
		AddFieldReadFromModelCode(ofStream, field, sdlType, true);
		FeedStream(ofStream, 1, false);
	}
	ofStream << QStringLiteral("\treturn true;\n}");
	FeedStream(ofStream, 3);

	return true;
}


// private methods

void CSdlClassTreeModelModificatorComp::AddFieldWriteToModelCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const imtsdl::CSdlType& sdlType,
			bool optional)
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
		AddCustomArrayFieldWriteToModelCode(stream, field, sdlType, optional);

		return;
	}

	else if (isArray){
		AddPrimitiveArrayFieldWriteToModelCode(stream, field, sdlType, isEnum, isUnion, optional);

		return;
	}

	else if ((isCustom && !isEnum && !isUnion)){
		AddCustomFieldWriteToModelCode(stream, field, sdlType, optional);

		return;
	}

	// Process scalar value
	FeedStreamHorizontally(stream);
	if (!optional && field.IsRequired()){
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field) << QStringLiteral("){");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;\n\t}");
		FeedStream(stream, 1, false);

		QString tempListVarName;
		GenerateListTempValueCode(stream, field, tempListVarName);

		FeedStreamHorizontally(stream);

		if (isEnum){
			const QString enumSourceVarName = '*' + field.GetId();
			const QString enumConvertedVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("StringValue");

			// declare target value, to store value
			stream << QStringLiteral("QString ");
			stream << enumConvertedVarName << ';';
			FeedStream(stream, 1, false);

			imtsdl::CSdlEnum foundEnum;
			[[maybe_unused]] bool found = GetSdlEnumForField(field, m_sdlEnumListCompPtr->GetEnums(false), foundEnum);
			Q_ASSERT(found);

			WriteConversionFromEnum(stream, foundEnum, enumSourceVarName, enumConvertedVarName, m_originalSchemaNamespaceCompPtr->GetText(), 1);

			FeedStreamHorizontally(stream);
			stream << QStringLiteral("model.SetData(\"") << field.GetId() << QStringLiteral("\", ");
			stream << enumConvertedVarName;
		}
		else if (isUnion){
			const QString unionSourceVarName = QStringLiteral("*") + field.GetId();
			const QString unionConvertedVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("VariantValue");


			// declare target value, to store value
			stream << QStringLiteral("QVariant ");
			stream << unionConvertedVarName << ';';
			FeedStream(stream, 1, false);

			imtsdl::CSdlUnion foundUnion;
			[[maybe_unused]] bool found = GetSdlUnionForField(field, m_sdlUnionListCompPtr->GetUnions(false), foundUnion);

			WriteConversionFromUnion(stream,
				foundUnion,
				unionSourceVarName,
				unionConvertedVarName,
				m_originalSchemaNamespaceCompPtr->GetText(),
				field.GetId(),
				"modelIndex",
				*m_sdlTypeListCompPtr,
				*m_sdlEnumListCompPtr,
				*m_sdlUnionListCompPtr,
				2,
				CSdlUnionConverter::ConversionType::CT_MODEL_SCALAR,
				"model.SetData(");
		}
		else {
			stream << QStringLiteral("model.SetData(\"") << field.GetId() << QStringLiteral("\", ");
			if (field.IsArray()){
				stream << tempListVarName;
			}
			else {
				stream << QStringLiteral("*") << field.GetId();
			}
		}
		if (!isUnion){
			stream << QStringLiteral(", modelIndex);");
		}
	}
	else {
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);

		QString tempListVarName;
		GenerateListTempValueCode(stream, field, tempListVarName, 2);

		FeedStreamHorizontally(stream, 2);
		if (isEnum){
			const QString enumSourceVarName = QStringLiteral("*") + field.GetId();
			const QString enumConvertedVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("StringValue");

			// declare target value, to store value
			stream << QStringLiteral("QString ");
			stream << enumConvertedVarName << ';';
			FeedStream(stream, 1, false);

			imtsdl::CSdlEnum foundEnum;
			[[maybe_unused]] bool found = GetSdlEnumForField(field, m_sdlEnumListCompPtr->GetEnums(false), foundEnum);
			Q_ASSERT(found);

			WriteConversionFromEnum(stream, foundEnum, enumSourceVarName, enumConvertedVarName, m_originalSchemaNamespaceCompPtr->GetText(), 2);

			FeedStreamHorizontally(stream, 2);
			stream << QStringLiteral("model.SetData(\"") << field.GetId() << QStringLiteral("\", ");
			stream << enumConvertedVarName;
		}
		else if (isUnion){
			const QString unionSourceVarName = QStringLiteral("*") + field.GetId();
			const QString unionConvertedVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("VariantValue");


			// declare target value, to store value
			stream << QStringLiteral("QVariant ");
			stream << unionConvertedVarName << ';';
			FeedStream(stream, 1, false);

			imtsdl::CSdlUnion foundUnion;
			[[maybe_unused]] bool found = GetSdlUnionForField(field, m_sdlUnionListCompPtr->GetUnions(false), foundUnion);

			WriteConversionFromUnion(stream,
				foundUnion,
				unionSourceVarName,
				unionConvertedVarName,
				m_originalSchemaNamespaceCompPtr->GetText(),
				field.GetId(),
				"modelIndex",
				*m_sdlTypeListCompPtr,
				*m_sdlEnumListCompPtr,
				*m_sdlUnionListCompPtr,
				2,
				imtsdlgencpp::CSdlUnionConverter::ConversionType::CT_MODEL_SCALAR,
				"model.SetData(");
		}
		else {
			stream << QStringLiteral("model.SetData(\"") << field.GetId() << QStringLiteral("\", ");
			if (field.IsArray()){
				stream << tempListVarName;
			}
			else {
				stream << QStringLiteral("*") << field.GetId();
			}
		}
		if (!isUnion){
			stream << QStringLiteral(", modelIndex);\n\t}");
		}
		else{
			stream << QStringLiteral("\n\t}");
		}
	}

	FeedStream(stream, 1, false);
}


void CSdlClassTreeModelModificatorComp::AddFieldReadFromModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool optional)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();

	bool isArray = false;
	bool isCustom = false;
	bool isEnum = false;
	bool isUnion = false;
	const QString convertedType = ConvertTypeWithNamespace(
		field,
		m_originalSchemaNamespaceCompPtr->GetText(),
		*m_sdlTypeListCompPtr,
		*m_sdlEnumListCompPtr,
		*m_sdlUnionListCompPtr,
		&isCustom,
		nullptr,
		&isArray,
		&isEnum,
		&isUnion);

	if ((isCustom && !isEnum && !isUnion) && isArray){
		AddCustomArrayFieldReadFromModelCode(stream, field, sdlType, optional);

		return;
	}

	else if (isArray){
		AddPrimitiveArrayFieldReadFromModelCode(stream, field, sdlType, isEnum, isUnion, optional);

		return;
	}

	else if (isCustom && !isEnum && !isUnion){
		AddCustomFieldReadFromModelCode(stream, field, sdlType, optional);

		return;
	}

	// Process scalar value
	FeedStreamHorizontally(stream);
	stream << QStringLiteral("QVariant ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data = model.GetData(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	if (!optional && field.IsRequired()){
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.isNull()){");
		FeedStream(stream, 1, false);

		AddErrorReport(stream, QStringLiteral("Field '%3' not exists, but required"), 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		if (isEnum){
			const QString dataVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Data");
			const QString enumSourceVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("StringValue");

			// declare temp value, to store string equivalent
			stream << QStringLiteral("QString ");
			stream << enumSourceVarName;
			stream << QStringLiteral(" = ");
			stream << dataVarName;
			stream << QStringLiteral(".toString();");
			FeedStream(stream, 1, false);

			imtsdl::CSdlEnum foundEnum;
			[[maybe_unused]] bool found = GetSdlEnumForField(field, m_sdlEnumListCompPtr->GetEnums(false), foundEnum);
			Q_ASSERT(found);

			WriteEnumConversionFromString(stream,
				foundEnum,
				enumSourceVarName,
				field.GetId(),
				m_originalSchemaNamespaceCompPtr->GetText());
		}
		else if (isUnion){
			const QString dataVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Data");

			imtsdl::CSdlUnion foundUnion;
			[[maybe_unused]] bool found = GetSdlUnionForField(field, m_sdlUnionListCompPtr->GetUnions(false), foundUnion);

			FeedStreamHorizontally(stream, 1);
			stream << QStringLiteral("QString itemTypename = ") << dataVarName;
			stream << QStringLiteral(".value<::imtbase::CTreeItemModel*>()->GetData(\"__typename\").toString();");
			FeedStream(stream, 2, false);

			WriteUnionConversionFromData(stream,
				foundUnion,
				dataVarName,
				field.GetId(),
				m_originalSchemaNamespaceCompPtr->GetText(),
				"modelIndex",
				*m_sdlTypeListCompPtr,
				*m_sdlEnumListCompPtr,
				*m_sdlUnionListCompPtr);
		}
		else {
			stream << GetSettingValueString(
				field,
				sdlNamespace,
				*m_sdlTypeListCompPtr,
				GetDecapitalizedValue(field.GetId()) + QStringLiteral("Data.") + GetFromVariantConversionStringExt(field));
			FeedStream(stream, 1, false);
		}
	}
	else {
		stream << QStringLiteral("if (!") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Data.isNull()){");
		FeedStream(stream, 1, false);

		if (isEnum){
			FeedStreamHorizontally(stream, 2);
			const QString dataVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Data");
			const QString enumSourceVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("StringValue");

			// declare target value, to store value

			// declare temp value, to store string equivalent
			stream << QStringLiteral("QString ");
			stream << enumSourceVarName;
			stream << QStringLiteral(" = ");
			stream << dataVarName;
			stream << QStringLiteral(".toString();");
			FeedStream(stream, 1, false);

			imtsdl::CSdlEnum foundEnum;
			[[maybe_unused]] bool found = GetSdlEnumForField(field, m_sdlEnumListCompPtr->GetEnums(false), foundEnum);
			Q_ASSERT(found);

			WriteEnumConversionFromString(stream, foundEnum, enumSourceVarName, field.GetId(), m_originalSchemaNamespaceCompPtr->GetText(), 2);
		}
		else if (isUnion){
			const QString dataVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Data");

			imtsdl::CSdlUnion foundUnion;
			[[maybe_unused]] bool found = GetSdlUnionForField(field, m_sdlUnionListCompPtr->GetUnions(false), foundUnion);

			FeedStreamHorizontally(stream, 1);
			stream << QStringLiteral("QString itemTypename = ") << dataVarName;
			stream << QStringLiteral(".value<::imtbase::CTreeItemModel*>()->GetData(\"__typename\").toString();");
			FeedStream(stream, 2, false);

			WriteUnionConversionFromData(stream,
				foundUnion,
				dataVarName,
				field.GetId(),
				m_originalSchemaNamespaceCompPtr->GetText(),
				"modelIndex",
				*m_sdlTypeListCompPtr,
				*m_sdlEnumListCompPtr,
				*m_sdlUnionListCompPtr,
				2);
		}
		else{
			FeedStreamHorizontally(stream, 2);
			stream << GetSettingValueString(
				field,
				sdlNamespace,
				*m_sdlTypeListCompPtr,
				GetDecapitalizedValue(field.GetId()) + QStringLiteral("Data.") + GetFromVariantConversionStringExt(field));
			FeedStream(stream, 1, false);
		}

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassTreeModelModificatorComp::AddCustomFieldWriteToModelCode(QTextStream& stream, const imtsdl::CSdlField& field, const imtsdl::CSdlType& sdlType, bool optional)
{
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);
	if (!optional && field.IsRequired()){
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field) << QStringLiteral("){");
		FeedStream(stream, 1, false);
		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;\n\t}");
		FeedStream(stream, 1, false);
		AddCustomFieldWriteToModelImplCode(stream, field, sdlType, optional);
	}
	else {
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		AddCustomFieldWriteToModelImplCode(stream, field, sdlType, optional, 2);
		stream << QStringLiteral("\n\t}");
	}
}


void CSdlClassTreeModelModificatorComp::AddCustomFieldWriteToModelImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const imtsdl::CSdlType& /*sdlType*/,
			bool /*optional*/,
			quint16 hIndents)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);

	// Create a new model
	FeedStreamHorizontally(stream, hIndents);
	stream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	stream << QStringLiteral("::CTreeItemModel* ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("NewModelPtr = model.AddTreeModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);

	// Define check variable and add to model
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const bool is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Added = ");
	stream << field.GetId();
	stream << QStringLiteral("->WriteToModel(*");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("NewModelPtr, 0);");
	FeedStream(stream, 1, false);

	// add checks
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Added){");
	FeedStream(stream, 1, false);

	AddErrorReport(stream, QStringLiteral("Unable to write field '%3'"), hIndents + 1, QStringList({QString("\"%1\"").arg(field.GetId())}));

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CSdlClassTreeModelModificatorComp::AddCustomFieldReadFromModelCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const imtsdl::CSdlType& sdlType,
			bool optional)
{
	FeedStreamHorizontally(stream);

	stream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	stream << QStringLiteral("::CTreeItemModel* ");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("DataModelPtr = model.GetTreeItemModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);

	if (!optional && field.IsRequired()){
		stream << QStringLiteral("if (");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("DataModelPtr == nullptr){");
		FeedStream(stream, 1, false);

		AddErrorReport(stream, QStringLiteral("Field '%3' is missing, but required"), 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);

		AddCustomFieldReadFromModelImplCode(stream, field, sdlType, optional);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (");
		stream << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral("DataModelPtr != nullptr){");
		FeedStream(stream, 1, false);

		AddCustomFieldReadFromModelImplCode(stream, field, sdlType, optional, 2);
		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassTreeModelModificatorComp::AddCustomFieldReadFromModelImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const imtsdl::CSdlType& /*sdlType*/,
			bool /*optional*/,
			quint16 hIndents)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);

	// reset pointer for object
	FeedStreamHorizontally(stream, hIndents);
	stream << field.GetId();
	stream << QStringLiteral(" = ");
	structNameConverter.addVersion = true;
	stream << structNameConverter.GetString();
	stream << QStringLiteral("();");
	FeedStream(stream, 1, false);

	// read from model
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("const bool is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Readed = ");
	stream << field.GetId();
	stream << QStringLiteral("->ReadFromModel(");
	stream <<  QStringLiteral("*");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral("DataModelPtr, modelIndex);");	
	FeedStream(stream, 1, false);

	// reading checks
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("if (!is");
	stream << GetCapitalizedValue(field.GetId());
	stream << QStringLiteral("Readed){");
	FeedStream(stream, 1, false);

	AddErrorReport(stream, QStringLiteral("Unable to read field '%3'"), hIndents + 1, QStringList({QString("\"%1\"").arg(field.GetId())}));

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);
}


void CSdlClassTreeModelModificatorComp::AddPrimitiveArrayFieldWriteToModelCode(
	QTextStream& stream,
	const imtsdl::CSdlField& field,
	const imtsdl::CSdlType& sdlType,
	bool isEnum,
	bool isUnion,
	bool optional)
{
	if (!optional && field.IsRequired()){
		if (field.IsArray() && field.IsNonEmpty()){
			AddArrayInternalChecksFail(stream, field, true);
		}
		else if (!optional && (!field.IsArray() || field.IsNonEmpty())){
			AddArrayInternalChecksFail(stream, field, false);
		}
		AddPrimitiveArrayFieldWriteToModelImplCode(stream, field, sdlType, isEnum, isUnion, optional);
		FeedStream(stream, 1, false);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		AddPrimitiveArrayFieldWriteToModelImplCode(stream, field, sdlType, isEnum, isUnion, optional, 2);
		stream << QStringLiteral("\n\t}");
		FeedStream(stream, 1, false);
	}
}


void CSdlClassTreeModelModificatorComp::AddPrimitiveArrayFieldWriteToModelImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const imtsdl::CSdlType& /*sdlType*/,
			bool isEnum,
			bool isUnion,
			bool /*optional*/,
			quint16 hIndents)
{
	// add a new model,to store list
	const QString newTreeModelVarName = QStringLiteral("new") + GetCapitalizedValue(field.GetId()) + QStringLiteral("ModelPtr");
	FeedStreamHorizontally(stream, hIndents);
	stream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	stream << QStringLiteral("::CTreeItemModel* ") << newTreeModelVarName;
	stream << QStringLiteral(" = model.AddTreeModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << newTreeModelVarName << QStringLiteral("->setIsArray(true);");
	FeedStream(stream, 1, false);

	// declare loop
	FeedStreamHorizontally(stream, hIndents);
	const QString treeModelIndexVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	stream << QStringLiteral("for (qsizetype ") << treeModelIndexVarName;
	stream << QStringLiteral(" = 0; ") << treeModelIndexVarName;
	stream << QStringLiteral(" < ") << field.GetId();
	stream << QStringLiteral("->size(); ++") << treeModelIndexVarName;
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);

	if(isEnum){
		FeedStreamHorizontally(stream, hIndents + 1);
		const QString enumSourceVarName =  field.GetId() + QStringLiteral("->at(") + treeModelIndexVarName + ')';
		const QString enumConvertedVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("StringValue");

		// declare target value, to store value
		stream << QStringLiteral("QString ");
		stream << enumConvertedVarName << ';';
		FeedStream(stream, 1, false);

		imtsdl::CSdlEnum foundEnum;
		[[maybe_unused]] bool found = GetSdlEnumForField(field, m_sdlEnumListCompPtr->GetEnums(false), foundEnum);
		Q_ASSERT(found);

		WriteConversionFromEnum(stream, foundEnum, enumSourceVarName, enumConvertedVarName, m_originalSchemaNamespaceCompPtr->GetText(), hIndents + 1);
	}
	else if (isUnion){
		FeedStreamHorizontally(stream, hIndents + 1);
		const QString unionSourceVarName = field.GetId() + QStringLiteral("->at(") + treeModelIndexVarName + ')';
		const QString unionConvertedVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("VariantValue");

		// declare target value, to store value
		stream << QStringLiteral("QVariant ");
		stream << unionConvertedVarName << ';';
		FeedStream(stream, 1, false);

		imtsdl::CSdlUnion foundUnion;
		[[maybe_unused]] bool found = GetSdlUnionForField(field, m_sdlUnionListCompPtr->GetUnions(false), foundUnion);
		Q_ASSERT(found);

		WriteConversionFromUnion(stream, foundUnion,
			unionSourceVarName,
			unionConvertedVarName,
			m_originalSchemaNamespaceCompPtr->GetText(),
			newTreeModelVarName,
			treeModelIndexVarName,
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr,
			hIndents + 1,
			CSdlUnionConverter::ConversionType::CT_MODEL_ARRAY,
			newTreeModelVarName + QString("->SetData(QByteArray(), "));
	}

	if (!isUnion){
		// inLoop: insert ien item to model
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << newTreeModelVarName << QStringLiteral("->InsertNewItem();");
		FeedStream(stream, 1, false);

		// inLoop: add item and check
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << newTreeModelVarName << QStringLiteral("->SetData(QByteArray(), ");
		if (isEnum){
			stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("StringValue");
		}
		else{
			stream << field.GetId();
			stream << QStringLiteral("->at(") << treeModelIndexVarName << ')';
		}
		stream << ',' << ' ' << treeModelIndexVarName << ')' << ';';
		FeedStream(stream, 1, false);
	}

	// end of loop
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
}


void CSdlClassTreeModelModificatorComp::AddPrimitiveArrayFieldReadFromModelCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const imtsdl::CSdlType& /*sdlType*/,
			bool isEnum, 
			bool isUnion,
			bool optional)
{
	FeedStreamHorizontally(stream, 1);
	stream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	stream << QStringLiteral("::CTreeItemModel* ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model = model.GetTreeItemModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);

	// value checks
	if (!optional && field.IsRequired()){
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model == nullptr){");
		FeedStream(stream, 1, false);

		AddErrorReport(stream, QStringLiteral("Field '%3' is missing, but required"), 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);

		AddPrimitiveArrayFieldReadFromModelImplCode(stream, field, isEnum, isUnion, optional);
	}
	else {
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model != nullptr){");
		FeedStream(stream, 1, false);

		AddPrimitiveArrayFieldReadFromModelImplCode(stream, field, isEnum, isUnion, optional, 2);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}


void CSdlClassTreeModelModificatorComp::AddPrimitiveArrayFieldReadFromModelImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool isEnum,
			bool isUnion,
			bool /*optional*/,
			quint16 hIndents)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	const QString countVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Count");

	// declare count value
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("int ") << countVariableName << QStringLiteral(" = ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model->GetItemsCount();");
	FeedStream(stream, 1, false);

	// value checks
	if (field.IsNonEmpty()){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("if (") << countVariableName << QStringLiteral(" <= 0){");
		FeedStream(stream, 1, false);

		AddErrorReport(stream, QStringLiteral("Field '%3' is empty"), hIndents + 1, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	// declare temp list var
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, true);
	structNameConverter.addVersion = true;

	const QString listVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("List");
	FeedStreamHorizontally(stream, hIndents);
	if (isEnum || isUnion){
		stream << structNameConverter.GetString();
	}
	else{
		stream << ConvertTypeWithNamespace(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr);
	}
	stream << ' ' << listVariableName << ';';
	FeedStream(stream, 1, false);

	//declare for loop
	const QString indexVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("for (int ") << indexVariableName << QStringLiteral(" = 0; ");
	stream << indexVariableName << QStringLiteral(" < ") << countVariableName;
	stream << QStringLiteral("; ++") << indexVariableName << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// inLoop: declare temp var
	FeedStreamHorizontally(stream, hIndents + 1);
	if (isEnum){
		const QString dataVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Data");
		const QString enumSourceVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("StringValue");

		structNameConverter.listWrap = false;
		stream << structNameConverter.GetString();
		stream << ' ' << dataVarName << ';';
		FeedStream(stream, 1, false);

		// declare temp value, to store string equivalent
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("QString ");
		stream << enumSourceVarName;
		stream << QStringLiteral(" = ");
		stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model->GetData(QByteArray(), ");
		stream << indexVariableName << QStringLiteral(").toString();");
		FeedStream(stream, 1, false);

		imtsdl::CSdlEnum foundEnum;
		[[maybe_unused]] bool found = GetSdlEnumForField(field, m_sdlEnumListCompPtr->GetEnums(false), foundEnum);
		Q_ASSERT(found);

		WriteEnumConversionFromString(stream, foundEnum, enumSourceVarName, dataVarName, m_originalSchemaNamespaceCompPtr->GetText(), hIndents + 1);
	}
	else if (isUnion){
		const QString dataVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Data");
		const QString unionSourceVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("VariantValue");

		structNameConverter.listWrap = false;
		stream << structNameConverter.GetString();
		stream << ' ' << dataVarName << ';';
		FeedStream(stream, 1, false);

		// declare temp value, to store string equivalent
		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("QVariant ");
		stream << unionSourceVarName;
		stream << QStringLiteral(" = ");
		stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model->GetData(QByteArray(), ");
		stream << indexVariableName << QStringLiteral(");");
		FeedStream(stream, 1, false);

		imtsdl::CSdlUnion foundUnion;
		[[maybe_unused]] bool found = GetSdlUnionForField(field, m_sdlUnionListCompPtr->GetUnions(false), foundUnion);

		WriteUnionConversionFromData(stream,
			foundUnion,
			unionSourceVarName,
			dataVarName,
			m_originalSchemaNamespaceCompPtr->GetText(),
			indexVariableName,
			*m_sdlTypeListCompPtr,
			*m_sdlEnumListCompPtr,
			*m_sdlUnionListCompPtr,
			hIndents + 1,
			CSdlUnionConverter::ConversionType::CT_MODEL_ARRAY,
			field.GetId());
	}
	else{
		stream << OptListConvertTypeWithNamespace(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);
		stream << ' ' << GetDecapitalizedValue(field.GetId());
		stream << QStringLiteral(" = ");
		stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model->GetData(QByteArray(), ");
		stream << indexVariableName << QStringLiteral(").");
		stream << GetFromVariantConversionStringExt(field, true);
		stream << ';';
	}
	FeedStream(stream, 1, false);

	// inLoop: add variable to tempList
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << listVariableName;
	stream << QStringLiteral(" << ");
	if (isEnum || isUnion){
		stream << GetDecapitalizedValue(field.GetId()) + QStringLiteral("Data") << ';';
	}
	else{
		stream << GetDecapitalizedValue(field.GetId()) << ';';
	}
	FeedStream(stream, 1, false);

	// inLoop: end
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// set value list to object
	FeedStreamHorizontally(stream, hIndents);
	stream << GetSettingValueString(field, sdlNamespace, *m_sdlTypeListCompPtr, listVariableName);
	FeedStream(stream, 1, false);
}


void CSdlClassTreeModelModificatorComp::AddCustomArrayFieldWriteToModelCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const imtsdl::CSdlType& sdlType,
			bool optional)
{
	if (!optional && field.IsRequired()){
		if (field.IsArray()){
			AddArrayInternalChecksFail(stream, field, field.IsNonEmpty());
		}
		AddCustomArrayFieldWriteToModelImplCode(stream, field, sdlType, optional);
	}
	else {
		FeedStreamHorizontally(stream);
		stream << QStringLiteral("if (");
		stream << GetNullCheckString(field, false);
		stream << QStringLiteral("){");
		FeedStream(stream, 1, false);
		AddCustomArrayFieldWriteToModelImplCode(stream, field, sdlType, optional, 2);
		stream << QStringLiteral("\n\t}");
	}
}


void CSdlClassTreeModelModificatorComp:: AddCustomArrayFieldWriteToModelImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const imtsdl::CSdlType& /*sdlType*/,
			bool /*optional*/,
			quint16 hIndents)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);

	const QString newTreeModelVarName = QStringLiteral("new") + GetCapitalizedValue(field.GetId()) + QStringLiteral("ModelPtr");

	// add a new model,to store list
	FeedStreamHorizontally(stream, hIndents);
	stream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	stream << QStringLiteral("::CTreeItemModel* ") << newTreeModelVarName;
	stream << QStringLiteral(" = model.AddTreeModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents);
	stream << newTreeModelVarName << QStringLiteral("->setIsArray(true);");
	FeedStream(stream, 1, false);

	// declare loop
	FeedStreamHorizontally(stream, hIndents);
	const QString treeModelIndexVarName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	stream << QStringLiteral("for (qsizetype ") << treeModelIndexVarName;
	stream << QStringLiteral(" = 0; ") << treeModelIndexVarName;
	stream << QStringLiteral(" < ") << field.GetId();
	stream << QStringLiteral("->size(); ++") << treeModelIndexVarName;
	stream << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// inLoop: insert ien item to model
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << newTreeModelVarName << QStringLiteral("->InsertNewItem();");
	FeedStream(stream, 1, false);

	// inLoop: add item and check
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!(");
	stream << QStringLiteral("");
	stream << field.GetId();
	stream << QStringLiteral("->at(") << treeModelIndexVarName;
	stream << QStringLiteral(").WriteToModel(*") << newTreeModelVarName;
	stream << QStringLiteral(", ") << treeModelIndexVarName;
	stream << QStringLiteral("))){");
	FeedStream(stream, 1, false);

	// inLoop: error report
	AddErrorReport(stream, QStringLiteral("Unable to write field '%3'"), hIndents + 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';

	// end of loop
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
}


void CSdlClassTreeModelModificatorComp::AddCustomArrayFieldReadFromModelCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			const imtsdl::CSdlType& /*sdlType*/,
			bool optional)
{
	FeedStreamHorizontally(stream, 1);
	stream << GetEscapedNamespace(QStringLiteral("imtbase"), QString());
	stream << QStringLiteral("::CTreeItemModel* ") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model = model.GetTreeItemModel(");
	stream << '"' << field.GetId() << '"';
	stream << QStringLiteral(", modelIndex);");
	FeedStream(stream, 1, false);
	FeedStreamHorizontally(stream);

	// value checks
	if (!optional && (field.IsRequired())){
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model == nullptr){");
		FeedStream(stream, 1, false);

		AddErrorReport(stream, QStringLiteral("Field '%3' is missing, but required."), 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, 2);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);

		AddCustomArrayFieldReadFromModelImplCode(stream, field, optional);
		FeedStream(stream, 1, false);
	}
	else {
		stream << QStringLiteral("if (") << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model != nullptr){");
		FeedStream(stream, 1, false);

		AddCustomArrayFieldReadFromModelImplCode(stream, field, optional, 2);
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream);
		stream << '}';
		FeedStream(stream, 1, false);
	}
}

void CSdlClassTreeModelModificatorComp:: AddCustomArrayFieldReadFromModelImplCode(
			QTextStream& stream,
			const imtsdl::CSdlField& field,
			bool optional,
			quint16 hIndents)
{
	const QString sdlNamespace = m_originalSchemaNamespaceCompPtr->GetText();
	CStructNamespaceConverter structNameConverter(field, sdlNamespace, *m_sdlTypeListCompPtr, *m_sdlEnumListCompPtr, *m_sdlUnionListCompPtr, false);
	structNameConverter.addVersion = true;
	structNameConverter.listWrap = true;

	FeedStreamHorizontally(stream, hIndents);

	const QString countVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Count");
	// declare count value
	stream << QStringLiteral("int ") << countVariableName << QStringLiteral(" = ");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model->GetItemsCount();");
	FeedStream(stream, 1, false);

	// value checks
	if (field.IsNonEmpty()){
		FeedStreamHorizontally(stream, hIndents);
		stream << QStringLiteral("if (") << countVariableName << QStringLiteral(" <= 0){");
		FeedStream(stream, 1, false);

		AddErrorReport(stream, QStringLiteral("Field '%3' is empty"), hIndents, QStringList({QString("\"%1\"").arg(field.GetId())}));

		FeedStreamHorizontally(stream, hIndents + 1);
		stream << QStringLiteral("return false;");
		FeedStream(stream, 1, false);

		FeedStreamHorizontally(stream, hIndents);
		stream << '}';
		FeedStream(stream, 1, false);
	}

	// declare temp list var
	const QString listVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("List");
	FeedStreamHorizontally(stream, hIndents);
	stream << structNameConverter.GetString();
	stream << ' ' << listVariableName << ';';
	FeedStream(stream, 1, false);

	//declare for loop
	const QString indexVariableName = GetDecapitalizedValue(field.GetId()) + QStringLiteral("Index");
	FeedStreamHorizontally(stream, hIndents);
	stream << QStringLiteral("for (int ") << indexVariableName << QStringLiteral(" = 0; ");
	stream << indexVariableName << QStringLiteral(" < ") << countVariableName;
	stream << QStringLiteral("; ++") << indexVariableName << QStringLiteral("){");
	FeedStream(stream, 1, false);

	// inLoop: declare temp var
	FeedStreamHorizontally(stream, hIndents + 1);
	structNameConverter.listWrap = false;
	stream << structNameConverter.GetString();
	stream << ' ' << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);

	// inLoop: read and checks
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << QStringLiteral("if (!");
	stream << GetDecapitalizedValue(field.GetId());
	stream << QStringLiteral(".");
	if (optional){
		stream << QStringLiteral("Opt");
	}
	stream << QStringLiteral("ReadFromModel(*");
	stream << GetDecapitalizedValue(field.GetId()) << QStringLiteral("Model, ");
	stream << indexVariableName << QStringLiteral(")){");
	FeedStream(stream, 1, false);

	AddErrorReport(stream, QStringLiteral("Unable to read field '%3'"), hIndents + 2, QStringList({QString("\"%1\"").arg(field.GetId())}));

	FeedStreamHorizontally(stream, hIndents + 2);
	stream << QStringLiteral("return false;");
	FeedStream(stream, 1, false);

	FeedStreamHorizontally(stream, hIndents + 1);
	stream << '}';
	FeedStream(stream, 1, false);

	// inLoop: add variable to tempList
	FeedStreamHorizontally(stream, hIndents + 1);
	stream << listVariableName;

	stream << QStringLiteral(" << ");
	stream << GetDecapitalizedValue(field.GetId()) << ';';
	FeedStream(stream, 1, false);

	// inLoop: end
	FeedStreamHorizontally(stream, hIndents);
	stream << '}';
	FeedStream(stream, 1, false);

	// set value list to object
	FeedStreamHorizontally(stream, hIndents);
	stream << GetSettingValueString(field, sdlNamespace, *m_sdlTypeListCompPtr, listVariableName);
	FeedStream(stream, 1, false);
}

QList<imtsdl::IncludeDirective> CSdlClassTreeModelModificatorComp::GetIncludeDirectives() const
{
	static QList<imtsdl::IncludeDirective> retVal = {CreateImtDirective(QStringLiteral("<imtbase/CTreeItemModel.h>"))};

	return retVal;
}


QString CSdlClassTreeModelModificatorComp::GetReadMethodName() const
{
	return QStringLiteral("ReadFromModel");
}


QString CSdlClassTreeModelModificatorComp::GetWriteMethodName() const
{
	return QStringLiteral("WriteToModel");
}


ICxxModifier::ArgumentList CSdlClassTreeModelModificatorComp::GetArguments() const
{
	ArgumentList retVal;

	Argument arg;
	arg.Name = QStringLiteral("model");
	arg.Type = QStringLiteral("::imtbase::CTreeItemModel");
	retVal << arg;

	Argument arg2;
	arg2.Name = QStringLiteral("modelIndex");
	arg2.Type = QStringLiteral("int");
	arg2.DefaultValue = QStringLiteral("0");
	retVal << arg2;

	return retVal;
}



} // namespace imtsdlgencpp

