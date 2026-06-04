#pragma once

#include "DigitalPrinter_fwd.h"

#ifdef QT_QML_LIB
#include <QtQml/QQmlEngine>
#endif // QT_QML_LIB

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
#include <imtbase/CItemModelBase.h>
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtsdl/TListModelBase.h>
#include <imtsdl/TElementList.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>

// custom types includes
#include <modsdlsdl/SDL/1.0/CPP/PrinterBase.h>


namespace sdl::V1_0::modsdl
{



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




class CDigitalPrinterSpecificationObjectList;

class CDigitalPrinterSpecificationObject: public ::imtbase::CItemModelBase, public CDigitalPrinterSpecification
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_base READ GetBase WRITE SetBase NOTIFY baseChanged)
	Q_PROPERTY(QVariant m_printingTechnology READ GetPrintingTechnology WRITE SetPrintingTechnology NOTIFY printingTechnologyChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CDigitalPrinterSpecificationObject(QObject* parent = nullptr);

	QVariant GetBase();
	void SetBase(const QVariant& v);
	Q_INVOKABLE bool hasBase();
	Q_INVOKABLE void emplaceBase();
	void ResetBase();
	QVariant GetPrintingTechnology();
	void SetPrintingTechnology(const QVariant& v);
	Q_INVOKABLE bool hasPrintingTechnology();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void baseChanged();
	void printingTechnologyChanged();

protected:
	QVariant m_baseQObjectPtr;
};





class CDigitalPrinterSpecificationObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CDigitalPrinterSpecification, sdl::V1_0::modsdl::CDigitalPrinterSpecificationObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CDigitalPrinterSpecification, sdl::V1_0::modsdl::CDigitalPrinterSpecificationObject> BaseClass;

	explicit CDigitalPrinterSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::modsdl::CDigitalPrinterSpecificationObject* item);
	Q_INVOKABLE sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::modsdl::CDigitalPrinterSpecificationObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::modsdl::CDigitalPrinterSpecificationObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::modsdl::CDigitalPrinterSpecificationObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CDigitalPrinterObjectList;

class CDigitalPrinterObject: public ::imtbase::CItemModelBase, public CDigitalPrinter
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_base READ GetBase WRITE SetBase NOTIFY baseChanged)
	Q_PROPERTY(QVariant m_printingTechnology READ GetPrintingTechnology WRITE SetPrintingTechnology NOTIFY printingTechnologyChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CDigitalPrinterObject(QObject* parent = nullptr);

	QVariant GetBase();
	void SetBase(const QVariant& v);
	Q_INVOKABLE bool hasBase();
	Q_INVOKABLE void emplaceBase();
	void ResetBase();
	QVariant GetPrintingTechnology();
	void SetPrintingTechnology(const QVariant& v);
	Q_INVOKABLE bool hasPrintingTechnology();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void baseChanged();
	void printingTechnologyChanged();

protected:
	QVariant m_baseQObjectPtr;
};





class CDigitalPrinterObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CDigitalPrinter, sdl::V1_0::modsdl::CDigitalPrinterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::modsdl::CDigitalPrinter, sdl::V1_0::modsdl::CDigitalPrinterObject> BaseClass;

	explicit CDigitalPrinterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::modsdl::CDigitalPrinterObject* item);
	Q_INVOKABLE sdl::V1_0::modsdl::CDigitalPrinterObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::modsdl::CDigitalPrinterObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::modsdl::CDigitalPrinterObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::modsdl::CDigitalPrinterObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};


#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterDigitalPrinterQmlTypes()
{
	qmlRegisterType<CDigitalPrinterSpecificationObject>("modsdlDigitalPrinterSdl", 1, 0, "DigitalPrinterSpecification");
	qmlRegisterType<CDigitalPrinterObject>("modsdlDigitalPrinterSdl", 1, 0, "DigitalPrinter");
	qmlRegisterSingletonType<EnumPrintingTechnology>("modsdlDigitalPrinterSdl", 1, 0, "PrintingTechnology", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumPrintingTechnology *enumType = new EnumPrintingTechnology();
		return enumType;
	});
}
#endif

} // namespace sdl::V1_0::modsdl


Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CDigitalPrinterSpecification);
Q_DECLARE_METATYPE(sdl::V1_0::modsdl::CDigitalPrinter);
