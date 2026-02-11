// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CSdlCollectionControllerCompBase.h>


// ACF includes
#include <istd/TDelPtr.h>


namespace imtservergql
{


// protected methods

imtbase::CTreeItemModel* CSdlCollectionControllerCompBase::ListObjects(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Unable to list objects. Component reference 'ObjectCollection' was not set");

		SendCriticalMessage(0, errorMessage);

		return nullptr;
	}

	int offset = 0;
	int count = -1;

	const imtgql::CGqlParamObject* viewParamsGql = nullptr;
	QList<imtgql::CGqlParamObject> inputParams;
	inputParams.append(gqlRequest.GetParams());
	if (!inputParams.isEmpty()){
		viewParamsGql = inputParams.at(0).GetParamArgumentObjectPtr("viewParams");
	}

	iprm::CParamsSet filterParams;

	if (viewParamsGql != nullptr){
		offset = viewParamsGql->GetParamArgumentValue("offset").toInt();
		count = viewParamsGql->GetParamArgumentValue("count").toInt();
		PrepareFilters(gqlRequest, *viewParamsGql, filterParams);
	}

	int elementsCount = m_objectCollectionCompPtr->GetElementsCount(&filterParams);

	int pagesCount = std::ceil(elementsCount / (double)count);
	if (pagesCount <= 0){
		pagesCount = 1;
	}

	istd::TDelPtr<imtbase::IObjectCollectionIterator> objectCollectionIterator(m_objectCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), offset, count, &filterParams));
	if (!objectCollectionIterator.IsValid()){
		errorMessage = QString("Object collection iterator could not be created");

		SendCriticalMessage(0, errorMessage, "CSdlCollectionControllerCompBase");

		return nullptr;
	}

	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());

	imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");
	if (dataModelPtr != nullptr){
		istd::TDelPtr<imtbase::ITreeModelWrittable> listPayloadWrittablePtr(
					CreateSdlItemList(
								gqlRequest,
								pagesCount,
								elementsCount,
								objectCollectionIterator.GetPtr(),
								errorMessage));
		if (listPayloadWrittablePtr.IsValid()){
			if (!listPayloadWrittablePtr->WriteToModel(*dataModelPtr)){
				errorMessage = QString("Unable to setup GraphQL-item. Unable to write the list model");

				SendCriticalMessage(0, errorMessage, "CSdlCollectionControllerCompBase");

				return nullptr;
			}
		}
		else{
			return nullptr;
		}
	}
	else {
		return nullptr;
	}

	return rootModelPtr.PopPtr();
}


imtbase::CTreeItemModel* CSdlCollectionControllerCompBase::GetObject(
			const imtgql::CGqlRequest& gqlRequest,
			QString& errorMessage) const
{
	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QString("Internal error: Object collection component was not set or resolved");

		SendCriticalMessage(0, errorMessage, "CSdlCollectionControllerCompBase");

		return nullptr;
	}

	QByteArray objectId;

	const imtgql::CGqlParamObject* inputParamPtr = gqlRequest.GetParamObject("input");
	if (inputParamPtr != nullptr){
		objectId = inputParamPtr->GetParamArgumentValue("Id").toByteArray();
	}

	imtbase::IObjectCollection::DataPtr dataPtr;
	if (m_objectCollectionCompPtr->GetObjectData(objectId, dataPtr)){
		istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
		imtbase::CTreeItemModel* dataModelPtr = rootModelPtr->AddTreeModel("data");

		istd::TDelPtr<imtbase::ITreeModelWrittable> substrateWrittablePtr(CreateSdlItem(gqlRequest,objectId, dataPtr, errorMessage));

		if (!substrateWrittablePtr.IsValid()){
			if (errorMessage.isEmpty()){
				errorMessage = "Failed to read object";
			}

			SendCriticalMessage(0, errorMessage, "CSdlCollectionControllerCompBase");
			return nullptr;
		}

		if(!substrateWrittablePtr->WriteToModel(*dataModelPtr)){
			errorMessage = QString("Unable to setup GraphQL-item. Unable to write model");
			SendCriticalMessage(0, errorMessage, "CSdlCollectionControllerCompBase");
			Q_ASSERT(0);
			return nullptr;
		}

		return rootModelPtr.PopPtr();
	}

	errorMessage = QT_TR_NOOP(QString("Unable to get an object with ID: '%1'.").arg(qPrintable(objectId)));

	SendErrorMessage(0, errorMessage, "CSdlCollectionControllerCompBase");

	return nullptr;
}


} // namespace imtservergql


