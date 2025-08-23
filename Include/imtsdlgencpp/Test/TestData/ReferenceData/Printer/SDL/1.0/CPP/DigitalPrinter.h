#pragma once


#ifdef QT_QML_LIB
#include <QtQml/QQmlEngine>
#endif // QT_QML_LIB

// Qt includes
#include <QtCore/QObject>
#include <QtCore/QJsonObject>
#include <QtCore/QMetaEnum>
#include <QtCore/QVariant>
#include <QtCore/QJsonValue>
#include <QtCore/QJsonArray>

// ACF includes
#include <istd/TSharedNullable.h>

// ImtCore includes
#include <imtbase/CItemModelBase.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>
#include <imtbase/TListModelBase.h>
#include <imtgql/CGqlParamObject.h>
#include <imtbase/CTreeItemModel.h>

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
	QString GetInkjet() { return "Inkjet"; }
	QString GetToner() { return "Toner"; }
signals:
	void InkjetChanged();
	void TonerChanged();
};



class CDigitalPrinterSpecification
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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


class CDigitalPrinterSpecificationObjectList;

class CDigitalPrinterSpecificationObject: public ::imtbase::CItemModelBase, public CDigitalPrinterSpecification
{
	Q_OBJECT
	Q_PROPERTY(sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* m_base READ GetBase WRITE SetBase NOTIFY baseChanged)
	Q_PROPERTY(QString m_printingTechnology READ GetPrintingTechnology WRITE SetPrintingTechnology NOTIFY printingTechnologyChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CDigitalPrinterSpecificationObject(QObject* parent = nullptr);

	sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* GetBase();
	void SetBase(sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* v);
	Q_INVOKABLE bool hasBase();
	Q_INVOKABLE void createBase();
	QString GetPrintingTechnology();
	void SetPrintingTechnology(QString v);
	Q_INVOKABLE bool hasPrintingTechnology();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void baseChanged();
	void printingTechnologyChanged();
	void finished();

protected:
	sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* m_baseQObjectPtr;
};


class CDigitalPrinterSpecificationObjectList: public ::imtbase::TListModelBase<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecification::V1_0, sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecification::V1_0, sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject> BaseClass;

	CDigitalPrinterSpecificationObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList* copyMe(){
		sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList* objectListPtr = new sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject>()){
				return nullptr;
			}

			sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* itemObjectPtr = item.value<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject>()){
				return nullptr;
			}

			sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* itemObjectPtr = item.value<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject>()){
				return nullptr;
			}

			sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* itemObjectPtr = item.value<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject>()){
				return false;
			}

			sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* selfItemObjectPtr = selfItem.value<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* otherItemObjectPtr = selfItem.value<sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_base"){
			sdl::modsdl::DigitalPrinter::CDigitalPrinterSpecificationObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetBase());
		}
		if (nameId == "m_printingTechnology"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).printingTechnology.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};
class CDigitalPrinter
{
public:
	enum ProtocolVersion
	{
		PV_AUTO = -1, ///< acceptable ONLY for write methods. Uses available member with highest version to serialize
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


class CDigitalPrinterObjectList;

class CDigitalPrinterObject: public ::imtbase::CItemModelBase, public CDigitalPrinter
{
	Q_OBJECT
	Q_PROPERTY(sdl::modsdl::PrinterBase::CPrinterBaseObject* m_base READ GetBase WRITE SetBase NOTIFY baseChanged)
	Q_PROPERTY(QString m_printingTechnology READ GetPrintingTechnology WRITE SetPrintingTechnology NOTIFY printingTechnologyChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CDigitalPrinterObject(QObject* parent = nullptr);

	sdl::modsdl::PrinterBase::CPrinterBaseObject* GetBase();
	void SetBase(sdl::modsdl::PrinterBase::CPrinterBaseObject* v);
	Q_INVOKABLE bool hasBase();
	Q_INVOKABLE void createBase();
	QString GetPrintingTechnology();
	void SetPrintingTechnology(QString v);
	Q_INVOKABLE bool hasPrintingTechnology();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void baseChanged();
	void printingTechnologyChanged();
	void finished();

protected:
	sdl::modsdl::PrinterBase::CPrinterBaseObject* m_baseQObjectPtr;
};


class CDigitalPrinterObjectList: public ::imtbase::TListModelBase<sdl::modsdl::DigitalPrinter::CDigitalPrinter::V1_0, sdl::modsdl::DigitalPrinter::CDigitalPrinterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::modsdl::DigitalPrinter::CDigitalPrinter::V1_0, sdl::modsdl::DigitalPrinter::CDigitalPrinterObject> BaseClass;

	CDigitalPrinterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/){
		return true;
	}

	Q_INVOKABLE int getItemsCount(){
		return rowCount();
	}
	Q_INVOKABLE QVariantMap get(int row) const{
		QVariantMap data;
		QModelIndex idx = index(row, 0);
		if (!idx.isValid()) return data;
		QHash<int, QByteArray> roles = roleNames();
		for (auto it = roles.begin(); it != roles.end(); ++it)
			data[it.value()] = idx.data(it.key());
		return data;
	}
	Q_INVOKABLE void append(sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList* copyMe(){
		sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList* objectListPtr = new sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::modsdl::DigitalPrinter::CDigitalPrinterObject>()){
				return nullptr;
			}

			sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* itemObjectPtr = item.value<sdl::modsdl::DigitalPrinter::CDigitalPrinterObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::modsdl::DigitalPrinter::CDigitalPrinterObject*>(itemObjectPtr->copyMe()));
		}

		return objectListPtr;
	}

	Q_INVOKABLE QString toJson(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::modsdl::DigitalPrinter::CDigitalPrinterObject>()){
				return nullptr;
			}

			sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* itemObjectPtr = item.value<sdl::modsdl::DigitalPrinter::CDigitalPrinterObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toJson();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE QString toGraphQL(){
		QString retVal = QStringLiteral("[");

		for (int i = 0; i < this->rowCount(); i++){
			if (i > 0 && i < this->rowCount() - 1){
				retVal += QStringLiteral(", ");
			}

			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::modsdl::DigitalPrinter::CDigitalPrinterObject>()){
				return nullptr;
			}

			sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* itemObjectPtr = item.value<sdl::modsdl::DigitalPrinter::CDigitalPrinterObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::DigitalPrinter::CDigitalPrinterObjectList* otherModelPtr){
		if (otherModelPtr == nullptr){
			return false;
		}

		if (this == otherModelPtr){
			return false;
		}

		if (this->rowCount() != otherModelPtr->rowCount()){
			return false;
		}

		for (int i = 0; i < this->rowCount(); i++){
			QVariant selfItem = this->getData("item", i);
			QVariant otherItem = otherModelPtr->getData("item", i);
			if (!selfItem.canConvert<sdl::modsdl::DigitalPrinter::CDigitalPrinterObject>()){
				return false;
			}

			sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* selfItemObjectPtr = selfItem.value<sdl::modsdl::DigitalPrinter::CDigitalPrinterObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* otherItemObjectPtr = selfItem.value<sdl::modsdl::DigitalPrinter::CDigitalPrinterObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* item){
		if (index < 0 || index > Version_1_0->size()) return;
		beginInsertRows(QModelIndex(), index, index);
		Version_1_0->insert(index, *item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE void remove(int index){
		if (index < 0 || index >= Version_1_0->size()) return;
		beginRemoveRows(QModelIndex(), index, index);
		Version_1_0->removeAt(index);
		ClearCache();
		endRemoveRows();
	}
	Q_INVOKABLE void clear(){
		beginResetModel();
		ClearCache();
		Version_1_0->clear();
		endResetModel();
	}
	Q_INVOKABLE QVariant getData(const QString& nameId, int index){
		if (nameId == "item" && Version_1_0.has_value() && index >= 0 && index < Version_1_0->count()){
			sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_base"){
			sdl::modsdl::DigitalPrinter::CDigitalPrinterObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetBase());
		}
		if (nameId == "m_printingTechnology"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).printingTechnology.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};
#ifdef QT_QML_LIB
static void RegisterQmlTypes()
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