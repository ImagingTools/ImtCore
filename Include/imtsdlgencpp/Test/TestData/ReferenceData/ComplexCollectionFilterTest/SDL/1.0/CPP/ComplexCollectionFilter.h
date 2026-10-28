#pragma once

#include "ComplexCollectionFilter_fwd.h"

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
#include <imtbasesdl/SDL/1.0/CPP/ImtBaseTypes.h>


namespace sdl::V1_0::imtbase
{


inline namespace ComplexCollectionFilterSdlEnums
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



} // inline namespace ComplexCollectionFilterSdlEnums


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

	istd::TNullableValue<CTimeRange> timeRange;
	istd::TNullableValue<QString> timeUnit;
	istd::TNullableValue<QString> interpretationMode;
	istd::TNullableValue<int> unitMultiplier;

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

	istd::TNullableValue<QString> fieldId;
	istd::TNullableValue<QString> sortingOrder;

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

	istd::TNullableValue<QByteArray> fieldId;
	istd::TNullableValue<QString> filterValue;
	istd::TNullableValue<ValueType> filterValueType;
	istd::TNullableValue<imtsdl::TElementList<FilterOperation>> filterOperations;

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

	istd::TNullableValue<imtsdl::TElementList<CFieldFilter>> fieldFilters;
	istd::TNullableValue<imtsdl::TElementList<CGroupFilter>> groupFilters;
	istd::TNullableValue<LogicalOperation> logicalOperation;

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

	istd::TNullableValue<imtsdl::TElementList<CFieldSortingInfo>> sortingInfo;
	istd::TNullableValue<CGroupFilter> fieldsFilter;
	istd::TNullableValue<CTimeFilter> timeFilter;
	istd::TNullableValue<imtsdl::TElementList<QByteArray>> distinctFields;

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

} // namespace sdl::V1_0::imtbase


Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CTimeFilter);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CFieldSortingInfo);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CFieldFilter);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CGroupFilter);
Q_DECLARE_METATYPE(sdl::V1_0::imtbase::CComplexCollectionFilter);
