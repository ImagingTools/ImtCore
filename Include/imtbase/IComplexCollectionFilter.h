// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
		/**
		* Field value is equal to the filter value.
		* Applies to scalar fields.
		*/
		FO_EQUAL = 0,

		/**
		* Field value is not equal to the filter value.
		* Applies to scalar fields.
		*/
		FO_NOT_EQUAL,

		/**
		* Field value is less than the filter value.
		* Applies to scalar fields.
		*/
		FO_LESS,

		/**
		* Field value is greater than the filter value.
		* Applies to scalar fields.
		*/
		FO_GREATER,

		/**
		* Field value is greater than or equal to the filter value.
		* Applies to scalar fields.
		*/
		FO_NOT_LESS,

		/**
		* Field value is less than or equal to the filter value.
		* Applies to scalar fields.
		*/
		FO_NOT_GREATER,

		/**
		* Field value contains the filter value.
		* Semantics depend on the field type (e.g. string substring, collection containment).
		*/
		FO_CONTAINS, // Begin, Middle, End

		// array semantics
		FO_ARRAY_HAS_ANY,
		FO_ARRAY_NOT_HAS_ANY,
		FO_ARRAY_IS_EMPTY,
		FO_ARRAY_NOT_IS_EMPTY,
		FO_ARRAY_HAS_ALL,
		FO_ARRAY_NOT_HAS_ALL,
		FO_ARRAY_ILIKE_ANY,
		FO_ARRAY_NOT_ILIKE_ANY
	};
	I_DECLARE_ENUM(FieldOperation,
		FO_EQUAL,
		FO_NOT_EQUAL,
		FO_LESS,
		FO_GREATER,
		FO_NOT_LESS,
		FO_NOT_GREATER,
		FO_CONTAINS,
		FO_ARRAY_HAS_ANY,
		FO_ARRAY_NOT_HAS_ANY,
		FO_ARRAY_IS_EMPTY,
		FO_ARRAY_NOT_IS_EMPTY,
		FO_ARRAY_HAS_ALL,
		FO_ARRAY_NOT_HAS_ALL,
		FO_ARRAY_ILIKE_ANY,
		FO_ARRAY_NOT_ILIKE_ANY
	);

	enum LogicalOperation
	{
		LO_AND = 0,
		LO_OR
	};
	I_DECLARE_ENUM(LogicalOperation, LO_AND, LO_OR);

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

	struct FilterExpression
	{
		FilterExpression(
			const QVector<FieldFilter>& fieldFilters = QVector<FieldFilter>(),
			const QVector<FilterExpression>& filterExpressions = QVector<FilterExpression>(),
			LogicalOperation logicalOperation = LO_AND
		):
		fieldFilters(fieldFilters),
		filterExpressions(filterExpressions),
		logicalOperation(logicalOperation) {};

		bool operator==(const FilterExpression& other) const;
		bool operator!=(const FilterExpression& other) const;

		QVector<FieldFilter> fieldFilters;
		QVector<FilterExpression> filterExpressions;
		LogicalOperation logicalOperation = LO_AND;
	};

	struct TextFilter
	{
		TextFilter(){}

		bool operator==(const TextFilter& other) const;
		bool operator!=(const TextFilter& other) const;

		QString text;
		QByteArrayList fieldIds;
	};

	enum FieldType
	{
		FT_SCALAR,
		FT_ARRAY
	};
	I_DECLARE_ENUM(FieldType, FT_SCALAR, FT_ARRAY);

	enum SupportedOperations
	{
		SO_NONE = 0,
		SO_SORT = 1 << 0,
		SO_TEXT_FILTER = 1 << 1
	};
	I_DECLARE_ENUM(SupportedOperations, SO_NONE, SO_SORT, SO_TEXT_FILTER);

	struct FieldMetaInfo
	{
		FieldMetaInfo(){}

		bool operator==(const FieldMetaInfo& other) const;
		bool operator!=(const FieldMetaInfo& other) const;

		FieldType type = FT_SCALAR;
		int flags = SO_NONE;
		bool isDistinct = false;
		SortingOrder sortingOrder = SO_NO_ORDER;
	};

	struct FieldInfo
	{
		FieldInfo(){}

		FieldInfo(
			const QByteArray& fieldId,
			FieldMetaInfo metaInfo = FieldMetaInfo()
		):
		id(fieldId),
		metaInfo(metaInfo) {};

		bool operator==(const FieldInfo& other) const;
		bool operator!=(const FieldInfo& other) const;

		QByteArray id;
		FieldMetaInfo metaInfo;
	};

	typedef QVector<FieldInfo> Fields;

	virtual FieldInfo* GetEditableFieldInfo(const QByteArray& fieldId) = 0;
	virtual const Fields& GetFields() const = 0;
	virtual void SetFields(const Fields& filter) = 0;

	virtual const FilterExpression& GetFilterExpression() const = 0;
	virtual void SetFilterExpression(const FilterExpression& filter) = 0;

	// TODO: Remove time filter
	virtual const imtbase::ITimeFilterParam& GetTimeFilter() const = 0;
	virtual void SetTimeFilter(const imtbase::ITimeFilterParam& filter) = 0;

	virtual QString GetTextFilter() const = 0;
	virtual void SetTextFilter(const QString& textFilter) = 0;
};


} // namespace imtbase


