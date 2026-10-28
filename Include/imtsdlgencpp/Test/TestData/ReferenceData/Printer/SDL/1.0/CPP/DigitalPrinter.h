#pragma once

#include "DigitalPrinter_fwd.h"

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QMetaEnum>

// ACF includes
#include <istd/TNullableValue.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtsdl/TElementList.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>

// custom types includes
#include <modsdlsdl/SDL/1.0/CPP/PrinterBase.h>


namespace sdl::V1_0::modsdl
{


inline namespace DigitalPrinterSdlEnums
{



Q_NAMESPACE

enum class PrintingTechnology {
	Inkjet,
	Toner,
};

Q_ENUM_NS(PrintingTechnology)


class EnumPrintingTechnology: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString Inkjet READ GetInkjet NOTIFY InkjetChanged)
	Q_PROPERTY(QString Toner READ GetToner NOTIFY TonerChanged)

protected:
	static QString GetInkjet() { return "Inkjet"; }
	static QString GetToner() { return "Toner"; }

signals:
	void InkjetChanged();
	void TonerChanged();
};



} // inline namespace DigitalPrinterSdlEnums


class CDigitalPrinterSpecification
{
public:
	struct DigitalPrinterSpecificationFields
	{
		static const inline QString Base = "base";
		static const inline QString PrintingTechnology = "printingTechnology";
	};

	istd::TNullableValue<CPrinterSpecificationBase> base;
	istd::TNullableValue<PrintingTechnology> printingTechnology;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CDigitalPrinterSpecification& other) const;
	[[nodiscard]] bool operator!=(const CDigitalPrinterSpecification& other) const {return !(operator==(other));}
		[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
		[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
		[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

class CDigitalPrinter
{
public:
	struct DigitalPrinterFields
	{
		static const inline QString Base = "base";
		static const inline QString PrintingTechnology = "printingTechnology";
	};

	istd::TNullableValue<CPrinterBase> base;
	istd::TNullableValue<PrintingTechnology> printingTechnology;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CDigitalPrinter& other) const;
	[[nodiscard]] bool operator!=(const CDigitalPrinter& other) const {return !(operator==(other));}
		[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0) const;
		[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
		[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0);
	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject);
	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject);
};

} // namespace sdl::V1_0::modsdl


Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CDigitalPrinterSpecification);
Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CDigitalPrinter);
