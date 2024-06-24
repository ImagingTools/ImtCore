#include <imtgql/CSdlCollectionControllerCompBase.h>


// ACF includes
#include <istd/TDelPtr.h>



namespace imtgql
{


// protected methods

imtbase::CTreeItemModel* CSdlCollectionControllerCompBase::ListObjects(
	const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()) {
		errorMessage = QString("Unable to list objects. Component reference 'ObjectCollection' was not set");

		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	int offset = 0, count = -1;

	const imtgql::CGqlObject* viewParamsGql = nullptr;
	const QList<imtgql::CGqlObject> inputParams = gqlRequest.GetParams();
	if (inputParams.size() > 0) {
		viewParamsGql = inputParams.at(0).GetFieldArgumentObjectPtr("viewParams");
	}

	iprm::CParamsSet filterParams;

	if (viewParamsGql != nullptr){
		offset = viewParamsGql->GetFieldArgumentValue("Offset").toInt();
		count = viewParamsGql->GetFieldArgumentValue("Count").toInt();
		PrepareFilters(gqlRequest, *viewParamsGql, filterParams);
	}

	int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

	int pagesCount = std::ceil(elementsCount / (double)count);
	if (pagesCount <= 0) {
		pagesCount = 1;
	}


	istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(m_objectCollectionCompPtr->CreateObjectCollectionIterator(offset, count, &filterParams));
	if (objectCollectionIterator == nullptr){
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = rootModelPtr->AddTreeModel("data");

	istd::TDelPtr<imtbase::ITreeModelWrittable> listPayloadWrittablePtr(CreateSdlItemList(gqlRequest,pagesCount, elementsCount, objectCollectionIterator.GetPtr(), errorMessage));

	if (dataModel != nullptr && !listPayloadWrittablePtr->WriteToModel(*dataModel)){
		errorMessage = QString("Unable to setup gql item. Unable to write list model.");
		SendCriticalMessage(0, errorMessage, "CSpotColorCollectionControllerComp");

		return nullptr;
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CSdlCollectionControllerCompBase::GetObject(
	const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{

	if (!m_objectCollectionCompPtr.IsValid()) {
		errorMessage = QString("Internal error");
		SendErrorMessage(0, errorMessage, "CSubstrateCollectionControllerComp");

		return nullptr;
	}

	QByteArray commandId = gqlRequest.GetCommandId();

	QByteArray objectId;

	const imtgql::CGqlObject* inputParamPtr = gqlRequest.GetParam("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetFieldArgumentValue("Id").toByteArray();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)) {
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
		imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

		istd::TDelPtr<imtbase::ITreeModelWrittable> substrateWrittablePtr(CreateSdlItem(gqlRequest,objectId, dataPtr, errorMessage));

		if (!substrateWrittablePtr.IsValid() || !substrateWrittablePtr->WriteToModel(*dataModelPtr)){
			errorMessage = QString("Unable to setup gql item. Unable to write model.");
			SendCriticalMessage(0, errorMessage, "CSubstrateCollectionControllerComp");
			Q_ASSERT(0);
		}

		return rootModelPtr.PopPtr();
	}

	errorMessage = QT_TR_NOOP(QString("Unable to get an substrate with ID: '%1'.").arg(qPrintable(objectId)));
	SendErrorMessage(0, errorMessage, "CSubstrateCollectionControllerComp");

	return nullptr;
}


} // namespace imtgql


