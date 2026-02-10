// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CObjectMetaInfoCollectionControllerComp.h>


// std includes
#include <cmath>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>


namespace imtservergql
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

		const imtgql::CGqlParamObject* viewParamsGql = nullptr;
		QList<imtgql::CGqlParamObject> inputParams;
		inputParams.append(gqlRequest.GetParams());
		if (!inputParams.isEmpty()){
			viewParamsGql = inputParams.at(0).GetParamArgumentObjectPtr("viewParams");
		}

		iprm::CParamsSet filterParams;
		imtbase::CCollectionFilter m_filter;
		int offset = 0;
		int count = -1;
		if (viewParamsGql != nullptr){
			offset = viewParamsGql->GetParamArgumentValue("offset").toInt();
			count = viewParamsGql->GetParamArgumentValue("count").toInt();

			QByteArray filterBA = viewParamsGql->GetParamArgumentValue("FilterModel").toByteArray();
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
            this->SetAdditionalFilters(gqlRequest, *viewParamsGql, &filterParams);
		}

		int pagesCount = std::ceil(m_objectCollectionCompPtr->GetElementsCount(&filterParams) / (double)count);
		if (pagesCount < 0){
			pagesCount = 1;
		}

		notificationModel->SetData("PagesCount", pagesCount);

		itemsModel->SetIsArray(true);
		dataModel->SetExternTreeModel("items", itemsModel);
		dataModel->SetExternTreeModel("notification", notificationModel);
	}
	rootModelPtr->SetExternTreeModel("data", dataModel);

	return rootModelPtr.PopPtr();
}


} // namespace imtservergql


