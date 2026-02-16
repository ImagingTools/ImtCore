#include "DigitalPrinter.h"


namespace sdl::modsdl::DigitalPrinter
{


QByteArray CDigitalPrinterSpecification::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CDigitalPrinterSpecification::V1_0::operator==(const V1_0& other) const
{
	return 
				base == other.base &&
				printingTechnology == other.printingTechnology;
}


bool CDigitalPrinterSpecification::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "DigitalPrinterSpecification", modelIndex);


	if (base){
		::imtbase::CTreeItemModel* baseNewModelPtr = model.AddTreeModel("base", modelIndex);
		const bool isBaseAdded = base->WriteToModel(*baseNewModelPtr, 0);
		if (!isBaseAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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


bool CDigitalPrinterSpecification::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* baseDataModelPtr = model.GetTreeItemModel("base", modelIndex);
	if (baseDataModelPtr != nullptr){
		base = PrinterBase::CPrinterSpecificationBase::V1_0();
		const bool isBaseRead = base->ReadFromModel(*baseDataModelPtr, modelIndex);
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool CDigitalPrinterSpecification::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* baseDataModelPtr = model.GetTreeItemModel("base", modelIndex);
	if (baseDataModelPtr != nullptr){
		base = PrinterBase::CPrinterSpecificationBase::V1_0();
		const bool isBaseRead = base->ReadFromModel(*baseDataModelPtr, modelIndex);
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool CDigitalPrinterSpecification::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (base){
		::imtgql::CGqlParamObject baseGqlObject;
		const bool isBaseAdded = base->WriteToGraphQlObject(baseGqlObject);
		if (!isBaseAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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


bool CDigitalPrinterSpecification::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("base") && (gqlObject.GetParamArgumentObjectPtr("base") != nullptr)){
		base = PrinterBase::CPrinterSpecificationBase::V1_0();
		const bool isBaseRead = base->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("base"));
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool CDigitalPrinterSpecification::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("base") && (gqlObject.GetParamArgumentObjectPtr("base") != nullptr)){
		base = PrinterBase::CPrinterSpecificationBase::V1_0();
		const bool isBaseRead = base->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("base"));
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool CDigitalPrinterSpecification::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (base){
		QJsonObject baseJsonObject;
		const bool isBaseAdded = base->WriteToJsonObject(baseJsonObject);
		if (!isBaseAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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


bool CDigitalPrinterSpecification::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("base") && jsonObject["base"].isObject()){
		base = PrinterBase::CPrinterSpecificationBase::V1_0();
		const bool isBaseRead = base->ReadFromJsonObject(jsonObject["base"].toObject());
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool CDigitalPrinterSpecification::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("base") && jsonObject["base"].isObject()){
		base = PrinterBase::CPrinterSpecificationBase::V1_0();
		const bool isBaseRead = base->OptReadFromJsonObject(jsonObject["base"].toObject());
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


// serialize methods

bool CDigitalPrinterSpecification::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinterSpecification::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinterSpecification::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinterSpecification::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinterSpecification::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinterSpecification::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinterSpecification::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinterSpecification::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinterSpecification::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




QByteArray CDigitalPrinter::V1_0::GetVersionId()
{
	return QByteArrayLiteral("1.0");
}


bool CDigitalPrinter::V1_0::operator==(const V1_0& other) const
{
	return 
				base == other.base &&
				printingTechnology == other.printingTechnology;
}


bool CDigitalPrinter::V1_0::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
{
	model.SetData("__typename", "DigitalPrinter", modelIndex);


	if (base){
		::imtbase::CTreeItemModel* baseNewModelPtr = model.AddTreeModel("base", modelIndex);
		const bool isBaseAdded = base->WriteToModel(*baseNewModelPtr, 0);
		if (!isBaseAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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


bool CDigitalPrinter::V1_0::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* baseDataModelPtr = model.GetTreeItemModel("base", modelIndex);
	if (baseDataModelPtr != nullptr){
		base = PrinterBase::CPrinterBase::V1_0();
		const bool isBaseRead = base->ReadFromModel(*baseDataModelPtr, modelIndex);
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool CDigitalPrinter::V1_0::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* baseDataModelPtr = model.GetTreeItemModel("base", modelIndex);
	if (baseDataModelPtr != nullptr){
		base = PrinterBase::CPrinterBase::V1_0();
		const bool isBaseRead = base->ReadFromModel(*baseDataModelPtr, modelIndex);
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool CDigitalPrinter::V1_0::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
{
	if (base){
		::imtgql::CGqlParamObject baseGqlObject;
		const bool isBaseAdded = base->WriteToGraphQlObject(baseGqlObject);
		if (!isBaseAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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


bool CDigitalPrinter::V1_0::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("base") && (gqlObject.GetParamArgumentObjectPtr("base") != nullptr)){
		base = PrinterBase::CPrinterBase::V1_0();
		const bool isBaseRead = base->ReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("base"));
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool CDigitalPrinter::V1_0::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("base") && (gqlObject.GetParamArgumentObjectPtr("base") != nullptr)){
		base = PrinterBase::CPrinterBase::V1_0();
		const bool isBaseRead = base->OptReadFromGraphQlObject(*gqlObject.GetParamArgumentObjectPtr("base"));
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool CDigitalPrinter::V1_0::WriteToJsonObject(QJsonObject& jsonObject) const
{
	if (base){
		QJsonObject baseJsonObject;
		const bool isBaseAdded = base->WriteToJsonObject(baseJsonObject);
		if (!isBaseAdded){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to write field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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


bool CDigitalPrinter::V1_0::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("base") && jsonObject["base"].isObject()){
		base = PrinterBase::CPrinterBase::V1_0();
		const bool isBaseRead = base->ReadFromJsonObject(jsonObject["base"].toObject());
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


bool CDigitalPrinter::V1_0::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("base") && jsonObject["base"].isObject()){
		base = PrinterBase::CPrinterBase::V1_0();
		const bool isBaseRead = base->OptReadFromJsonObject(jsonObject["base"].toObject());
		if (!isBaseRead){
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unable to read field: '%3'").arg(__FILE__, QString::number(__LINE__), "base").toLocal8Bit().constData();)

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
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: Unexpected value for enum '%3', actual: '%4'").arg(__FILE__, QString::number(__LINE__), "PrintingTechnology", printingTechnologyStringValue);)

			return false;
		}
	}

	return true;
}


// serialize methods

bool CDigitalPrinter::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToModel(model, modelIndex);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinter::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinter::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromModel(model, modelIndex);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinter::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToGraphQlObject(gqlObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinter::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinter::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromGraphQlObject(gqlObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinter::WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version) const
{
	if (version == PV_AUTO){
		if (Version_1_0){
			return Version_1_0->WriteToJsonObject(jsonObject);
		}
		else {
			I_IF_DEBUG(qWarning() << QString("%1:%2 Error: For auto version, 'version' object not initialized.").arg(__FILE__, QString::number(__LINE__)).toLocal8Bit().constData();)

			return false;
		}
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			qCritical() << "Uninitialized version member";
			Q_ASSERT_X(false, __func__, "Uninitialized version member");

			return false;
		}

		return Version_1_0->WriteToJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinter::ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->ReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}


bool CDigitalPrinter::OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version)
{
	if (version == PV_AUTO){
		qCritical() << "AUTO protocol is NOT supported for read methods!";
		Q_ASSERT_X(false, __func__, "AUTO protocol is NOT supported for read methods!");

		return false;
	}
	else if (version == PV_1_0){
		if (!Version_1_0){
			Version_1_0 = V1_0();
		}

		return Version_1_0->OptReadFromJsonObject(jsonObject);
	}

	qCritical() << "Invalid version";
	Q_ASSERT_X(false, __func__, "Invalid version");

	return false;
}




CDigitalPrinterSpecificationObject::CDigitalPrinterSpecificationObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CDigitalPrinterSpecificationObject::baseChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDigitalPrinterSpecificationObject::printingTechnologyChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CDigitalPrinterSpecificationObject::GetBase()
{
	if (Version_1_0->base.has_value()){
		if (!m_baseQObjectPtr.isValid()){
			m_baseQObjectPtr = CreateObject("base");
			auto itemPtr = m_baseQObjectPtr.value<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->base;
		}
		return m_baseQObjectPtr;
	}

	return QVariant();
}


void CDigitalPrinterSpecificationObject::SetBase(const QVariant& v)
{
	if (v.isValid()){
		sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* itemPtr = v.value<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject*>();
		if (itemPtr != nullptr)  Version_1_0->base = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->base = nullptr;
	}
	m_baseQObjectPtr = v;

	baseChanged();
}


bool CDigitalPrinterSpecificationObject::hasBase()
{
	 return Version_1_0->base.HasValue();
}


void CDigitalPrinterSpecificationObject::emplaceBase()
{
	Version_1_0->base.emplace();

}


void CDigitalPrinterSpecificationObject::ResetBase()
{
	auto* itemPtr = m_baseQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_baseQObjectPtr = QVariant();
}


QVariant CDigitalPrinterSpecificationObject::GetPrintingTechnology()
{
	if (Version_1_0->printingTechnology.has_value()){
		sdl::modsdl::DigitalPrinter::PrintingTechnology valueType = Version_1_0->printingTechnology.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::modsdl::DigitalPrinter::PrintingTechnology>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QVariant();
}


void CDigitalPrinterSpecificationObject::SetPrintingTechnology(const QVariant& v)
{
	Version_1_0->printingTechnology.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::modsdl::DigitalPrinter::PrintingTechnology>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		Version_1_0->printingTechnology = (sdl::modsdl::DigitalPrinter::PrintingTechnology)key;
	}
	printingTechnologyChanged();
}


bool CDigitalPrinterSpecificationObject::hasPrintingTechnology()
{
	 return Version_1_0->printingTechnology.HasValue();
}


QString CDigitalPrinterSpecificationObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CDigitalPrinterSpecificationObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CDigitalPrinterSpecificationObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CDigitalPrinterSpecificationObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CDigitalPrinterSpecificationObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "base"){
		return QVariant::fromValue(new sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject(this));
	}
	return QVariant();
}


QString CDigitalPrinterSpecificationObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "base")){
		return "base";
	}
	if (propertyName == (QString("m_") + "printingTechnology")){
		return "printingTechnology";
	}

	return propertyName;
}





bool CDigitalPrinterSpecificationObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CDigitalPrinterSpecificationObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CDigitalPrinterSpecificationObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CDigitalPrinterSpecificationObjectList::append(sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* item)
{
	BaseClass::append(item);
}


sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList* sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList::copyMe()
{
	sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList* retVal = new sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList::addElement(sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* item)
{
	append(item);
}


void sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList::isEqualWithModel(sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList::insert(int index, sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_base"){
			return itemPtr->GetBase();
		}
		if (nameId == "m_printingTechnology"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->printingTechnology.value());
		}
	return QVariant();
}
CDigitalPrinterObject::CDigitalPrinterObject(QObject* parent): ::imtbase::CItemModelBase(parent){
	Version_1_0.emplace();

	QObject::connect(this, &CDigitalPrinterObject::baseChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDigitalPrinterObject::printingTechnologyChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CDigitalPrinterObject::GetBase()
{
	if (Version_1_0->base.has_value()){
		if (!m_baseQObjectPtr.isValid()){
			m_baseQObjectPtr = CreateObject("base");
			auto itemPtr = m_baseQObjectPtr.value<sdl::modsdl::PrinterBase::CPrinterBaseObject*>();
			if (itemPtr != nullptr) itemPtr->Version_1_0 = Version_1_0->base;
		}
		return m_baseQObjectPtr;
	}

	return QVariant();
}


void CDigitalPrinterObject::SetBase(const QVariant& v)
{
	if (v.isValid()){
		sdl::modsdl::PrinterBase::CPrinterBaseObject* itemPtr = v.value<sdl::modsdl::PrinterBase::CPrinterBaseObject*>();
		if (itemPtr != nullptr)  Version_1_0->base = itemPtr->Version_1_0;
	}
	else {
		Version_1_0->base = nullptr;
	}
	m_baseQObjectPtr = v;

	baseChanged();
}


bool CDigitalPrinterObject::hasBase()
{
	 return Version_1_0->base.HasValue();
}


void CDigitalPrinterObject::emplaceBase()
{
	Version_1_0->base.emplace();

}


void CDigitalPrinterObject::ResetBase()
{
	auto* itemPtr = m_baseQObjectPtr.value<QObject*>();
	if (itemPtr != nullptr){
		itemPtr->deleteLater();
	}
	m_baseQObjectPtr = QVariant();
}


QVariant CDigitalPrinterObject::GetPrintingTechnology()
{
	if (Version_1_0->printingTechnology.has_value()){
		sdl::modsdl::DigitalPrinter::PrintingTechnology valueType = Version_1_0->printingTechnology.value();
		QMetaEnum metaEnum = QMetaEnum::fromType<sdl::modsdl::DigitalPrinter::PrintingTechnology>();
		QString retval = metaEnum.valueToKey((int)valueType);

		return retval;
	}

	return QVariant();
}


void CDigitalPrinterObject::SetPrintingTechnology(const QVariant& v)
{
	Version_1_0->printingTechnology.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::modsdl::DigitalPrinter::PrintingTechnology>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		Version_1_0->printingTechnology = (sdl::modsdl::DigitalPrinter::PrintingTechnology)key;
	}
	printingTechnologyChanged();
}


bool CDigitalPrinterObject::hasPrintingTechnology()
{
	 return Version_1_0->printingTechnology.HasValue();
}


QString CDigitalPrinterObject::toJson() const
{
	QJsonObject jsonObject;
	bool res = WriteToJsonObject(jsonObject);
	if (res){
		QJsonDocument document;
		document.setObject(jsonObject);
		return document.toJson(QJsonDocument::Compact);
	}

	return QString();
}


bool CDigitalPrinterObject::createFromJson(const QString& json)
{
	QJsonDocument document = QJsonDocument::fromJson(json.toUtf8());
	return fromObject(document.object());

}


bool CDigitalPrinterObject::fromObject(const QJsonObject& jsonObject)
{
	beginChanges();
	bool res = ReadFromJsonObject(jsonObject);
	if (res){
		QVariantList changelist;
		modelChanged(changelist);
	}

	endChanges();

	finished();

	return res;
}


QString CDigitalPrinterObject::toGraphQL() const
{
	return BaseClass::toGraphQL();
}


QVariant CDigitalPrinterObject::CreateObject(const QString& key)
{
	Q_UNUSED(key);	if (key == "base"){
		return QVariant::fromValue(new sdl::modsdl::PrinterBase::CPrinterBaseObject(this));
	}
	return QVariant();
}


QString CDigitalPrinterObject::getJSONKeyForProperty(const QString& propertyName) const
{
	if (propertyName == (QString("m_") + "base")){
		return "base";
	}
	if (propertyName == (QString("m_") + "printingTechnology")){
		return "printingTechnology";
	}

	return propertyName;
}





bool CDigitalPrinterObjectList::containsKey(const QString& /*nameId*/, int /*index*/) const
{
	return true;
}


int CDigitalPrinterObjectList::getItemsCount() const
{
	return rowCount();
}


QVariantMap CDigitalPrinterObjectList::get(int row) const
{
	return BaseClass::get(row);
}


void CDigitalPrinterObjectList::append(sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* item)
{
	BaseClass::append(item);
}


sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList* sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList::copyMe()
{
	sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList* retVal = new sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList::addElement(sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* item)
{
	append(item);
}


void sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList::isEqualWithModel(sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList::insert(int index, sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::modsdl::DigitalPrinter::CDigitalPrinterObject*>();
	if (itemPtr == nullptr) return QVariant();
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
		return QVariant::fromValue(item);
	}
		if (nameId == "m_base"){
			return itemPtr->GetBase();
		}
		if (nameId == "m_printingTechnology"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index)->printingTechnology.value());
		}
	return QVariant();
}
} // namespace sdl::modsdl::DigitalPrinter
