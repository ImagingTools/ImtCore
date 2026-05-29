#include "DigitalPrinter.h"
#include "DigitalPrinter_fwd.h"


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


bool CDigitalPrinterSpecification::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CDigitalPrinterSpecification::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* baseDataModelPtr = model.GetTreeItemModel("base", modelIndex);
	if (baseDataModelPtr != nullptr){
		base = CPrinterSpecificationBase();
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


bool CDigitalPrinterSpecification::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* baseDataModelPtr = model.GetTreeItemModel("base", modelIndex);
	if (baseDataModelPtr != nullptr){
		base = CPrinterSpecificationBase();
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


bool CDigitalPrinterSpecification::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CDigitalPrinterSpecification::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("base") && (gqlObject.GetParamArgumentObjectPtr("base") != nullptr)){
		base = CPrinterSpecificationBase();
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


bool CDigitalPrinterSpecification::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("base") && (gqlObject.GetParamArgumentObjectPtr("base") != nullptr)){
		base = CPrinterSpecificationBase();
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


bool CDigitalPrinterSpecification::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CDigitalPrinterSpecification::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("base") && jsonObject["base"].isObject()){
		base = CPrinterSpecificationBase();
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


bool CDigitalPrinterSpecification::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("base") && jsonObject["base"].isObject()){
		base = CPrinterSpecificationBase();
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


bool CDigitalPrinter::WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex) const
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


bool CDigitalPrinter::ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* baseDataModelPtr = model.GetTreeItemModel("base", modelIndex);
	if (baseDataModelPtr != nullptr){
		base = CPrinterBase();
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


bool CDigitalPrinter::OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex)
{
	::imtbase::CTreeItemModel* baseDataModelPtr = model.GetTreeItemModel("base", modelIndex);
	if (baseDataModelPtr != nullptr){
		base = CPrinterBase();
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


bool CDigitalPrinter::WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const
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


bool CDigitalPrinter::ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("base") && (gqlObject.GetParamArgumentObjectPtr("base") != nullptr)){
		base = CPrinterBase();
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


bool CDigitalPrinter::OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject)
{
	if (gqlObject.ContainsParam("base") && (gqlObject.GetParamArgumentObjectPtr("base") != nullptr)){
		base = CPrinterBase();
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


bool CDigitalPrinter::WriteToJsonObject(QJsonObject& jsonObject) const
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


bool CDigitalPrinter::ReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("base") && jsonObject["base"].isObject()){
		base = CPrinterBase();
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


bool CDigitalPrinter::OptReadFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.contains("base") && jsonObject["base"].isObject()){
		base = CPrinterBase();
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

CDigitalPrinterSpecificationObject::CDigitalPrinterSpecificationObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CDigitalPrinterSpecificationObject::baseChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDigitalPrinterSpecificationObject::printingTechnologyChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CDigitalPrinterSpecificationObject::GetBase()
{
	if (!m_baseQObjectPtr.isValid()){
		m_baseQObjectPtr = CreateObject("base");
		auto itemPtr = m_baseQObjectPtr.value<sdl::V1_0::modsdl::CPrinterSpecificationBaseObject*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CDigitalPrinterSpecification::base;
	}
	return m_baseQObjectPtr;
}


void CDigitalPrinterSpecificationObject::SetBase(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::modsdl::CPrinterSpecificationBaseObject* itemPtr = v.value<sdl::V1_0::modsdl::CPrinterSpecificationBaseObject*>();
		if (itemPtr != nullptr)  base = static_cast<const decltype(base)::value_type&>(*itemPtr);
	}
	else {
		base = nullptr;
	}
	m_baseQObjectPtr = v;

	baseChanged();
}


bool CDigitalPrinterSpecificationObject::hasBase()
{
	 return base.HasValue();
}


void CDigitalPrinterSpecificationObject::emplaceBase()
{
	ResetBase();
	base.emplace();
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
	sdl::V1_0::modsdl::PrintingTechnology valueType = printingTechnology.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::modsdl::PrintingTechnology>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CDigitalPrinterSpecificationObject::SetPrintingTechnology(const QVariant& v)
{

	printingTechnology.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::modsdl::PrintingTechnology>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		printingTechnology = (sdl::V1_0::modsdl::PrintingTechnology)key;
	}
	printingTechnologyChanged();
}


bool CDigitalPrinterSpecificationObject::hasPrintingTechnology()
{
	 return printingTechnology.HasValue();
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
		return QVariant::fromValue(new sdl::V1_0::modsdl::CPrinterSpecificationBaseObject(this));
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


void CDigitalPrinterSpecificationObjectList::append(sdl::V1_0::modsdl::CDigitalPrinterSpecificationObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList* sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList::copyMe()
{
	sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList* retVal = new sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList::addElement(sdl::V1_0::modsdl::CDigitalPrinterSpecificationObject* item)
{
	append(item);
}


void sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList::isEqualWithModel(sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList::insert(int index, sdl::V1_0::modsdl::CDigitalPrinterSpecificationObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::modsdl::CDigitalPrinterSpecificationObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_base"){
		return itemPtr->GetBase();
	}
	if (nameId == "m_printingTechnology"){
		return QVariant::fromValue(at(index)->printingTechnology.value());
	}

	return QVariant();
}
CDigitalPrinterObject::CDigitalPrinterObject(QObject* parent): ::imtbase::CItemModelBase(parent){

	QObject::connect(this, &CDigitalPrinterObject::baseChanged, this, &CItemModelBase::OnInternalModelChanged);
	QObject::connect(this, &CDigitalPrinterObject::printingTechnologyChanged, this, &CItemModelBase::OnInternalModelChanged);
}


QVariant CDigitalPrinterObject::GetBase()
{
	if (!m_baseQObjectPtr.isValid()){
		m_baseQObjectPtr = CreateObject("base");
		auto itemPtr = m_baseQObjectPtr.value<sdl::V1_0::modsdl::CPrinterBaseObject*>();
		if (itemPtr != nullptr) itemPtr->Version_1_0 = CDigitalPrinter::base;
	}
	return m_baseQObjectPtr;
}


void CDigitalPrinterObject::SetBase(const QVariant& v)
{

	if (v.isValid()){
		sdl::V1_0::modsdl::CPrinterBaseObject* itemPtr = v.value<sdl::V1_0::modsdl::CPrinterBaseObject*>();
		if (itemPtr != nullptr)  base = static_cast<const decltype(base)::value_type&>(*itemPtr);
	}
	else {
		base = nullptr;
	}
	m_baseQObjectPtr = v;

	baseChanged();
}


bool CDigitalPrinterObject::hasBase()
{
	 return base.HasValue();
}


void CDigitalPrinterObject::emplaceBase()
{
	ResetBase();
	base.emplace();
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
	sdl::V1_0::modsdl::PrintingTechnology valueType = printingTechnology.value();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::modsdl::PrintingTechnology>();
	QString retval = metaEnum.valueToKey((int)valueType);

	return retval;
}


void CDigitalPrinterObject::SetPrintingTechnology(const QVariant& v)
{

	printingTechnology.emplace();
	QMetaEnum metaEnum = QMetaEnum::fromType<sdl::V1_0::modsdl::PrintingTechnology>();
	int key = metaEnum.keyToValue(v.value<QString>().toUtf8());
	if (key > -1){
		printingTechnology = (sdl::V1_0::modsdl::PrintingTechnology)key;
	}
	printingTechnologyChanged();
}


bool CDigitalPrinterObject::hasPrintingTechnology()
{
	 return printingTechnology.HasValue();
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
		return QVariant::fromValue(new sdl::V1_0::modsdl::CPrinterBaseObject(this));
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


void CDigitalPrinterObjectList::append(sdl::V1_0::modsdl::CDigitalPrinterObject* item)
{
	BaseClass::append(item);
}


sdl::V1_0::modsdl::CDigitalPrinterObjectList* sdl::V1_0::modsdl::CDigitalPrinterObjectList::copyMe()
{
	sdl::V1_0::modsdl::CDigitalPrinterObjectList* retVal = new sdl::V1_0::modsdl::CDigitalPrinterObjectList();
	BaseClass::fromMe(retVal);
	return retVal;
}


QString sdl::V1_0::modsdl::CDigitalPrinterObjectList::toJson()
{
	return BaseClass::toJson();
}


QString sdl::V1_0::modsdl::CDigitalPrinterObjectList::toGraphQL()
{
	return BaseClass::toGraphQL();
}


void sdl::V1_0::modsdl::CDigitalPrinterObjectList::addElement(sdl::V1_0::modsdl::CDigitalPrinterObject* item)
{
	append(item);
}


void sdl::V1_0::modsdl::CDigitalPrinterObjectList::removeElement(int index)
{
	remove(index);
}


bool sdl::V1_0::modsdl::CDigitalPrinterObjectList::isEqualWithModel(sdl::V1_0::modsdl::CDigitalPrinterObjectList* otherModelPtr)
{
	return BaseClass::isEqualWithModel(otherModelPtr);
}


void sdl::V1_0::modsdl::CDigitalPrinterObjectList::insert(int index, sdl::V1_0::modsdl::CDigitalPrinterObject* item)
{
	return BaseClass::insert(index, item);
}


void sdl::V1_0::modsdl::CDigitalPrinterObjectList::remove(int index)
{
	return BaseClass::remove(index);
}


void sdl::V1_0::modsdl::CDigitalPrinterObjectList::clear()
{
	return BaseClass::clear();
}


QVariant sdl::V1_0::modsdl::CDigitalPrinterObjectList::getData(const QString& nameId, int index)
{
	QVariant item = GetOrCreateCachedObject(index);
	auto* itemPtr = item.value<sdl::V1_0::modsdl::CDigitalPrinterObject*>();
	if (itemPtr == nullptr){
		return QVariant();
	}
	if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < count()){
		return QVariant::fromValue(item);
	}
	if (nameId == "m_base"){
		return itemPtr->GetBase();
	}
	if (nameId == "m_printingTechnology"){
		return QVariant::fromValue(at(index)->printingTechnology.value());
	}

	return QVariant();
}
} // namespace sdl::V1_0::modsdl
