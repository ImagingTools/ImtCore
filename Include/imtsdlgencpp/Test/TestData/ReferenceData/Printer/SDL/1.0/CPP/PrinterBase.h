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
#include <imtbase/TListModelBase.h>
#include <imtservergql/CPermissibleGqlRequestHandlerComp.h>


namespace sdl::modsdl::PrinterBase
{


Q_NAMESPACE

// forward declaration for union types
class PrinterSpecification;
class CPrinterSpecificationObject;
class CPrinterSpecificationObjectList;
class SimpleUnion;
class CSimpleUnionObject;
class CSimpleUnionObjectList;
class MixedUnion;
class CMixedUnionObject;
class CMixedUnionObjectList;

class CPrinterSpecificationBase
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
		struct PrinterSpecificationBaseFields
		{
			static const inline QString Name = "name";
		};

		istd::TSharedNullable<QString> name;

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

class CLink
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
		struct LinkFields
		{
			static const inline QString Link = "link";
		};

		istd::TSharedNullable<QByteArray> link;

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

class CPrinterBase
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
		struct PrinterBaseFields
		{
			static const inline QString Name = "name";
			static const inline QString Specification = "specification";
			static const inline QString SimpleTest = "simpleTest";
			static const inline QString MixedTest = "mixedTest";
		};

		istd::TSharedNullable<QString> name;
		istd::TSharedNullable<std::shared_ptr<PrinterSpecification>> specification;
		istd::TSharedNullable<std::shared_ptr<SimpleUnion>> simpleTest;
		istd::TSharedNullable<std::shared_ptr<MixedUnion>> mixedTest;

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

class CPrinterList
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
		struct PrinterListFields
		{
			static const inline QString Data = "data";
		};

		istd::TSharedNullable<QList<CPrinterBase::V1_0>> data;

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




class CPrinterSpecificationBaseObjectList;

class CPrinterSpecificationBaseObject: public ::imtbase::CItemModelBase, public CPrinterSpecificationBase
{
	Q_OBJECT
	Q_PROPERTY(QString m_name READ GetName WRITE SetName NOTIFY nameChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPrinterSpecificationBaseObject(QObject* parent = nullptr);

	QString GetName();
	void SetName(QString v);
	Q_INVOKABLE bool hasName();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void nameChanged();
	void finished();

protected:
};


class CPrinterSpecificationBaseObjectList: public ::imtbase::TListModelBase<sdl::modsdl::PrinterBase::CPrinterSpecificationBase::V1_0, sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::modsdl::PrinterBase::CPrinterSpecificationBase::V1_0, sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject> BaseClass;

	CPrinterSpecificationBaseObjectList(QObject* parent = nullptr): BaseClass(parent) {}

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
	Q_INVOKABLE void append(sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList* copyMe(){
		sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList* objectListPtr = new sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject>()){
				return nullptr;
			}

			sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* itemObjectPtr = item.value<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject*>(itemObjectPtr->copyMe()));
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
			if (!item.canConvert<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject>()){
				return nullptr;
			}

			sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* itemObjectPtr = item.value<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject*>();
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
			if (!item.canConvert<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject>()){
				return nullptr;
			}

			sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* itemObjectPtr = item.value<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObjectList* otherModelPtr){
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
			if (!selfItem.canConvert<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject>()){
				return false;
			}

			sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* selfItemObjectPtr = selfItem.value<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* otherItemObjectPtr = selfItem.value<sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* item){
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
			sdl::modsdl::PrinterBase::CPrinterSpecificationBaseObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_name"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).name.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CLinkObjectList;

class CLinkObject: public ::imtbase::CItemModelBase, public CLink
{
	Q_OBJECT
	Q_PROPERTY(QString m_link READ GetLink WRITE SetLink NOTIFY linkChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CLinkObject(QObject* parent = nullptr);

	QString GetLink();
	void SetLink(QString v);
	Q_INVOKABLE bool hasLink();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void linkChanged();
	void finished();

protected:
};


class CLinkObjectList: public ::imtbase::TListModelBase<sdl::modsdl::PrinterBase::CLink::V1_0, sdl::modsdl::PrinterBase::CLinkObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::modsdl::PrinterBase::CLink::V1_0, sdl::modsdl::PrinterBase::CLinkObject> BaseClass;

	CLinkObjectList(QObject* parent = nullptr): BaseClass(parent) {}

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
	Q_INVOKABLE void append(sdl::modsdl::PrinterBase::CLinkObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::modsdl::PrinterBase::CLinkObjectList* copyMe(){
		sdl::modsdl::PrinterBase::CLinkObjectList* objectListPtr = new sdl::modsdl::PrinterBase::CLinkObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::modsdl::PrinterBase::CLinkObject>()){
				return nullptr;
			}

			sdl::modsdl::PrinterBase::CLinkObject* itemObjectPtr = item.value<sdl::modsdl::PrinterBase::CLinkObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::modsdl::PrinterBase::CLinkObject*>(itemObjectPtr->copyMe()));
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
			if (!item.canConvert<sdl::modsdl::PrinterBase::CLinkObject>()){
				return nullptr;
			}

			sdl::modsdl::PrinterBase::CLinkObject* itemObjectPtr = item.value<sdl::modsdl::PrinterBase::CLinkObject*>();
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
			if (!item.canConvert<sdl::modsdl::PrinterBase::CLinkObject>()){
				return nullptr;
			}

			sdl::modsdl::PrinterBase::CLinkObject* itemObjectPtr = item.value<sdl::modsdl::PrinterBase::CLinkObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::modsdl::PrinterBase::CLinkObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::PrinterBase::CLinkObjectList* otherModelPtr){
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
			if (!selfItem.canConvert<sdl::modsdl::PrinterBase::CLinkObject>()){
				return false;
			}

			sdl::modsdl::PrinterBase::CLinkObject* selfItemObjectPtr = selfItem.value<sdl::modsdl::PrinterBase::CLinkObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::modsdl::PrinterBase::CLinkObject* otherItemObjectPtr = selfItem.value<sdl::modsdl::PrinterBase::CLinkObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::modsdl::PrinterBase::CLinkObject* item){
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
			sdl::modsdl::PrinterBase::CLinkObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_link"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).link.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CPrinterBaseObjectList;

class CPrinterBaseObject: public ::imtbase::CItemModelBase, public CPrinterBase
{
	Q_OBJECT
	Q_PROPERTY(QString m_name READ GetName WRITE SetName NOTIFY nameChanged)
	Q_PROPERTY(sdl::modsdl::PrinterBase::CPrinterSpecificationObject* m_specification READ GetSpecification WRITE SetSpecification NOTIFY specificationChanged)
	Q_PROPERTY(sdl::modsdl::PrinterBase::CSimpleUnionObject* m_simpleTest READ GetSimpleTest WRITE SetSimpleTest NOTIFY simpleTestChanged)
	Q_PROPERTY(sdl::modsdl::PrinterBase::CMixedUnionObject* m_mixedTest READ GetMixedTest WRITE SetMixedTest NOTIFY mixedTestChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPrinterBaseObject(QObject* parent = nullptr);

	QString GetName();
	void SetName(QString v);
	Q_INVOKABLE bool hasName();
	sdl::modsdl::PrinterBase::CPrinterSpecificationObject* GetSpecification();
	void SetSpecification(sdl::modsdl::PrinterBase::CPrinterSpecificationObject* v);
	Q_INVOKABLE bool hasSpecification();
	Q_INVOKABLE void createSpecification();
	sdl::modsdl::PrinterBase::CSimpleUnionObject* GetSimpleTest();
	void SetSimpleTest(sdl::modsdl::PrinterBase::CSimpleUnionObject* v);
	Q_INVOKABLE bool hasSimpleTest();
	Q_INVOKABLE void createSimpleTest();
	sdl::modsdl::PrinterBase::CMixedUnionObject* GetMixedTest();
	void SetMixedTest(sdl::modsdl::PrinterBase::CMixedUnionObject* v);
	Q_INVOKABLE bool hasMixedTest();
	Q_INVOKABLE void createMixedTest();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void nameChanged();
	void specificationChanged();
	void simpleTestChanged();
	void mixedTestChanged();
	void finished();

protected:
	sdl::modsdl::PrinterBase::CPrinterSpecificationObject* m_specificationQObjectPtr;
	sdl::modsdl::PrinterBase::CSimpleUnionObject* m_simpleTestQObjectPtr;
	sdl::modsdl::PrinterBase::CMixedUnionObject* m_mixedTestQObjectPtr;
};


class CPrinterBaseObjectList: public ::imtbase::TListModelBase<sdl::modsdl::PrinterBase::CPrinterBase::V1_0, sdl::modsdl::PrinterBase::CPrinterBaseObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::modsdl::PrinterBase::CPrinterBase::V1_0, sdl::modsdl::PrinterBase::CPrinterBaseObject> BaseClass;

	CPrinterBaseObjectList(QObject* parent = nullptr): BaseClass(parent) {}

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
	Q_INVOKABLE void append(sdl::modsdl::PrinterBase::CPrinterBaseObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::modsdl::PrinterBase::CPrinterBaseObjectList* copyMe(){
		sdl::modsdl::PrinterBase::CPrinterBaseObjectList* objectListPtr = new sdl::modsdl::PrinterBase::CPrinterBaseObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::modsdl::PrinterBase::CPrinterBaseObject>()){
				return nullptr;
			}

			sdl::modsdl::PrinterBase::CPrinterBaseObject* itemObjectPtr = item.value<sdl::modsdl::PrinterBase::CPrinterBaseObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::modsdl::PrinterBase::CPrinterBaseObject*>(itemObjectPtr->copyMe()));
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
			if (!item.canConvert<sdl::modsdl::PrinterBase::CPrinterBaseObject>()){
				return nullptr;
			}

			sdl::modsdl::PrinterBase::CPrinterBaseObject* itemObjectPtr = item.value<sdl::modsdl::PrinterBase::CPrinterBaseObject*>();
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
			if (!item.canConvert<sdl::modsdl::PrinterBase::CPrinterBaseObject>()){
				return nullptr;
			}

			sdl::modsdl::PrinterBase::CPrinterBaseObject* itemObjectPtr = item.value<sdl::modsdl::PrinterBase::CPrinterBaseObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::modsdl::PrinterBase::CPrinterBaseObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::PrinterBase::CPrinterBaseObjectList* otherModelPtr){
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
			if (!selfItem.canConvert<sdl::modsdl::PrinterBase::CPrinterBaseObject>()){
				return false;
			}

			sdl::modsdl::PrinterBase::CPrinterBaseObject* selfItemObjectPtr = selfItem.value<sdl::modsdl::PrinterBase::CPrinterBaseObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::modsdl::PrinterBase::CPrinterBaseObject* otherItemObjectPtr = selfItem.value<sdl::modsdl::PrinterBase::CPrinterBaseObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::modsdl::PrinterBase::CPrinterBaseObject* item){
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
			sdl::modsdl::PrinterBase::CPrinterBaseObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_name"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).name.value());
		}
		if (nameId == "m_specification"){
			sdl::modsdl::PrinterBase::CPrinterBaseObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetSpecification());
		}
		if (nameId == "m_simpleTest"){
			sdl::modsdl::PrinterBase::CPrinterBaseObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetSimpleTest());
		}
		if (nameId == "m_mixedTest"){
			sdl::modsdl::PrinterBase::CPrinterBaseObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetMixedTest());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



class CPrinterListObjectList;

class CPrinterListObject: public ::imtbase::CItemModelBase, public CPrinterList
{
	Q_OBJECT
	Q_PROPERTY(sdl::modsdl::PrinterBase::CPrinterBaseObjectList* m_data READ GetData WRITE SetData NOTIFY dataChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CPrinterListObject(QObject* parent = nullptr);

	sdl::modsdl::PrinterBase::CPrinterBaseObjectList* GetData();
	void SetData(sdl::modsdl::PrinterBase::CPrinterBaseObjectList* v);
	Q_INVOKABLE bool hasData();
	Q_INVOKABLE void createData();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void dataChanged();
	void finished();

protected:
	sdl::modsdl::PrinterBase::CPrinterBaseObjectList* m_dataQObjectPtr;
};


class CPrinterListObjectList: public ::imtbase::TListModelBase<sdl::modsdl::PrinterBase::CPrinterList::V1_0, sdl::modsdl::PrinterBase::CPrinterListObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::modsdl::PrinterBase::CPrinterList::V1_0, sdl::modsdl::PrinterBase::CPrinterListObject> BaseClass;

	CPrinterListObjectList(QObject* parent = nullptr): BaseClass(parent) {}

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
	Q_INVOKABLE void append(sdl::modsdl::PrinterBase::CPrinterListObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::modsdl::PrinterBase::CPrinterListObjectList* copyMe(){
		sdl::modsdl::PrinterBase::CPrinterListObjectList* objectListPtr = new sdl::modsdl::PrinterBase::CPrinterListObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::modsdl::PrinterBase::CPrinterListObject>()){
				return nullptr;
			}

			sdl::modsdl::PrinterBase::CPrinterListObject* itemObjectPtr = item.value<sdl::modsdl::PrinterBase::CPrinterListObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::modsdl::PrinterBase::CPrinterListObject*>(itemObjectPtr->copyMe()));
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
			if (!item.canConvert<sdl::modsdl::PrinterBase::CPrinterListObject>()){
				return nullptr;
			}

			sdl::modsdl::PrinterBase::CPrinterListObject* itemObjectPtr = item.value<sdl::modsdl::PrinterBase::CPrinterListObject*>();
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
			if (!item.canConvert<sdl::modsdl::PrinterBase::CPrinterListObject>()){
				return nullptr;
			}

			sdl::modsdl::PrinterBase::CPrinterListObject* itemObjectPtr = item.value<sdl::modsdl::PrinterBase::CPrinterListObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::modsdl::PrinterBase::CPrinterListObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::modsdl::PrinterBase::CPrinterListObjectList* otherModelPtr){
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
			if (!selfItem.canConvert<sdl::modsdl::PrinterBase::CPrinterListObject>()){
				return false;
			}

			sdl::modsdl::PrinterBase::CPrinterListObject* selfItemObjectPtr = selfItem.value<sdl::modsdl::PrinterBase::CPrinterListObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::modsdl::PrinterBase::CPrinterListObject* otherItemObjectPtr = selfItem.value<sdl::modsdl::PrinterBase::CPrinterListObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::modsdl::PrinterBase::CPrinterListObject* item){
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
			sdl::modsdl::PrinterBase::CPrinterListObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_data"){
			sdl::modsdl::PrinterBase::CPrinterListObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetData());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};


class PrinterSpecification: public std::variant<CPrinterSpecificationBase, CLink> {

public:

	typedef std::variant<CPrinterSpecificationBase, CLink> BaseClass;

	PrinterSpecification(){};
	PrinterSpecification(const CPrinterSpecificationBase& ref)
		: BaseClass(ref){};

	PrinterSpecification(const CLink& ref)
		: BaseClass(ref){};

};

class CPrinterSpecificationObject: public ::imtbase::CItemModelBase
{
	Q_OBJECT
	Q_PROPERTY(QString m_type READ GetType NOTIFY typeChanged)
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

public:
	typedef ::imtbase::CItemModelBase BaseClass;

	CPrinterSpecificationObject(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE QString GetType() const{
		return m_type;
	}

	Q_INVOKABLE void SetValue(const QVariant& value){
		if (value.canConvert<CPrinterSpecificationBase>()){
			m_type = "PrinterSpecificationBase";
		}

		if (value.canConvert<CLink>()){
			m_type = "Link";
		}

		m_value = value;
	}

	Q_INVOKABLE QVariant GetValue(){
		return QVariant();
	}

signals:
	void typeChanged();
	void valueChanged();

public:
	istd::TSharedNullable<std::shared_ptr<PrinterSpecification>> Version_1_0;
	QVariant m_value;
	QString m_type;
};



class SimpleUnion: public std::variant<QString, double> {

public:

	typedef std::variant<QString, double> BaseClass;

	SimpleUnion(){};
	SimpleUnion(const QString& ref)
		: BaseClass(ref){};

	SimpleUnion(const double& ref)
		: BaseClass(ref){};

};

class CSimpleUnionObject: public ::imtbase::CItemModelBase
{
	Q_OBJECT
	Q_PROPERTY(QString m_type READ GetType NOTIFY typeChanged)
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

public:
	typedef ::imtbase::CItemModelBase BaseClass;

	CSimpleUnionObject(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE QString GetType() const{
		return m_type;
	}

	Q_INVOKABLE void SetValue(const QVariant& value){
		if (value.canConvert<QString>()){
			m_type = "String";
		}

		if (value.canConvert<double>()){
			m_type = "Double";
		}

		m_value = value;
	}

	Q_INVOKABLE QVariant GetValue(){
		return QVariant();
	}

signals:
	void typeChanged();
	void valueChanged();

public:
	istd::TSharedNullable<std::shared_ptr<SimpleUnion>> Version_1_0;
	QVariant m_value;
	QString m_type;
};



class MixedUnion: public std::variant<QString, CLink> {

public:

	typedef std::variant<QString, CLink> BaseClass;

	MixedUnion(){};
	MixedUnion(const QString& ref)
		: BaseClass(ref){};

	MixedUnion(const CLink& ref)
		: BaseClass(ref){};

};

class CMixedUnionObject: public ::imtbase::CItemModelBase
{
	Q_OBJECT
	Q_PROPERTY(QString m_type READ GetType NOTIFY typeChanged)
	Q_PROPERTY(QVariant m_value READ GetValue WRITE SetValue NOTIFY valueChanged)

public:
	typedef ::imtbase::CItemModelBase BaseClass;

	CMixedUnionObject(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE QString GetType() const{
		return m_type;
	}

	Q_INVOKABLE void SetValue(const QVariant& value){
		if (value.canConvert<QString>()){
			m_type = "String";
		}

		if (value.canConvert<CLink>()){
			m_type = "Link";
		}

		m_value = value;
	}

	Q_INVOKABLE QVariant GetValue(){
		return QVariant();
	}

signals:
	void typeChanged();
	void valueChanged();

public:
	istd::TSharedNullable<std::shared_ptr<MixedUnion>> Version_1_0;
	QVariant m_value;
	QString m_type;
};



struct GetPrintersRequestInfo
{
	bool isDataRequested = true;
		struct dataRequestInfo
		{
			bool isNameRequested = true;
			bool isSpecificationRequested = true;
			bool isSimpleTestRequested = true;
			bool isMixedTestRequested = true;
		} data;
};


struct GetPrintersRequestArguments
{
};


class CGetPrintersGqlRequest
{
public:
	static QByteArray GetCommandId();

	[[nodiscard]] static bool SetupGqlRequest(::imtgql::CGqlRequest& gqlRequest, const GetPrintersRequestArguments& requestArguments, const GetPrintersRequestInfo& requestInfo = GetPrintersRequestInfo());

	CGetPrintersGqlRequest(const ::imtgql::CGqlRequest& gqlRequest, bool optRead);
	bool IsValid() const;
	const GetPrintersRequestArguments& GetRequestedArguments() const;
	GetPrintersRequestInfo GetRequestInfo() const;
	const ::imtgql::IGqlContext* GetRequestContext() const;

private:
	bool m_isValid;
	GetPrintersRequestArguments m_requestedArguments;
	GetPrintersRequestInfo m_requestInfo;
	const ::imtgql::IGqlContext* m_gqlContextPtr;
};


class CGraphQlHandlerCompBase: public ::imtservergql::CPermissibleGqlRequestHandlerComp
{

public:
	typedef ::imtservergql::CPermissibleGqlRequestHandlerComp BaseClass;

	I_BEGIN_BASE_COMPONENT(CGraphQlHandlerCompBase)
	I_END_COMPONENT

	// reimplemented (::imtservergql::CPermissibleGqlRequestHandlerComp)
	virtual bool IsRequestSupported(const imtgql::CGqlRequest& gqlRequest) const override;
	virtual ::imtbase::CTreeItemModel* CreateInternalResponse(const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const override;

protected:
	// abstract methods
	virtual CPrinterList OnGetPrinters(const CGetPrintersGqlRequest& getPrintersRequest, const ::imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const = 0;
};


#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterQmlTypes()
{
	qmlRegisterType<CPrinterSpecificationBaseObject>("modsdlPrinterBaseSdl", 1, 0, "PrinterSpecificationBase");
	qmlRegisterType<CLinkObject>("modsdlPrinterBaseSdl", 1, 0, "Link");
	qmlRegisterType<CPrinterBaseObject>("modsdlPrinterBaseSdl", 1, 0, "PrinterBase");
	qmlRegisterType<CPrinterListObject>("modsdlPrinterBaseSdl", 1, 0, "PrinterList");
}
#endif

} // namespace sdl::modsdl::PrinterBase


Q_DECLARE_METATYPE(sdl::modsdl::PrinterBase::CPrinterSpecificationBase::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::PrinterBase::CPrinterSpecificationBase);
Q_DECLARE_METATYPE(sdl::modsdl::PrinterBase::CLink::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::PrinterBase::CLink);
Q_DECLARE_METATYPE(sdl::modsdl::PrinterBase::CPrinterBase::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::PrinterBase::CPrinterBase);
Q_DECLARE_METATYPE(sdl::modsdl::PrinterBase::CPrinterList::V1_0);
Q_DECLARE_METATYPE(sdl::modsdl::PrinterBase::CPrinterList);
