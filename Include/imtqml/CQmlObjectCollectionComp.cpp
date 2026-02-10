// SPDX-License-Identifier: LGPL-2.1-or-later OR GPL-2.0-or-later OR GPL-3.0-or-later OR LicenseRef-ImtCore-Commercial
#include <imtqml/CQmlObjectCollectionComp.h>


// ACF includes
#include <iprm/TParamsPtr.h>
#include <iprm/CTextParam.h>
#include <iprm/ITextParam.h>
#include <iprm/CEnableableParam.h>

// ImtCore includes
#include <imtbase/CCollectionFilter.h>
#include <imtbase/CTreeItemModel.h>


namespace imtqml
{


// private methods

iprm::CParamsSet* CQmlObjectCollectionComp::GetFilters(QString filterParams)
{
	if (!filterParams.isEmpty()){
		imtbase::CCollectionFilter* collectionFilterPtr = new imtbase::CCollectionFilter();
		iprm::CParamsSet* objectFilterPtr = new iprm::CParamsSet();
		iprm::CParamsSet* filters = new iprm::CParamsSet();
		QByteArray filterBA = filterParams.toUtf8();
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
			collectionFilterPtr->SetFilteringInfoIds(filteringInfoIds);
		}
		QString filterText = generalModel.GetData("TextFilter").toString();
		if (!filterText.isEmpty()){
			collectionFilterPtr->SetTextFilter(filterText);
		}
		imtbase::CTreeItemModel* sortModel = generalModel.GetTreeItemModel("Sort");
		if (sortModel != nullptr){
			QByteArray headerId = sortModel->GetData("HeaderId").toByteArray();
			QByteArray sortOrder = sortModel->GetData("SortOrder").toByteArray();
			if (!headerId.isEmpty() && !sortOrder.isEmpty()){
				collectionFilterPtr->SetSortingOrder(sortOrder == "ASC" ? imtbase::ICollectionFilter::SO_ASC : imtbase::ICollectionFilter::SO_DESC);
				collectionFilterPtr->SetSortingInfoIds(QByteArrayList() << headerId);
			}
		}
		if (generalModel.ContainsKey("ObjectFilter")){
			imtbase::CTreeItemModel* objectFilterModelPtr = generalModel.GetTreeItemModel("ObjectFilter");
			QByteArray key;
			if (objectFilterModelPtr->ContainsKey("Key")){
				key = objectFilterModelPtr->GetData("Key").toByteArray();
			}

			iprm::CTextParam* textParamPtr = new iprm::CTextParam();
			if (objectFilterModelPtr->ContainsKey("Value")){
				QString value = objectFilterModelPtr->GetData("Value").toString();
				textParamPtr->SetText(value);
			}

			iprm::CEnableableParam* enableableParamPtr = new iprm::CEnableableParam();
			if (objectFilterModelPtr->ContainsKey("IsEqual")){
				bool isEqual = objectFilterModelPtr->GetData("IsEqual").toBool();
				enableableParamPtr->SetEnabled(isEqual);

				iprm::CParamsSet* paramsSetPtr = new iprm::CParamsSet();
				paramsSetPtr->SetEditableParameter("Value", textParamPtr, true);
				paramsSetPtr->SetEditableParameter("IsEqual", enableableParamPtr, true);

				objectFilterPtr->SetEditableParameter(key, paramsSetPtr, true);
			}
			else{
				objectFilterPtr->SetEditableParameter(key, textParamPtr, true);
			}
		}
		else{
			imtbase::CTreeItemModel objectFilterModel;
			QByteArray key;
			iprm::CTextParam* textParamPtr = new iprm::CTextParam();
			objectFilterPtr->SetEditableParameter(key, textParamPtr, true);
		}
		filters->SetEditableParameter("Filter", collectionFilterPtr, true);
		filters->SetEditableParameter("ObjectFilter", objectFilterPtr, true);
		return filters;
	}
	return new iprm::CParamsSet();
}


int CQmlObjectCollectionComp::GetElementsCount(QString filterParams)
{
	if (m_objectCollectionCompPtr.IsValid()){
		if (!filterParams.isEmpty()){
			iprm::CParamsSet* filters = GetFilters(filterParams);
			return m_objectCollectionCompPtr->GetElementsCount(filters);
		}
		else{
			return m_objectCollectionCompPtr->GetElementsCount();
		}

	}
	return 0;
}


imtqml::CQmlObjectCollectionIterator* CQmlObjectCollectionComp::CreateObjectCollectionIterator(int offset, int count, QString selectionParams)
{
	if (m_objectCollectionCompPtr.IsValid()){
		iprm::CParamsSet* filters = new iprm::CParamsSet();
		if (!selectionParams.isEmpty()){
			filters = GetFilters(selectionParams);
		}
		imtbase::IObjectCollectionIterator* iterator = m_objectCollectionCompPtr->CreateObjectCollectionIterator(QByteArray(), offset, count, filters);
		if (iterator != nullptr){
			return new CQmlObjectCollectionIterator(iterator);
		}
	}

	return nullptr;
}


}


