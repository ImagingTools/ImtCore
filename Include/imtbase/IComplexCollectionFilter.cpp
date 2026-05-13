// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtbase/IComplexCollectionFilter.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtbase
{


// public methods of the embedded class FieldFilter

bool IComplexCollectionFilter::FieldFilter::operator==(const FieldFilter& other) const
{
	bool retVal = true;

	retVal = retVal && (fieldId == other.fieldId);
	retVal = retVal && (filterValue == other.filterValue);
	retVal = retVal && (filterOperation == other.filterOperation);

	return retVal;
}


bool IComplexCollectionFilter::FieldFilter::operator!=(const FieldFilter& other) const
{
	return !(*this == other);
}


// public methods of the embedded class FilterExpression

bool IComplexCollectionFilter::FilterExpression::operator==(const FilterExpression& other) const
{
	bool retVal = true;

	retVal = retVal && (fieldFilters == other.fieldFilters);
	retVal = retVal && (filterExpressions == other.filterExpressions);
	retVal = retVal && (logicalOperation == other.logicalOperation);

	return retVal;
}

bool IComplexCollectionFilter::FilterExpression::operator!=(const FilterExpression& other) const
{
	return !(*this == other);
}


// public methods of the embedded class FieldMetaInfo

bool IComplexCollectionFilter::FieldMetaInfo::operator==(const FieldMetaInfo& other) const
{
	bool retVal = true;

	retVal = retVal && (type == other.type);
	retVal = retVal && (flags == other.flags);
	retVal = retVal && (isDistinct == other.isDistinct);
	retVal = retVal && (sortingOrder == other.sortingOrder);

	return retVal;
}


bool IComplexCollectionFilter::FieldMetaInfo::operator!=(const FieldMetaInfo& other) const
{
	return !(*this == other);
}


// public methods of the embedded class TextFilter

bool IComplexCollectionFilter::TextFilter::operator==(const TextFilter& other) const
{
	bool retVal = true;

	retVal = retVal && (text == other.text);
	retVal = retVal && (fieldIds == other.fieldIds);

	return retVal;
}


bool IComplexCollectionFilter::TextFilter::operator!=(const TextFilter& other) const
{
	return !(*this == other);
}


// public methods of the embedded class FieldInfo

bool IComplexCollectionFilter::FieldInfo::operator==(const FieldInfo& other) const
{
	bool retVal = true;

	retVal = retVal && (id == other.id);
	retVal = retVal && (metaInfo == other.metaInfo);

	return retVal;
}


bool IComplexCollectionFilter::FieldInfo::operator!=(const FieldInfo& other) const
{
	return !(*this == other);
}


} // namespace imtbase


