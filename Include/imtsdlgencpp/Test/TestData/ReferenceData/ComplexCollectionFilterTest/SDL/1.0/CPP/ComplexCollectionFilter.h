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
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


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
	static QString GetInteger() { return "Integer"; }
	static QString GetNumber() { return "Number"; }
	static QString GetString() { return "String"; }
	static QString GetBool() { return "Bool"; }

signals:
	void IntegerChanged();
	void NumberChanged();
	void StringChanged();
	void BoolChanged();
};


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
	static QString GetNot() { return "Not"; }
	static QString GetEqual() { return "Equal"; }
	static QString GetLess() { return "Less"; }
	static QString GetGreater() { return "Greater"; }
	static QString GetContains() { return "Contains"; }

signals:
	void NotChanged();
	void EqualChanged();
	void LessChanged();
	void GreaterChanged();
	void ContainsChanged();
};


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
	static QString GetAnd() { return "And"; }
	static QString GetOr() { return "Or"; }

signals:
	void AndChanged();
	void OrChanged();
};



class CTimeFilter
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

class CFieldSortingInfo
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

class CFieldFilter
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
		istd::TSharedNullable<imtsdl::TElementList<FilterOperation>> filterOperations;

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

class CGroupFilter
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
		struct GroupFilterFields
		{
			static const inline QString FieldFilters = "fieldFilters";
			static const inline QString GroupFilters = "groupFilters";
			static const inline QString LogicalOperation = "logicalOperation";
		};

		istd::TSharedNullable<imtsdl::TElementList<CFieldFilter::V1_0>> fieldFilters;
		istd::TSharedNullable<imtsdl::TElementList<CGroupFilter::V1_0>> groupFilters;
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

class CComplexCollectionFilter
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
		struct ComplexCollectionFilterFields
		{
			static const inline QString SortingInfo = "sortingInfo";
			static const inline QString FieldsFilter = "fieldsFilter";
			static const inline QString TimeFilter = "timeFilter";
			static const inline QString DistinctFields = "distinctFields";
		};

		istd::TSharedNullable<imtsdl::TElementList<CFieldSortingInfo::V1_0>> sortingInfo;
		istd::TSharedNullable<CGroupFilter::V1_0> fieldsFilter;
		istd::TSharedNullable<CTimeFilter::V1_0> timeFilter;
		istd::TSharedNullable<imtsdl::TElementList<QByteArray>> distinctFields;

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
	Q_PROPERTY(QVariant m_timeRange READ GetTimeRange WRITE SetTimeRange NOTIFY timeRangeChanged)
	Q_PROPERTY(QVariant m_timeUnit READ GetTimeUnit WRITE SetTimeUnit NOTIFY timeUnitChanged)
	Q_PROPERTY(QVariant m_interpretationMode READ GetInterpretationMode WRITE SetInterpretationMode NOTIFY interpretationModeChanged)
	Q_PROPERTY(QVariant m_unitMultiplier READ GetUnitMultiplier WRITE SetUnitMultiplier NOTIFY unitMultiplierChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CTimeFilterObject(QObject* parent = nullptr);

	QVariant GetTimeRange();
	void SetTimeRange(const QVariant& v);
	Q_INVOKABLE bool hasTimeRange();
	Q_INVOKABLE void emplaceTimeRange();
	void ResetTimeRange();
	QVariant GetTimeUnit();
	void SetTimeUnit(const QVariant& v);
	Q_INVOKABLE bool hasTimeUnit();
	QVariant GetInterpretationMode();
	void SetInterpretationMode(const QVariant& v);
	Q_INVOKABLE bool hasInterpretationMode();
	QVariant GetUnitMultiplier();
	void SetUnitMultiplier(const QVariant& v);
	Q_INVOKABLE bool hasUnitMultiplier();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void timeRangeChanged();
	void timeUnitChanged();
	void interpretationModeChanged();
	void unitMultiplierChanged();

protected:
	QVariant m_timeRangeQObjectPtr;
};





class CTimeFilterObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject> BaseClass;

	explicit CTimeFilterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* item);
	Q_INVOKABLE sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CTimeFilterObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ComplexCollectionFilter::CTimeFilterObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CFieldSortingInfoObjectList;

class CFieldSortingInfoObject: public ::imtbase::CItemModelBase, public CFieldSortingInfo
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_fieldId READ GetFieldId WRITE SetFieldId NOTIFY fieldIdChanged)
	Q_PROPERTY(QVariant m_sortingOrder READ GetSortingOrder WRITE SetSortingOrder NOTIFY sortingOrderChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CFieldSortingInfoObject(QObject* parent = nullptr);

	QVariant GetFieldId();
	void SetFieldId(const QVariant& v);
	Q_INVOKABLE bool hasFieldId();
	QVariant GetSortingOrder();
	void SetSortingOrder(const QVariant& v);
	Q_INVOKABLE bool hasSortingOrder();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void fieldIdChanged();
	void sortingOrderChanged();

protected:
};





class CFieldSortingInfoObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo::V1_0, sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo::V1_0, sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject> BaseClass;

	explicit CFieldSortingInfoObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* item);
	Q_INVOKABLE sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfoObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CFieldFilterObjectList;

class CFieldFilterObject: public ::imtbase::CItemModelBase, public CFieldFilter
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_fieldId READ GetFieldId WRITE SetFieldId NOTIFY fieldIdChanged)
	Q_PROPERTY(QVariant m_filterValue READ GetFilterValue WRITE SetFilterValue NOTIFY filterValueChanged)
	Q_PROPERTY(QVariant m_filterValueType READ GetFilterValueType WRITE SetFilterValueType NOTIFY filterValueTypeChanged)
	Q_PROPERTY(QVariant m_filterOperations READ GetFilterOperations WRITE SetFilterOperations NOTIFY filterOperationsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CFieldFilterObject(QObject* parent = nullptr);

	QVariant GetFieldId();
	void SetFieldId(const QVariant& v);
	Q_INVOKABLE bool hasFieldId();
	QVariant GetFilterValue();
	void SetFilterValue(const QVariant& v);
	Q_INVOKABLE bool hasFilterValue();
	QVariant GetFilterValueType();
	void SetFilterValueType(const QVariant& v);
	Q_INVOKABLE bool hasFilterValueType();
	QVariant GetFilterOperations();
	void SetFilterOperations(const QVariant& v);
	Q_INVOKABLE bool hasFilterOperations();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void fieldIdChanged();
	void filterValueChanged();
	void filterValueTypeChanged();
	void filterOperationsChanged();

protected:
};





class CFieldFilterObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject> BaseClass;

	explicit CFieldFilterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* item);
	Q_INVOKABLE sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CFieldFilterObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ComplexCollectionFilter::CFieldFilterObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CGroupFilterObjectList;

class CGroupFilterObject: public ::imtbase::CItemModelBase, public CGroupFilter
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_fieldFilters READ GetFieldFilters WRITE SetFieldFilters NOTIFY fieldFiltersChanged)
	Q_PROPERTY(QVariant m_groupFilters READ GetGroupFilters WRITE SetGroupFilters NOTIFY groupFiltersChanged)
	Q_PROPERTY(QVariant m_logicalOperation READ GetLogicalOperation WRITE SetLogicalOperation NOTIFY logicalOperationChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CGroupFilterObject(QObject* parent = nullptr);

	QVariant GetFieldFilters();
	void SetFieldFilters(const QVariant& v);
	Q_INVOKABLE bool hasFieldFilters();
	Q_INVOKABLE void emplaceFieldFilters();
	void ResetFieldFilters();
	Q_INVOKABLE QVariant createFieldFiltersArrayElement(const QVariant& v);
	QVariant GetGroupFilters();
	void SetGroupFilters(const QVariant& v);
	Q_INVOKABLE bool hasGroupFilters();
	Q_INVOKABLE void emplaceGroupFilters();
	void ResetGroupFilters();
	Q_INVOKABLE QVariant createGroupFiltersArrayElement(const QVariant& v);
	QVariant GetLogicalOperation();
	void SetLogicalOperation(const QVariant& v);
	Q_INVOKABLE bool hasLogicalOperation();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void fieldFiltersChanged();
	void groupFiltersChanged();
	void logicalOperationChanged();

protected:
	QVariant m_fieldFiltersQObjectPtr;
	QVariant m_groupFiltersQObjectPtr;
};





class CGroupFilterObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject> BaseClass;

	explicit CGroupFilterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* item);
	Q_INVOKABLE sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CGroupFilterObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ComplexCollectionFilter::CGroupFilterObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};



class CComplexCollectionFilterObjectList;

class CComplexCollectionFilterObject: public ::imtbase::CItemModelBase, public CComplexCollectionFilter
{
	Q_OBJECT
	Q_PROPERTY(QVariant m_sortingInfo READ GetSortingInfo WRITE SetSortingInfo NOTIFY sortingInfoChanged)
	Q_PROPERTY(QVariant m_fieldsFilter READ GetFieldsFilter WRITE SetFieldsFilter NOTIFY fieldsFilterChanged)
	Q_PROPERTY(QVariant m_timeFilter READ GetTimeFilter WRITE SetTimeFilter NOTIFY timeFilterChanged)
	Q_PROPERTY(QVariant m_distinctFields READ GetDistinctFields WRITE SetDistinctFields NOTIFY distinctFieldsChanged)

	typedef ::imtbase::CItemModelBase BaseClass;

public:
	CComplexCollectionFilterObject(QObject* parent = nullptr);

	QVariant GetSortingInfo();
	void SetSortingInfo(const QVariant& v);
	Q_INVOKABLE bool hasSortingInfo();
	Q_INVOKABLE void emplaceSortingInfo();
	void ResetSortingInfo();
	Q_INVOKABLE QVariant createSortingInfoArrayElement(const QVariant& v);
	QVariant GetFieldsFilter();
	void SetFieldsFilter(const QVariant& v);
	Q_INVOKABLE bool hasFieldsFilter();
	Q_INVOKABLE void emplaceFieldsFilter();
	void ResetFieldsFilter();
	QVariant GetTimeFilter();
	void SetTimeFilter(const QVariant& v);
	Q_INVOKABLE bool hasTimeFilter();
	Q_INVOKABLE void emplaceTimeFilter();
	void ResetTimeFilter();
	QVariant GetDistinctFields();
	void SetDistinctFields(const QVariant& v);
	Q_INVOKABLE bool hasDistinctFields();
	// CItemModelBase implemented
	Q_INVOKABLE QString toJson() const override;
	Q_INVOKABLE virtual bool createFromJson(const QString& json) override;
	Q_INVOKABLE virtual bool fromObject(const QJsonObject& jsonObject) override;
	Q_INVOKABLE QString toGraphQL() const override;
	Q_INVOKABLE QVariant CreateObject(const QString& key) override;
	Q_INVOKABLE QString getJSONKeyForProperty(const QString& propertyName) const override;

signals:
	void sortingInfoChanged();
	void fieldsFilterChanged();
	void timeFilterChanged();
	void distinctFieldsChanged();

protected:
	QVariant m_sortingInfoQObjectPtr;
	QVariant m_fieldsFilterQObjectPtr;
	QVariant m_timeFilterQObjectPtr;
};





class CComplexCollectionFilterObjectList: public ::imtsdl::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0, sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject> BaseClass;

	explicit CComplexCollectionFilterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* item);
	Q_INVOKABLE sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilterObject* item);
	Q_INVOKABLE void remove(int index) override;
	Q_INVOKABLE void clear() override;
	Q_INVOKABLE QVariant getData(const QString& nameId, int index) override;

signals:
	void countChanged();
};


#ifdef QT_QML_LIB
[[maybe_unused]] static void RegisterQmlTypes()
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
#endif

} // namespace sdl::imtbase::ComplexCollectionFilter


Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CTimeFilter);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CFieldFilter);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CGroupFilter);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter);
