// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtserverapp/CComplexCollectionFilterRepresentationController.h>


// ImtCore includes
#include <imtbase/CComplexCollectionFilter.h>
#include <imtbase/CTimeFilterParam.h>
#include <imtauth/IUserInfo.h>
#include <imtsdl/TElementList.h>


namespace imtserverapp
{


const QMap<int, imtbase::IComplexCollectionFilter::FieldOperation> CComplexCollectionFilterRepresentationController::s_allowableFlagsCombination ={
	// Scalar operations
	{FOF_EQUAL, imtbase::IComplexCollectionFilter::FieldOperation::FO_EQUAL},
	{FOF_EQUAL | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_EQUAL},

	{FOF_GREATER, imtbase::IComplexCollectionFilter::FieldOperation::FO_GREATER},
	{FOF_GREATER | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_GREATER},
	{FOF_GREATER | FOF_EQUAL, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_LESS},

	{FOF_LESS, imtbase::IComplexCollectionFilter::FieldOperation::FO_LESS},
	{FOF_LESS | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_LESS},
	{FOF_LESS | FOF_EQUAL, imtbase::IComplexCollectionFilter::FieldOperation::FO_NOT_GREATER},

	{FOF_CONTAINS, imtbase::IComplexCollectionFilter::FieldOperation::FO_CONTAINS},

	// Array operations
	{FOF_ARRAY_IS_EMPTY, imtbase::IComplexCollectionFilter::FieldOperation::FO_ARRAY_IS_EMPTY},
	{FOF_ARRAY_HAS_ANY, imtbase::IComplexCollectionFilter::FieldOperation::FO_ARRAY_HAS_ANY},
	{FOF_ARRAY_HAS_ALL, imtbase::IComplexCollectionFilter::FieldOperation::FO_ARRAY_HAS_ALL},
	{FOF_ARRAY_ILIKE_ANY, imtbase::IComplexCollectionFilter::FieldOperation::FO_ARRAY_ILIKE_ANY},

	// Array with NOT modifier
	{FOF_ARRAY_IS_EMPTY | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_ARRAY_NOT_IS_EMPTY},
	{FOF_ARRAY_HAS_ANY | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_ARRAY_NOT_HAS_ANY},
	{FOF_ARRAY_HAS_ALL | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_ARRAY_NOT_HAS_ALL},
	{FOF_ARRAY_ILIKE_ANY | FOF_NOT, imtbase::IComplexCollectionFilter::FieldOperation::FO_ARRAY_NOT_ILIKE_ANY}
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
	
	return complexFilterPtr != nullptr;
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

	// Time Filter
	sdl::imtbase::ComplexCollectionFilter::CTimeFilter::V1_0 sdlTimeFilter;
	const imtbase::ITimeFilterParam& timeFilter = complexFilterPtr->GetTimeFilter();
	if (!m_timeFilterParamRepresentationController.GetSdlRepresentationFromDataModel(sdlTimeFilter, timeFilter)){
		return false;
	}
	sdlRepresentation.timeFilter = sdlTimeFilter;

	// Distinct fields
	QByteArrayList distinctFieldsList;
	QByteArrayList textFilterFieldsList;
	QList<sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo::V1_0> sdlFieldSortingInfoList;

	imtbase::IComplexCollectionFilter::Fields fields = complexFilterPtr->GetFields();
	for (const imtbase::IComplexCollectionFilter::FieldInfo& fieldInfo : fields){
		if (fieldInfo.metaInfo.isDistinct){
			distinctFieldsList << fieldInfo.id;
		}

		if (fieldInfo.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_TEXT_FILTER){
			textFilterFieldsList << fieldInfo.id;
		}

		if (fieldInfo.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_SORT){
			sdl::imtbase::ComplexCollectionFilter::CFieldSortingInfo::V1_0 sdlFieldSortingInfo;
			sdlFieldSortingInfo.fieldId = fieldInfo.id;

			if (fieldInfo.metaInfo.sortingOrder == imtbase::IComplexCollectionFilter::SO_ASC){
				sdlFieldSortingInfo.sortingOrder = "ASC";
			}
			else if (fieldInfo.metaInfo.sortingOrder == imtbase::IComplexCollectionFilter::SO_DESC){
				sdlFieldSortingInfo.sortingOrder = "DESC";
			}
			else{
				return false; 
			}

			sdlFieldSortingInfoList << sdlFieldSortingInfo;
		}
	}
	sdlRepresentation.distinctFields.Emplace().FromList(distinctFieldsList);
	sdlRepresentation.sortingInfo.Emplace().FromList(sdlFieldSortingInfoList);

	// Text filter
	sdl::imtbase::ComplexCollectionFilter::CTextFilter::V1_0 sdlTextFilter;
	sdlTextFilter.text = complexFilterPtr->GetTextFilter();
	sdlTextFilter.fieldIds.Emplace().FromList(textFilterFieldsList);
	sdlRepresentation.textFilter = sdlTextFilter;

	const imtbase::IComplexCollectionFilter::FilterExpression& groupFilter = complexFilterPtr->GetFilterExpression();

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

	imtbase::IComplexCollectionFilter::Fields fields;

	if (sdlRepresentation.textFilter){
		Filter::CTextFilter::V1_0 textFilter = *sdlRepresentation.textFilter;
		if (textFilter.text){
			complexFilterPtr->SetTextFilter(*textFilter.text);
		}

		if (textFilter.fieldIds){
			for (const QByteArray& fieldId : textFilter.fieldIds->ToList()){
				CreateOrUpdateFieldInfo(fields, fieldId, std::nullopt, true);
			}
		}
	}

	for (const istd::TSharedNullable<Filter::CFieldSortingInfo::V1_0>& sourceSortingItem  : std::as_const(sourceSorting)){
		if (sourceSortingItem->fieldId && !sourceSortingItem->fieldId->isEmpty()){
			QByteArray fieldId = sourceSortingItem->fieldId->toUtf8();
			if (sourceSortingItem->sortingOrder){
				QString order = *sourceSortingItem->sortingOrder;
				if (!order.isEmpty()){
					if (order == "ASC"){
						CreateOrUpdateFieldInfo(fields, fieldId, std::nullopt, std::nullopt, std::nullopt, std::nullopt, imtbase::IComplexCollectionFilter::SO_ASC);
					}
					else if (order == "DESC"){
						CreateOrUpdateFieldInfo(fields, fieldId, std::nullopt, std::nullopt, std::nullopt, std::nullopt, imtbase::IComplexCollectionFilter::SO_DESC);
					}
					else{
						return false;
					}
				}
			}
		}
	}

	if (sdlRepresentation.distinctFields){
		QByteArrayList distinctFieldIds = sdlRepresentation.distinctFields->ToList();
		for (const QByteArray& fieldId : distinctFieldIds){
			CreateOrUpdateFieldInfo(fields, fieldId, std::nullopt, std::nullopt, std::nullopt, true, std::nullopt);
		}
	}

	complexFilterPtr->SetFields(fields);

	if (sdlRepresentation.timeFilter){
		imtbase::CTimeFilterParam timeFilter;
		Filter::CTimeFilter::V1_0 timeFilterSdl = *sdlRepresentation.timeFilter;
		if (!m_timeFilterParamRepresentationController.GetDataModelFromSdlRepresentation(timeFilter, timeFilterSdl)){
			return false;
		}

		complexFilterPtr->SetTimeFilter(timeFilter);
	}

	imtbase::IComplexCollectionFilter::FilterExpression targetFilter;
	if (!GetGroupFilterFromSdlRepresentation(sourceFilter, targetFilter)){
		return false;
	}

	complexFilterPtr->AddFilterExpression(targetFilter);

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
			const sdl::imtbase::ComplexCollectionFilter::FieldFilterUnion& u,
			imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter) const
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	bool isOk = true;
	bool retVal = true;

	if (const auto *fieldFilterSdl = std::get_if<Filter::CFieldFilter>(&u)){
		if (!fieldFilterSdl->Version_1_0.HasValue()){
			return false;
		}

		Filter::CFieldFilter::V1_0 representation = *fieldFilterSdl->Version_1_0;
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

		if (!GetQVariantFromSdlValue(value, sourceFilterValueType, fieldFilter.filterValue)){
			return false;
		}

		retVal = retVal && isOk;

		if (retVal){
			if (!representation.filterOperations || representation.filterOperations->isEmpty()){
				return false;
			}

			int flags = ComputeFlagsFromSdlOperations(*representation.filterOperations);

			if (!s_allowableFlagsCombination.contains(flags)){
				return false;
			}

			fieldFilter.filterOperation = s_allowableFlagsCombination[flags];
		}
	}
	else if (const auto* arrayFilterSdl = std::get_if<Filter::CArrayFieldFilter>(&u)){
		if (!arrayFilterSdl->Version_1_0.HasValue()){
			return false;
		}

		Filter::CArrayFieldFilter::V1_0 representation = *arrayFilterSdl->Version_1_0;
		fieldFilter.fieldId = *representation.fieldId;

		if (!representation.filterValueType.HasValue()){
			return false;
		}

		if (!representation.filterValues){
			return false;
		}

		QStringList values = representation.filterValues->ToList();

		Filter::ValueType sourceFilterValueType = *representation.filterValueType;
		QVariantList variantValues;

		for (const QString& val : values){
			QVariant value;
			if (!GetQVariantFromSdlValue(val, sourceFilterValueType, value)){
				return false;
			}
			variantValues.append(value);
		}

		fieldFilter.filterValue = variantValues;

		if (!representation.filterOperations.HasValue() || representation.filterOperations->isEmpty()){
			return false;
		}

		int flags = ComputeFlagsFromSdlOperations(*representation.filterOperations);
		if (!s_allowableFlagsCombination.contains(flags)){
			return false;
		}
	
		fieldFilter.filterOperation = s_allowableFlagsCombination[flags];
	}


	return retVal;
}


bool CComplexCollectionFilterRepresentationController::GetSdlRepresentationFromFieldFilter(
			const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter,
			sdl::imtbase::ComplexCollectionFilter::FieldFilterUnion& u) const
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	int typeId = fieldFilter.filterValue.typeId();

	if (typeId == QMetaType::QVariantList){ // Array
		Filter::CArrayFieldFilter arrayFilter;
		arrayFilter.Version_1_0.Emplace();
		Filter::CArrayFieldFilter::V1_0 representation = *arrayFilter.Version_1_0;

		representation.fieldId = fieldFilter.fieldId;

		QVariantList values = fieldFilter.filterValue.value<QVariantList>();
		QStringList stringValues;
		for (const QVariant& v : values) {
			stringValues.append(v.toString());
		}
		representation.filterValues.Emplace().FromList(stringValues);

		if (!values.isEmpty()){
			const QVariant& first = values.first();
			if (!GetSdlMetaTypeFromVariantType(first.typeId(), *representation.filterValueType)){
				return false;
			}
		}
		else{
			representation.filterValueType = Filter::ValueType::String;
		}

		representation.filterOperations.Emplace();
		MapFieldOperationToSdlOperations(fieldFilter.filterOperation, *representation.filterOperations);

		u = std::move(arrayFilter);
	}
	else{
		Filter::CFieldFilter scalarFilter;
		scalarFilter.Version_1_0.Emplace();
		Filter::CFieldFilter::V1_0 representation = *scalarFilter.Version_1_0;

		representation.fieldId = fieldFilter.fieldId;

		representation.filterValue = fieldFilter.filterValue.toString();

		if (typeId == QMetaType::Int){
			representation.filterValueType = Filter::ValueType::Integer;
		}
		else if (typeId == QMetaType::Bool){
			representation.filterValueType = Filter::ValueType::Bool; 
		}
		else if (typeId == QMetaType::QString || typeId == QMetaType::QByteArray ){
			representation.filterValueType = Filter::ValueType::String;
		}
		else if ( typeId == QMetaType::Double || typeId == QMetaType::Float || typeId == QMetaType::LongLong || typeId == QMetaType::Long){
			representation.filterValueType = Filter::ValueType::Number;
		}
		else{
			return false;
		}

		representation.filterOperations.Emplace();
		MapFieldOperationToSdlOperations(fieldFilter.filterOperation, *representation.filterOperations);

		u = std::move(scalarFilter);
	}

	return true;
}



bool CComplexCollectionFilterRepresentationController::GetGroupFilterFromSdlRepresentation(
			const sdl::imtbase::ComplexCollectionFilter::CGroupFilter::V1_0& representation,
			imtbase::IComplexCollectionFilter::FilterExpression& groupFilter) const
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	imtsdl::TElementList<Filter::FieldFilterUnion> sourceFieldSubFilters;
	if (representation.fieldFilters){
		sourceFieldSubFilters = *representation.fieldFilters;
	}
	imtsdl::TElementList<Filter::CGroupFilter::V1_0> sourceGroupSubFilters;
	if (representation.groupFilters){
		sourceGroupSubFilters = *representation.groupFilters;
	}
	QVector<imtbase::IComplexCollectionFilter::FieldFilter> targetFieldSubFilters;
	QVector<imtbase::IComplexCollectionFilter::FilterExpression> targetGroupSubFilters;

	for (const istd::TSharedNullable<Filter::FieldFilterUnion>& sourceFieldSubFilter : std::as_const(sourceFieldSubFilters)){
		imtbase::IComplexCollectionFilter::FieldFilter targetFieldSubFilter;

		if (!GetFieldFilterFromSdlRepresentation(*sourceFieldSubFilter, targetFieldSubFilter)){
			return false;
		}

		targetFieldSubFilters.append(targetFieldSubFilter);
	}

	for (const istd::TSharedNullable<Filter::CGroupFilter::V1_0>& sourceGroupSubFilter : std::as_const(sourceGroupSubFilters)){
		imtbase::IComplexCollectionFilter::FilterExpression targetGroupSubFilter;

		if (!GetGroupFilterFromSdlRepresentation(*sourceGroupSubFilter, targetGroupSubFilter)){
			return false;
		}

		targetGroupSubFilters.append(targetGroupSubFilter);
	}

	groupFilter.fieldFilters = targetFieldSubFilters;
	groupFilter.filterExpressions = targetGroupSubFilters;

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
			const imtbase::IComplexCollectionFilter::FilterExpression& groupFilter,
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
	for (const imtbase::IComplexCollectionFilter::FilterExpression& groupFilter : std::as_const(groupFilter.filterExpressions)){
		Filter::CGroupFilter::V1_0 sdlGroupFilter;
		if (!GetSdlRepresentationFromGroupFilter(groupFilter, sdlGroupFilter)){
			return false;
		}

		sdlGroupFilters << sdlGroupFilter;
	}
	representation.groupFilters = sdlGroupFilters;

	// Fields filter
	imtsdl::TElementList<Filter::FieldFilterUnion> sdlFieldFilters;
	for (const imtbase::IComplexCollectionFilter::FieldFilter& fieldFilter : std::as_const(groupFilter.fieldFilters)){
		Filter::FieldFilterUnion sdlFieldFilter;
		if (!GetSdlRepresentationFromFieldFilter(fieldFilter, sdlFieldFilter)){
			return false;
		}

		sdlFieldFilters << sdlFieldFilter;
	}
	representation.fieldFilters = sdlFieldFilters;

	return true;
}


bool CComplexCollectionFilterRepresentationController::GetSdlMetaTypeFromVariantType(const int& typeId, sdl::imtbase::ComplexCollectionFilter::ValueType& sdlType) const
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	if (typeId == QMetaType::Int){
		sdlType = Filter::ValueType::Integer;
	}
	else if (typeId == QMetaType::Bool){
		sdlType = Filter::ValueType::Bool; 
	}
	else if (typeId == QMetaType::QString || typeId == QMetaType::QByteArray ){
		sdlType = Filter::ValueType::String;
	}
	else if ( typeId == QMetaType::Double || typeId == QMetaType::Float || typeId == QMetaType::LongLong || typeId == QMetaType::Long){
		sdlType = Filter::ValueType::Number;
	}
	else{
		return false;
	}

	return true;
}


bool CComplexCollectionFilterRepresentationController::GetQVariantFromSdlValue(
			const QString& sdlValue,
			const sdl::imtbase::ComplexCollectionFilter::ValueType& valueType,
			QVariant& value) const
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	bool isOk = true;
	switch (valueType){
	case Filter::ValueType::Integer:
		value = sdlValue.toLongLong(&isOk);
		break;
	case Filter::ValueType::Number:
		value = sdlValue.toDouble(&isOk);
		break;
	case Filter::ValueType::String:
		value = sdlValue;
		break;
	case Filter::ValueType::Bool:
		if (sdlValue.compare("true", Qt::CaseInsensitive) == 0){
			value = true;
		}
		else if (sdlValue.compare("false", Qt::CaseInsensitive) == 0){
			value = false;
		}
		else{
			return false;
		}
		break;
	default:
		return false;
	}

	return isOk;
}


int CComplexCollectionFilterRepresentationController::ComputeFlagsFromSdlOperations(
	const imtsdl::TElementList<sdl::imtbase::ComplexCollectionFilter::FilterOperation>& filterOperations) const
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

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
	if (filterOperations.contains(Filter::FilterOperation::ArrayIsEmpty)){
		flags |= FOF_ARRAY_IS_EMPTY;
	}
	if (filterOperations.contains(Filter::FilterOperation::ArrayHasAny)){
		flags |= FOF_ARRAY_HAS_ANY;
	}
	if (filterOperations.contains(Filter::FilterOperation::ArrayHasAll)){
		flags |= FOF_ARRAY_HAS_ALL;
	}
	if (filterOperations.contains(Filter::FilterOperation::ArrayILikeAny)){
		flags |= FOF_ARRAY_ILIKE_ANY;
	}

	return flags;
}


void CComplexCollectionFilterRepresentationController::MapFieldOperationToSdlOperations(
			imtbase::IComplexCollectionFilter::FieldOperation op,
			imtsdl::TElementList<sdl::imtbase::ComplexCollectionFilter::FilterOperation>& outOperations) const
{
	namespace Filter = sdl::imtbase::ComplexCollectionFilter;

	switch(op){
	case imtbase::IComplexCollectionFilter::FO_EQUAL:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> ({Filter::FilterOperation::Equal});
		break;
	case imtbase::IComplexCollectionFilter::FO_NOT_EQUAL:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> {Filter::FilterOperation::Not, Filter::FilterOperation::Equal};
		break;
	case imtbase::IComplexCollectionFilter::FO_LESS:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> {Filter::FilterOperation::Less};
		break;
	case imtbase::IComplexCollectionFilter::FO_GREATER:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> {Filter::FilterOperation::Greater};
		break;
	case imtbase::IComplexCollectionFilter::FO_NOT_LESS:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> {Filter::FilterOperation::Not, Filter::FilterOperation::Less};
		break;
	case imtbase::IComplexCollectionFilter::FO_NOT_GREATER:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> {Filter::FilterOperation::Not, Filter::FilterOperation::Greater};
		break;
	case imtbase::IComplexCollectionFilter::FO_CONTAINS:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> {Filter::FilterOperation::Contains};
		break;
	case imtbase::IComplexCollectionFilter::FO_ARRAY_HAS_ANY:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> ({Filter::FilterOperation::ArrayHasAny});
		break;
	case imtbase::IComplexCollectionFilter::FO_ARRAY_HAS_ALL:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> ({Filter::FilterOperation::ArrayHasAll});
		break;
	case imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_HAS_ANY:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> ({Filter::FilterOperation::Not, Filter::FilterOperation::ArrayHasAny});
		break;
	case imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_HAS_ALL:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> ({Filter::FilterOperation::Not, Filter::FilterOperation::ArrayHasAll});
		break;
	case imtbase::IComplexCollectionFilter::FO_ARRAY_IS_EMPTY:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> ({Filter::FilterOperation::ArrayIsEmpty});
		break;
	case imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_IS_EMPTY:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> ({Filter::FilterOperation::Not, Filter::FilterOperation::ArrayIsEmpty});
		break;
	case imtbase::IComplexCollectionFilter::FO_ARRAY_ILIKE_ANY:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> ({Filter::FilterOperation::ArrayILikeAny});
		break;
	case imtbase::IComplexCollectionFilter::FO_ARRAY_NOT_ILIKE_ANY:
		outOperations = imtsdl::TElementList<Filter::FilterOperation> ({Filter::FilterOperation::Not, Filter::FilterOperation::ArrayILikeAny});
		break;
	}
}


bool CComplexCollectionFilterRepresentationController::FieldIsExists(const imtbase::IComplexCollectionFilter::Fields& fields, const QByteArray& fieldId) const
{
	for (const imtbase::IComplexCollectionFilter::FieldInfo& fieldInfo : fields){
		if (fieldInfo.id == fieldId){
			return true;
		}
	}

	return false;
}


bool CComplexCollectionFilterRepresentationController::CreateOrUpdateFieldInfo(
			imtbase::IComplexCollectionFilter::Fields& fields,
			const QByteArray& fieldId,
			std::optional<imtbase::IComplexCollectionFilter::FieldType> fieldType,
			std::optional<bool> filterable,
			std::optional<bool> sortable,
			std::optional<bool> isDistinct,
			std::optional<imtbase::IComplexCollectionFilter::SortingOrder> sortingOrder) const
{
	imtbase::IComplexCollectionFilter::FieldInfo* target = nullptr;

	for (auto& fieldInfo : fields){
		if (fieldInfo.id == fieldId){
			target = &fieldInfo;
			break;
		}
	}

	if (target == nullptr){
		fields.push_back({fieldId, {}});

		target = &fields.back();
	}

	auto& meta = target->metaInfo;

	if (fieldType){
		meta.type = *fieldType;
	}

	if (filterable){
		if (*filterable){
			meta.flags |= imtbase::IComplexCollectionFilter::SO_TEXT_FILTER;
		}
		else{
			meta.flags &= ~imtbase::IComplexCollectionFilter::SO_TEXT_FILTER;
		}
	}

	if (sortable){
		if (*sortable){
			meta.flags |= imtbase::IComplexCollectionFilter::SO_SORT;
		}
		else{
			meta.flags &= ~imtbase::IComplexCollectionFilter::SO_SORT;
		}
	}

	if (isDistinct){
		meta.isDistinct = *isDistinct;
	}

	if (sortingOrder){
		if (*sortingOrder != imtbase::IComplexCollectionFilter::SO_NO_ORDER){
			meta.flags |= imtbase::IComplexCollectionFilter::SO_SORT;
		}

		meta.sortingOrder = *sortingOrder;
	}

	return true;
}


} // namespace imtserverapp


