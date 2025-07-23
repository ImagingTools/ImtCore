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
#include <1.0/CPP/ImtBaseTypes.h>



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


} // namespace sdl::imtbase::ComplexCollectionFilter


Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0);
Q_DECLARE_METATYPE(sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter);





