#include "DigitalPrinter.h"


namespace sdl::V1_0::modsdl
{


QByteArray CDigitalPrinterSpecification::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CDigitalPrinterSpecification::operator==(const CDigitalPrinterSpecification& other) const
{
	return 
				base == other.base &&
				printingTechnology == other.printingTechnology;
}


bool V1_0::modsdl::CDigitalPrinterSpecification::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "DigitalPrinterSpecification", modelIndex);


	if (base){
		::imtbase::CTreeItemModel* baseNewModelPtr = model.AddTreeModel("base", modelIndex);
		const bool isBaseAdded = base->WriteToModel(*baseNewModelPtr, 0);
		if (!isBaseAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}

	}
	if (printingTechnology){
		QString printingTechnologyStringValue;
		switch (*printingTechnology){
		case PrintingTechnology::Inkjet:
			printingTechnologyStringValue = "Inkjet";
			break;
		case PrintingTechnology::Toner:
			printingTechnologyStringValue = "Toner";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
		model.SetData("printingTechnology", printingTechnologyStringValue, modelIndex);
	}


	return true;
}


bool V1_0::modsdl::CDigitalPrinterSpecification::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* baseDataModelPtr = model.GetTreeItemModel("base", modelIndex);
	if (baseDataModelPtr != nullptr){
		base = V1_0::modsdl::CPrinterSpecificationBase();
		const bool isBaseRead = base->ReadFromModel(*baseDataModelPtr, modelIndex);
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
	}

	QVariant printingTechnologyData = model.GetData("printingTechnology", modelIndex);
	if (!printingTechnologyData.isNull()){
		QString printingTechnologyStringValue = printingTechnologyData.toString();
		if(printingTechnologyStringValue == "Inkjet"){
			printingTechnology = PrintingTechnology::Inkjet;
		}
		else if(printingTechnologyStringValue == "Toner"){
			printingTechnology = PrintingTechnology::Toner;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool V1_0::modsdl::CDigitalPrinterSpecification::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* baseDataModelPtr = model.GetTreeItemModel("base", modelIndex);
	if (baseDataModelPtr != nullptr){
		base = V1_0::modsdl::CPrinterSpecificationBase();
		const bool isBaseRead = base->ReadFromModel(*baseDataModelPtr, modelIndex);
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
	}

	QVariant printingTechnologyData = model.GetData("printingTechnology", modelIndex);
	if (!printingTechnologyData.isNull()){
		QString printingTechnologyStringValue = printingTechnologyData.toString();
		if(printingTechnologyStringValue == "Inkjet"){
			printingTechnology = PrintingTechnology::Inkjet;
		}
		else if(printingTechnologyStringValue == "Toner"){
			printingTechnology = PrintingTechnology::Toner;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool V1_0::modsdl::CDigitalPrinterSpecification::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (base){
		::imtgql::CGqlParamObject baseGqlObject;
		const bool isBaseAdded = base->WriteToGraphQlObject(baseGqlObject);
		if (!isBaseAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("base", baseGqlObject);
	}

	if (printingTechnology){
		QString printingTechnologyStringValue;
		switch (*printingTechnology){
		case PrintingTechnology::Inkjet:
			printingTechnologyStringValue = "Inkjet";
			break;
		case PrintingTechnology::Toner:
			printingTechnologyStringValue = "Toner";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
		gqlObject.InsertParam("printingTechnology", QVariant(printingTechnologyStringValue));
	}

	gqlObject.InsertParam("__typename", QVariant("DigitalPrinterSpecification"));

	return true;
}


bool V1_0::modsdl::CDigitalPrinterSpecification::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("base") && (gqlObject.GetParamArgumentObjectPtr("base") != nullptr)){
		base = V1_0::modsdl::CPrinterSpecificationBase();
		const bool isBaseRead = base->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("base"));
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("printingTechnology") && (gqlObject["printingTechnology"].userType() == QMetaType::QString || gqlObject["printingTechnology"].userType() == QMetaType::QByteArray)){
		const QString printingTechnologyStringValue = gqlObject["printingTechnology"].toString();
		if(printingTechnologyStringValue == "Inkjet"){
			printingTechnology = PrintingTechnology::Inkjet;
		}
		else if(printingTechnologyStringValue == "Toner"){
			printingTechnology = PrintingTechnology::Toner;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool V1_0::modsdl::CDigitalPrinterSpecification::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("base") && (gqlObject.GetParamArgumentObjectPtr("base") != nullptr)){
		base = V1_0::modsdl::CPrinterSpecificationBase();
		const bool isBaseRead = base->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("base"));
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("printingTechnology") && (gqlObject["printingTechnology"].userType() == QMetaType::QString || gqlObject["printingTechnology"].userType() == QMetaType::QByteArray)){
		const QString printingTechnologyStringValue = gqlObject["printingTechnology"].toString();
		if(printingTechnologyStringValue == "Inkjet"){
			printingTechnology = PrintingTechnology::Inkjet;
		}
		else if(printingTechnologyStringValue == "Toner"){
			printingTechnology = PrintingTechnology::Toner;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool V1_0::modsdl::CDigitalPrinterSpecification::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (base){
		QJsonObject baseJsonObject;
		const bool isBaseAdded = base->WriteToJsonObject(baseJsonObject);
		if (!isBaseAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["base"] = baseJsonObject;
	}

	if (printingTechnology){
		QString printingTechnologyStringValue;
		switch (*printingTechnology){
		case PrintingTechnology::Inkjet:
			printingTechnologyStringValue = "Inkjet";
			break;
		case PrintingTechnology::Toner:
			printingTechnologyStringValue = "Toner";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
		jsonObject["printingTechnology"] = QJsonValue::fromVariant(printingTechnologyStringValue);
	}

	jsonObject["__typename"] = "DigitalPrinterSpecification";

	return true;
}


bool V1_0::modsdl::CDigitalPrinterSpecification::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("base") && jsonObject["base"].isObject()){
		base = V1_0::modsdl::CPrinterSpecificationBase();
		const bool isBaseRead = base->ReadFromJsonObject(jsonObject["base"].toObject());
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("printingTechnology") && jsonObject["printingTechnology"].isString()){
		const QString printingTechnologyStringValue = jsonObject["printingTechnology"].toString();
		if(printingTechnologyStringValue == "Inkjet"){
			printingTechnology = PrintingTechnology::Inkjet;
		}
		else if(printingTechnologyStringValue == "Toner"){
			printingTechnology = PrintingTechnology::Toner;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool V1_0::modsdl::CDigitalPrinterSpecification::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("base") && jsonObject["base"].isObject()){
		base = V1_0::modsdl::CPrinterSpecificationBase();
		const bool isBaseRead = base->OptReadFromJsonObject(jsonObject["base"].toObject());
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("printingTechnology") && jsonObject["printingTechnology"].isString()){
		const QString printingTechnologyStringValue = jsonObject["printingTechnology"].toString();
		if(printingTechnologyStringValue == "Inkjet"){
			printingTechnology = PrintingTechnology::Inkjet;
		}
		else if(printingTechnologyStringValue == "Toner"){
			printingTechnology = PrintingTechnology::Toner;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}




QByteArray CDigitalPrinter::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CDigitalPrinter::operator==(const CDigitalPrinter& other) const
{
	return 
				base == other.base &&
				printingTechnology == other.printingTechnology;
}


bool V1_0::modsdl::CDigitalPrinter::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "DigitalPrinter", modelIndex);


	if (base){
		::imtbase::CTreeItemModel* baseNewModelPtr = model.AddTreeModel("base", modelIndex);
		const bool isBaseAdded = base->WriteToModel(*baseNewModelPtr, 0);
		if (!isBaseAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}

	}
	if (printingTechnology){
		QString printingTechnologyStringValue;
		switch (*printingTechnology){
		case PrintingTechnology::Inkjet:
			printingTechnologyStringValue = "Inkjet";
			break;
		case PrintingTechnology::Toner:
			printingTechnologyStringValue = "Toner";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
		model.SetData("printingTechnology", printingTechnologyStringValue, modelIndex);
	}


	return true;
}


bool V1_0::modsdl::CDigitalPrinter::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* baseDataModelPtr = model.GetTreeItemModel("base", modelIndex);
	if (baseDataModelPtr != nullptr){
		base = V1_0::modsdl::CPrinterBase();
		const bool isBaseRead = base->ReadFromModel(*baseDataModelPtr, modelIndex);
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
	}

	QVariant printingTechnologyData = model.GetData("printingTechnology", modelIndex);
	if (!printingTechnologyData.isNull()){
		QString printingTechnologyStringValue = printingTechnologyData.toString();
		if(printingTechnologyStringValue == "Inkjet"){
			printingTechnology = PrintingTechnology::Inkjet;
		}
		else if(printingTechnologyStringValue == "Toner"){
			printingTechnology = PrintingTechnology::Toner;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool V1_0::modsdl::CDigitalPrinter::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* baseDataModelPtr = model.GetTreeItemModel("base", modelIndex);
	if (baseDataModelPtr != nullptr){
		base = V1_0::modsdl::CPrinterBase();
		const bool isBaseRead = base->ReadFromModel(*baseDataModelPtr, modelIndex);
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
	}

	QVariant printingTechnologyData = model.GetData("printingTechnology", modelIndex);
	if (!printingTechnologyData.isNull()){
		QString printingTechnologyStringValue = printingTechnologyData.toString();
		if(printingTechnologyStringValue == "Inkjet"){
			printingTechnology = PrintingTechnology::Inkjet;
		}
		else if(printingTechnologyStringValue == "Toner"){
			printingTechnology = PrintingTechnology::Toner;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool V1_0::modsdl::CDigitalPrinter::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (base){
		::imtgql::CGqlParamObject baseGqlObject;
		const bool isBaseAdded = base->WriteToGraphQlObject(baseGqlObject);
		if (!isBaseAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
		gqlObject.InsertParam("base", baseGqlObject);
	}

	if (printingTechnology){
		QString printingTechnologyStringValue;
		switch (*printingTechnology){
		case PrintingTechnology::Inkjet:
			printingTechnologyStringValue = "Inkjet";
			break;
		case PrintingTechnology::Toner:
			printingTechnologyStringValue = "Toner";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
		gqlObject.InsertParam("printingTechnology", QVariant(printingTechnologyStringValue));
	}

	gqlObject.InsertParam("__typename", QVariant("DigitalPrinter"));

	return true;
}


bool V1_0::modsdl::CDigitalPrinter::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("base") && (gqlObject.GetParamArgumentObjectPtr("base") != nullptr)){
		base = V1_0::modsdl::CPrinterBase();
		const bool isBaseRead = base->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("base"));
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("printingTechnology") && (gqlObject["printingTechnology"].userType() == QMetaType::QString || gqlObject["printingTechnology"].userType() == QMetaType::QByteArray)){
		const QString printingTechnologyStringValue = gqlObject["printingTechnology"].toString();
		if(printingTechnologyStringValue == "Inkjet"){
			printingTechnology = PrintingTechnology::Inkjet;
		}
		else if(printingTechnologyStringValue == "Toner"){
			printingTechnology = PrintingTechnology::Toner;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool V1_0::modsdl::CDigitalPrinter::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("base") && (gqlObject.GetParamArgumentObjectPtr("base") != nullptr)){
		base = V1_0::modsdl::CPrinterBase();
		const bool isBaseRead = base->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("base"));
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
	}

	if (gqlObject.ContainsParam("printingTechnology") && (gqlObject["printingTechnology"].userType() == QMetaType::QString || gqlObject["printingTechnology"].userType() == QMetaType::QByteArray)){
		const QString printingTechnologyStringValue = gqlObject["printingTechnology"].toString();
		if(printingTechnologyStringValue == "Inkjet"){
			printingTechnology = PrintingTechnology::Inkjet;
		}
		else if(printingTechnologyStringValue == "Toner"){
			printingTechnology = PrintingTechnology::Toner;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool V1_0::modsdl::CDigitalPrinter::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (base){
		QJsonObject baseJsonObject;
		const bool isBaseAdded = base->WriteToJsonObject(baseJsonObject);
		if (!isBaseAdded){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
		jsonObject["base"] = baseJsonObject;
	}

	if (printingTechnology){
		QString printingTechnologyStringValue;
		switch (*printingTechnology){
		case PrintingTechnology::Inkjet:
			printingTechnologyStringValue = "Inkjet";
			break;
		case PrintingTechnology::Toner:
			printingTechnologyStringValue = "Toner";
			break;
		default:
			Q_ASSERT(false);
			break;
		}
		jsonObject["printingTechnology"] = QJsonValue::fromVariant(printingTechnologyStringValue);
	}

	jsonObject["__typename"] = "DigitalPrinter";

	return true;
}


bool V1_0::modsdl::CDigitalPrinter::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("base") && jsonObject["base"].isObject()){
		base = V1_0::modsdl::CPrinterBase();
		const bool isBaseRead = base->ReadFromJsonObject(jsonObject["base"].toObject());
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("printingTechnology") && jsonObject["printingTechnology"].isString()){
		const QString printingTechnologyStringValue = jsonObject["printingTechnology"].toString();
		if(printingTechnologyStringValue == "Inkjet"){
			printingTechnology = PrintingTechnology::Inkjet;
		}
		else if(printingTechnologyStringValue == "Toner"){
			printingTechnology = PrintingTechnology::Toner;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool V1_0::modsdl::CDigitalPrinter::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("base") && jsonObject["base"].isObject()){
		base = V1_0::modsdl::CPrinterBase();
		const bool isBaseRead = base->OptReadFromJsonObject(jsonObject["base"].toObject());
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

			return false;
		}
	}

	if (jsonObject.contains("printingTechnology") && jsonObject["printingTechnology"].isString()){
		const QString printingTechnologyStringValue = jsonObject["printingTechnology"].toString();
		if(printingTechnologyStringValue == "Inkjet"){
			printingTechnology = PrintingTechnology::Inkjet;
		}
		else if(printingTechnologyStringValue == "Toner"){
			printingTechnology = PrintingTechnology::Toner;
		}
		else {
			I_IF_DEBUG(qWarning() << QStringLiteral("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}




} // namespace sdl::V1_0::modsdl
