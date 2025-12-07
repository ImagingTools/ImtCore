#include <imtserverapp/CComplexCollectionFilterRepresentationController.h>


// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/CTimeFilterParam.h>
#include <imtauth/IUserInfo.h>
#include <imtsdl/TElementList.h>


namespace imtserverapp
{


const QMap<int, imtbase::IComplexCollectionFilter::FieldOperation> CComplexCollectionFilterRepresentationController::s_allowableFlagsCombination = {
	{FOF_EQUAL, imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL},
	{FOF_EQUAL | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_EQUAL},

	{FOF_GREATER, imtbase::IComplexCollectionFilter::FieldOperation::FO_GREATER},
	{FOF_GREATER | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_GREATER},
	{FOF_GREATER | FOF_EQUAL, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_LESS},

	{FOF_LESS, imtbase::IComplexCollectionFilter::FieldOperation::FO_LESS},
	{FOF_LESS | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_LESS},
	{FOF_LESS | FOF_EQUAL, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_GREATER},

	{FOF_CONTAINS, imtbase::IComplexCollectionFilter::FieldOperation::FO_CONTAINS}
};


// public methods

CComplexCollectionFilterRepresentationController::CComplexCollectionFilterRepresentationController()
{
	m_modelId = QByteArrayLiteral("ComplexFilter");
	m_name = QStringLiteral("Complex Filter");
	m_description = QStringLiteral("Complex filter");
}


// reimplemented (imtserverapp::TJsonRepresentationControllerCompWrap<sdl::imtbase::ImtBaseTypes::CParamsSet>)

QByteArray CComplexCollectionFilterRepresentationController::GetTypeId() const
{
	return QByteArrayLiteral("ComplexFilter");
}


bool CComplexCollectionFilterRepresentationController::IsModelSupported(const istd::IChangeable& dataModel) const
{
	const imtbase::IComplexCollectionFilter* complexFilterPtr = dynamic_cast<const imtbase::IComplexCollectionFilter*>(&dataModel);
	if (complexFilterPtr != nullptr){
		return true;
	}

	return false;
}


bool CComplexCollectionFilterRepresentationController::GetSdlRepresentationFromDataModel(
			sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0& sdlRepresentation,
			const istd::IChangeable& dataModel,
			const iprm::IParamsSet* /*paramsPtr*/) const
{
	const imtbase::CComplexCollectionFilter* complexFilterPtr = dynamic_cast<const imtbase::CComplexCollectionFilter*>(&dataModel);
	if (complexFilterPtr == nullptr){
		return false;
	}

	// Sorting info
	imtsdl::TElementList<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo::V1_0> sdlFieldSortingInfoList;
	const imtbase::IComplexCollectionFilter::FieldSortingInfoList& fieldSortingInfoList = complexFilterPtr->GetSortingInfo();
	for (const imtbase::IComplexCollectionFilter::FieldSortingInfo& fieldSortingInfo : std::as_const(fieldSortingInfoList)){
		sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo::V1_0 sdlFieldSortingInfo;
		sdlFieldSortingInfo.fieldId = fieldSortingInfo.fieldId;

		if (fieldSortingInfo.sortingOrder == imtbase::IComplexCollectionFilter::SO_ASC){
			sdlFieldSortingInfo.sortingOrder = "ASC";
		}
		else if (fieldSortingInfo.sortingOrder == imtbase::IComplexCollectionFilter::SO_DESC){
			sdlFieldSortingInfo.sortingOrder = "DESC";
		}
		else{
			return false; 
		}

		sdlFieldSortingInfoList << sdlFieldSortingInfo;
	}
	sdlRepresentation.sortingInfo = sdlFieldSortingInfoList;

	// Time Filter
	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 sdlTimeFilter;
	const imtbase::ITimeFilterParam& timeFilter = complexFilterPtr->GetTimeFilter();
	if (!m_timeFilterParamRepresentationController.GetSdlRepresentationFromDataModel(sdlTimeFilter, timeFilter)){
		return false;
	}
	sdlRepresentation.timeFilter = sdlTimeFilter;

	// Distinct fields
	imtsdl::TElementList<QByteArray> distinctFieldsList;
	distinctFieldsList.FromList(complexFilterPtr->GetDistinctFieldsList());
	sdlRepresentation.distinctFields = distinctFieldsList;

	// Text filter
	sdl::imtbase::ComplexCollectionFilter::CTextFilter::V1_0 sdlTextFilter;
	sdlTextFilter.text = complexFilterPtr->GetTextFilter();
	sdlTextFilter.fieldIds.Emplace();
	sdlTextFilter.fieldIds->FromList(complexFilterPtr->GetTextFilterFieldsList());
	sdlRepresentation.textFilter = sdlTextFilter;

	const imtbase::IComplexCollectionFilter::GroupFilter& groupFilter = complexFilterPtr->GetFieldsFilter();

	sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0 sdlGroupFilter;
	if (!GetSdlRepresentationFromGroupFilter(groupFilter, sdlGroupFilter)){
		return false;
	}
	sdlRepresentation.fieldsFilter = sdlGroupFilter;

	return true;
}


bool CComplexCollectionFilterRepresentationController::GetDataModelFromSdlRepresentation(
			istd::IChangeable& dataModel,
			const sdl::imtbase::ComplexCollectionFilter::CComplexCollectionFilter::V1_0& sdlRepresentation) const
{
	imtbase::CComplexCollectionFilter* complexFilterPtr = dynamic_cast<imtbase::CComplexCollectionFilter*>(&dataModel);
	if (complexFilterPtr == nullptr){
		return false;
	}

	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	imtsdl::TElementList<Filter::CFieldSortingInfo::V1_0> sourceSorting;
	if (sdlRepresentation.sortingInfo){
		sourceSorting = *sdlRepresentation.sortingInfo;
	}

	Filter::CGroupFilter::V1_0 sourceFilter;
	if (sdlRepresentation.fieldsFilter){
		sourceFilter = *sdlRepresentation.fieldsFilter;
	}

	Filter::CTimeFilter::V1_0 sourceTimeFilter;
	if (sdlRepresentation.timeFilter){
		sourceTimeFilter = *sdlRepresentation.timeFilter;
	}

	imtbase::IComplexCollectionFilter::FieldSortingInfoList sorting;
	for (const istd::TSharedNullable<Filter::CFieldSortingInfo::V1_0>& sourceSortingItem  : std::as_const(sourceSorting)){
		imtbase::IComplexCollectionFilter::FieldSortingInfo fieldSorting;

		if (sourceSortingItem->fieldId && !sourceSortingItem->fieldId->isEmpty()){
			fieldSorting.fieldId = sourceSortingItem->fieldId->toLatin1();
			if (sourceSortingItem->sortingOrder){
				QString order = *sourceSortingItem->sortingOrder;
				if (!order.isEmpty()){
					if (order == "ASC"){
						fieldSorting.sortingOrder = imtbase::IComplexCollectionFilter::SO_ASC;
					}
					else if (order == "DESC"){
						fieldSorting.sortingOrder = imtbase::IComplexCollectionFilter::SO_DESC;
					}
					else {
						return false;
					}

					sorting.append(fieldSorting);
				}
			}
		}
	}

	if (!sorting.isEmpty()){
		complexFilterPtr->SetSortingInfo(sorting);
	}

	imtbase::IComplexCollectionFilter::GroupFilter targetFilter;
	if (!GetGroupFilterFromSdlRepresentation(sourceFilter, targetFilter)){
		return false;
	}

	complexFilterPtr->AddGroupFilter(targetFilter);

	if (sdlRepresentation.timeFilter){
		imtbase::CTimeFilterParam timeFilter;
		Filter::CTimeFilter::V1_0 timeFilterSdl = *sdlRepresentation.timeFilter;
		if (!m_timeFilterParamRepresentationController.GetDataModelFromSdlRepresentation(timeFilter, timeFilterSdl)){
			return false;
		}

		complexFilterPtr->SetTimeFilter(timeFilter);
	}

	if (sdlRepresentation.distinctFields){
		complexFilterPtr->SetDistinctFieldsList(sdlRepresentation.distinctFields->ToList());
	}

	if (sdlRepresentation.textFilter){
		Filter::CTextFilter::V1_0 textFilter = *sdlRepresentation.textFilter;
		if (textFilter.text){
			complexFilterPtr->SetTextFilter(*textFilter.text);
		}

		if (textFilter.fieldIds){
			complexFilterPtr->SetTextFilterFieldsList(textFilter.fieldIds->ToList());
		}
	}

	return true;
}


// private methods

QString CComplexCollectionFilterRepresentationController::GetFlagsAsString(int flags) const
{
	QString retVal;

	if (flags & FOF_NOT){
		retVal = "NOT";
	}

	if (flags & FOF_EQUAL){
		retVal += retVal.isEmpty() ? "EQUAL" : ", EQUAL";
	}

	if (flags & FOF_GREATER){
		retVal += retVal.isEmpty() ? "GREATER" : ", GREATER";
	}

	if (flags & FOF_LESS){
		retVal += retVal.isEmpty() ? "LESS" : ", LESS";
	}

	if (flags & FOF_CONTAINS){
		retVal += retVal.isEmpty() ? "CONTAINS" : ", CONTAINS";
	}

	return retVal;
}


bool CComplexCollectionFilterRepresentationController::GetFieldFilterFromSdlRepresentation(
			const sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0& representation,
			imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter) const
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	bool isOk = true;
	bool retVal = true;

	if (!representation.fieldId){
		return false;
	}
	fieldFilter.fieldId = *representation.fieldId;

	QString value;
	if (!representation.filterValue){
		return false;
	}
	value = *representation.filterValue;

	if (!representation.filterValueType){
		return false;
	}

	Filter::ValueType sourceFilterValueType = *representation.filterValueType;
	switch (sourceFilterValueType){
	case Filter::ValueType::Integer:
		fieldFilter.filterValue = value.toLongLong(&isOk);
		break;
	case Filter::ValueType::Number:
		fieldFilter.filterValue = value.toDouble(&isOk);
		break;
	case Filter::ValueType::String:
		fieldFilter.filterValue = value;
		break;
	case Filter::ValueType::Bool:
		if (value.compare("true", Qt::CaseInsensitive) == 0){
			fieldFilter.filterValue = true;
		}
		else if (value.compare("false", Qt::CaseInsensitive) == 0){
			fieldFilter.filterValue = false;
		}
		else{
			return false;
		}
		break;
	default:
		return false;
	}

	retVal = retVal && isOk;

	if (retVal){
		if (!representation.filterOperations || representation.filterOperations->isEmpty()){
			return false;
		}
		imtsdl::TElementList<Filter::FilterOperation> filterOperations = *representation.filterOperations;

		int flags = 0;

		if (filterOperations.contains(Filter::FilterOperation::Not)){
			flags |= FOF_NOT;
		}
		if (filterOperations.contains(Filter::FilterOperation::Equal)){
			flags |= FOF_EQUAL;
		}
		if (filterOperations.contains(Filter::FilterOperation::Less)){
			flags |= FOF_LESS;
		}
		if (filterOperations.contains(Filter::FilterOperation::Greater)){
			flags |= FOF_GREATER;
		}
		if (filterOperations.contains(Filter::FilterOperation::Contains)){
			flags |= FOF_CONTAINS;
		}

		if (!s_allowableFlagsCombination.contains(flags)){
			return false;
		}

		fieldFilter.filterOperation = s_allowableFlagsCombination[flags];
	}

	return retVal;
}


bool CComplexCollectionFilterRepresentationController::GetSdlRepresentationFromFieldFilter(
			const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter,
			sdl::imtbase::ComplexCollectionFilter::CFieldFilter::V1_0& representation) const
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	representation.fieldId = fieldFilter.fieldId;

	int typeId = fieldFilter.filterValue.typeId();
	if (typeId == QMetaType::Int){
		representation.filterValueType = Filter::ValueType::Integer;
	}
	else if (typeId == QMetaType::Bool){
		representation.filterValueType = Filter::ValueType::Bool;
	}
	else if (typeId == QMetaType::QString || typeId == QMetaType::QByteArray ){
		representation.filterValueType = Filter::ValueType::String;
	}
	else if (	typeId == QMetaType::Double ||
				typeId == QMetaType::Float ||
				typeId == QMetaType::LongLong ||
				typeId == QMetaType::Long){
		representation.filterValueType = Filter::ValueType::Number;
	}
	else{
		return false;
	}

	representation.filterValue = fieldFilter.filterValue.toString();

	switch(fieldFilter.filterOperation){
	case imtbase::IComplexCollectionFilter::FO_EQUAL:
		representation.filterOperations = imtsdl::TElementList<Filter::FilterOperation> ({Filter::FilterOperation::Equal});
		break;
	case imtbase::IComplexCollectionFilter::FO_NOT_EQUAL:
		representation.filterOperations = imtsdl::TElementList<Filter::FilterOperation> {Filter::FilterOperation::Not, Filter::FilterOperation::Equal};
		break;
	case imtbase::IComplexCollectionFilter::FO_LESS:
		representation.filterOperations = imtsdl::TElementList<Filter::FilterOperation> {Filter::FilterOperation::Less};
		break;
	case imtbase::IComplexCollectionFilter::FO_GREATER:
		representation.filterOperations = imtsdl::TElementList<Filter::FilterOperation> {Filter::FilterOperation::Greater};
		break;
	case imtbase::IComplexCollectionFilter::FO_NOT_LESS:
		representation.filterOperations = imtsdl::TElementList<Filter::FilterOperation> {Filter::FilterOperation::Not, Filter::FilterOperation::Less};
		break;
	case imtbase::IComplexCollectionFilter::FO_NOT_GREATER:
		representation.filterOperations = imtsdl::TElementList<Filter::FilterOperation> {Filter::FilterOperation::Not, Filter::FilterOperation::Greater};
		break;
	case imtbase::IComplexCollectionFilter::FO_CONTAINS:
		representation.filterOperations = imtsdl::TElementList<Filter::FilterOperation> {Filter::FilterOperation::Contains};
		break;
	}

	return true;
}


bool CComplexCollectionFilterRepresentationController::GetGroupFilterFromSdlRepresentation(
			const sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0& representation,
			imtbase::IComplexCollectionFilter::GroupFilter& groupFilter) const
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	imtsdl::TElementList<Filter::CFieldFilter::V1_0> sourceFieldSubFilters;
	if (representation.fieldFilters){
		sourceFieldSubFilters = *representation.fieldFilters;
	}
	imtsdl::TElementList<Filter::CGroupFilter::V1_0> sourceGroupSubFilters;
	if (representation.groupFilters){
		sourceGroupSubFilters = *representation.groupFilters;
	}
	QVector<imtbase::IComplexCollectionFilter::FieldFilter> targetFieldSubFilters;
	QVector<imtbase::IComplexCollectionFilter::GroupFilter> targetGroupSubFilters;

	for (const istd::TSharedNullable<Filter::CFieldFilter::V1_0>& sourceFieldSubFilter : std::as_const(sourceFieldSubFilters)){
		imtbase::IComplexCollectionFilter::FieldFilter targetFieldSubFilter;

		if (!GetFieldFilterFromSdlRepresentation(*sourceFieldSubFilter, targetFieldSubFilter)){
			return false;
		}

		targetFieldSubFilters.append(targetFieldSubFilter);
	}

	for (const istd::TSharedNullable<Filter::CGroupFilter::V1_0>& sourceGroupSubFilter : std::as_const(sourceGroupSubFilters)){
		imtbase::IComplexCollectionFilter::GroupFilter targetGroupSubFilter;

		if (!GetGroupFilterFromSdlRepresentation(*sourceGroupSubFilter, targetGroupSubFilter)){
			return false;
		}

		targetGroupSubFilters.append(targetGroupSubFilter);
	}

	groupFilter.fieldFilters = targetFieldSubFilters;
	groupFilter.groupFilters = targetGroupSubFilters;

	if (!representation.groupFilters.has_value() && !representation.fieldFilters.has_value()){
		return true;
	}

	if (!representation.logicalOperation){
		return false;
	}
	Filter::LogicalOperation sourceLogicalOperation = *representation.logicalOperation;

	switch (sourceLogicalOperation){
	case Filter::LogicalOperation::And:
		groupFilter.logicalOperation = imtbase::IComplexCollectionFilter::LO_AND;
		break;
	case Filter::LogicalOperation::Or:
		groupFilter.logicalOperation = imtbase::IComplexCollectionFilter::LO_OR;
		break;
	default:
		return false;
	}

	return true;
}


bool CComplexCollectionFilterRepresentationController::GetSdlRepresentationFromGroupFilter(
			const imtbase::IComplexCollectionFilter::GroupFilter& groupFilter,
			sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0& representation) const
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	// Logical operation
	imtbase::IComplexCollectionFilter::LogicalOperation logicalOperation = groupFilter.logicalOperation;
	switch(logicalOperation){
	case imtbase::IComplexCollectionFilter::LO_AND:
		representation.logicalOperation = Filter::LogicalOperation::And;
		break;
	case imtbase::IComplexCollectionFilter::LO_OR:
		representation.logicalOperation = Filter::LogicalOperation::Or;
		break;
	}

	// Groups filter
	imtsdl::TElementList<Filter::CGroupFilter::V1_0> sdlGroupFilters;
	for (const imtbase::IComplexCollectionFilter::GroupFilter& groupFilter : std::as_const(groupFilter.groupFilters)){
		Filter::CGroupFilter::V1_0 sdlGroupFilter;
		if (!GetSdlRepresentationFromGroupFilter(groupFilter, sdlGroupFilter)){
			return false;
		}

		sdlGroupFilters << sdlGroupFilter;
	}
	representation.groupFilters = sdlGroupFilters;

	// Fields filter
	imtsdl::TElementList<Filter::CFieldFilter::V1_0> sdlFieldFilters;
	for (const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : std::as_const(groupFilter.fieldFilters)){
		Filter::CFieldFilter::V1_0 sdlFieldFilter;
		if (!GetSdlRepresentationFromFieldFilter(fieldFilter, sdlFieldFilter)){
			return false;
		}

		sdlFieldFilters << sdlFieldFilter;
	}
	representation.fieldFilters = sdlFieldFilters;

	return true;
}


} // namespace imtserverapp


