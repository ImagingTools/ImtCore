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


namespace sdl::V1_0::imtbase
{


class CTimeFilter
{
public:
	struct TimeFilterFields
	{
		static const inline QString TimeRange = "timeRange";
		static const inline QString TimeUnit = "timeUnit";
		static const inline QString InterpretationMode = "interpretationMode";
		static const inline QString UnitMultiplier = "unitMultiplier";
	};

	istd::TSharedNullable<CTimeRange> timeRange;
	istd::TSharedNullable<QString> timeUnit;
	istd::TSharedNullable<QString> interpretationMode;
	istd::TSharedNullable<int> unitMultiplier;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CTimeFilter& other) const;
	[[nodiscard]] bool operator!=(const CTimeFilter& other) const {return !(operator==(other));}
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

class CFieldSortingInfo
{
public:
	struct FieldSortingInfoFields
	{
		static const inline QString FieldId = "fieldId";
		static const inline QString SortingOrder = "sortingOrder";
	};

	istd::TSharedNullable<QString> fieldId;
	istd::TSharedNullable<QString> sortingOrder;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CFieldSortingInfo& other) const;
	[[nodiscard]] bool operator!=(const CFieldSortingInfo& other) const {return !(operator==(other));}
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

class CFieldFilter
{
public:
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

	[[nodiscard]] bool operator==(const CFieldFilter& other) const;
	[[nodiscard]] bool operator!=(const CFieldFilter& other) const {return !(operator==(other));}
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

class CGroupFilter
{
public:
	struct GroupFilterFields
	{
		static const inline QString FieldFilters = "fieldFilters";
		static const inline QString GroupFilters = "groupFilters";
		static const inline QString LogicalOperation = "logicalOperation";
	};

	istd::TSharedNullable<imtsdl::TElementList<CFieldFilter>> fieldFilters;
	istd::TSharedNullable<imtsdl::TElementList<CGroupFilter>> groupFilters;
	istd::TSharedNullable<LogicalOperation> logicalOperation;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CGroupFilter& other) const;
	[[nodiscard]] bool operator!=(const CGroupFilter& other) const {return !(operator==(other));}
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

class CComplexCollectionFilter
{
public:
	struct ComplexCollectionFilterFields
	{
		static const inline QString SortingInfo = "sortingInfo";
		static const inline QString FieldsFilter = "fieldsFilter";
		static const inline QString TimeFilter = "timeFilter";
		static const inline QString DistinctFields = "distinctFields";
	};

	istd::TSharedNullable<imtsdl::TElementList<CFieldSortingInfo>> sortingInfo;
	istd::TSharedNullable<CGroupFilter> fieldsFilter;
	istd::TSharedNullable<CTimeFilter> timeFilter;
	istd::TSharedNullable<imtsdl::TElementList<QByteArray>> distinctFields;

	static QByteArray GetVersionId();

	[[nodiscard]] bool operator==(const CComplexCollectionFilter& other) const;
	[[nodiscard]] bool operator!=(const CComplexCollectionFilter& other) const {return !(operator==(other));}
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





class CTimeFilterObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CTimeFilter, sdl::V1_0::imtbase::CTimeFilterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CTimeFilter, sdl::V1_0::imtbase::CTimeFilterObject> BaseClass;

	explicit CTimeFilterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CTimeFilterObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CTimeFilterObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CTimeFilterObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CTimeFilterObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CTimeFilterObject* item);
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





class CFieldSortingInfoObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CFieldSortingInfo, sdl::V1_0::imtbase::CFieldSortingInfoObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CFieldSortingInfo, sdl::V1_0::imtbase::CFieldSortingInfoObject> BaseClass;

	explicit CFieldSortingInfoObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CFieldSortingInfoObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CFieldSortingInfoObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CFieldSortingInfoObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CFieldSortingInfoObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CFieldSortingInfoObject* item);
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





class CFieldFilterObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CFieldFilter, sdl::V1_0::imtbase::CFieldFilterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CFieldFilter, sdl::V1_0::imtbase::CFieldFilterObject> BaseClass;

	explicit CFieldFilterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CFieldFilterObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CFieldFilterObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CFieldFilterObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CFieldFilterObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CFieldFilterObject* item);
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





class CGroupFilterObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CGroupFilter, sdl::V1_0::imtbase::CGroupFilterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CGroupFilter, sdl::V1_0::imtbase::CGroupFilterObject> BaseClass;

	explicit CGroupFilterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CGroupFilterObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CGroupFilterObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CGroupFilterObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CGroupFilterObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CGroupFilterObject* item);
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





class CComplexCollectionFilterObjectList: public ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CComplexCollectionFilter, sdl::V1_0::imtbase::CComplexCollectionFilterObject>
{
	Q_OBJECT
	Q_PROPERTY(int count READ rowCount() NOTIFY countChanged())
public:
	typedef ::imtsdl::TListModelBase<sdl::V1_0::imtbase::CComplexCollectionFilter, sdl::V1_0::imtbase::CComplexCollectionFilterObject> BaseClass;

	explicit CComplexCollectionFilterObjectList(QObject* parent = nullptr): BaseClass(parent) {}

	Q_INVOKABLE bool containsKey(const QString& /*nameId*/, int /*index*/) const;
	Q_INVOKABLE int getItemsCount() const;
	Q_INVOKABLE QVariantMap get(int row) const override;
	Q_INVOKABLE void append(sdl::V1_0::imtbase::CComplexCollectionFilterObject* item);
	Q_INVOKABLE sdl::V1_0::imtbase::CComplexCollectionFilterObjectList* copyMe();
	Q_INVOKABLE QString toJson();
	Q_INVOKABLE QString toGraphQL();
	Q_INVOKABLE void addElement(sdl::V1_0::imtbase::CComplexCollectionFilterObject* item);
	Q_INVOKABLE void removeElement(int index);
	Q_INVOKABLE bool isEqualWithModel(sdl::V1_0::imtbase::CComplexCollectionFilterObjectList* otherModelPtr);
	Q_INVOKABLE void insert(int index, sdl::V1_0::imtbase::CComplexCollectionFilterObject* item);
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

} // namespace sdl::V1_0::imtbase


Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CTimeFilter);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CFieldSortingInfo);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CFieldFilter);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CGroupFilter);
Q_DECLARE_METATYPE(sdl::V1_0::V1_0::CComplexCollectionFilter);
