#pragma once


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
#include <istd/TSharedNullable.h>

// ImtCore includes
#include <imtbase/CItemModelBase.h>
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtsdl/TListModelBase.h>
#include <imtsdl/TElementList.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>

// custom types includes
#include <modsdlsdl/SDL/1.0/CPP/PrinterBase.h>


namespace sdl::modsdl::DigitalPrinter
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



class CDigitalPrinterSpecification
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct DigitalPrinterSpecificationFields
		{
			static const inline QString Base = "base";
			static const inline QString PrintingTechnology = "printingTechnology";
		};

		istd::TSharedNullable<PrinterBase::CPrinterSpecificationBase::V1_0> base;
		istd::TSharedNullable<PrintingTechnology> printingTechnology;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
};

class CDigitalPrinter
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with the highest version to serialize
		PV_1_0,
		PV_LAST = PV_1_0
	};

	// V1_0 struct
	struct V1_0
	{
		struct DigitalPrinterFields
		{
			static const inline QString Base = "base";
			static const inline QString PrintingTechnology = "printingTechnology";
		};

		istd::TSharedNullable<PrinterBase::CPrinterBase::V1_0> base;
		istd::TSharedNullable<PrintingTechnology> printingTechnology;

		static QByteArray GetVersionId();

		[[nodiscard]] bool operator==(const V1_0& other) const;
		[[nodiscard]] bool operator!=(const V1_0& other) const {return !(operator==(other));}
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

	// available version members
	istd::TSharedNullable<V1_0> Version_1_0;

	// serialize methods
	[[nodiscard]] bool WriteToModel(::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromModel(const ::imtbase::CTreeItemModel& model, int modelIndex = 0, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToGraphQlObject(::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromGraphQlObject(const ::imtgql::CGqlParamObject& gqlObject, ProtocolVersion version = PV_LAST);

	[[nodiscard]] bool WriteToJsonObject(QJsonObject& jsonObject, ProtocolVersion version = PV_AUTO) const;
	[[nodiscard]] bool ReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
	[[nodiscard]] bool OptReadFromJsonObject(const QJsonObject& jsonObject, ProtocolVersion version = PV_LAST);
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





class CDigitalPrinterSpecificationObjectList: public ::imtsdl::TListModelBase<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecification::V1_0, sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecification::V1_0, sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject> BaseClass;

	explicit CDigitalPrinterSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* item);
	Q_INVOKABLE sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* item);
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





class CDigitalPrinterObjectList: public ::imtsdl::TListModelBase<sdl::modsdl::DigitalPrinter::CDigitalPrinter::V1_0, sdl::modsdl::DigitalPrinter::CDigitalPrinterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::modsdl::DigitalPrinter::CDigitalPrinter::V1_0, sdl::modsdl::DigitalPrinter::CDigitalPrinterObject> BaseClass;

	explicit CDigitalPrinterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* item);
	Q_INVOKABLE sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};


#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterQmlTypes()
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

} // namespace sdl::modsdl::DigitalPrinter


Q_DECLARE_METATYPE(sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecification::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecification);
Q_DECLARE_METATYPE(sdl::modsdl::DigitalPrinter::CDigitalPrinter::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::DigitalPrinter::CDigitalPrinter);
