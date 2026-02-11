// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
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
	if (m_filterExpression.fieldFilters.contains(fieldFilter)){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_filterExpression.fieldFilters << fieldFilter;

	return true;
}


bool CComplexCollectionFilter::AddFilterExpression(const FilterExpression& groupFilter)
{
	if (m_filterExpression.filterExpressions.contains(groupFilter)){
		return false;
	}

	istd::CChangeNotifier changeNotifier(this);

	m_filterExpression.filterExpressions << groupFilter;

	return true;
}


QByteArrayList CComplexCollectionFilter::GetDistinctFieldsList() const
{
	QByteArrayList retVal;

	for (const CComplexCollectionFilter::FieldInfo& info : m_fields){
		if (info.metaInfo.isDistinct){
			retVal << info.id;
		}
	}

	return retVal;
}


QByteArrayList CComplexCollectionFilter::GetFilterableFieldList() const
{
	QByteArrayList retVal;

	for (const CComplexCollectionFilter::FieldInfo& info : m_fields){
		if (info.metaInfo.flags & CComplexCollectionFilter::SO_TEXT_FILTER){
			retVal << info.id;
		}
	}

	return retVal;
}


// reimplemented (imtbase::IComplexCollectionFilter)

CComplexCollectionFilter::FieldInfo* CComplexCollectionFilter::GetEditableFieldInfo(const QByteArray& fieldId)
{
	for (CComplexCollectionFilter::FieldInfo& info : m_fields){
		if (info.id == fieldId){
			return &info;
		}
	}

	return nullptr;
}


const CComplexCollectionFilter::Fields& CComplexCollectionFilter::GetFields() const
{
	return m_fields;
}


void CComplexCollectionFilter::SetFields(const Fields& fields)
{
	if (m_fields != fields){
		istd::CChangeNotifier changeNotifier(this);

		m_fields = fields;
	}
}


const IComplexCollectionFilter::FilterExpression& CComplexCollectionFilter::GetFilterExpression() const
{
	return m_filterExpression;
}


void CComplexCollectionFilter::SetFilterExpression(const FilterExpression& filter)
{
	if (m_filterExpression != filter){
		istd::CChangeNotifier changeNotifier(this);

		m_filterExpression = filter;
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


// reimplemented (iser::ISerializable)

bool CComplexCollectionFilter::Serialize(iser::IArchive &archive)
{
	bool retVal = true;

	istd::CChangeNotifier changeNotifier(archive.IsStoring() ? nullptr : this);

	// Serialization of the sorting model
	int sortingItemCount = m_fields.size();

	static iser::CArchiveTag sortingFieldListTag("SortingFieldList", "Sorting field list", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag sortingFieldTag("SortingField", "Sorting field", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginMultiTag(sortingFieldListTag, sortingFieldTag, sortingItemCount);

	m_fields.resize(sortingItemCount);
	for (int i = 0; i < sortingItemCount; i++){
		FieldInfo sortInfo;
		if (archive.IsStoring()){
			sortInfo = m_fields[i];
		}

		retVal = retVal && archive.BeginTag(sortingFieldTag);
		retVal = retVal && SerializeFields(sortInfo, archive);
		retVal = retVal && archive.EndTag(sortingFieldTag);

		if (!archive.IsStoring()){
			m_fields[i] = sortInfo;
		}
	}

	retVal = retVal && archive.EndTag(sortingFieldListTag);

	// Serialization of the filtering model
	static iser::CArchiveTag filterTag("FieldsFilter", "Fields filter", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(filterTag);
	retVal = retVal && SerializeGroupFilter(m_filterExpression, archive);
	retVal = retVal && archive.EndTag(filterTag);

	// Serialization of the time filter model
	static iser::CArchiveTag timeFilterTag("TimeFilter", "Time filter", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginTag(timeFilterTag);
	retVal = retVal && m_timeFilter.Serialize(archive);
	retVal = retVal && archive.EndTag(timeFilterTag);

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

		m_filterExpression = implPtr->m_filterExpression;
		m_fields = implPtr->m_fields;
		m_timeFilter = implPtr->m_timeFilter;
		m_textFilter = implPtr->m_textFilter;

		return true;
	}

	return false;
}


bool CComplexCollectionFilter::IsEqual(const IChangeable &object) const
{
	const CComplexCollectionFilter* implPtr = dynamic_cast<const CComplexCollectionFilter*>(&object);
	if (implPtr != nullptr){
		return
			(m_filterExpression == implPtr->m_filterExpression) &&
			(m_fields == implPtr->m_fields) &&
			(m_textFilter == implPtr->m_textFilter) &&
			(m_timeFilter.IsEqual(implPtr->m_timeFilter));
	}

	return false;
}


istd::IChangeableUniquePtr CComplexCollectionFilter::CloneMe(CompatibilityMode mode) const
{
	istd::IChangeableUniquePtr clonePtr(new CComplexCollectionFilter);
	if (clonePtr->CopyFrom(*this, mode)){
		return clonePtr;
	}

	return nullptr;
}


bool CComplexCollectionFilter::ResetData(CompatibilityMode /*mode*/)
{
	istd::CChangeNotifier changeNotifier(this);

	m_fields.clear();
	m_filterExpression.fieldFilters.clear();
	m_filterExpression.filterExpressions.clear();
	m_filterExpression.logicalOperation = LO_AND;
	m_textFilter.text.clear();
	m_textFilter.fieldIds.clear();
	m_timeFilter.ResetData();

	return true;
}


// private methods

bool CComplexCollectionFilter::SerializeFields(IComplexCollectionFilter::FieldInfo& object, iser::IArchive& archive)
{
	bool retVal = true;

	static iser::CArchiveTag fieldIdTag("FieldId", "Field-ID", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fieldIdTag);
	retVal = retVal && archive.Process(object.id);
	retVal = retVal && archive.EndTag(fieldIdTag);

	static iser::CArchiveTag fieldTypeTag("FieldType", "Field Type", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(fieldTypeTag);
	retVal = retVal && I_SERIALIZE_ENUM(FieldType, archive, object.metaInfo.type);
	retVal = retVal && archive.EndTag(fieldTypeTag);

	static iser::CArchiveTag flagsTag("Flags", "Flags", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(flagsTag);
	// retVal = retVal && I_SERIALIZE_FLAG(SupportedOperations, archive, object.metaInfo.flags);
	retVal = retVal && archive.EndTag(flagsTag);

	static iser::CArchiveTag isDistinctTag("IsDistinct", "Is Distinct", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(isDistinctTag);
	retVal = retVal && archive.Process(object.metaInfo.isDistinct);
	retVal = retVal && archive.EndTag(isDistinctTag);

	static iser::CArchiveTag sortingOrderTag("SortingOrder", "Sorting Order", iser::CArchiveTag::TT_LEAF);
	retVal = retVal && archive.BeginTag(sortingOrderTag);
	retVal = retVal && I_SERIALIZE_ENUM(SortingOrder, archive, object.metaInfo.sortingOrder);
	retVal = retVal && archive.EndTag(sortingOrderTag);

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


bool CComplexCollectionFilter::SerializeGroupFilter(IComplexCollectionFilter::FilterExpression& object, iser::IArchive& archive)
{
	bool retVal = true;

	// Serialization of the sorting model
	int fieldFilterCount = object.fieldFilters.size();

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
	int groupFilterCount = object.filterExpressions.size();

	static iser::CArchiveTag groupFilterListTag("GroupFilterList", "Group filter list", iser::CArchiveTag::TT_MULTIPLE);
	static iser::CArchiveTag groupFilterTag("FilterExpression", "Group filter", iser::CArchiveTag::TT_GROUP);
	retVal = retVal && archive.BeginMultiTag(groupFilterListTag, groupFilterTag, groupFilterCount);

	object.filterExpressions.resize(groupFilterCount);
	for (int i = 0; i < groupFilterCount; i++){
		FilterExpression groupFilter;
		if (archive.IsStoring()){
			groupFilter = object.filterExpressions[i];
		}

		retVal = retVal && archive.BeginTag(groupFilterTag);
		retVal = retVal && SerializeGroupFilter(groupFilter, archive);
		retVal = retVal && archive.EndTag(groupFilterTag);

		if (!archive.IsStoring()){
			object.filterExpressions[i] = groupFilter;
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


