#include <imtbase/CComplexCollectionFilter.h>


// ACF includes
#include <istd/TDelPtr.h>
#include <istd/CChangeNotifier.h>
#include <iser/IArchive.h>
#include <iser/CArchiveTag.h>
#include <iser/CPrimitiveTypesSerializer.h>


namespace imtbase
{


bool CComplexCollectionFilter::AddFieldFilter(const FieldFilter& fieldFilter)
{
	if (m_fieldsFilter.fieldFilters.contains(fieldFilter)){
		return false;
	}
	
	istd::CChangeNotifier changeNotifier(this);
	
	m_fieldsFilter.fieldFilters << fieldFilter;
	
	return true;
}


bool CComplexCollectionFilter::AddGroupFilter(const GroupFilter& groupFilter)
{
	if (m_fieldsFilter.groupFilters.contains(groupFilter)){
		return false;
	}
	
	istd::CChangeNotifier changeNotifier(this);
	
	m_fieldsFilter.groupFilters << groupFilter;
	
	return true;
}


// reimplemented (imtbase::IComplexCollectionFilter)

const IComplexCollectionFilter::FieldSortingInfoList& CComplexCollectionFilter::GetSortingInfo() const
{
	return m_sortingInfo;
}


void CComplexCollectionFilter::SetSortingInfo(const FieldSortingInfoList& info)
{
	if (m_sortingInfo != info){
		istd::CChangeNotifier changeNotifier(this);

		m_sortingInfo = info;
	}
}


const IComplexCollectionFilter::GroupFilter& CComplexCollectionFilter::GetFieldsFilter() const
{
	return m_fieldsFilter;
}


void CComplexCollectionFilter::SetFieldsFilter(const GroupFilter& filter)
{
	if (m_fieldsFilter != filter){
		istd::CChangeNotifier changeNotifier(this);

		m_fieldsFilter = filter;
	}
}


const imtbase::ITimeFilterParam& CComplexCollectionFilter::GetTimeFilter() const
{
	return m_timeFilter;
}


void CComplexCollectionFilter::SetTimeFilter(const imtbase::ITimeFilterParam& filter)
{
	istd::CChangeNotifier changeNotifier(this);

	m_timeFilter.CopyFrom(filter);
}


const QByteArrayList& CComplexCollectionFilter::GetDistinctFieldsList() const
{
	return m_distinctFields;
}


void CComplexCollectionFilter::SetDistinctFieldsList(const QByteArrayList& filedIds)
{
	QSet<QByteArray> idsSet(filedIds.cbegin(), filedIds.cend());

	if (idsSet.count() == filedIds.count()){
		istd::CChangeNotifier changeNotifier(this);

		m_distinctFields = filedIds;
	}
}


QString CComplexCollectionFilter::GetTextFilter() const
{
	return m_textFilter.text;
}


void CComplexCollectionFilter::SetTextFilter(const QString& textFilter)
{
	if (m_textFilter.text != textFilter){
		istd::CChangeNotifier changeNotifier(this);

		m_textFilter.text = textFilter;
	}
}


const QByteArrayList& CComplexCollectionFilter::GetTextFilterFieldsList() const
{
	return m_textFilter.fieldIds;
}


void CComplexCollectionFilter::SetTextFilterFieldsList(const QByteArrayList& fieldIds)
{
	if (m_textFilter.fieldIds != fieldIds){
		istd::CChangeNotifier changeNotifier(this);

		m_textFilter.fieldIds = fieldIds;
	}
}


// reimplemented (iser::ISerializable)

bool CComplexCollectionFilter::Serialize(iser::IArchive &archive)
{
	bool retVal = true;

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	// Serialization of the sorting model
	int sortingItemCount = m_sortingInfo.count();

	static iser::CArchiveTag sortingFieldListTag("SortingFieldList", "Sorting field list", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag sortingFieldTag("SortingField", "Sorting field", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginMultiTag(sortingFieldListTag, sortingFieldTag, sortingItemCount);

	m_sortingInfo.resize(sortingItemCount);
	for (int i = 0; i < sortingItemCount; i++){
		FieldSortingInfo sortInfo;
		if (archive.IsStoring()){
			sortInfo = m_sortingInfo[i];
		}

		retVal = retVal && archive.BeginTag(sortingFieldTag);
		retVal = retVal && SerializeFieldSortingInfo(sortInfo, archive);
		retVal = retVal && archive.EndTag(sortingFieldTag);

		if (!archive.IsStoring()){
			m_sortingInfo[i] = sortInfo;
		}
	}

	retVal = retVal && archive.EndTag(sortingFieldListTag);

	// Serialization of the filtering model
	static iser::CArchiveTag filterTag("FieldsFilter", "Fields filter", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(filterTag);
	retVal = retVal && SerializeGroupFilter(m_fieldsFilter, archive);
	retVal = retVal && archive.EndTag(filterTag);

	// Serialization of the time filter model
	static iser::CArchiveTag timeFilterTag("TimeFilter", "Time filter", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(timeFilterTag);
	retVal = retVal && m_timeFilter.Serialize(archive);
	retVal = retVal && archive.EndTag(timeFilterTag);

	// Serialization of the distinct fields IDs
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeContainer<QByteArrayList>(archive, m_distinctFields, "DistinctFields", "FieldId");

	return retVal;
}


// reimplemented (istd::IChangeable)

int CComplexCollectionFilter::GetSupportedOperations() const
{
	return SO_COPY | SO_CLONE | SO_COMPARE | SO_RESET;
}


bool CComplexCollectionFilter::CopyFrom(const IChangeable &object, CompatibilityMode /*mode*/)
{
	const CComplexCollectionFilter* implPtr = dynamic_cast<const CComplexCollectionFilter*>(&object);
	if (implPtr != nullptr){
		istd::CChangeNotifier changeNotifier(this);

		m_sortingInfo = implPtr->m_sortingInfo;
		m_fieldsFilter = implPtr->m_fieldsFilter;
		m_timeFilter = implPtr->m_timeFilter;
		m_distinctFields = implPtr->m_distinctFields;

		return true;
	}

	return false;
}


bool CComplexCollectionFilter::IsEqual(const IChangeable &object) const
{
	const CComplexCollectionFilter* implPtr = dynamic_cast<const CComplexCollectionFilter*>(&object);
	if (implPtr != nullptr){
		return
			(m_sortingInfo == implPtr->m_sortingInfo) &&
			(m_fieldsFilter == implPtr->m_fieldsFilter) &&
			(m_timeFilter.IsEqual(implPtr->m_timeFilter) &&
			(m_distinctFields == implPtr->m_distinctFields));
	}

	return false;
}


istd::IChangeable* CComplexCollectionFilter::CloneMe(CompatibilityMode mode) const
{
	istd::TDelPtr<CComplexCollectionFilter> clonePtr(new CComplexCollectionFilter);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr.PopPtr();
	}

	return nullptr;
}


bool CComplexCollectionFilter::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_sortingInfo.clear();

	m_fieldsFilter.fieldFilters.clear();
	m_fieldsFilter.groupFilters.clear();
	m_fieldsFilter.logicalOperation = LO_AND;
	m_distinctFields.clear();

	return m_timeFilter.ResetData();
}


// private methods

bool CComplexCollectionFilter::SerializeFieldSortingInfo(IComplexCollectionFilter::FieldSortingInfo& object, iser::IArchive& archive)
{
	bool retVal = true;

	static iser::CArchiveTag fieldIdTag("FieldId", "Collection field ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fieldIdTag);
	retVal = retVal && archive.Process(object.fieldId);
	retVal = retVal && archive.EndTag(fieldIdTag);

	static iser::CArchiveTag orderTag("SortingOrder", "Sorting order", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(orderTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<
		SortingOrder,
		IComplexCollectionFilter::ToString,
		IComplexCollectionFilter::FromString>(archive, object.sortingOrder);
	retVal = retVal && archive.EndTag(orderTag);

	return retVal;
}


bool CComplexCollectionFilter::SerializeFieldFilter(IComplexCollectionFilter::FieldFilter& object, iser::IArchive& archive)
{
	bool retVal = true;

	static iser::CArchiveTag fieldIdTag("FieldId", "Collection field ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fieldIdTag);
	retVal = retVal && archive.Process(object.fieldId);
	retVal = retVal && archive.EndTag(fieldIdTag);

	static iser::CArchiveTag valueTag("Value", "Value", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(valueTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeQVariant(archive, object.filterValue);
	retVal = retVal && archive.EndTag(valueTag);

	static iser::CArchiveTag operationTag("Operation", "Operation", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(operationTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<
		FieldOperation,
		IComplexCollectionFilter::ToString,
		IComplexCollectionFilter::FromString>(archive, object.filterOperation);
	retVal = retVal && archive.EndTag(operationTag);

	return retVal;
}


bool CComplexCollectionFilter::SerializeGroupFilter(IComplexCollectionFilter::GroupFilter& object, iser::IArchive& archive)
{
	bool retVal = true;

	// Serialization of the sorting model
	int fieldFilterCount = object.fieldFilters.count();

	static iser::CArchiveTag fieldFilterListTag("FieldFilterList", "Field filter list", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag fieldFilterTag("FieldFilter", "Field filter", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginMultiTag(fieldFilterListTag, fieldFilterTag, fieldFilterCount);

	object.fieldFilters.resize(fieldFilterCount);
	for (int i = 0; i < fieldFilterCount; i++){
		FieldFilter fieldFilter;
		if (archive.IsStoring()){
			fieldFilter = object.fieldFilters[i];
		}

		retVal = retVal && archive.BeginTag(fieldFilterTag);
		retVal = retVal && SerializeFieldFilter(fieldFilter, archive);
		retVal = retVal && archive.EndTag(fieldFilterTag);

		if (!archive.IsStoring()){
			object.fieldFilters[i] = fieldFilter;
		}
	}

	retVal = retVal && archive.EndTag(fieldFilterListTag);

	// Serialization of the sorting model
	int groupFilterCount = object.groupFilters.count();

	static iser::CArchiveTag groupFilterListTag("GroupFilterList", "Group filter list", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag groupFilterTag("GroupFilter", "Group filter", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginMultiTag(groupFilterListTag, groupFilterTag, groupFilterCount);

	object.groupFilters.resize(groupFilterCount);
	for (int i = 0; i < groupFilterCount; i++){
		GroupFilter groupFilter;
		if (archive.IsStoring()){
			groupFilter = object.groupFilters[i];
		}

		retVal = retVal && archive.BeginTag(groupFilterTag);
		retVal = retVal && SerializeGroupFilter(groupFilter, archive);
		retVal = retVal && archive.EndTag(groupFilterTag);

		if (!archive.IsStoring()){
			object.groupFilters[i] = groupFilter;
		}
	}

	retVal = retVal && archive.EndTag(groupFilterListTag);

	// Serialization of the operation
	static iser::CArchiveTag operationTag("Operation", "Operation", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(operationTag);
	retVal = retVal && iser::CPrimitiveTypesSerializer::SerializeEnum<
		LogicalOperation,
		IComplexCollectionFilter::ToString,
		IComplexCollectionFilter::FromString>(archive, object.logicalOperation);
	retVal = retVal && archive.EndTag(operationTag);

	return retVal;
}


} // namespace imtbase


