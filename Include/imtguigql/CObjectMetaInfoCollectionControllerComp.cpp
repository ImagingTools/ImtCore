#include <imtguigql/CObjectMetaInfoCollectionControllerComp.h>


// STL includes
#include <cmath>

// ImtCore includes
#include <idoc/CStandardDocumentMetaInfo.h>
#include <imtbase/CCollectionFilter.h>
#include <imtdb/CSqlDatabaseObjectCollectionComp.h>


namespace imtguigql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

// protected methods


imtbase::CTreeItemModel* CObjectMetaInfoCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	istd::TDelPtr<imtbase::CTreeItemModel> rootModelPtr(new imtbase::CTreeItemModel());
	imtbase::CTreeItemModel* dataModel = nullptr;
	imtbase::CTreeItemModel* itemsModel = nullptr;
	imtbase::CTreeItemModel* notificationModel = nullptr;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error").toUtf8();
	}

	if (!errorMessage.isEmpty()){
		imtbase::CTreeItemModel* errorsItemModel = rootModelPtr->AddTreeModel("errors");
		errorsItemModel->SetData("message", errorMessage);
	}
	else{
		dataModel = new imtbase::CTreeItemModel();
		itemsModel = new imtbase::CTreeItemModel();
		notificationModel = new imtbase::CTreeItemModel();

		const imtgql::CGqlObject* viewParamsGql = nullptr;
		const QList<imtgql::CGqlObject>* inputParams = gqlRequest.GetParams();
		if (inputParams->size() > 0){
			viewParamsGql = inputParams->at(0).GetFieldArgumentObjectPtr("viewParams");
		}

		iprm::CParamsSet filterParams;
		imtbase::CCollectionFilter m_filter;
		int offset = 0, count = -1;
		if (viewParamsGql != nullptr){
			offset = viewParamsGql->GetFieldArgumentValue("Offset").toInt();
			count = viewParamsGql->GetFieldArgumentValue("Count").toInt();

			QByteArray filterBA = viewParamsGql->GetFieldArgumentValue("FilterModel").toByteArray();
			if (!filterBA.isEmpty()){
				imtbase::CTreeItemModel generalModel;
				generalModel.CreateFromJson(filterBA);

				imtbase::CTreeItemModel* filterModel = generalModel.GetTreeItemModel("FilterIds");
				if (filterModel != nullptr){
					QByteArrayList filteringInfoIds;
					for (int i = 0; i < filterModel->GetItemsCount(); i++){
						QByteArray headerId = filterModel->GetData("Id", i).toByteArray();
						if (!headerId.isEmpty()){
							filteringInfoIds << headerId;
						}
					}
					m_filter.SetFilteringInfoIds(filteringInfoIds);
				}

				QString filterText = generalModel.GetData("TextFilter").toString();
				if (!filterText.isEmpty()){
					m_filter.SetTextFilter(filterText);
				}

				imtbase::CTreeItemModel* sortModel = generalModel.GetTreeItemModel("Sort");
				if (sortModel != nullptr){
					QByteArray headerId = sortModel->GetData("HeaderId").toByteArray();
					QByteArray sortOrder = sortModel->GetData("SortOrder").toByteArray();
					if (!headerId.isEmpty() && !sortOrder.isEmpty()){
						m_filter.SetSortingOrder(sortOrder == "ASC" ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);
						m_filter.SetSortingInfoIds(QByteArrayList() << headerId);
					}
				}
			}

			filterParams.SetEditableParameter("Filter", &m_filter);
			this->SetAdditionalFilters(*viewParamsGql, &filterParams);
		}

		int pagesCount = std::ceil(m_objectCollectionCompPtr->GetElementsCount(&filterParams) / (double)count);
		if (pagesCount < 0){
			pagesCount = 1;
		}

		notificationModel->SetData("PagesCount", pagesCount);

		imtdb::CSqlDatabaseObjectCollectionComp* sqlDatabaseObjectCollectionCompPtr = dynamic_cast<imtdb::CSqlDatabaseObjectCollectionComp*>(m_objectCollectionCompPtr.GetPtr());

		if (sqlDatabaseObjectCollectionCompPtr != nullptr){
			imtdb::CSqlDatabaseObjectCollectionComp::ObgectsMetaInfos objectMetaInfos;
			QByteArrayList metaInfoIds = GetInformationIds(gqlRequest, "");
			sqlDatabaseObjectCollectionCompPtr->GetObjectsMetaInfos(objectMetaInfos, metaInfoIds, offset, count, &filterParams);
			for (idoc::MetaInfoPtr metaInfoPtr: objectMetaInfos){
				int itemIndex = itemsModel->InsertNewItem();
				if (itemIndex >= 0){
					for (QByteArray metaInfoId: metaInfoIds){
//						QVariant elementInformation = metaInfoPtr->GetMetaInfo(metaInfoId);
//						if (metaInfoId == QByteArray("Added") || metaInfoId == QByteArray("LastModified")){
//							elementInformation = elementInformation.toDateTime().toString("dd.MM.yyyy hh:mm:ss");
//						}
//						itemsModel->SetData(metaInfoId, elementInformation, itemIndex);
					}

				}
			}
		}

		itemsModel->SetIsArray(true);
		dataModel->SetExternTreeModel("items", itemsModel);
		dataModel->SetExternTreeModel("notification", notificationModel);
	}
	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


} // namespace imtgql


