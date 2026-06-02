// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtservergql/CObjectMetaInfoCollectionControllerComp.h>

// std includes
#include <cmath>

// Qt includes
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

// ImtCore includes
#include <GeneratedFiles/imtbasesdl/SDL/1.0/CPP/ImtCollection.h>
#include <imtbase/CCollectionFilter.h>


namespace imtservergql
{


// reimplemented (imtgql::IGqlRepresentationDataController)

// protected methods


QJsonObject CObjectMetaInfoCollectionControllerComp::ListObjects(const imtgql::CGqlRequest& gqlRequest, QString& errorMessage) const
{
	QJsonObject rootObj;

	if (!m_objectCollectionCompPtr.IsValid()){
		errorMessage = QObject::tr("Internal error");
		QJsonArray errorsArray;
		QJsonObject errorObj;
		errorObj.insert(QStringLiteral("message"), errorMessage);
		errorsArray.append(errorObj);
		rootObj.insert(QStringLiteral("errors"), errorsArray);
		return rootObj;
	}

	if (errorMessage.isEmpty()){
		QJsonObject dataObj;
		QJsonArray itemsArray;
		QJsonObject notificationObj;

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

		notificationObj.insert(QStringLiteral("PagesCount"), pagesCount);
		dataObj.insert(QStringLiteral("items"), itemsArray);
		dataObj.insert(QStringLiteral("notification"), notificationObj);
		rootObj.insert(QStringLiteral("data"), dataObj);
	}

	return rootObj;
}


} // namespace imtservergql
