#include <imtbase/IComplexCollectionFilter.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtbase
{


// public methods of the embedded class FieldFilter

bool IComplexCollectionFilter::FieldSortingInfo::operator==(const FieldSortingInfo& other) const
{
	bool retVal = true;

	retVal = retVal && (fieldId == other.fieldId);
	retVal = retVal && (sortingOrder == other.sortingOrder);

	return retVal;
}


bool IComplexCollectionFilter::FieldSortingInfo::operator!=(const FieldSortingInfo& other) const
{
	return !(*this == other);
}


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


// public methods of the embedded class GroupFilter

bool IComplexCollectionFilter::GroupFilter::operator==(const GroupFilter& other) const
{
	bool retVal = true;

	retVal = retVal && (fieldFilters == other.fieldFilters);
	retVal = retVal && (groupFilters == other.groupFilters);
	retVal = retVal && (logicalOperation == other.logicalOperation);

	return retVal;
}

bool IComplexCollectionFilter::GroupFilter::operator!=(const GroupFilter& other) const
{
	return !(*this == other);
}


} // namespace imtbase


