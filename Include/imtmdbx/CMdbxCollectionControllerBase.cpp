#include <imtmdbx/CMdbxCollectionControllerBase.h>


// ImtCore includes
#include <imtbase/IIdentifiable.h>


namespace imtmdbx
{


namespace ComplexFilter = ::sdl::V1_0::imtbase;


// public methods

CMdbxCollectionControllerBase::CMdbxCollectionControllerBase()
{
	imtserverapp::IJsonRepresentationController::RepresentationInfo info = m_complexCollectionFilterRepresentationController.GetRepresentationInfo();
	if (!m_selectionParams.SetEditableParameter(info.modelId, &m_complexCollectionFilter)){
		return;
	}

	m_paramSetRepresentationController.RegisterSubController(m_complexCollectionFilterRepresentationController);
}


// protected methods

bool CMdbxCollectionControllerBase::processComplexFilterFromParamSet(
			FilterViewParams& retVal,
			iprm::CParamsSet* filterParams,
			const sdl::V1_0::imtbase::CParamsSet& selectionParams) const
{
	QJsonObject jsonObject;
	if (!selectionParams.WriteToJsonObject(jsonObject)){
		return false;
	}

	if (!m_paramSetRepresentationController.GetDataModelFromRepresentation(jsonObject, *filterParams)){
		return false;
	}

	using Filter = imtbase::IComplexCollectionFilter;

	const QByteArray complexFilterId = QByteArrayLiteral("ComplexFilter");
	if (!filterParams->GetParamIds().contains(complexFilterId)){
		return true;
	}

	auto* complexFilterParamPtr = dynamic_cast<imtbase::IComplexCollectionFilter*>(filterParams->GetEditableParameter(complexFilterId));
	if (complexFilterParamPtr == nullptr){
		return false;
	}

	// text filter
	const QString textFilter = complexFilterParamPtr->GetTextFilter();
	if(!textFilter.isEmpty()){
		retVal.textFilter = textFilter;
	}

	// sorting info
	const imtbase::IComplexCollectionFilter::Fields fields = complexFilterParamPtr->GetFields();
	for (const imtbase::IComplexCollectionFilter::FieldInfo& info : fields){
		const bool isSortField = info.metaInfo.flags & imtbase::IComplexCollectionFilter::SO_SORT;
		if (!isSortField){
			continue;
		}

		if(!info.id.isEmpty()){
			retVal.sortId = info.id;
		}

		if(info.metaInfo.sortingOrder != imtbase::IComplexCollectionFilter::SO_NO_ORDER){
			retVal.sortingOrder = Filter::ToString(info.metaInfo.sortingOrder).toUpper();
		}
	}

	return true;
}


int CMdbxCollectionControllerBase::GetElementCount(
			imtmdbx::CMaskContainer& maskContainer,
			DocumentTablePtr tablePtr,
			bool noTextFilter) const
{
	if (!maskContainer.isEmpty()){
		return noTextFilter ? maskContainer.GetUnitCount() : -1;
	}

	if (!noTextFilter){
		return -1;
	}

	if (tablePtr->MoveToFirst()){
		return -1;
	}

	int retVal = 1;
	while(tablePtr->MoveToNext()){
		++retVal;
	}

	tablePtr->MoveToFirst();

	return retVal;
}


bool CMdbxCollectionControllerBase::CheckMaskMatch(
			quint64 offset,
			quint64 bitPosition,
			int cacheLimit,
			imtmdbx::CMaskContainer& maskContainer,
			QList<QPair<quint64, quint64>>& itemCache) const
{
	for (const auto& pair : itemCache) {
		if (pair.second != offset) {
			continue;
		}

		return (pair.first >> bitPosition) & 1ULL;
	}

	quint64 item = imtmdbx::QUINT64_MAX;
	if (!maskContainer.GetItem(offset, item)){
		return false;
	}

	itemCache.append(qMakePair(item, offset));
	if (itemCache.length() > cacheLimit){
		itemCache.removeFirst();
	}

	return (item >> bitPosition) & 1ULL;
}


QString CMdbxCollectionControllerBase::ConvertParentIdsFromUuid(
			const QString& parentIds,
			DocumentTablePtr addressTable) const
{
	if(parentIds.isEmpty()){
		return QString();
	}

	QString retVal;
	const QStringList parentList = parentIds.split(',');
	for(const QString& parentId : parentList){
		quint64 intId = 0;
		bool keyValid = addressTable->GetKey(intId, parentId.toLocal8Bit());
		if(keyValid){
			if(!retVal.isEmpty()){
				retVal.append(',');
			}
			retVal += QString::number(intId);
		}
	}

	return retVal;
}


QString CMdbxCollectionControllerBase::ConvertParentIdsToUuid(
			const QString& parentIdsStr,
			DocumentTablePtr addressTable) const
{
	if (parentIdsStr.isEmpty()) {
		return QString();
	}

	QString retVal;

	const QStringList parentList = parentIdsStr.split(',');
	for (const QString& parentId : parentList) {
		const QUuid uuid = QUuid::fromString(parentId);
		QString parentUuid = uuid.isNull() ? addressTable->GetDocument(parentId.toInt()) : parentId;

		if (parentUuid.isEmpty()) {
			return QString();
		}

		if (!retVal.isEmpty()) {
			retVal.append(',');
		}
		retVal.append(parentUuid);
	}

	return retVal;
}


CMdbxCollectionControllerBase::FilterViewParams CMdbxCollectionControllerBase::processViewParams(
			const istd::TNullableValue<::sdl::V1_0::imtbase::CCollectionViewParams> viewParams) const
{
	if(!viewParams){
		return FilterViewParams();
	}

	FilterViewParams retVal;
	if(viewParams->count){
		retVal.count = *viewParams->count;
	}
	if(viewParams->offset){
		retVal.offset = *viewParams->offset;
	}

	if(!viewParams->filterModel){
		return retVal;
	}

	const ComplexFilter::CComplexCollectionFilter& complexFilter = *viewParams->filterModel;

	processSdlComplexFilter(retVal, complexFilter);

	return retVal;
}


void CMdbxCollectionControllerBase::processSdlComplexFilter(
			FilterViewParams& retVal,
			const istd::TNullableValue<ComplexFilter::CComplexCollectionFilter> complexFilter) const
{
	if(!complexFilter){
		return;
	}

	// text filter
	if(complexFilter->textFilter && complexFilter->textFilter->text){
		retVal.textFilter = *complexFilter->textFilter->text;
	}

	// sorting info
	if(!complexFilter->sortingInfo){
		return;
	}

	const imtsdl::TElementList<ComplexFilter::CFieldSortingInfo>& sortingInfo = *complexFilter->sortingInfo;
	const istd::TNullableValue<ComplexFilter::CFieldSortingInfo>& element = sortingInfo.first();
	if(element->fieldId){
		retVal.sortId = *element->fieldId;
	}
	if(element->sortingOrder){
		retVal.sortingOrder = *element->sortingOrder;
	}
}


} // namespace imtmdbx

