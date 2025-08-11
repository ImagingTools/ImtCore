/// \file ValueType.h

#pragma once


#include <QtCore/QMetaEnum>


namespace sdl::imtbase::ComplexCollectionFilter
{


Q_NAMESPACE
enum class ValueType {
	Integer,
	Number,
	String,
	Bool,
};

Q_ENUM_NS(ValueType)


class EnumValueType: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString Integer READ GetInteger NOTIFY IntegerChanged)
	Q_PROPERTY(QString Number READ GetNumber NOTIFY NumberChanged)
	Q_PROPERTY(QString String READ GetString NOTIFY StringChanged)
	Q_PROPERTY(QString Bool READ GetBool NOTIFY BoolChanged)
protected:
	QString GetInteger() { return "Integer"; }
	QString GetNumber() { return "Number"; }
	QString GetString() { return "String"; }
	QString GetBool() { return "Bool"; }
signals:
	void IntegerChanged();
	void NumberChanged();
	void StringChanged();
	void BoolChanged();
};


} // namespace sdl::imtbase::ComplexCollectionFilter




/// \file FilterOperation.h

#pragma once


#include <QtCore/QMetaEnum>


namespace sdl::imtbase::ComplexCollectionFilter
{


Q_NAMESPACE
enum class FilterOperation {
	Not,
	Equal,
	Less,
	Greater,
	Contains,
};

Q_ENUM_NS(FilterOperation)


class EnumFilterOperation: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString Not READ GetNot NOTIFY NotChanged)
	Q_PROPERTY(QString Equal READ GetEqual NOTIFY EqualChanged)
	Q_PROPERTY(QString Less READ GetLess NOTIFY LessChanged)
	Q_PROPERTY(QString Greater READ GetGreater NOTIFY GreaterChanged)
	Q_PROPERTY(QString Contains READ GetContains NOTIFY ContainsChanged)
protected:
	QString GetNot() { return "Not"; }
	QString GetEqual() { return "Equal"; }
	QString GetLess() { return "Less"; }
	QString GetGreater() { return "Greater"; }
	QString GetContains() { return "Contains"; }
signals:
	void NotChanged();
	void EqualChanged();
	void LessChanged();
	void GreaterChanged();
	void ContainsChanged();
};


} // namespace sdl::imtbase::ComplexCollectionFilter




/// \file LogicalOperation.h

#pragma once


#include <QtCore/QMetaEnum>


namespace sdl::imtbase::ComplexCollectionFilter
{


Q_NAMESPACE
enum class LogicalOperation {
	And,
	Or,
};

Q_ENUM_NS(LogicalOperation)


class EnumLogicalOperation: public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString And READ GetAnd NOTIFY AndChanged)
	Q_PROPERTY(QString Or READ GetOr NOTIFY OrChanged)
protected:
	QString GetAnd() { return "And"; }
	QString GetOr() { return "Or"; }
signals:
	void AndChanged();
	void OrChanged();
};


} // namespace sdl::imtbase::ComplexCollectionFilter




/// \file CTimeFilter.h

#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>

// ACF includes
#include <istd/TSharedNullable.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtbase/CItemModelBase.h>
#include <imtbase/TListModelBase.h>

// custom types includes
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>



namespace sdl::imtbase::ComplexCollectionFilter
{


class CTimeFilter
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
		struct TimeFilterFields
		{
			static const inline QString TimeRange = "timeRange";
			static const inline QString TimeUnit = "timeUnit";
			static const inline QString InterpretationMode = "interpretationMode";
			static const inline QString UnitMultiplier = "unitMultiplier";
		};

		istd::TSharedNullable<ImtBaseTypes::CTimeRange::V1_0> timeRange;
		istd::TSharedNullable<QString> timeUnit;
		istd::TSharedNullable<QString> interpretationMode;
		istd::TSharedNullable<int> unitMultiplier;

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


class CTimeFilterObjectList;

class CTimeFilterObject: public ::imtbase::CItemModelBase, public CTimeFilter
{
	Q_OBJECT
	Q_PROPERTY(sdl::imtbase::ImtBaseTypes::CTimeRangeObject* m_timeRange READ GetTimeRange WRITE SetTimeRange NOTIFY timeRangeChanged)
	Q_PROPERTY(QString m_timeUnit READ GetTimeUnit WRITE SetTimeUnit NOTIFY timeUnitChanged)
	Q_PROPERTY(QString m_interpretationMode READ GetInterpretationMode WRITE SetInterpretationMode NOTIFY interpretationModeChanged)
	Q_PROPERTY(int m_unitMultiplier READ GetUnitMultiplier WRITE SetUnitMultiplier NOTIFY unitMultiplierChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CTimeFilterObject(QObject* parent = nullptr);

	sdl::imtbase::ImtBaseTypes::CTimeRangeObject* GetTimeRange();
	void SetTimeRange(sdl::imtbase::ImtBaseTypes::CTimeRangeObject* v);
	Q_INVOKABLE bool hasTimeRange();
	Q_INVOKABLE void createTimeRange();
	QString GetTimeUnit();
	void SetTimeUnit(QString v);
	Q_INVOKABLE bool hasTimeUnit();
	QString GetInterpretationMode();
	void SetInterpretationMode(QString v);
	Q_INVOKABLE bool hasInterpretationMode();
	int GetUnitMultiplier();
	void SetUnitMultiplier(int v);
	Q_INVOKABLE bool hasUnitMultiplier();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void timeRangeChanged();
	void timeUnitChanged();
	void interpretationModeChanged();
	void unitMultiplierChanged();
	void finished();

protected:
	sdl::imtbase::ImtBaseTypes::CTimeRangeObject* m_timeRangeQObjectPtr;
};


class CTimeFilterObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject> BaseClass;

	CTimeFilterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

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
	Q_INVOKABLE void append(sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList* copyMe(){
		sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList* objectListPtr = new sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject*>(itemObjectPtr->copyMe()));
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
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject*>();
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
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList* otherModelPtr){
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
			if (!selfItem.canConvert<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject>()){
				return false;
			}

			sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* item){
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
			sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_timeRange"){
			sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetTimeRange());
		}
		if (nameId == "m_timeUnit"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).timeUnit.value());
		}
		if (nameId == "m_interpretationMode"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).interpretationMode.value());
		}
		if (nameId == "m_unitMultiplier"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).unitMultiplier.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



} // namespace sdl::imtbase::ComplexCollectionFilter


Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CTimeFilter);





/// \file CFieldSortingInfo.h

#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>

// ACF includes
#include <istd/TSharedNullable.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtbase/CItemModelBase.h>
#include <imtbase/TListModelBase.h>



namespace sdl::imtbase::ComplexCollectionFilter
{


class CFieldSortingInfo
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
		struct FieldSortingInfoFields
		{
			static const inline QString FieldId = "fieldId";
			static const inline QString SortingOrder = "sortingOrder";
		};

		istd::TSharedNullable<QString> fieldId;
		istd::TSharedNullable<QString> sortingOrder;

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


class CFieldSortingInfoObjectList;

class CFieldSortingInfoObject: public ::imtbase::CItemModelBase, public CFieldSortingInfo
{
	Q_OBJECT
	Q_PROPERTY(QString m_fieldId READ GetFieldId WRITE SetFieldId NOTIFY fieldIdChanged)
	Q_PROPERTY(QString m_sortingOrder READ GetSortingOrder WRITE SetSortingOrder NOTIFY sortingOrderChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CFieldSortingInfoObject(QObject* parent = nullptr);

	QString GetFieldId();
	void SetFieldId(QString v);
	Q_INVOKABLE bool hasFieldId();
	QString GetSortingOrder();
	void SetSortingOrder(QString v);
	Q_INVOKABLE bool hasSortingOrder();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void fieldIdChanged();
	void sortingOrderChanged();
	void finished();

protected:
};


class CFieldSortingInfoObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo::V1_0, sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo::V1_0, sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject> BaseClass;

	CFieldSortingInfoObjectList(QObject* parent = nullptr): BaseClass(parent) {}

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
	Q_INVOKABLE void append(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* copyMe(){
		sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* objectListPtr = new sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject*>(itemObjectPtr->copyMe()));
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
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject*>();
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
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* otherModelPtr){
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
			if (!selfItem.canConvert<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject>()){
				return false;
			}

			sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* item){
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
			sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_fieldId"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).fieldId.value());
		}
		if (nameId == "m_sortingOrder"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).sortingOrder.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



} // namespace sdl::imtbase::ComplexCollectionFilter


Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo);





/// \file CFieldFilter.h

#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>

// ACF includes
#include <istd/TSharedNullable.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtbase/CItemModelBase.h>
#include <imtbase/TListModelBase.h>



namespace sdl::imtbase::ComplexCollectionFilter
{


class CFieldFilter
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
		struct FieldFilterFields
		{
			static const inline QString FieldId = "fieldId";
			static const inline QString FilterValue = "filterValue";
			static const inline QString FilterValueType = "filterValueType";
			static const inline QString FilterOperations = "filterOperations";
		};

		istd::TSharedNullable<QByteArray> fieldId;
		istd::TSharedNullable<QString> filterValue;
		istd::TSharedNullable<ValueType> filterValueType;
		istd::TSharedNullable<QList<FilterOperation>> filterOperations;

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


class CFieldFilterObjectList;

class CFieldFilterObject: public ::imtbase::CItemModelBase, public CFieldFilter
{
	Q_OBJECT
	Q_PROPERTY(QString m_fieldId READ GetFieldId WRITE SetFieldId NOTIFY fieldIdChanged)
	Q_PROPERTY(QString m_filterValue READ GetFilterValue WRITE SetFilterValue NOTIFY filterValueChanged)
	Q_PROPERTY(QString m_filterValueType READ GetFilterValueType WRITE SetFilterValueType NOTIFY filterValueTypeChanged)
	Q_PROPERTY(QList<QString> m_filterOperations READ GetFilterOperations WRITE SetFilterOperations NOTIFY filterOperationsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CFieldFilterObject(QObject* parent = nullptr);

	QString GetFieldId();
	void SetFieldId(QString v);
	Q_INVOKABLE bool hasFieldId();
	QString GetFilterValue();
	void SetFilterValue(QString v);
	Q_INVOKABLE bool hasFilterValue();
	QString GetFilterValueType();
	void SetFilterValueType(QString v);
	Q_INVOKABLE bool hasFilterValueType();
	QList<QString> GetFilterOperations();
	void SetFilterOperations(QList<QString> v);
	Q_INVOKABLE bool hasFilterOperations();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void fieldIdChanged();
	void filterValueChanged();
	void filterValueTypeChanged();
	void filterOperationsChanged();
	void finished();

protected:
};


class CFieldFilterObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject> BaseClass;

	CFieldFilterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

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
	Q_INVOKABLE void append(sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* copyMe(){
		sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* objectListPtr = new sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject*>(itemObjectPtr->copyMe()));
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
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject*>();
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
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* otherModelPtr){
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
			if (!selfItem.canConvert<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject>()){
				return false;
			}

			sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* item){
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
			sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_fieldId"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).fieldId.value());
		}
		if (nameId == "m_filterValue"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).filterValue.value());
		}
		if (nameId == "m_filterValueType"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).filterValueType.value());
		}
		if (nameId == "m_filterOperations"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).filterOperations.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



} // namespace sdl::imtbase::ComplexCollectionFilter


Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CFieldFilter);





/// \file CGroupFilter.h

#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QObject>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>

// ACF includes
#include <istd/TSharedNullable.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtbase/CItemModelBase.h>
#include <imtbase/TListModelBase.h>



namespace sdl::imtbase::ComplexCollectionFilter
{


class CGroupFilter
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
		struct GroupFilterFields
		{
			static const inline QString FieldFilters = "fieldFilters";
			static const inline QString GroupFilters = "groupFilters";
			static const inline QString LogicalOperation = "logicalOperation";
		};

		istd::TSharedNullable<QList<CFieldFilter::V1_0>> fieldFilters;
		istd::TSharedNullable<QList<CGroupFilter::V1_0>> groupFilters;
		istd::TSharedNullable<LogicalOperation> logicalOperation;

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


class CGroupFilterObjectList;

class CGroupFilterObject: public ::imtbase::CItemModelBase, public CGroupFilter
{
	Q_OBJECT
	Q_PROPERTY(sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* m_fieldFilters READ GetFieldFilters WRITE SetFieldFilters NOTIFY fieldFiltersChanged)
	Q_PROPERTY(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* m_groupFilters READ GetGroupFilters WRITE SetGroupFilters NOTIFY groupFiltersChanged)
	Q_PROPERTY(QString m_logicalOperation READ GetLogicalOperation WRITE SetLogicalOperation NOTIFY logicalOperationChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CGroupFilterObject(QObject* parent = nullptr);

	sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* GetFieldFilters();
	void SetFieldFilters(sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* v);
	Q_INVOKABLE bool hasFieldFilters();
	Q_INVOKABLE void createFieldFilters();
	sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* GetGroupFilters();
	void SetGroupFilters(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* v);
	Q_INVOKABLE bool hasGroupFilters();
	Q_INVOKABLE void createGroupFilters();
	QString GetLogicalOperation();
	void SetLogicalOperation(QString v);
	Q_INVOKABLE bool hasLogicalOperation();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void fieldFiltersChanged();
	void groupFiltersChanged();
	void logicalOperationChanged();
	void finished();

protected:
	sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* m_fieldFiltersQObjectPtr;
	sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* m_groupFiltersQObjectPtr;
};


class CGroupFilterObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject> BaseClass;

	CGroupFilterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

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
	Q_INVOKABLE void append(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* copyMe(){
		sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* objectListPtr = new sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject*>(itemObjectPtr->copyMe()));
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
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject*>();
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
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* otherModelPtr){
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
			if (!selfItem.canConvert<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject>()){
				return false;
			}

			sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* item){
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
			sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_fieldFilters"){
			sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetFieldFilters());
		}
		if (nameId == "m_groupFilters"){
			sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetGroupFilters());
		}
		if (nameId == "m_logicalOperation"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).logicalOperation.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



} // namespace sdl::imtbase::ComplexCollectionFilter


Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CGroupFilter);





/// \file CComplexCollectionFilter.h

#pragma once


// Qt includes
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>
#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QVariant>
#include <QtCore/QVariantMap>
#include <QtCore/QSet>

// ACF includes
#include <istd/TSharedNullable.h>

// ImtCore includes
#include <imtbase/CTreeItemModel.h>
#include <imtgql/CGqlParamObject.h>
#include <imtbase/CItemModelBase.h>
#include <imtbase/TListModelBase.h>



namespace sdl::imtbase::ComplexCollectionFilter
{


class CComplexCollectionFilter
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
		struct ComplexCollectionFilterFields
		{
			static const inline QString SortingInfo = "sortingInfo";
			static const inline QString FieldsFilter = "fieldsFilter";
			static const inline QString TimeFilter = "timeFilter";
			static const inline QString DistinctFields = "distinctFields";
		};

		istd::TSharedNullable<QList<CFieldSortingInfo::V1_0>> sortingInfo;
		istd::TSharedNullable<CGroupFilter::V1_0> fieldsFilter;
		istd::TSharedNullable<CTimeFilter::V1_0> timeFilter;
		istd::TSharedNullable<QList<QByteArray>> distinctFields;

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


class CComplexCollectionFilterObjectList;

class CComplexCollectionFilterObject: public ::imtbase::CItemModelBase, public CComplexCollectionFilter
{
	Q_OBJECT
	Q_PROPERTY(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* m_sortingInfo READ GetSortingInfo WRITE SetSortingInfo NOTIFY sortingInfoChanged)
	Q_PROPERTY(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* m_fieldsFilter READ GetFieldsFilter WRITE SetFieldsFilter NOTIFY fieldsFilterChanged)
	Q_PROPERTY(sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* m_timeFilter READ GetTimeFilter WRITE SetTimeFilter NOTIFY timeFilterChanged)
	Q_PROPERTY(QList<QString> m_distinctFields READ GetDistinctFields WRITE SetDistinctFields NOTIFY distinctFieldsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CComplexCollectionFilterObject(QObject* parent = nullptr);

	sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* GetSortingInfo();
	void SetSortingInfo(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* v);
	Q_INVOKABLE bool hasSortingInfo();
	Q_INVOKABLE void createSortingInfo();
	sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* GetFieldsFilter();
	void SetFieldsFilter(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* v);
	Q_INVOKABLE bool hasFieldsFilter();
	Q_INVOKABLE void createFieldsFilter();
	sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* GetTimeFilter();
	void SetTimeFilter(sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* v);
	Q_INVOKABLE bool hasTimeFilter();
	Q_INVOKABLE void createTimeFilter();
	QList<QString> GetDistinctFields();
	void SetDistinctFields(QList<QString> v);
	Q_INVOKABLE bool hasDistinctFields();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QObject* CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void sortingInfoChanged();
	void fieldsFilterChanged();
	void timeFilterChanged();
	void distinctFieldsChanged();
	void finished();

protected:
	sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* m_sortingInfoQObjectPtr;
	sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* m_fieldsFilterQObjectPtr;
	sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* m_timeFilterQObjectPtr;
};


class CComplexCollectionFilterObjectList: public ::imtbase::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtbase::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject> BaseClass;

	CComplexCollectionFilterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

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
	Q_INVOKABLE void append(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* item){
		beginInsertRows(QModelIndex(), rowCount(), rowCount());
		Version_1_0->append(*item->Version_1_0);
		ClearCache();
		endInsertRows();
	}
	Q_INVOKABLE sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList* copyMe(){
		sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList* objectListPtr = new sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList();

		for (int i = 0; i < this->rowCount(); i++){
			QVariant item = this->getData("item", i);
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject*>();
			if (itemObjectPtr == nullptr){
				return nullptr;
			}

			objectListPtr->addElement(dynamic_cast<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject*>(itemObjectPtr->copyMe()));
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
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject*>();
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
			if (!item.canConvert<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject>()){
				return nullptr;
			}

			sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* itemObjectPtr = item.value<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject*>();
			if (itemObjectPtr == nullptr){
				return QString();
			}

			retVal += itemObjectPtr->toGraphQL();
		}

		retVal += QStringLiteral("]");

		return retVal;
	}

	Q_INVOKABLE void addElement(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* item){
		append(item);
	}

	Q_INVOKABLE void removeElement(int index){
		remove(index);
	}

	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList* otherModelPtr){
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
			if (!selfItem.canConvert<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject>()){
				return false;
			}

			sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* selfItemObjectPtr = selfItem.value<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject*>();
			if (selfItemObjectPtr == nullptr){
				return false;
			}

			sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* otherItemObjectPtr = selfItem.value<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject*>();
			if (otherItemObjectPtr == nullptr){
				return false;
			}

			if (!selfItemObjectPtr->isEqualWithModel(otherItemObjectPtr)){
				return false;
			}
		}

		return true;
	}

	Q_INVOKABLE void insert(int index, sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* item){
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
			sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal);
		}
		if (nameId == "m_sortingInfo"){
			sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetSortingInfo());
		}
		if (nameId == "m_fieldsFilter"){
			sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetFieldsFilter());
		}
		if (nameId == "m_timeFilter"){
			sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* retVal = GetOrCreateCachedObject(index);
			return QVariant::fromValue(retVal->GetTimeFilter());
		}
		if (nameId == "m_distinctFields"){
			return QVariant::fromValue(Version_1_0.GetPtr()->at(index).distinctFields.value());
		}
		return QVariant();
	}
	signals:
	void countChanged();
};



} // namespace sdl::imtbase::ComplexCollectionFilter


Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter);





/// \file QmlRegister.h

#pragma once


#ifdef QT_QML_LIB
#include <QtQml/QQmlEngine>


namespace sdl::imtbase::ComplexCollectionFilter
{


static void RegisterQmlTypes()
{
	qmlRegisterType<CTimeFilterObject>("imtbaseComplexCollectionFilterSdl", 1, 0, "TimeFilter");
	qmlRegisterType<CFieldSortingInfoObject>("imtbaseComplexCollectionFilterSdl", 1, 0, "FieldSortingInfo");
	qmlRegisterType<CFieldFilterObject>("imtbaseComplexCollectionFilterSdl", 1, 0, "FieldFilter");
	qmlRegisterType<CGroupFilterObject>("imtbaseComplexCollectionFilterSdl", 1, 0, "GroupFilter");
	qmlRegisterType<CComplexCollectionFilterObject>("imtbaseComplexCollectionFilterSdl", 1, 0, "ComplexCollectionFilter");
	qmlRegisterSingletonType<EnumValueType>("imtbaseComplexCollectionFilterSdl", 1, 0, "ValueType", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumValueType *enumType = new EnumValueType();
		return enumType;
	});
	qmlRegisterSingletonType<EnumFilterOperation>("imtbaseComplexCollectionFilterSdl", 1, 0, "FilterOperation", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumFilterOperation *enumType = new EnumFilterOperation();
		return enumType;
	});
	qmlRegisterSingletonType<EnumLogicalOperation>("imtbaseComplexCollectionFilterSdl", 1, 0, "LogicalOperation", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
		Q_UNUSED(engine)
		Q_UNUSED(scriptEngine)

		EnumLogicalOperation *enumType = new EnumLogicalOperation();
		return enumType;
	});
}
} // namespace sdl::imtbase::ComplexCollectionFilter
#endif




