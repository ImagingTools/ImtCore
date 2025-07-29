#pragma once


// ACF includes
#include <iser/ISerializable.h>

// ImtCore includes
#include <imtbase/ITimeFilterParam.h>


namespace imtbase
{


/**
	Interface for a collection filter.
	\ingroup Collection
*/
class IComplexCollectionFilter: virtual public iser::ISerializable
{
public:
	enum SortingOrder
	{
		SO_NO_ORDER = 0,
		SO_ASC,
		SO_DESC
	};
	I_DECLARE_ENUM(SortingOrder, SO_NO_ORDER, SO_ASC, SO_DESC);

	enum FieldOperation
	{
		FO_EQUAL = 0,
		FO_NOT_EQUAL,
		FO_LESS,
		FO_GREATER,
		FO_NOT_LESS,
		FO_NOT_GREATER,
		FO_CONTAINS
	};
	I_DECLARE_ENUM(FieldOperation,
		FO_EQUAL,
		FO_NOT_EQUAL,
		FO_LESS,
		FO_GREATER,
		FO_NOT_LESS,
		FO_NOT_GREATER,
		FO_CONTAINS
	);

	enum LogicalOperation
	{
		LO_AND = 0,
		LO_OR
	};
	I_DECLARE_ENUM(LogicalOperation, LO_AND, LO_OR);

	struct FieldSortingInfo
	{
		FieldSortingInfo(
			const QByteArray& fieldId = QByteArray(),
			SortingOrder sortingOrder = SO_ASC
		):
		fieldId(fieldId),
		sortingOrder(sortingOrder) {};

		bool operator==(const FieldSortingInfo& other) const;
		bool operator!=(const FieldSortingInfo& other) const;

		QByteArray fieldId;
		SortingOrder sortingOrder = SO_ASC;
	};
	typedef QVector<FieldSortingInfo> FieldSortingInfoList;

	struct FieldFilter
	{
		FieldFilter(
			const QByteArray& fieldId = QByteArray(),
			const QVariant& filterValue = QVariant(),
			FieldOperation filterOperation = FO_EQUAL
		):
		fieldId(fieldId),
		filterValue(filterValue),
		filterOperation(filterOperation) {};

		bool operator==(const FieldFilter& other) const;
		bool operator!=(const FieldFilter& other) const;

		QByteArray fieldId;
		QVariant filterValue;
		FieldOperation filterOperation = FO_EQUAL;
	};

	struct GroupFilter
	{
		GroupFilter(
			const QVector<FieldFilter>& fieldFilters = QVector<FieldFilter>(),
			const QVector<GroupFilter>& groupFilters = QVector<GroupFilter>(),
			LogicalOperation logicalOperation = LO_AND
		):
		fieldFilters(fieldFilters),
		groupFilters(groupFilters),
		logicalOperation(logicalOperation) {};

		bool operator==(const GroupFilter& other) const;
		bool operator!=(const GroupFilter& other) const;

		QVector<FieldFilter> fieldFilters;
		QVector<GroupFilter> groupFilters;
		LogicalOperation logicalOperation = LO_AND;
	};

	struct TextFilter
	{
		QString text;
		QByteArrayList fieldIds;
	};

	virtual const FieldSortingInfoList& GetSortingInfo() const = 0;
	virtual void SetSortingInfo(const FieldSortingInfoList& info) = 0;
	virtual const GroupFilter& GetFieldsFilter() const = 0;
	virtual void SetFieldsFilter(const GroupFilter& filter) = 0;
	virtual const imtbase::ITimeFilterParam& GetTimeFilter() const = 0;
	virtual void SetTimeFilter(const imtbase::ITimeFilterParam& filter) = 0;
	virtual const QByteArrayList& GetDistinctFieldsList() const = 0;
	virtual void SetDistinctFieldsList(const QByteArrayList& filedIds) = 0;
	virtual QString GetTextFilter() const = 0;
	virtual void SetTextFilter(const QString& textFilter) = 0;
	virtual const QByteArrayList& GetTextFilterFieldsList() const = 0;
	virtual void SetTextFilterFieldsList(const QByteArrayList& fieldIds) = 0;
};


} // namespace imtbase


